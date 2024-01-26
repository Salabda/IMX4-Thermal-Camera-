#include <iostream>
#include "MeasureCoreNet.h"
#include <vector>
#include <thread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include <chrono>
#include "Display.hpp"
#include "Socket.cpp"
#include <format>
using namespace std;
int ALARM_MAX_TEMP = 70;
const char* strFilePath = "\Output";
int* user = 0;
//set up code ip adress
string ip = "192.168.1.168";

//Values to initialize camera output
int32_t LoginID;
int32_t StreamId;

//Real world coordinates of camera covered FoV
//cv::Point2f lowerright(710, 1545);
//cv::Point2f upperright(1200, 150);
//cv::Point2f lowerleft(300, 1460);
//cv::Point2f upperleft(378, 0);

//Jan Test
cv::Point2f lowerright(-328, 297);
cv::Point2f upperright(-8, 129);
cv::Point2f lowerleft(-272, 490);
cv::Point2f upperleft(74, 447);


int main() {
	// run socket server
	SocketServer server;
	std::thread t([&server]() {
		server.runServer(); // Assuming runServer is a member function of SocketServer
		});

	//Init pointers to start callback functions
	GD_MTC_IRDeviceInfo deviceInfo;
	GD_MTC_IRLinkInfo irLinkInfo;
	GD_MTC_IRDeviceType deviceType;

	memset(&irLinkInfo, 0, sizeof(irLinkInfo));
	irLinkInfo.Y16CB = y16CallbackFunc;
	irLinkInfo.RGBCB = rgbCallbackFunc;
	//irLinkInfo.SnapCB = snapPicCallbackFunc;

	//Initialization
	if (GD_MTC_SDK_Init() != NO_ERROR) {
		cout << "SDK initialization failed." << GD_MTC_SDK_Init() << endl;
		return 1;
	}

	//Set dev.type to the camera we  have
	deviceInfo.DevType = NET384x288NOCLIPHS;
	strcpy_s(deviceInfo.IpAddress, "192.168.1.168");

	//Logging into device
	LoginID = GD_MTC_SDK_Login(deviceInfo, 0);
	if (LoginID == ERROR_LOGIN_FAIL) {
		cout << "Login failed." << endl;
		GD_MTC_SDK_Release();
		return 1;
	}

	//Scannign for device type
	if (GD_MTC_SDK_GetDeviceType("192.168.1.168", deviceType) == false) {
		cout << "Can't find device type." << endl;
		GD_MTC_SDK_Release();
		return 1;
	}
	//sleep to wait for the camera
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	cout << " --------------------------MAIN = 1" << endl;
	//Opening Stream
	irLinkInfo.nPullDataType = PULL_DATA_TYPE_H264_AND_Y16;
	StreamId = GD_MTC_SDK_OpenStream(LoginID, &irLinkInfo, 2, 25, 0);
	if (StreamId == ERROR_OPENSTREAM_FAIL) {
		cout << "Failed to Open Stream." << endl;
		GD_MTC_SDK_CloseStream(StreamId);
		GD_MTC_SDK_Release();
		return 1;
	}
	//sleep to wait for the camera
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	cout << " --------------------------MAIN = 2" << endl;

	//Count frames
	auto start = std::chrono::high_resolution_clock::now();
	int frameCount = 0;

	//Init variables for the loop
	int h = globalContentRGB.ImgHeight;
	int w = globalContentRGB.ImgWidth;
	int l = globalContentRGB.DataLen;
	bool showRedCircle = true;
	auto start2 = std::chrono::high_resolution_clock::now();

	cv::Mat Alert(270, 690, CV_8UC3, cv::Scalar(0, 0, 0)); // CV_8UC3 represents 8-bit unsigned char, 3 channels (RGB)
	cv::Mat NotAlert(270, 690, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat Result;

	//int32_t autofocus = GD_MTC_SDK_FocusAuto(StreamId, user);
	//int32_t supported = GD_MTC_SDK_SupportFunction(NET384x288NOCLIPHS, PULL_DATA_TYPE_H264_AND_Y16, 72);
	//if (supported == 0) {
	//	cout << "Not Supported." << endl;
	//	return 1;
	//}

	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	while (true) {
		// cv::Mat Alert(270, 690, CV_8UC3, cv::Scalar(0, 0, 0));
		//Init variables for the loops
		int x = globalMaxInfo.PointX;
		int y = globalMaxInfo.PointY;
		float hotspot_temp = globalMaxInfo.PointTemp;
		cv::Point2f thermal_cam_coor(x, y);


		if (h == 0 || w == 0) {
			//cout << " -----------------------while EMPTY IMAGE w=	" << globalContentRGB.ImgWidth << endl;

			continue;
		}

		//Initial rgb image
		cv::Mat combinedImage3(h, w, CV_8UC3, globalContentRGB.RGBData);

		//calculate transform matrix and apply it to the image and hotspot's coordinates
		std::pair<cv::Mat, cv::Point2f> result = img_to_world_coor(combinedImage3, thermal_cam_coor);
		// Retrieve the results from the returned pair
		cv::Mat after_transform = result.first; //resized image after transform
		// Rotate the image clockwise by 45 degrees
		double angle = 180;
		cv::Point2f center(after_transform.cols / 2.0, after_transform.rows / 2.0);
		cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
		cv::warpAffine(after_transform, after_transform, rotationMatrix, after_transform.size());

		cv::Point2f transformedPoint = result.second; //Point in world coor
		//get processed temperature
		float total_lengthtocamera = calc_dist_cam_to_hotspot(transformedPoint);	
		float estimated_temp = estimate_temperature(total_lengthtocamera, hotspot_temp);

		std::vector<float> AlarmState = FireAlarm(transformedPoint, hotspot_temp);
		std::string alarmStateString;
		std::cout <<"FireAlarm Temperature " << hotspot_temp << endl;	
		std::cout <<"FireAlarm Distance    " << total_lengthtocamera << endl;
		std::cout <<"FireAlarm Location    " << transformedPoint << endl;
		std::cout << "-------------------------------------------------------" << endl;


		if (AlarmState.size() != 0) {
			if (AlarmState[3] == 1) {
				auto end = std::chrono::high_resolution_clock::now();
				std::cout << "Value of end: " << std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()).count() << " milliseconds since epoch." << std::endl;
				std::cout << "Value of start2: " << std::chrono::duration_cast<std::chrono::milliseconds>(start2.time_since_epoch()).count() << " milliseconds since epoch." << std::endl;
				auto duration = end - start2;

				std::cout << "duration: " << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << endl;
				if (std::chrono::duration_cast<std::chrono::seconds>(duration).count() > 3) {
					alarmStateString = "{\"x\": " + std::to_string(-AlarmState[0] / 100.0) + "," +
						"\"y\": " + std::to_string((AlarmState[1] / 100.0)) + "," +
						"\"temp\": " + std::to_string(hotspot_temp) + "}\n";
					server.broadcast(alarmStateString);


					cv::putText(Alert, "Alarm On", cv::Point2i(20, 135), cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(0, 0, 255));
					cv::Point center_coordinates(475, 130);
					int radius = 100;
					cv::Scalar color(0, 0, 255);  // BGR color for red: (blue=0, green=0, red=255)
					int thickness = -1;  // Use -1 to fill the circle
					// Draw a red circle on the black image


					if (showRedCircle) {
						cv::circle(Alert, center_coordinates, radius, color, thickness);
						showRedCircle = false;
					}
					else {
						cv::Scalar color(255, 255, 255);
						cv::circle(Alert, center_coordinates, radius, cv::Scalar(255, 255, 255), thickness);
						showRedCircle = true;
					}
				}
			}
			else {
				alarmStateString = "{\"x\":0,\"y\":0,\"temp\":0}\n";
				server.broadcast(alarmStateString);
				start2 = std::chrono::high_resolution_clock::now();
				cv::putText(Alert, "Alarm Off", cv::Point2i(20, 135), cv::FONT_HERSHEY_COMPLEX, 2, cv::Scalar(0, 255, 0));
				cv::Point center_coordinates(475, 130);
				int radius = 100;
				cv::Scalar color(0, 255, 0);  // BGR color for red: (blue=0, green=0, red=255)
				int thickness = -1;  // Use -1 to fill the circle
				// Draw a red circle on the black image
				cv::circle(Alert, center_coordinates, radius, color, thickness);



			}

		}


		//Display processed image
		if (!after_transform.empty()) {
			cv::Rect cropRegion(100, 76, 500, 520);  // Example values; adjust as needed
			cv::Mat croppedImage = after_transform(cropRegion).clone();
			cv::imshow("Transformed Output", croppedImage);
			cv::imshow("Unedited Thermal Camera Output", combinedImage3);
			cv::Mat info_image(250, 650, CV_8UC3, cv::Scalar(0, 0, 0)); // CV_8UC3 represents 8-bit unsigned char, 3 channels (RGB)
			cv::putText(info_image, "T cam: " + cv::format("%1.1f", hotspot_temp), cv::Point(10, 70), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(255, 255, 255), 2);
			cv::putText(info_image, "T calib: " + cv::format("%1.1f", estimated_temp), cv::Point(10, 140), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(255, 255, 255), 2);
			cv::putText(info_image, "x: "+ cv::format("%2.2f", -AlarmState[0] / 100.0)+"  "+"y: "+ cv::format("%2.2f", AlarmState[1] / 100.0), cv::Point(10, 210), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(255, 255, 255), 2);
			cv::imshow("Information", info_image);
			
			cv::imshow("Alert", Alert);

			cv::waitKey(1);
		}

		// Calculate time taken for each iteration
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;

		// Increment frame count
		frameCount++;

		// Calculate FPS every second
		if (elapsed.count() >= 1.0) {
			double fps = frameCount / elapsed.count();
			std::cout << "FPS: " << fps << std::endl;
			// Reset timer and frame count for the next second
			start = std::chrono::high_resolution_clock::now();
			frameCount = 0;
		}




		//cout << " --------------------------MAIN = 3" << endl;
		//////Take a Picture
		//int32_t picture = GD_MTC_SDK_SnapPictureUniform(StreamId, strFilePath, false, true, true, 0, user);
		//if (picture != NO_ERROR) {
		//	cout << "Failed to Take a Picture." << endl;
		//	GD_MTC_SDK_CloseStream(StreamId);
		//	GD_MTC_SDK_Release();
		//	return 1;
		//}
		//cout << " --------------------------MAIN = 4" << endl;

	}

	cout << "END = " << endl;
	GD_MTC_SDK_CloseStream(StreamId);
	GD_MTC_SDK_Release();
	return 0;
}
