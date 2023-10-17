#pragma once
#include "MeasureCoreNet.h"

typedef int (CALLBACK* PROCESS_Y16PARM_CB)(void * pFrameData, void *pUser);

// 视频连接参数数据结构
struct GD_MTC_IRLinkInfo_Expert
{
	GD_MTC_IRLinkInfo_Expert()
	{
		memset(this, 0, sizeof(*this));
	}
	PROCESS_Y16PARM_CB Y16ParmCB;//Y16数据回调函数
} ;


extern "C" GD_MTC_SDK_API int32_t GD_MTC_SDK_OpenStream_Expert(const int iLoginID, GD_MTC_IRLinkInfo *pIRLinkInfo, GD_MTC_IRLinkInfo_Expert * pIRLinkInfoExpert, int iPaletteIndex, int iFps, int *pUser);