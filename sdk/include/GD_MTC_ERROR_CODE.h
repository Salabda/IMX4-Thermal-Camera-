﻿#pragma once
#ifdef NO_ERROR
#undef NO_ERROR
#endif

typedef enum
{
	NO_ERROR = 0,						// 没有错误
	ERROR_GETDEVICEINFO_FAIL = 1,		// 获取设备分辨率失败
	ERROR_OPENSTREAM_FAIL = 2,			// 打开视频流失败
	ERROR_CLOSESTREAM_FAIL = 3,			// 关闭视频流失败
	ERROR_CHANGEIPADDRESS_FAIL = 4,		// 修改IP地址失败
	ERROR_SLAP_FAIL = 5,				// 拍照失败
	ERROR_STARTSCOPE_FAIL = 6,			// 开始录像失败
	ERROR_STOPSCOPE_FAIL = 7,			// 结束录像失败
	ERROR_SHUTTER_FAIL = 8,				// 打快门失败
	ERROR_PALETTE_FAIL = 9,				// 切换色带失败
	ERROR_NEARFOCUS_FAIL = 10,			// 调近焦失败
	ERROR_FARFOCUS_FAIL = 11,			// 调远焦失败
	ERROR_AUTOFOCUS_FAIL = 12,			// 自动调近焦失败
	ERROR_STOPFOCUS_FAIL = 13,			// 停止调焦失败
	ERROR_GETSTEPPINGMOTORPOS_FAIL = 14,	// 获取步进电机位置失败
	ERROR_SETSTEPPINGMOTORPOS_FAIL = 15,	// 设置步进电机位置失败
	ERROR_QUERYVERSIONINFO_FAIL = 16,	// 查询固件版本信息失败
	ERROR_OPENFILE_FAIL = 17,			// 打开文件失败
	ERROR_ADDRECTANALYSER_FAIL = 18,	// 实时视频添加矩形分析对象失败
	ERROR_DELETERECTANALYSER_FAIL = 19,	// 实时视频删除矩形分析对象失败
	ERROR_CHANGEANALYSERTEMPERATURESHOWTYPE_FAIL = 20,	// 切换分析对象的温度显示类型失败
	ERROR_DECODEH264STREAM_FAIL = 21,	// H264码流解码失败
	ERROR_STOPDECODEH264STREAM_FAIL = 22,	// 停止H264码流解码失败
	ERROR_ENABLEADDSTRING_FAIL = 23,	// 开启实时视频叠加字符功能失败
	ERROR_ADDSTRING_FAIL = 24,			// 实时视频叠加字符失败
	ERROR_DISABLEADDSTRING_FAIL = 25,	// 关闭实时视频叠加字符功能失败
	ERROR_ENABLESHUTTER_FAIL = 26,		// 开启自动打快门功能失败
	ERROR_DISABLESHUTTER_FAIL = 27,		// 关闭自动打快门功能失败
	ERROR_SETSHUTTERTIME_FAIL = 28,		// 设置自动尝试时间失败
	ERROR_GETDEVICESERIALNUMBER_FAIL = 29,	// 获取设备序列号失败
	ERROR_GETSDKVERSION_FAIL = 30,		// 获取SDK版本信息失败
	ERROR_LOGIN_FAIL,					
	ERROR_LOGOUT_FAIL,
	ERROR_GETALARMTEMP_FAIL,			// 获取设备报警温度失败
	ERROR_SETALARMTEMP_FAIL,			// 设置设备报警温度失败
	ERROR_CANCLEALARMTEMP_FAIL,			// 取消设备温度报警功能失败	
	ERROR_SETCURRENTDATE_FAIL,			// 设置当前日期失败
	ERROR_SETCURRENTTIME_FAIL,			// 设置当前时间失败
	ERROR_SETFRAMEFREQUENCY_FAIL,		// 设置y16帧频失败
	ERROR_GENERAL_FAIL,
	ERROR_FRAMEWORK_ERROR
}GD_MTC_ERROR_CODE;

#define DEVICE_STATE_ARM_ERROR_NO_CURVE_FILE					0x00000001
#define DEVICE_STATE_ARM_ERROR_Y16_ABNORMAL						0x00000002
#define DEVICE_STATE_ARM_ERROR_NO_MEASURE_PARAM					0x00000004
#define DEVICE_STATE_ARM_ERROR_FONT_MISSING						0x00000008

#define DEVICE_STATE_FPGA_ERROR_INVALID_SENSOR_CONFIG			0x00000010
#define DEVICE_STATE_FPGA_ERROR_LOAD_FLASH_PARAM_FAILED			0x00000020
#define DEVICE_STATE_FPGA_ERROR_DEVICE_WORK_TEMP_OVER_RANGE		0x00000040


