#pragma once

#include "GD_MTC_ERROR_CODE.h"
#include "MeasureCoreNet_StructBasic.h"

enum LiveStream_DeviceState
{
	LiveStream_Y8DataRecvSuccess = 1,
	LiveStream_Y8DataRecvFail = 2,
	LiveStream_Y16DataRecvSuccess = 3,
	LiveStream_Y16DataRecvFail = 4,
};

struct LiveStream_LiveVideoStreamRGBCallBack
{
	unsigned char * pRGBData;
	int nWidth;
	int nHeight;
};

struct LiveStream_H264StreamCallBack
{
	unsigned char * pPackageData;
	int nSize;
};

struct LiveStream_DeviceInfo
{
	LiveStream_DeviceInfo()
	{
		memset(this, 0, sizeof(*this));
	}
	char cIpAddress[20];                    
	char cIpv6[32];                         
	char cSubMask[20];   					
	char cMacAddress[20];
	char cGateWay[16];
	char cDns[20];
};

struct POINT_INFO
{
	float temp;
	int x;
	int y;
};

typedef int(*LIVEVIDEOSTREAMSTATERECV_CB)(const int state, void * pUser);
typedef int(*LIVEVIDEOMEASURESTATERECV_CB)(const int state, void * pUser);
typedef int(*H264STREAMRECV_CB)(const LiveStream_H264StreamCallBack * pInfo, void * pUser);
typedef int(*LIVEVIDEOSTREAMRGBRECV_CB)(const LiveStream_LiveVideoStreamRGBCallBack * pInfo, void * pUser);
typedef int (* LIVEVIDEOSEARCHDEVICE_CB)(const LiveStream_DeviceInfo * pDeviceInfo, int nDeviceInfoSize, void * pUser);
