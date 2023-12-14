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
cv::Point2f lowerright(710, 1545);
cv::Point2f upperright(1200, 150);
cv::Point2f lowerleft(300, 1460);
cv::Point2f upperleft(378, 0);


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

	while (true) {

		//Init variables for the loop
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
		cv::Mat img_resized = result.first; //resized image after transform
		cv::Point2f transformedPoint = result.second; //Point in world coor
		//get processed temperature
		float total_lengthtocamera = calc_dist_cam_to_hotspot(transformedPoint);
		float estimated_temp = estimate_temperature(total_lengthtocamera, hotspot_temp);
		std::vector<float> AlarmState = FireAlarm(transformedPoint, estimated_temp);
		//for (auto value : AlarmState) {
		//	std::cout << value << " ";

		//}
		//std::cout << std::endl;

		//Display processed image
		if (!img_resized.empty()) {
			//cout << "img_resized is not empty." << std::endl;
			cv::imshow("img_resized ", img_resized);
			cv::waitKey(1);
		}

		// Calculate time taken for each iteration
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;

		// Increment frame count
		frameCount++;

		// Calculate FPS every second
		if (elapsed.count() >= 1.0) {
			server.broadcast("fuck");
			double fps = frameCount / elapsed.count();
			std::cout << "FPS: " << fps << std::endl;
			// Reset timer and frame count for the next second
			start = std::chrono::high_resolution_clock::now();
			frameCount = 0;
		}


		//int32_t autofocus = GD_MTC_SDK_FocusAuto(StreamId, user);
		//int32_t supported = GD_MTC_SDK_SupportFunction(NET384x288NOCLIPHS, PULL_DATA_TYPE_H264_AND_Y16, 72);
		//if (supported == 0) {
		//	cout << "Not Supported." << endl;
		//	return 1;
		//}

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
