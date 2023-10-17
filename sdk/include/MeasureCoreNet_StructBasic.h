#pragma once

// 获取数据流模式
enum PULL_DATA_TYPE
{
	PULL_DATA_TYPE_H264_ONLY = 0,
	PULL_DATA_TYPE_Y16_ONLY,
	PULL_DATA_TYPE_H264_AND_Y16
};

// 光标组合定义
enum GD_MTC_CURSOR_MODE
{
	CURSOR_MODE_MAX = 0,
	CURSOR_MODE_MIN,
	CURSOR_MODE_AVG,
	CURSOR_MODE_MAX_MIN,
	CURSOR_MODE_MAX_AVG,
	CURSOR_MODE_MIN_AVG,
	CURSOR_MODE_MAX_MIN_AVG
};

// 温度点数据结构
struct GD_MTC_TempPointInfo
{
	GD_MTC_TempPointInfo()
	{
		PointX = 0;
		PointY = 0;
		PointTemp = 0.0f;
	}

	short PointX;		// 温度点X坐标
	short PointY;		// 温度点Y坐标
	float PointTemp;	// 温度点温度
};

// 点数据结构
struct GD_MTC_PointInfo
{
	short PointX;		// 温度点X坐标
	short PointY;		// 温度点Y坐标
};

// 线数据结构
struct GD_MTC_LineInfo
{
	GD_MTC_PointInfo Point1;
	GD_MTC_PointInfo Point2;
};



struct GD_MTC_PLP_PointInfo
{
	GD_MTC_PLP_PointInfo()
	{
		PointX = 0;
		PointY = 0;
		nHide = 0;
		nCursorType = CURSOR_MODE_MAX;
		nPlpId = 0;
		PointTemp = 0.0f;
	}

	int nPlpId;
	int nHide;
	int nCursorType;

	short PointX;		// 温度点X坐标
	short PointY;		// 温度点Y坐标
	float PointTemp;	// 温度点温度
};

// 线数据结构
struct GD_MTC_PLP_LineInfo
{
	GD_MTC_PLP_LineInfo()
	{
		nHide = 0;
		nCursorType = CURSOR_MODE_MAX;
		nPlpId = 0;
	}

	int nPlpId;
	int nHide;
	int nCursorType;

	GD_MTC_PointInfo PointBegin;
	GD_MTC_PointInfo PointEnd;

	GD_MTC_TempPointInfo PointTempMax;	// 温度点温度
	GD_MTC_TempPointInfo PointTempMin;	// 温度点温度
	GD_MTC_TempPointInfo PointTempAvg;	// 温度点温度
};


// 矩形区域数据结构
struct GD_MTC_PLP_RectangleInfo
{
	GD_MTC_PLP_RectangleInfo()
	{
		nHide = 0;
		nCursorType = CURSOR_MODE_MAX;
		nPlpId = 0;
	}

	int nPlpId;
	int nHide;
	int nCursorType;

	short X;	// 矩形左上角X坐标
	short Y;	// 矩形左上角Y坐标
	int Width;	// 矩形宽度
	int Height;	// 矩形高度

	GD_MTC_TempPointInfo PointTempMax;	// 温度点温度
	GD_MTC_TempPointInfo PointTempMin;	// 温度点温度
	GD_MTC_TempPointInfo PointTempAvg;	// 温度点温度
};
