#pragma once
#include <memory.h>

//#define USING_AMBIENT_AS_REF  0//djj130

class CParamMeasure
{
public:
	typedef unsigned long       DWORD;
	typedef int                 BOOL;
	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;

	CParamMeasure();

	CParamMeasure(const CParamMeasure & obj);

	~CParamMeasure();

	CParamMeasure & operator=(const CParamMeasure & other);

	void copyObj(const CParamMeasure & other);



	//WORD	Version;		//高版本
	//short	Contrast; 		//对比度
	//short	Brightness; 	//亮度

	WORD	ImageWidth;	 	//图像宽度
	WORD	ImageHeight; 	//图像高度
	//WORD	CurveIndex; 	//曲线编号(档位)
	WORD	Emiss; 			//测温对象发射率

	int BackgroundFlag;
	short BackgroundTemp; //背景温度
	short	Ambient; 		//快门传感器温度/
	//int Shutter_temp;
	short 	AtmosphereTemp;	//大气温度
	short 	DetectorTemp;			//探测器温度
	short		signalBoardTemp; //信号板101温传温度
	short		LastCalibratesignalBoardTemp; // fzj add 2018-09-14 17:07:30
	//short 	SettedDetectorTemp;	//大气温度
	short		DetectorGear;				//目前档位
	WORD	RelHum;			//湿度
	WORD	Distance; 		//距离
	//short Correction_B2;
	WORD	WholeImageEmiss;//整个图像的发射率
	WORD	K_F;//用于调整曲线的斜率， 每个探测器的斜率都是不一样的，但是规律是一致的
	//long	Correction_X0; 	//校正系数,在M4，928等机型中都是*1000的结果上传的
	//long	Correction_X1;
	//float	Correction_X0_Float; 	//校正系数,浮点型,转为TP8预留，为原始值，没有乘1000
	//float	Correction_X1_Float;//校正系数,浮点型,转为TP8预留
	unsigned short	CurveLength;	//曲线长度	//原来不知道什么原因由 short 改成 int，后发现改后m4的图片读取不正确，故改回！

	//bool TambAdjust;//是否用反射率校正
	//int  TambAdjustValue;//反射率校正值
	//bool TambAdjust_Manual;			//判断下位机是否打开反射率校正，初始是false，被强行置为true
	//int BodyTempAdjustValue; //体温校正值
	//int AdjustValueManualSet;//体温校正开关
	short StartTemp;//开机温度

	float coefficientK1;
	float coefficientK2;
	float coefficientB0;
	float coefficientK3;
	float coefficientK4;
	float coefficientB2;
	float currentShutterT;
	float LastCalibrateShutterT;						//上次打快门的时候的快门温度

	float Distance_Coeff_a0[4];
	float Distance_Coeff_a1[4];
	float Distance_Coeff_b0[4];
	float Distance_Coeff_b1[4];
	float Distance_Coeff_c0[4];
	float Distance_Coeff_c1[4];

	short*	CurveData;		//曲线数据
	//int RefAreaAD;//参考区域的AD ,需要通过pRefArea计算平均的AD
// 	int  Mode;//测温模式:0,环境温度参考测温；1，参考区域测温模式	
// 	//tp9 专用
// 	//////////////TP9/////////////////////////////////////////////////
// 	//TP9_HEADER m_TP9PublicParam;
// 	int TP9CurveID;
// 	int TP9_LENS;
// 
// 	//bool ReadTP9Object(TP9_HEADER * TP9Param);	//read the content of the head of the TP Image
// 	int TP9_Temp_Min;							//TP的测温范围
// 	int TP9_Temp_Max;
// 	bool TP9UseVL;


	short nDeviceRealTimeAlarmFlag;	// NET640x480机芯实时温度报警状态

};
