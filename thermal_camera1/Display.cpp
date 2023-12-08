#include <opencv2/opencv.hpp>
#include <iostream>
#include "MeasureCoreNet.h"
#include "Display.hpp"

using namespace std;
float vertical_length_cm = 1460; //(3.22 * 4.2)
float horizontal_length_cm = 1200;//(3.78 * 2) + 4

GlobalRGB globalContentRGB;
extern cv::Mat image_source;
float map_value(float value, float old_min, float old_max, float new_min, float new_max) {
    float old_range = (old_max - old_min);
    float new_range = (new_max - new_min);
    float new_value = ((value - old_min) * new_range / old_range);
    return new_value;
}

double calc_dist_temp(cv::Point2f woorld_coortry) {
    // All in cm
    double camera_height = 517;
    std::vector<double> camera_x_y = { 275.3, 734.9 + 1610 };

    double camera2hotspot_flat_x = woorld_coortry.x - camera_x_y[0];
    double camera2hotspot_flat_y = -1 * woorld_coortry.y + camera_x_y[1];

    double camera2hotspot_flat_length = std::sqrt(
        std::pow(camera2hotspot_flat_x, 2) + std::pow(camera2hotspot_flat_y, 2)
    );

    double camera2hotspot_3d_length = std::sqrt(
        std::pow(camera2hotspot_flat_length, 2) + std::pow(camera_height, 2)
    );

    return camera2hotspot_3d_length;
}

float fix_temp(double camera2hotspot_length, double temperature) {
    double distance = camera2hotspot_length / 100;
    double thermal_camera = temperature;

    //ADD THE MODEL COEFFICIENTS

    double predicted_temperature = 0.0; // Initializing predicted_temperature

    // Extract the value and convert it to a float
    if (predicted_temperature == 0.0) {
        std::cout << "The value is empty." << std::endl;
        return 2000.0;
    }
    else {
        //


    }
}

std::pair<cv::Mat, cv::Point2f> pers_selected_area(cv::Mat frame_copy, cv::Point2i thermal_cam_point) {
    // Remapping size from thermal to RGB
    thermal_cam_point.x = map_value(thermal_cam_point.x, 0, 384, 0, frame_copy.cols);
    thermal_cam_point.y = map_value(thermal_cam_point.y, 0, 288, 0, frame_copy.rows);

    cv::Matx31f point(thermal_cam_point.x, thermal_cam_point.y, 1); // Extended to a 3D point

    float x1 = 0, y1 = frame_copy.rows;
    float x2 = 0, y2 = 0;
    float x3 = frame_copy.cols, y3 = 0;
    float x4 = frame_copy.cols, y4 = frame_copy.rows;

    // Define the coordinates as cv::Point2f
    cv::Point2f objects[4] = { cv::Point2f(x1, y1), cv::Point2f(x2, y2), cv::Point2f(x3, y3), cv::Point2f(x4, y4) };
    cv::Point2f converted_points[4] = { cv::Point2f(300, 1460),
                                       cv::Point2f(378, 0),
                                       cv::Point2f(1200, 150),
                                       cv::Point2f(710, 1545) };

    cv::Mat matrix = cv::getPerspectiveTransform(objects, converted_points);
    // Transform the point using matrix multiplication
    cv::Mat img_output;
    cv::warpPerspective(frame_copy, img_output, matrix, cv::Size(horizontal_length_cm, vertical_length_cm + 100));

    // Transform the point using matrix multiplication
    cv::Mat transformed_point = matrix * point;
    transformed_point /= transformed_point.at<float>(2, 0);
    transformed_point.convertTo(transformed_point, CV_32F);

    cv::Point2f point_duple(transformed_point.at<float>(0, 0), transformed_point.at<float>(1, 0));
    cv::circle(img_output, point_duple, 8, cv::Scalar(255, 0, 0), -1);
    cv::Mat img_resized;
    cv::resize(img_output, img_resized, cv::Size(600, 750));

    return std::make_pair(img_resized, point_duple);

}

cv::Mat WriteOnVideo(const GD_MTC_CALLBACK_Y16Info* pY16Info,GD_MTC_TempPointInfo pTempPointMax) {

    //// Get the dimensions of the video frames
    //int frame_width = cropped_frame.rows;
    //int frame_height = cropped_frame.cols;

    int frame_width = pY16Info->ImgWidth;
    int frame_height = pY16Info->ImgHeight;

    int x = pTempPointMax.PointX;
    int y = pTempPointMax.PointY;
    cv::Point2i Max_Temp_px_coor (x, y);
    float Max_temp = pTempPointMax.PointTemp;


    cv::Mat frame(288, 384, CV_8UC3, pY16Info->Y16Data);

    std::pair<cv::Mat, cv::Point2f> result = pers_selected_area(frame, Max_Temp_px_coor);

    cv::Mat img_output = result.first;
    cv::Point2f world_coor = result.second;

    // Enter the fixed and totallength
    float total_lengthtocamera = calc_dist_temp(world_coor);

    float fixed_temp = fix_temp(total_lengthtocamera, Max_temp);

    cv::Mat resized_frame;
  
    // Create a black canvas
    cv::Mat black_canvas = cv::Mat::zeros(cv::Size(frame_width + 100, frame_height), CV_8UC3);
    // Put the resized frame on the left side of the canvas

    // Retrieve the elements from the returned pair


    resized_frame.copyTo(black_canvas(cv::Rect(0, 0, resized_frame.cols, resized_frame.rows)));



    // Create text information
    std::string text3 = "Camera Temp = " + std::to_string(Max_temp);
    std::string text6 = "Fixed Temp = " + std::to_string(fixed_temp);
    std::string text9 = "Total Length = " + std::to_string(total_lengthtocamera);

    // Put the text on the right side of the canvas
    
    cv::putText(black_canvas, text3, cv::Point(static_cast<int>(frame_width * 0.38), static_cast<int>(frame_height * 0.60)),
        cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(255, 0, 0), 3);
    cv::putText(black_canvas, text6, cv::Point(static_cast<int>(frame_width * 0.38), static_cast<int>(frame_height * 0.75)),
        cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(0, 0, 255), 3);
    cv::putText(black_canvas, text9, cv::Point(static_cast<int>(frame_width * 0.38), static_cast<int>(frame_height * 0.34)),
        cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(255, 100, 50), 3);



    return black_canvas;
}


int CALLBACK rgbCallbackFunc(const GD_MTC_CALLBACK_RGBInfo* RGBInfo, void* pUser) {
	
	int w = RGBInfo->ImgWidth;
	int h = RGBInfo->ImgHeight;
    globalContentRGB.ImgHeight = h;
    globalContentRGB.ImgWidth = w;
    globalContentRGB.RGBData = RGBInfo->RGBData;

	if (h == 0 || w == 0) {
		//cout << " -----------------------rgbCallbackFunc EMPTY IMAGE	w=	" << RGBInfo->ImgWidth << endl;
		return 0; }
	cv::Mat combinedImage(RGBInfo->ImgHeight, RGBInfo->ImgWidth, CV_8UC3, RGBInfo->RGBData);
    image_source = combinedImage;
	if(combinedImage.empty())
	{
		//cout << " -----------------------rgbCallbackFunc EMPTY IMAGE	w=	" << RGBInfo->ImgWidth << endl;
	}
	else {
		//cout << " --------------------------rgbCallbackFunc	w= " << RGBInfo->ImgWidth << endl;
		cv::imshow("Camera Feed", combinedImage);
		cv::waitKey(1);
	}

	return 1;
}


int CALLBACK y16CallbackFunc(const GD_MTC_CALLBACK_Y16Info* pY16Info, void* pUser)
{
 //   GD_MTC_TempPointInfo pTempPointMax;

	//int w = pY16Info->ImgWidth;
	//int h = pY16Info->ImgHeight;
	//float* pTempMatrix = new float[w * h];

	//if (h == 0 || w == 0) { 
	//	cout << " -----------------------y16CallbackFunc EMPTY Matrix	w=	" << pY16Info->ImgWidth << endl;

	//	delete[] pTempMatrix; // Clean up dynamically allocated memory
	//	return 1; }


	//if (0 == GD_MTC_SDK_GetTempMatrix(pY16Info->pOpque, pTempMatrix)) { 

	//	if (pTempMatrix) {
	//		//cout << " -----------------y16CallbackFunc TEMP MATRIX	pTempMatrix=[" << pTempMatrix[0] << "]" << endl;
	//	}
	//	else {
	//		//cout << " -----------------y16CallbackFunc EMPTY TEMP MATRIX	pTempMatrix=" << pTempMatrix << endl;
	//	}
	//}
	//else {
	//	cout << " -----------------y16CallbackFunc GD_MTC_SDK_GetTempMatrix Error	"  << endl;
	//}


	//if (0 == GD_MTC_SDK_GetHighTemp(pY16Info->pOpque, &pTempPointMax)){

	//	if (pTempMatrix) {
	//		cout << " -----------------y16CallbackFunc TEMP MATRIX	 pTempPointMax->PointTemp " << pTempPointMax.PointTemp << endl;
	//		//logFloatsToMaxTempFile(pTempPointMax);
	//	}
	//	else {
	//		//cout << " -----------------y16CallbackFunc EMPTY TEMP MATRIX	pTempMatrix=" << pTempMatrix << endl;
	//	}
	//}
	//else {
	//	cout << " -----------------y16CallbackFunc GD_MTC_SDK_GetTempMatrix Error	" << endl;
	//}

	//delete[] pTempMatrix;
	return 1;
}





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