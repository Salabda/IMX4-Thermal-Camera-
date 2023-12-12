#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "MeasureCoreNet.h"

struct GlobalMaxPointInfo
{
	int PointX;
	int PointY;
	float PointTemp;
};


struct GlobalRGB
{
	int ImgWidth;
	int ImgHeight;
	unsigned int* RGBData;
	void* pOpque;
	int DataLen;
};


extern GlobalRGB globalContentRGB;
extern GlobalMaxPointInfo globalMaxInfo;

extern float vertical_length_cm;
extern float horizontal_length_cm;

float map_value_to_new_range(float value, float old_min, float old_max, float new_min, float new_max);

float calc_dist_cam_to_hotspot(const cv::Point2f world_coord);
float estimate_temperature(double camera2hotspot_length, double temperature);

std::pair<cv::Mat, cv::Point2f> img_to_world_coor(cv::Mat frame_copy, cv::Point2i thermal_cam_point);

cv::Mat WriteOnVideo(const GD_MTC_CALLBACK_Y16Info* pY16Info, GD_MTC_TempPointInfo pTempPointMax);

int CALLBACK rgbCallbackFunc(const GD_MTC_CALLBACK_RGBInfo* RGBInfo, void* pUser);

int CALLBACK y16CallbackFunc(const GD_MTC_CALLBACK_Y16Info* pY16Info, void* pUser);

