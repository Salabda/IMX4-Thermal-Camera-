#pragma once
#include "GD_MTC_SDK_API.h"
#include<vector>
using namespace std;

#include "MeasureCoreNet2_Struct.h"
// 搜索设备
extern "C" GD_MTC_SDK_API int LiveStream_StartSearchDevice(LIVEVIDEOSEARCHDEVICE_CB pSearchDeviceCallBack, int *pUser);

// 获取设备图像分辨率
extern "C" GD_MTC_SDK_API int LiveStream_GetDeviceInfo(int * width, int * height, const char * strIpAddr = 0);

// 开视频 
//extern "C" GD_MTC_SDK_API int LiveStream_OpenStream(const char * strIpAddr, HWND hWnd, H264STREAMRECV_CB h264StreamCallBack, LIVEVIDEOSTREAMRGBRECV_CB liveVideoStreamRGBCallback, LIVEVIDEOSTREAMSTATERECV_CB liveVideoStreamStateCallBack, LIVEVIDEOMEASURESTATERECV_CB liveVideoMeasureStateCallBack);
extern "C" GD_MTC_SDK_API int LiveStream_OpenStream(const char * strIpAddr, void * hWnd, H264STREAMRECV_CB h264StreamCallBack, LIVEVIDEOSTREAMRGBRECV_CB liveVideoStreamRGBCallback, LIVEVIDEOSTREAMSTATERECV_CB liveVideoStreamStateCallBack, LIVEVIDEOMEASURESTATERECV_CB liveVideoMeasureStateCallBack, PULL_DATA_TYPE nPullDataType = PULL_DATA_TYPE_H264_ONLY, void * pUser = 0);

// 关闭视频
extern "C" GD_MTC_SDK_API int LiveStream_CloseStream(int iSessionID);

// H264码流解码显示
//extern "C" GD_MTC_SDK_API int LiveStream_DecodeH264Stream(LiveStream_H264StreamCallBack * h264Stream, HWND hWnd);
extern "C" GD_MTC_SDK_API int LiveStream_DecodeH264Stream(LiveStream_H264StreamCallBack * h264Stream, void * hWnd, const char * strIpAddr = 0);

// 停止H264解码
extern "C" GD_MTC_SDK_API int LiveStream_StopDecodeH264Stream(const char * strIpAddr = 0);

// 打快门
extern "C" GD_MTC_SDK_API int LiveStream_Shutter(const char * strIpAddr);

// 快门使能开
extern "C" GD_MTC_SDK_API int LiveStream_EnableShutter(const char * strIpAddr);

// 快门使能关
extern "C" GD_MTC_SDK_API int LiveStream_DisableShutter(const char * strIpAddr);

// 设置快门补偿时间
extern "C" GD_MTC_SDK_API int LiveStream_SetShutterTime(const char * strIpAddr, int min);

// 切换色带
extern "C" GD_MTC_SDK_API int LiveStream_Palette(const char * strIpAddr, int index);

// Y16帧率
extern "C" GD_MTC_SDK_API int LiveStream_SetFrameRate(const char * strIpAddr, int nFPS);

// 近焦
extern "C" GD_MTC_SDK_API int LiveStream_NearFocus(const char * strIpAddr);

// 远焦
extern "C" GD_MTC_SDK_API int LiveStream_FarFocus(const char * strIpAddr);

// 自动调焦
extern "C" GD_MTC_SDK_API int LiveStream_AutoFocus(const char * strIpAddr);

// 停止调焦
extern "C" GD_MTC_SDK_API int LiveStream_StopFocus(const char * strIpAddr);

// 修改IP地址
extern "C" GD_MTC_SDK_API int LiveStream_ChangeDeviceAddress(const char * sNewIP, const char * sNetMask, const char * sNewGateWay, const char * sNewDNS, const char * sOriIP);

// 拍照
extern "C" GD_MTC_SDK_API int LiveStream_Slap(const char * filename, int iSessionID);

// 开始录像
extern "C" GD_MTC_SDK_API int LiveStream_ScopeStart(const char * filename, const char * strIpAddr = 0);

// 结束录像
extern "C" GD_MTC_SDK_API int LiveStream_ScopeStop(const char * strIpAddr = 0);

// 获取步进电机的位置
extern "C" GD_MTC_SDK_API int LiveStream_GetSteppingMotorPosition(const char * strIpAddr);

// 设置步进电机的位置
extern "C" GD_MTC_SDK_API int LiveStream_SetSteppingMotorPosition(const char * strIpAddr, int pos);

// 查询固件版本号
extern "C" GD_MTC_SDK_API int LiveStream_QueryVersionInfo(const char * strIpAddr, char * armVersion, char * fpgaVersion);

// 获取图片的宽高
extern "C" GD_MTC_SDK_API int LiveStream_GetTempPictureDemension(const char * filename, int * pWidth, int * pHeight);

// 获取图片的任意点温度
extern "C" GD_MTC_SDK_API int LiveStream_GetPointTemperatureFromPicture(const char * filename, int x, int y, float* pointT);

// 获取图片全图温度矩阵
extern "C" GD_MTC_SDK_API int LiveStream_GetGlobalTemperatureFromPicture(const char * filename, float* imageT);

// 获取图片区域的温度信息
extern "C" GD_MTC_SDK_API int LiveStream_GetRectTemperatureFromPicture(const char * filename, int x, int y, int width, int height, POINT_INFO* maxInfo, POINT_INFO* minInfo, float* avgT);

// 添加分析对象
extern "C" GD_MTC_SDK_API int LiveStream_AddRectAnalyser(const char * strIpAddr, int x, int y, int width, int height, int type);

// 删除分析对象
extern "C" GD_MTC_SDK_API int LiveStream_DeleteRectAnalyser(const char * strIpAddr);

// 切换分析对象温度显示模式
extern "C" GD_MTC_SDK_API int LiveStream_ChangeAnalyserTemperatureShowType(const char * strIpAddr, int type);

// 开启叠加字符功能
extern "C" GD_MTC_SDK_API int LiveStream_EnableAddStringToStream(const char * strIpAddr);

// 图像叠加字符串
extern "C" GD_MTC_SDK_API int LiveStream_AddStringToStream(const char * strIpAddr, int x, int y, const char * content);

//extern "C" GD_MTC_SDK_API int LiveStream_AddStringToStream(const char * strIpAddr, int x, int y, const void * pStrData, int nStrByteLen, const char * strEncodeName);

// 关闭叠加字符功能
extern "C" GD_MTC_SDK_API int LiveStream_DisableAddStringToStream(const char * strIpAddr);

// 获取机器设备号
extern "C" GD_MTC_SDK_API int LiveStream_GetDeviceSerialNumber(const char * strIpAddr, char * serialNum);

// 获取SDK版本信息
extern "C" GD_MTC_SDK_API int LiveStream_GetSDKVersion(char * sdkVersion);

// 获取上次错误
extern "C" GD_MTC_SDK_API int LiveStream_GetLastError();

// 发射率
extern "C" GD_MTC_SDK_API int LiveStream_GetEmiss(const char * strIpAddr, float * pEmiss);
extern "C" GD_MTC_SDK_API int LiveStream_SetEmiss(const char * strIpAddr, float fEmiss);

// 距离
extern "C" GD_MTC_SDK_API int LiveStream_GetDistance(const char * strIpAddr, float * pDistance);
extern "C" GD_MTC_SDK_API int LiveStream_SetDistance(const char * strIpAddr, float fDistance);

// 湿度
extern "C" GD_MTC_SDK_API int LiveStream_GetHumidity(const char * strIpAddr, int * pHumidity);
extern "C" GD_MTC_SDK_API int LiveStream_SetHumidity(const char * strIpAddr, int iHumidity);

// 背景温度
extern "C" GD_MTC_SDK_API int LiveStream_GetBackgroundTemp(const char * strIpAddr, float * pBgTemp);
extern "C" GD_MTC_SDK_API int LiveStream_SetBackgroundTemp(const char * strIpAddr, float fBgTemp);

// 背温开关
extern "C" GD_MTC_SDK_API int LiveStream_GetBackgroundTempSwitch(const char * strIpAddr, int * pEnable);
extern "C" GD_MTC_SDK_API int LiveStream_SetBackgroundTempSwitch(const char * strIpAddr, int nEnable);

// B2
extern "C" GD_MTC_SDK_API int LiveStream_GetB2(const char * strIpAddr, float * pB2);
extern "C" GD_MTC_SDK_API int LiveStream_SetB2(const char * strIpAddr, float fB2);

// 获取与档位相关的KF参数
extern "C" GD_MTC_SDK_API int LiveStream_GetGearKf(const char * strIpAddr, int * pKf);

// 设置与档位相关的KF参数
extern "C" GD_MTC_SDK_API int LiveStream_SetGearKf(const char * strIpAddr, int nKf);

// 档位
extern "C" GD_MTC_SDK_API int LiveStream_GetVskGear(const char * strIpAddr, int * pGear);
extern "C" GD_MTC_SDK_API int LiveStream_SetVskGear(const char * strIpAddr, int nGear);

// 保存参数
extern "C" GD_MTC_SDK_API int LiveStream_SaveParam(const char * strIpAddr);


extern "C" GD_MTC_SDK_API int LiveStream_AddPlpPoint(const char * strIpAddr, GD_MTC_PLP_PointInfo * pPoint, int nNum = 1);

extern "C" GD_MTC_SDK_API int LiveStream_AddPlpLine(const char * strIpAddr, GD_MTC_PLP_LineInfo * pLine, int nNum = 1);

extern "C" GD_MTC_SDK_API int LiveStream_AddPlpRect(const char * strIpAddr, GD_MTC_PLP_RectangleInfo * pRect, int nNum = 1);

extern "C" GD_MTC_SDK_API int LiveStream_UpdatePlpPoint(const char * strIpAddr, GD_MTC_PLP_PointInfo * pPoint, int nNum = 1);

extern "C" GD_MTC_SDK_API int LiveStream_UpdatePlpLine(const char * strIpAddr, GD_MTC_PLP_LineInfo * pLine, int nNum = 1);

extern "C" GD_MTC_SDK_API int LiveStream_UpdatePlpRect(const char * strIpAddr, GD_MTC_PLP_RectangleInfo * pRect, int nNum = 1);

extern "C" GD_MTC_SDK_API int LiveStream_GetPlpPoint(const char * strIpAddr, GD_MTC_PLP_PointInfo * pPoint, int * pNum);

extern "C" GD_MTC_SDK_API int LiveStream_GetPlpLine(const char * strIpAddr, GD_MTC_PLP_LineInfo * pLine, int * pNum);

extern "C" GD_MTC_SDK_API int LiveStream_GetPlpRect(const char * strIpAddr, GD_MTC_PLP_RectangleInfo * pRect, int * pNum);

extern "C" GD_MTC_SDK_API int LiveStream_DeletePlp(const char * strIpAddr, int nPlpId);

extern "C" GD_MTC_SDK_API int LiveStream_DeleteAllPlp(const char * strIpAddr);

extern "C" GD_MTC_SDK_API int LiveStream_SyncPlpToLocal(const char * strIpAddr, int nFlag);

extern "C" GD_MTC_SDK_API int LiveStream_GetDeviceState(const char * strIpAddr, int * pState, int * pLen);

extern "C" GD_MTC_SDK_API int LiveStream_GetRgbTempMatrixMaxMinAvgTemp(const char * strIpAddr, unsigned char * pRgbData, int * pRgbWidth, int * pRgbHeight, float * pTempMatrix, int * pMatrixWidth, int * pMatrixHeight, GD_MTC_TempPointInfo * pTempMax, GD_MTC_TempPointInfo * pTempMin, GD_MTC_TempPointInfo * pTempAvg);

extern "C" GD_MTC_SDK_API int LiveStream_GetArmPort(const char * strIpAddr, int * pPort);

extern "C" GD_MTC_SDK_API int LiveStream_SetArmPort(const char * strIpAddr, int nPort);

extern "C" GD_MTC_SDK_API int LiveStream_GetFpgaPort(const char * strIpAddr, int * pPort);

extern "C" GD_MTC_SDK_API int LiveStream_SetFpgaPort(const char * strIpAddr, int nPort);

extern "C" GD_MTC_SDK_API int LiveStream_GetY16Port(const char * strIpAddr, int * pPort);

extern "C" GD_MTC_SDK_API int LiveStream_SetY16Port(const char * strIpAddr, int nPort);