#pragma once
#include <memory>
#include <memory.h>

struct IMAGE_MEASURE_input
{
	IMAGE_MEASURE_input()
	{
		memset(this, 0, sizeof(*this));
	}
	int	image_width;
	int	image_height;

	int emiss;
	int	emiss_type;
	int rel_hum;
	int distance;
	int reflectT;
	int adjust_tamb;
	int switch_tamb;
	int ambient;

	int Shutter_temp;

	int Correction1;
	int Correction2;

	int K_F;
	int Distance_a0;
	int Distance_a1;
	int Distance_a2;
	int Distance_a3;
	int Distance_a4;
	int Distance_a5;

	int K1;
	int K3;
	int K4;
	int B2;

	int JpmTemp;
	int JpmTempShutter;
	int ChangedRtemp;

	int TempFilter;             //测温档位, 0:-20-150, 1:150-800, 两个档位用到的计算Y16校正的模型不同
	int DeltaY16;

	//20160808 by wb
	int FID;
	int VSK;
	int Gain;
	int Int;
	int DetectorFilter;
	int K2;
	int B1;
	int Len;
	int CenterX;

	//20160810 by wb
	int IntCorrection;
	int AppVersion1;
	int AppVersion2;
	int FPGAVersion1;
	int FPGAVersion2;
	int ARMVersion1;
	int ARMVersion2;

	//int reserve[81];

	//20180426 by wb
	int Last_shutter_temp;	//最近一次打快门时快门温度
	int Len_temp;		//实时镜筒温度
	int Last_len_temp;	//最近一次打快门时镜筒温度
	int reserve[78];
};