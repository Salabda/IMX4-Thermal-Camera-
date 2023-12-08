#pragma once
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include "MeasureCoreNet.h"

struct GlobalY16
{
	int ImgWidth;		
	int ImgHeight;		
	void* pOpque;
};
struct GlobalRGB
{
	int ImgWidth;
	int ImgHeight;
	unsigned int* RGBData;
	void* pOpque;
};

extern cv::Mat image_source;

extern GlobalRGB globalContentRGB;
extern GlobalY16 globalContentY16;





float map_value(float value, float old_min, float old_max, float new_min, float new_max);

double calc_dist_temp(cv::Point2f woorld_coortry);

float fix_temp(double camera2hotspot_length, double temperature);

std::pair<cv::Mat, cv::Point2f> pers_selected_area(cv::Mat frame_copy, cv::Point2i thermal_cam_point);

cv::Mat WriteOnVideo(const GD_MTC_CALLBACK_Y16Info* pY16Info, GD_MTC_TempPointInfo pTempPointMax);

int CALLBACK rgbCallbackFunc(const GD_MTC_CALLBACK_RGBInfo* RGBInfo, void* pUser);

int CALLBACK y16CallbackFunc(const GD_MTC_CALLBACK_Y16Info* pY16Info, void* pUser);

