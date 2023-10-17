#ifndef MEASURECORENET_H
#define MEASURECORENET_H
#include "PlatformType.h"
#include "GD_MTC_SDK_API.h"
#include <memory.h>
#include "GD_MTC_ERROR_CODE.h"
#include "MeasureCoreNet_StructBasic.h"
#include "MeasureCoreNet2_Struct.h"

#if USE_PLATFORM_TYPE_WINDOWS
#define CALLBACK    __stdcall
#elif USE_PLATFORM_TYPE_LINUX
#define CALLBACK
#endif

typedef int int32_t;
typedef unsigned int uint;


// 网络命令字
enum GD_MTC_CommandType
{
	ZoomOne,				// 1倍放大
	ZoomTwo,				// 2倍放大
	ZoomFour,				// 4倍放大
	Palette,				// 调色版
	Shutter,				// 快门补偿
	SetTmpCF,				// 切换下位机温度显示单位
	SDCardReset,			// SD卡格式化
	SAVEJPG,				// SD卡保存JPG图片
	STARTAVI,				// SD卡开始AVI录像
	STOPAVI,				// SD卡停止AVI录像
	SetCurrentDate,			// 设置设备当前日期
	SetCurrentTime,			// 设置设备当前时间
	BackgroundTempSwitch,	// 背景温度是否可调
	ShutterSwitch,			// 快门开关
	Focus,					// 调焦, 0表示停止调焦,1表示远焦, 2表示近焦,3表示自动调焦
};


// #GD_MTC_IRDeviceType 
// see:#SDKversion
// 设备类型
enum GD_MTC_IRDeviceType
{
	OR,
	OR2,
	NET,
	NETHS,
	NET800x600,
	NET384x288,
	NET384x288NOCLIP,
	NET384x288NOCLIPFIXFOCUS,
	ZU02B384x288,
	NET800x600HS,
	NET640x480HS,
	NET384x288NOCLIPHS,
	DEVICE_TYPE_UNKNOWN,
	ORHS,
	ZU07A384x288,
	NET384x288HAOBOHS,
	ZU02C384x288HS,
	ZS02B640x480HS,
	NET640x480HAOBOHS,
	NET800x600E2HS,
	NETE2HS
};

// 画面调光模式
enum GD_MTC_ADJUST_LIGHT_MODE
{
	ADJUST_LIGHT_MODE_BRIGHTNESS_CONTRAST,
	ADJUST_LIGHT_MODE_TEMPERATURE_AUTO,
	ADJUST_LIGHT_MODE_TEMPERATURE_MANUAL
};

enum GD_MTC_ANALYSE_MODE
{
	GD_MTC_ANALYSE_MODE_CLOSED = 0,
	GD_MTC_ANALYSE_MODE_POINT,
	GD_MTC_ANALYSE_MODE_RECT
};

enum GD_MTC_ALIGNMENT_FLAG
{
	GD_MTC_ALIGNMENT_FLAG_ALIGNLEFT,
	GD_MTC_ALIGNMENT_FLAG_ALIGNRIGHT,
	GD_MTC_ALIGNMENT_FLAG_ALIGNTOP,
	GD_MTC_ALIGNMENT_FLAG_ALIGNBOTTOM
};

// 电子变焦倍数
enum GD_MTC_EX
{
	GD_MTC_EX_1,
	GD_MTC_EX_2,
	GD_MTC_EX_4
};

// 电子变焦信息
struct GD_MTC_EX_INFO
{
	GD_MTC_EX nExLevel;
	int nCenterX;
	int nCenterY;
};

struct GD_MTC_RECT_ANALYSER_INFO
{
 	int nAnalyseMode = 0;	 // 0表示关闭;1表示点分析;2表示区域分析
 	int nCursorMode = 0;	 // 0表示最高温;1表示最低温;2表示平均温;3表示最高温和最低温;4表示最高温和平均温;5表示最低温和平均温;6表示最高温和最低温和平均温

	int x = 0;
	int y = 0;
	int nWidth = 0;
	int nHeight = 0;
	int nEnableDigitalTempWithCursor = 0;
	int bEnableCursorInRect = 0;

	GD_MTC_RECT_ANALYSER_INFO()
	{
		nAnalyseMode = 1;
		nCursorMode = 2;
	}
};

struct GD_MTC_ADD_STRING_INFO
{
	int nEnabled;
	int x;
	int y;
	unsigned char strData[128];
	int nStrByteLen;
	GD_MTC_ADD_STRING_INFO()
	{
		nEnabled = 0;
		x = 0;
		y = 0;
		memset(strData, 0, sizeof(strData));
		nStrByteLen = 0;
	}
};

struct GD_MTC_SHOW_PALETTE_INFO
{
	bool bShow;
	GD_MTC_ALIGNMENT_FLAG nAlignment;
	int nSpacePixel;
	int nHeadSpacePixel;
	int nTailSpacePixel;
	int nPaletteWidth;

	GD_MTC_SHOW_PALETTE_INFO()
	{
		//bShow = true;
		bShow = false;
		nAlignment = GD_MTC_ALIGNMENT_FLAG_ALIGNRIGHT;
		nSpacePixel = 5;
		nHeadSpacePixel = -1;
		nTailSpacePixel = -1;
		nPaletteWidth = 10;
	}
};


// 设备参数数据结构
struct GD_MTC_IRDeviceInfo
{
	GD_MTC_IRDeviceInfo()
	{
		memset(this, 0, sizeof(*this));
	}
	char IpAddress[20];             // 设备IP地址，20个字节
	char Ipv6[32];					// 设备IPv6，
	char SubMask[20];				// 子网掩码
	char MacAddress[20];			// 设备Mac地址，20个字节
	char GateWay[16];				// 设备网关，16个字节
	int32_t Port;					// 设备端口
	GD_MTC_IRDeviceType DevType;	// 设备类型
	char DeviceName[40];			// 设备名称
	char DeviceVersion[40];			// 设备版本号
	char UserName[40];				// 用户名,40个字节
	char PassWord[20];				// 密码,20个字节
	int32_t ImgWidth;				// 图像宽
	int32_t ImgHeight;				// 图像高
	int32_t IRorVR;					// 可见光、红外标识，0为红外，1为可见光
	int32_t channelNumber;			// 通道数目
	int32_t GroupNum;				// 所属组号

	int nMotorPosMin;
	int nMotorPosMax;
	
	int nAutoShutterLimitMinuteMin;
	int nAutoShutterLimitMinuteMax;

	float fEmissMin;
	float fEmissMax;

	float fDistanceMin;
	float fDistanceMax;

	int nHumidityMin;
	int nHumidityMax;

	int nAddStringMaxLen;

	int nVskGearMin;
	int nVskGearMax;
	char strVskGearDesc[256];
};


struct GD_MTC_AlamInfo
{
	unsigned char * pData;
	int nDataLen;
};


// RGB视频回调参数数据结构
struct GD_MTC_CALLBACK_RGBInfo
{
	unsigned int * RGBData;		// RGB数据
	int RGBDataLen;				// RGB数据长度
	int ImgWidth;				// 图像宽
	int ImgHeight;				// 图像高
};


// H264视频回调参数数据结构
struct GD_MTC_CALLBACK_H264Info
{
	unsigned int * H264Data;		// H264数据
	int H264DataLen;			// H264数据长度
	int ImgWidth;				// 图像宽
	int ImgHeight;				// 图像高
};


// Y16数据结构
struct GD_MTC_Y16OutputInfo
{
	int nDeviceRealTimeMaxAlarmFlag;		// 机芯温度报警状态
	int nDeviceRealTimeMinAlarmFlag;		// 机芯温度报警状态
	float fTemperatureAdjustLightSpanMin;	// 温度调光低值
	float fTemperatureAdjustLightSpanMax;	// 温度调光高值
	int nFocusing;							// 当前正在调焦标志
	float fCurrentShutterTemp;				// 实时快门温, 单位摄氏度

	float fPreShutterTemp;
	float fArmTemp;
	float fJpmTemp;
	float fLenTemp;

	short Y16_Max;		// 整场Y16最大值
	short Y16_Max_x;	// 整场Y16最大值的X坐标
	short Y16_Max_y;	// 整场Y16最大值的Y坐标
	short Y16_Average;	// 整场Y16平均值
	short Y16_Min;		// 整场Y16最小值
	short Y16_Min_x;	// 整场Y16最小值的X坐标
	short Y16_Min_y;	// 整场Y16最小值的X坐标
	short Y16_Center;	// 中心点Y16值
	short Y16_Center_x;	// 中心点Y16的X坐标
	short Y16_Center_y;	// 中心点Y16的Y坐标
};


// Y16视频回调参数数据结构
struct GD_MTC_CALLBACK_Y16Info
{
	short * Y16Data;	// Y16数据
	int Y16DataLen;		// Y16数据长度
	int ImgWidth;		// 图像宽
	int ImgHeight;		// 图像高
	void * pOpque;
	GD_MTC_Y16OutputInfo y16OutputInfo;
};


// 绘图回调函数数据结构
struct GD_MTC_CALLBACK_DrawFunInfo
{
	int32_t HWnd;	// 窗口句柄
	char * Hdc;		// 句柄指针
	void * PUser;	// 调用者指针
};


// 设备连接状态回调函数数据结构
struct GD_MTC_CALLBACK_LinkStateInfo
{
	char IPAddress[20];		// 设备IP地址
	short LinkState;		// 连接状态，0为成功，<0则失败
	void * PUser;			// 调用者指针
};


// 矩形区域数据结构
struct GD_MTC_RectangleInfo
{
	short X;	// 矩形左上角X坐标
	short Y;	// 矩形左上角Y坐标
	int Width;	// 矩形宽度
	int Height;	// 矩形高度
};

// Y16数据结构
struct GD_MTC_MeasureParamInfo
{
	float fEmiss;
	float fDistance;
	int nHumidity;
	float fBackgroundTemp;
	int nBackgroundTempEnable;
	float fB2;
	int nKf;
	int nVskGear;
};



// 回调函数定义
typedef int (CALLBACK* PROCESS_RGB_CB)(const GD_MTC_CALLBACK_RGBInfo * RGBInfo, void * pUser);
typedef int (CALLBACK* PROCESS_H264_CB)(const GD_MTC_CALLBACK_H264Info * H264Info, void * pUser);
typedef int (CALLBACK* PROCESS_Y16_CB)(const GD_MTC_CALLBACK_Y16Info * Y16Info, void * pUser);
typedef int (CALLBACK* PROCESS_LinkState_CB)(const GD_MTC_CALLBACK_LinkStateInfo * LinkStateInfo, void * pUser);
typedef int (CALLBACK* PROCESS_DrawFun_CB)(const GD_MTC_CALLBACK_DrawFunInfo * DrawInfo, void * pUser);
typedef int (CALLBACK* PROCESS_Alarm_CB)(GD_MTC_AlamInfo * AlarmInfo, void * pUser);
typedef int (CALLBACK* PROCESS_SNAPPICTURE_CB)(int * iY8Data, int * iY8DataLength, int * iY16Data, int * iY16DataLength, void * pUser);
typedef int (CALLBACK* PROCESS_SEARCH_DEVICE_CB)(const GD_MTC_IRDeviceInfo * pDeviceInfo, int nDeviceInfoSize, void * pUser);

typedef int (CALLBACK* PROCESS_EDIT_SNAP_PIC_CB)(unsigned char * pRgbData, int nRgbDataLen, int nRgbWidth, int nRgbHeight, void * pUser);


// 视频连接参数数据结构
struct GD_MTC_IRLinkInfo
{
	GD_MTC_IRLinkInfo()
	{
		memset(this, 0, sizeof(*this));

		// 默认取数据模式设置为Y8数据模式, 只在Y8模式下才支持取一帧数据的功能
		nPullDataType = PULL_DATA_TYPE::PULL_DATA_TYPE_H264_ONLY;
	}
	PULL_DATA_TYPE nPullDataType;
	PROCESS_DrawFun_CB DrawFunCB;		// 绘图回调函数
	PROCESS_RGB_CB RGBCB;				// RGB数据回调函数
	PROCESS_H264_CB H264CB;				// H264数据回调函数
	PROCESS_Y16_CB Y16CB;				// Y16数据回调函数
	PROCESS_LinkState_CB LinkStateCB;	// 连接状态回调函数
	PROCESS_Alarm_CB AlarmCB;			// 报警回调函数
	PROCESS_SNAPPICTURE_CB SnapCB;		// 抓图回调函数
};


extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Init();

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Release();

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Login(GD_MTC_IRDeviceInfo DeviceInfo, int * pUser);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Login2(const char * sDeviceIp, GD_MTC_IRDeviceType DevType = DEVICE_TYPE_UNKNOWN, int * pUser = 0);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Logout(const int iLoginID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_OpenStream(const int iLoginID, GD_MTC_IRLinkInfo * pIRLinkInfo, int iPaletteIndex, int iFps, int * pUser);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_OpenStream2(const int iLoginID, GD_MTC_IRLinkInfo * pIRLinkInfo, int * pUser);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_OpenStream3(const int iLoginID, PULL_DATA_TYPE nPullDataType = PULL_DATA_TYPE_Y16_ONLY, int * pUser = 0);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_CloseStream(const int iSessionID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_StartSearchDevice(PROCESS_SEARCH_DEVICE_CB pSearchDeviceCallBack, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_ChangeDeviceAddress(const int iSessionID, const char * sNewIP, const char * sNetMask, const char * sNewGateWay, const char * sNewDNS, const char * sOriIP, bool bDhcp = false);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_ChangeDeviceAddressWithoutConnection(const char * sNewIP, const char * sNetMask, const char * sNewGateWay, const char * sNewDNS, const char * sOriIP, bool bDhcp = false);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_EnableStreamReconnection(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_DisableStreamReconnection(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setY16FramePerSecond(const int iLoginID, int iFps, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_FocusNear(const int iSessionID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_FocusFar(const int iSessionID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_FocusStop(const int iSessionID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_FocusAuto(const int iSessionID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_FocusFarFine(const int iSessionID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_FocusNearFine(const int iSessionID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setMotorPostition(const int iLoginID, int iMotorPos, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getMotorPostition(const int iLoginID, int * pMotorPos, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetUartCheckAddress(const int iLoginID, int iValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetUartRate(const int iLoginID, int iValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetUartCheckAddress(const int iLoginID, int * pValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetUartRate(const int iLoginID, int * pValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getB2(const int iLoginID, float * pB2, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setB2(const int iLoginID, float fB2, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_saveB2(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_saveCalcParm(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setEmiss(const int iLoginID, float fEmiss, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setBackgroundTemp(const int iLoginID, float fBgTemp, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setDistance(const int iLoginID, float fDistance, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setHumidity(const int iLoginID, int iHumidity, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_setBackgroundTempSwitch(const int iLoginID, int nEnable, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getMeasureParamInfo(const int iLoginID, GD_MTC_MeasureParamInfo * pMeasureParmInfo);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getEmiss(const int iLoginID, float * pEmiss, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getBackgroundTemp(const int iLoginID, float * pBgTemp, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getDistance(const int iLoginID, float * pDistance, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getHumidity(const int iLoginID, int * pHumidity, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_getBackgroundTempSwitch(const int iLoginID, int * pEnable, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetPalette(const int iSessionID, int iPaletteIndex, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SendNetWorkCmd(const int iSessionID, GD_MTC_CommandType cmdType, int iCmdData, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetSDKVersion(const int iLoginID, char * SDKVersion, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetArmVersion(const int iLoginID, char * strArmVersion, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetFpgaVersion(const int iLoginID, char * strFpgaVersion, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetDeviceVersion(const int iLoginID, char * strDeviceVersion, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetDeviceSerialNumber(const int iLoginID, char * serialNum, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetDeviceInfo(const int iLoginID, GD_MTC_IRDeviceInfo * pIRDevice, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetTempMatrix(void * pOpque, float * pTempMatrix);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetPointTemp(void * pOpque, GD_MTC_TempPointInfo * pTempPoint);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetHighTemp(void * pOpque, GD_MTC_TempPointInfo * pTempPoint);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetLowTemp(void * pOpque, GD_MTC_TempPointInfo * pTempPoint);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetAvegTemp(void * pOpque, GD_MTC_TempPointInfo * pTempPoint);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRectHighTemp(void * pOpque, GD_MTC_RectangleInfo RecInfo, GD_MTC_TempPointInfo * pTempPoint);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRectLowTemp(void * pOpque, GD_MTC_RectangleInfo RecInfo, GD_MTC_TempPointInfo * pTempPoint);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRectAvegTemp(void * pOpque, GD_MTC_RectangleInfo RecInfo, GD_MTC_TempPointInfo * pTempPoint);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRectTemperatureInfoE1(void * pOpque, unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, int info[5]);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SnapPictureUniform(const int iSessionID, const char * strFilePath, bool bFullPath, bool bImagData, bool bTempData, PROCESS_EDIT_SNAP_PIC_CB pSnapPicCb = 0, int * pUser = 0);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SnapPictureUniform2(const int iSessionID, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SnapPictureNoCb(const int iSessionID, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SnapPictureNoCbFullPath(const int iSessionID, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SnapPictureWithoutTempNoCb(const int iSessionID, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SnapPictureWithoutImgNoCb(const int iSessionID, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SnapPicture(void * pOpque, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Shutter(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_EnableShutter(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_DisableShutter(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetShutterTime(const int iLoginID, int nMin, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_ShutterClose(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_ShutterOpen(const int iLoginID, int * pUser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRectAnalyserInfo(const int iLoginID, GD_MTC_RECT_ANALYSER_INFO * pRectAnalyser);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_AddRectAnalyser(const int iLoginID, int x, int y, int width, int height, int type);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_DeleteRectAnalyser(const int iLoginID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_ChangeAnalyserTemperatureShowType(const int iLoginID, int type);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_EnableRectAnalyserDigitalTempWithCursor(const int iLoginID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_DisableRectAnalyserDigitalTempWithCursor(const int iLoginID);



extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_AddPlpPoint(const int iLoginID, GD_MTC_PLP_PointInfo * pPoint, int nNum = 1);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_AddPlpLine(const int iLoginID, GD_MTC_PLP_LineInfo * pLine, int nNum = 1);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_AddPlpRect(const int iLoginID, GD_MTC_PLP_RectangleInfo * pRect, int nNum = 1);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_UpdatePlpPoint(const int iLoginID, GD_MTC_PLP_PointInfo * pPoint, int nNum = 1);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_UpdatePlpLine(const int iLoginID, GD_MTC_PLP_LineInfo * pLine, int nNum = 1);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_UpdatePlpRect(const int iLoginID, GD_MTC_PLP_RectangleInfo * pRect, int nNum = 1);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetPlpPoint(const int iLoginID, GD_MTC_PLP_PointInfo * pPoint, int * pNum);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetPlpLine(const int iLoginID, GD_MTC_PLP_LineInfo * pLine, int * pNum);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetPlpRect(const int iLoginID, GD_MTC_PLP_RectangleInfo * pRect, int * pNum);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_DeletePlp(const int iLoginID, int nPlpId);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_DeleteAllPlp(const int iLoginID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SyncPlpToLocal(const int iLoginID, int nFlag);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetDeviceState(const int iSessionID, int * pState, int * pLen);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_EnableAddStringToStream(const int iLoginID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_DisableAddStringToStream(const int iLoginID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_AddStringToStream(const int iLoginID, int x, int y, const void * pStrData, int nStrByteLen, const char * strEncodeName);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_AddStringToStream2(const int iLoginID, int x, int y, const char* content);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetAddStringInfo(const int iLoginID, GD_MTC_ADD_STRING_INFO * pAddStringInfo);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_StartRecord(const int iSessionID, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_StopRecord(const int iSessionID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_StartRecordAvi(const int iSessionID, const char * strFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_StopRecordAvi(const int iSessionID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRgbTempMatrixMaxMinAvgTemp(const int iSessionID, unsigned char * pRgbData, int * pRgbWidth, int * pRgbHeight, float * pTempMatrix, int * pMatrixWidth, int * pMatrixHeight, GD_MTC_TempPointInfo * pTempMax, GD_MTC_TempPointInfo * pTempMin, GD_MTC_TempPointInfo * pTempAvg);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetPointTemperatureFromPicture(const char * filename, int x, int y, float * pointT);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetGlobalTemperatureFromPicture(const char * filename, float * imageT);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRectTemperatureFromPicture(const char * filename, int x, int y, int width, int height, POINT_INFO * maxInfo, POINT_INFO * minInfo, float * avgT);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetRectTemperatureFromPictureE1(const char * filename, unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, int info[5]);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetTempPictureDemension(const char * filename, int * pWidth, int * pHeight);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetTempVideoFristFrameTemp(const char * filename, int * pCenterX, int * pCenterY, float * pTemp);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetGearKf(const int iSessionID, int * pKf);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetGearKfL(const int iLoginID, int * pKf);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetGearKf(const int iSessionID, int nKf);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SaveParam(const int iSessionID);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetVskGearL(const int iLoginID, int * pGear);
extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetVskGear(const int iSessionID, int * pGear);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetVskGear(const int iSessionID, int nGear);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetDeviceAlarmState(const int iSessionID, int * pState);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetDeviceAlarmState(const int iSessionID, int nState);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetDeviceAlarmMax(const int iSessionID, float * pTemp);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetDeviceAlarmMax(const int iSessionID, float fTemp);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetDeviceAlarmMin(const int iSessionID, float * pTemp);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetDeviceAlarmMin(const int iSessionID, float fTemp);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetAdjustLightMode(const int iSessionID, int nMode);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetAdjustLightMode(const int iSessionID, int * nMode);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetIgnoreRatio(const int iSessionID, int nValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetBrightness(const int iSessionID, int nValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetBrightness(const int iSessionID, int * pValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetContrast(const int iSessionID, int nValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetContrast(const int iSessionID, int * pValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetSpanMin(const int iSessionID, float nValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetSpanMin(const int iSessionID, float * pValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetSpanMax(const int iSessionID, float nValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetSpanMax(const int iSessionID, float * pValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetExLevel(const int iSessionID, GD_MTC_EX nExLevel, int nCenterX, int nCenterY);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetExLevel(const int iSessionID, GD_MTC_EX_INFO * pExInfo);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetShowPaletteInfo(const int iSessionID, GD_MTC_SHOW_PALETTE_INFO & showPaletteInfo);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetPaletteDataSize(const int iSessionID, int * pValue);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetPaletteData(const int iSessionID, unsigned char * pPaletteData, int * pDemension0, int * pDemension1, int * pDemension2, char * strDataFormat);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SupportFunction(GD_MTC_IRDeviceType nDeviceType, PULL_DATA_TYPE nPullDataType, int nFunctionType);

extern "C" GD_MTC_SDK_API bool GD_MTC_SDK_GetDeviceType(const char * strIpAddress, GD_MTC_IRDeviceType & nDeviceType);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetCmdPort(const char * strIpAddress, int * pPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetCmdPort(const char * strIpAddress, int nPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetArmPort(const char * strIpAddress, int * pPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetArmPort(const char * strIpAddress, int nPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetFpgaPort(const char * strIpAddress, int * pPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetFpgaPort(const char * strIpAddress, int nPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_GetY16Port(const char * strIpAddress, int * pPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetY16Port(const char * strIpAddress, int nPort);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_SetImageFlip(const int iSessionID, int nFlag);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test();

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test2(void * pUser, PROCESS_RGB_CB cb);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test3();

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test4(const char * strIpAddress);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test5(const char * strIpAddress, const char * strPicFilePath, int iPaletteIndex);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test6(const char * strIpAddress, int iPaletteIndex, int * pLoginId, int * pSessionId);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test7(int nLoginId, int nSessionId);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test8(int nSessionId, const char * strPicFilePath);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_Test_Log(const char * sMsg);

extern "C" GD_MTC_SDK_API int32_t GD_MTC_LogMemInfo();

#endif // MEASURECORENET_H
