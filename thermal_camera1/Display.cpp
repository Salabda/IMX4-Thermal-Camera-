#include <opencv2/opencv.hpp>
#include <iostream>
#include "MeasureCoreNet.h"
#include "Display.hpp"

using namespace std;
//float vertical_length_cm = 1460; //(3.22 * 4.2)
//float horizontal_length_cm = 1200;//(3.78 * 2) + 4

//january test
float vertical_length_cm = 498; //(3.22 * 4.2)
float horizontal_length_cm = 420;//(3.78 * 2) + 4

GlobalRGB  globalContentRGB;
GlobalMaxPointInfo globalMaxInfo;


size_t imgsize = static_cast<size_t>(288) * 384 * 3;

unsigned int* globalRGBArray = new unsigned int[imgsize];


float map_value_to_new_range(float value, float old_min, float old_max, float new_min, float new_max) {
    // Check for division by zero in the old_range calculation
    if (old_max - old_min == 0) {
        return new_min; // Avoid division by zero
    }

    // Calculate the ratio between the old and new ranges
    float scale = (new_max - new_min) / (old_max - old_min);

    // Perform the mapping calculation using the ratio
    return new_min + ((value - old_min) * scale);
}

float calc_dist_cam_to_hotspot(const cv::Point2f world_coord) {
    // All in cm
    float camera_height = 357;
    float camera_x_y[2] = { 635, 495 };
    float camera2hotspot_flat[2] = { world_coord.x - camera_x_y[0], -1 * world_coord.y + camera_x_y[1] };
    float camera2hotspot_flat_length = sqrt(pow(camera2hotspot_flat[0], 2) + pow(camera2hotspot_flat[1], 2));
    float camera2hotspot_3d_length = sqrt(pow(camera2hotspot_flat_length, 2) + pow(camera_height, 2));
    float cameratotal = camera2hotspot_3d_length / 100;
    // Uncomment the lines below to print intermediate results if needed
    // printf("camera2hotspot_x = %.4f camera2hotspot_y = %.4f\n", camera2hotspot_flat[0], camera2hotspot_flat[1]);
    // printf("camera2hotspot_flat_length = %.4f\n", camera2hotspot_flat_length);
    //printf("camera2hotspot_3d_length eb Display.cpp= %.4f\n", cameratotal);

    return cameratotal;
}

float estimate_temperature(double camera2hotspot_length, double camera_temp) {
    double distance = camera2hotspot_length;
    cout << "                           distance" << distance<<endl;
    // Given coefficients and input data
    std::vector<double> coefficients = { -1.2142039554619828, 0.0, 1.0452530978781331,
                                         -0.31582564670360447, -0.0011117833394660437,
                                          0.011652588266540559, 0.001998731805561371 };

    // Create expanded features
    std::vector<double> expanded_features = { 1, 1, camera_temp, distance, camera_temp * camera_temp, distance * camera_temp, distance * distance };

    // Initialize stim_temp_new with intercept
    double stim_temp_new = 0;

    // Calculate stim_temp_new using coefficients and expanded features
    for (size_t i = 0; i < expanded_features.size(); ++i) {
        stim_temp_new += coefficients[i] * expanded_features[i];
        //std::cout << "stim_temp_new: " << stim_temp_new << std::endl;
    }
    //stim_temp_new = camera_temp;

    return stim_temp_new;
}

std::vector<float> FireAlarm(cv::Point2f world_coord, float temp) {

    // Check if temperature exceeds the threshold
    if (temp > ALARM_MAX_TEMP) {
        //Alarm == 1 == yes fire
        std::vector<float> x_y_temp_alarm = { world_coord.x, world_coord.y, temp, 1 };
        return x_y_temp_alarm;
    }
    else if (temp < ALARM_MAX_TEMP) {
        //Alarm == 0 == no fire
        std::vector<float> x_y_temp_alarm = { world_coord.x, world_coord.y, temp, 0 };
        return x_y_temp_alarm;
    }
    else {
        // Empty
        //Should not be empty here but idk
        return {};
    }
}

std::pair<cv::Mat, cv::Point2f> img_to_world_coor(cv::Mat combinedImage3, cv::Point2i thermal_cam_point) {
     //Remapping size from thermal to RGB
    thermal_cam_point.x = map_value_to_new_range(thermal_cam_point.x, 0, 384, 0, combinedImage3.cols);
    thermal_cam_point.y = map_value_to_new_range(thermal_cam_point.y, 0, 288, 0, combinedImage3.rows);

    cv::Matx31f pixel_coor_point(thermal_cam_point.x, thermal_cam_point.y, 1); // Extended to a 3D point
    //rows = 288 height
    cv::Point2f objects[4] = {
    	cv::Point2f(0.0f, static_cast<float>(combinedImage3.rows)),
    	cv::Point2f(0.0f, 0.0f),
    	cv::Point2f(static_cast<float>(combinedImage3.cols), 0.0f), 
    	cv::Point2f(static_cast<float>(combinedImage3.cols), static_cast<float>(combinedImage3.rows))};


    cv::Point2f converted_points[4] = { cv::Point2f(286+71, 492),
    								   cv::Point2f(-71+71, 447),
    								   cv::Point2f(20+71, 129),
    								   cv::Point2f(342+71, 299) };

    cv::Mat matrix;
    cv::getPerspectiveTransform(objects, converted_points).convertTo(matrix, CV_32F);
    //cout << matrix << endl;

    //// Display the matrix
    //std::cout << "Matrix:" << std::endl;
    //std::cout << matrix << std::endl;

    //cv::Mat_<float> matrix = (cv::Mat_<float>(3, 3) <<
    //    2.0979974, 0.75893295, 378,
    //    0.38529655, 10.080974, 0,
    //    -3.5522939e-05, 0.0034325542, 1);
    cv::Mat img_output;
    cv::warpPerspective(combinedImage3, img_output, matrix, cv::Size(horizontal_length_cm+150, vertical_length_cm +50 ));

    // Transform the point using matrix multiplication
    cv::Mat world_coor_point = matrix * pixel_coor_point;

    world_coor_point /= world_coor_point.at<float>(2, 0);
    world_coor_point.convertTo(world_coor_point, CV_32F);

    cv::Point2f point_duple(world_coor_point.at<float>(0, 0), world_coor_point.at<float>(1, 0));
    cv::circle(img_output, point_duple, 8, cv::Scalar(255, 0, 0), -1);
    cv::Point2f point_duplee(world_coor_point.at<float>(0, 0)-71, world_coor_point.at<float>(1, 0));
    //cout << point_duplee << endl;
    cv::Mat img_resized;
    //cv::imshow("ouput", img_output);
    //cv::waitKey(1);


    cv::resize(img_output, img_resized, cv::Size(600, 780));
    return std::make_pair(img_resized, point_duplee);

}

//cv::Mat WriteOnVideo(const GD_MTC_CALLBACK_Y16Info* pY16Info,GD_MTC_TempPointInfo pTempPointMax) {
//
//    //// Get the dimensions of the video frames
//    //int frame_width = cropped_frame.rows;
//    //int frame_height = cropped_frame.cols;
//
//    int frame_width = pY16Info->ImgWidth;
//    int frame_height = pY16Info->ImgHeight;
//
//    int x = pTempPointMax.PointX;
//    int y = pTempPointMax.PointY;
//    cv::Point2i Max_Temp_px_coor (x, y);
//    float Max_temp = pTempPointMax.PointTemp;
//
//
//    cv::Mat frame(288, 384, CV_8UC3, pY16Info->Y16Data);
//
//    std::pair<cv::Mat, cv::Point2f> result = img_to_world_coor(frame, Max_Temp_px_coor);
//
//    cv::Mat img_output = result.first;
//    cv::Point2f world_coor = result.second;
//
//    // Enter the fixed and totallength
//    float total_lengthtocamera = calc_dist_cam_to_hotspot(world_coor);
//
//    float fixed_temp = estimate_temperature(total_lengthtocamera, Max_temp);
//
//    cv::Mat resized_frame;
//  
//    // Create a black canvas
//    cv::Mat black_canvas = cv::Mat::zeros(cv::Size(frame_width + 100, frame_height), CV_8UC3);
//    // Put the resized frame on the left side of the canvas
//
//    // Retrieve the elements from the returned pair
//
//
//    resized_frame.copyTo(black_canvas(cv::Rect(0, 0, resized_frame.cols, resized_frame.rows)));
//
//
//
//    // Create text information
//    std::string text3 = "Camera Temp = " + std::to_string(Max_temp);
//    std::string text6 = "Fixed Temp = " + std::to_string(fixed_temp);
//    std::string text9 = "Total Length = " + std::to_string(total_lengthtocamera);
//
//    // Put the text on the right side of the canvas
//    
//    cv::putText(black_canvas, text3, cv::Point(static_cast<int>(frame_width * 0.38), static_cast<int>(frame_height * 0.60)),
//        cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(255, 0, 0), 3);
//    cv::putText(black_canvas, text6, cv::Point(static_cast<int>(frame_width * 0.38), static_cast<int>(frame_height * 0.75)),
//        cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(0, 0, 255), 3);
//    cv::putText(black_canvas, text9, cv::Point(static_cast<int>(frame_width * 0.38), static_cast<int>(frame_height * 0.34)),
//        cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(255, 100, 50), 3);
//
//
//
//    return black_canvas;
//}


int CALLBACK rgbCallbackFunc(const GD_MTC_CALLBACK_RGBInfo* RGBInfo, void* pUser) {

    int w = RGBInfo->ImgWidth;
    int h = RGBInfo->ImgHeight;

    if (h == 0 || w == 0) {
        cout << " -----------------------rgbCallbackFunc EMPTY Matrix	w=	" << RGBInfo->ImgWidth << endl;

        return 1;
    }

    globalContentRGB.ImgHeight = h;
    globalContentRGB.ImgWidth = w;
    globalContentRGB.DataLen = RGBInfo->RGBDataLen;
    globalContentRGB.RGBData = globalRGBArray;
    memcpy(globalContentRGB.RGBData, RGBInfo->RGBData, imgsize);

    //cout << " -----------------------rgbCallbackFunc 	h=	" << globalContentRGB.ImgHeight << endl;
    //cout << " -----------------------rgbCallbackFunc	w=	" << globalContentRGB.ImgWidth << endl;
    //cout << " -----------------------rgbCallbackFunc	l=	" << globalContentRGB.DataLen << endl;
    //cout << "size" <<imgsize<<endl;

    //cv::Mat combinedImage2(h, w, CV_8UC3, RGBInfo->RGBData);
    //if (!combinedImage2.empty()) {
    //    std::cout << "globalRGBArray is not empty." << std::endl;
    //    cv::imshow("Camera Feed22", combinedImage2);
    //    cv::waitKey(1);
    //}


	return 0;
}


int CALLBACK y16CallbackFunc(const GD_MTC_CALLBACK_Y16Info* pY16Info, void* pUser)
{
    GD_MTC_TempPointInfo pTempPointMax;

	int w = pY16Info->ImgWidth;
	int h = pY16Info->ImgHeight;
	float* pTempMatrix = new float[w * h];

	if (h == 0 || w == 0) { 
		cout << " -----------------------y16CallbackFunc EMPTY Matrix	w=	" << pY16Info->ImgWidth << endl;

		delete[] pTempMatrix; // Clean up dynamically allocated memory
		return 1; }


	if (0 == GD_MTC_SDK_GetHighTemp(pY16Info->pOpque, &pTempPointMax)){

		if (pTempMatrix) {

            //cout << " -----------------y16CallbackFunc TEMP MATRIX	 pTempPointMax->PointTemp " << pTempPointMax.PointTemp << endl;
		   //logFloatsToMaxTempFile(pTempPointMax);
            globalMaxInfo.PointX = pTempPointMax.PointX;
            globalMaxInfo.PointY = pTempPointMax.PointY;
            globalMaxInfo.PointTemp = pTempPointMax.PointTemp;
        
		}
		else {
			//cout << " -----------------y16CallbackFunc EMPTY TEMP MATRIX	pTempMatrix=" << pTempMatrix << endl;
		}
	}
	else {
		cout << " -----------------y16CallbackFunc GD_MTC_SDK_GetTempMatrix Error	" << endl;
	}

	delete[] pTempMatrix;
	return 0;
}

//Log temp.point values and SnapPicCallback

//int CALLBACK snapPicCallbackFunc(int* iY8Data, int* iY8DataLength, int* iY16Data, int* iY16DataLength, void* pUser){
//	
//	cout << "snapPicCallbackFunc		iY8DataLength = " << iY8DataLength << endl;
//
//	return 1;
//}
//
//int CALLBACK editSnapCallbacFunc(unsigned char* pRgbData, int nRgbDataLen, int nRgbWidth, int nRgbHeight, void* pUser) {
//	cout << "editSnapCallbacFunc		" << nRgbDataLen << endl;
//	bool bFullPath = false; bool bImagData = true; bool bTempData = true;
//	PROCESS_EDIT_SNAP_PIC_CB pSnapPicCb = 0;
//	int32_t snapuni = GD_MTC_SDK_SnapPictureUniform(StreamId, strFilePath, bFullPath, bImagData, bTempData, pSnapPicCb, 0);
//	return 1;
//}
// //Log bin files 
//auto lastLogTime = std::chrono::system_clock::now();
//auto lastLogTimeMatrix = std::chrono::system_clock::now();
//std::ofstream TemperatureMatrixFile("Output/MarioLogs/TemperatureMatrixFile.bin", std::ios::binary | std::ios::app);
//void logFloatsToMaxTempFile(GD_MTC_TempPointInfo PointMax) {
//    auto currentTime = std::chrono::system_clock::now();
//
//    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastLogTime);
//
//    const int minTimeIntervalMs = 332;
//    if (timeDiff.count() >= minTimeIntervalMs) {
//        std::tm timeInfo;
//        std::time_t timestamp = std::chrono::system_clock::to_time_t(currentTime);
//
//        if (localtime_s(&timeInfo, &timestamp) != 0) {
//            std::cerr << "Failed to get local time." << std::endl;
//            return;
//        }
//
//        char filename[50];
//        // Create a CSV file with the date as the name (e.g., "2023-10-29.csv")
//        std::strftime(filename, sizeof(filename), "Output/MarioLogs/%Y-%m-%d.csv", &timeInfo);
//
//        // Open the output CSV file
//        std::ofstream MaxTempFile(filename, std::ios::app);
//
//        if (!MaxTempFile.is_open()) {
//            std::cerr << "Failed to open the output file." << std::endl;
//            return;
//        }
//
//        char timestampStr[14]; // Format for time without year, month, and date
//        std::strftime(timestampStr, sizeof(timestampStr), "%H:%M:%S", &timeInfo);
//
//        MaxTempFile << timestampStr << "," << "temp = " << PointMax.PointTemp << "," << "X = " << PointMax.PointX << "," << "Y = " << PointMax.PointY << std::endl;
//        MaxTempFile.close();
//
//        lastLogTime = currentTime;
//    }
//}