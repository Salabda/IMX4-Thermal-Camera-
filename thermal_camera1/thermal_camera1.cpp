#include <iostream>
#include "MeasureCoreNet.h"
#include <chrono>
#include <vector>
#include <thread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include "Display.hpp"

using namespace std;


int32_t LoginID;
int32_t StreamId;

const char* strFilePath = "\Output";
int* user = 0;
string ip = "192.168.1.168";


//int CALLBACK rgbCallbackFunc(const GD_MTC_CALLBACK_RGBInfo* RGBInfo, void* pUser) {
//	
//	//int w = RGBInfo->ImgWidth;
//	//int h = RGBInfo->ImgHeight;
//	//if (h == 0 || w == 0) {
//	//	//cout << " -----------------------rgbCallbackFunc EMPTY IMAGE	w=	" << RGBInfo->ImgWidth << endl;
//	//	return 0; }
//	//cv::Mat combinedImage(RGBInfo->ImgHeight, RGBInfo->ImgWidth, CV_8UC3, RGBInfo->RGBData);
//	//if(combinedImage.empty())
//	//{
//	//	//cout << " -----------------------rgbCallbackFunc EMPTY IMAGE	w=	" << RGBInfo->ImgWidth << endl;
//	//}
//	//else {
//	//	//cout << " --------------------------rgbCallbackFunc	w= " << RGBInfo->ImgWidth << endl;
//	//	cv::imshow("Camera Feed", combinedImage);
//	//	cv::waitKey(1);
//	//}
//	//combinedImage.release();
//	return 1;
//}

//
//int RGBvideofeed() {
//	GD_MTC_CALLBACK_RGBInfo rgbInfo;
//	memset(&rgbInfo, 0x00, sizeof(GD_MTC_CALLBACK_RGBInfo));
//	while (true)
//	{
//		//cout << "WHILE LOOP	" << endl;
//		int res = rgbCallbackFunc(&rgbInfo, &user);
//		if (res == 0) {
//			cout << "No ERROR." << res << endl;
//			return 1; 
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	}
//	return 1;
//}
//
//
//int CALLBACK y16CallbackFunc(const GD_MTC_CALLBACK_Y16Info* pY16Info, void* pUser)
//{
//	//GD_MTC_TempPointInfo pTempPointMax;
//
//	//int w = pY16Info->ImgWidth;
//	//int h = pY16Info->ImgHeight;
//	//float* pTempMatrix = new float[w * h];
//
//	//if (h == 0 || w == 0) { 
//	//	cout << " -----------------------y16CallbackFunc EMPTY Matrix	w=	" << pY16Info->ImgWidth << endl;
//
//	//	delete[] pTempMatrix; // Clean up dynamically allocated memory
//	//	return 1; }
//
//
//	//if (0 == GD_MTC_SDK_GetTempMatrix(pY16Info->pOpque, pTempMatrix)) { 
//
//	//	if (pTempMatrix) {
//	//		//cout << " -----------------y16CallbackFunc TEMP MATRIX	pTempMatrix=[" << pTempMatrix[0] << "]" << endl;
//	//	}
//	//	else {
//	//		//cout << " -----------------y16CallbackFunc EMPTY TEMP MATRIX	pTempMatrix=" << pTempMatrix << endl;
//	//	}
//	//}
//	//else {
//	//	cout << " -----------------y16CallbackFunc GD_MTC_SDK_GetTempMatrix Error	"  << endl;
//	//}
//
//
//	//if (0 == GD_MTC_SDK_GetHighTemp(pY16Info->pOpque, &pTempPointMax)){
//
//	//	if (pTempMatrix) {
//	//		cout << " -----------------y16CallbackFunc TEMP MATRIX	 pTempPointMax->PointTemp " << pTempPointMax.PointTemp << endl;
//	//		//logFloatsToMaxTempFile(pTempPointMax);
//	//	}
//	//	else {
//	//		//cout << " -----------------y16CallbackFunc EMPTY TEMP MATRIX	pTempMatrix=" << pTempMatrix << endl;
//	//	}
//	//}
//	//else {
//	//	cout << " -----------------y16CallbackFunc GD_MTC_SDK_GetTempMatrix Error	" << endl;
//	//}
//
//	//delete[] pTempMatrix;
//	return 1;
//}

//
//int Y16feed() {
//
//	while (true)
//	{
//		GD_MTC_CALLBACK_Y16Info Y16Info2;
//		memset(&Y16Info2, 0x00, sizeof(GD_MTC_CALLBACK_Y16Info));
//		y16CallbackFunc(&Y16Info2, 0);
//		std::this_thread::sleep_for(std::chrono::milliseconds(800));
//	}
//	return 1;
//}
//
//int videoandY16() {
//	GD_MTC_CALLBACK_RGBInfo rgbInfo;
//	memset(&rgbInfo, 0x00, sizeof(GD_MTC_CALLBACK_RGBInfo));
//	GD_MTC_CALLBACK_Y16Info Y16Info2;
//	memset(&Y16Info2, 0x00, sizeof(GD_MTC_CALLBACK_Y16Info));
//
//	return 1;
//	
//}

int main() {

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

	deviceInfo.DevType = NET384x288NOCLIPHS;
	strcpy_s(deviceInfo.IpAddress, "192.168.1.168");
	q
	//Logging into device
	LoginID = GD_MTC_SDK_Login(deviceInfo, user);
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
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	cout << " --------------------------MAIN = 1" << endl;
	//Opening Stream
	irLinkInfo.nPullDataType = PULL_DATA_TYPE_H264_AND_Y16;
	StreamId = GD_MTC_SDK_OpenStream(LoginID, &irLinkInfo, 2, 25, user);
	if (StreamId == ERROR_OPENSTREAM_FAIL) {
		cout << "Failed to Open Stream." << endl;
		GD_MTC_SDK_CloseStream(StreamId);
		GD_MTC_SDK_Release();
		return 1;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));


	while (true)
	{


		int w = globalContentRGB.ImgWidth;
		int h = globalContentRGB.ImgHeight;
		cout << " -----------------------while EMPTY IMAGE	h=	" << globalContentRGB.ImgHeight << endl;
		cout << " -----------------------whilec EMPTY IMAGE	w=	" << globalContentRGB.ImgWidth << endl;		
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		cv::imshow("Camera Feed22", image_source);
		cv::waitKey(1);


	} 
	return 1;
	


	cout << " --------------------------MAIN = 2" << endl;

	//thread Y16ProcessingThread(Y16feed);
	//thread videoProcessingThread(RGBvideofeed);


	//videoProcessingThread.join();
	//Y16ProcessingThread.join();

	/*thread videoandY16ProcessingThread(videoandY16);*/

	cout << "END = " << endl;

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

	GD_MTC_SDK_CloseStream(StreamId);
	GD_MTC_SDK_Release();
	return 0;

}

