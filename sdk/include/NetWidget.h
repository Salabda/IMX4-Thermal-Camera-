#pragma once

//#include <QtWidgets/QWidget>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QDateTime>
#include <QJsonObject>
#include "ui_NetWidget.h"
#include "MeasureCoreNet.h"
//#include "MeasureCoreNetExpert.h"


//class ExpertWidget;

namespace ft
{

	class NetWidget : public QWidget
	{
		Q_OBJECT

	public:
		NetWidget(QWidget *parent = Q_NULLPTR);
		~NetWidget();

		virtual bool eventFilter(QObject *watched, QEvent *event) override;

	signals:
		void showImg(QImage img);
		void showImg(QPixmap pix);
		void showImg(uchar * pData, int nWidth, int nHeight);
		void y16Parm(void * pY16Parm);
		void y16Data(int nAlarmMinFlag, int nAlarmMaxFlag, float fSpanMin, float fSpanMax);
		void searchDeviceResult(GD_MTC_IRDeviceInfo * pDeviceInfo, int nDeviceInfoSize, void *pUser);
		void linkStateChange(int nState);

		public slots:
		void onSearchDevice();
		void onOpenDevice();
		void onCloseDevice();
		void onCloseShutter();
		void onOpenShutter();
		void onShutter();
		void onChangePalette();
		void OnBnClickedButtonFrameRate();
		void OnBnClickedButtonPicCenterTemp();
		void OnBnClickedButtonVideoCenterTemp();

		void OnBnClickedButtonPicTempMatrix();
		void OnBnClickedButtonPicTempMax();

		bool startLink();
		void onBtnTest();

		void OnBnClickedBtnFocusAuto();

		void onBtnPressedNearFocus();
		void onBtnPressedFarFocus();
		void onBtnPressedNearFocusFine();
		void onBtnPressedFarFocusFine();
		void onBtnReleasedNearFocus();
		void onBtnReleasedFarFocus();


		void OnBnClickedButtonH264Only();
		void openStreamDlg(GD_MTC_IRDeviceType nDeviceType, PULL_DATA_TYPE nPullDataType);
		void OnBnClickedButtonY16Only();
		void OnBnClickedButtonH264AndY16();
		void OnBnClickedButtonCloseStream();
		void OnBnClickedButtonSetPalette();
		void OnBnClickedBtnMotorposGet2();
		void OnBnClickedBtnMotorposSet2();

		void OnBnClickedButtonShutter();
		void OnBnClickedEnableShutter();
		void OnBnClickedDisableShutter();
		void OnBnClickedSetShutterTime();

		int getMaxMinAvgType();
		void onBnClickedAddAnalyser();
		void onBnClickedDelAnalyser();
		void onRadioBnClickedMaxMinType();
		void onCheckedDigitalTempWithCursor();

		void onBnClickedAddString();
		void onBnClickedEnableAddString();
		void onBnClickedDisableAddString();

		void onBnClickedGetSerialNumber();

		void onBnClickedChangeIp();
		void onBnClickedChangeIpWithoutConnection();
		void onRadioBtnIpTypeChanged();

		void onBtnClickedY16Record();
		void onBtnClickedAviRecord();
		void onBtnClickedSnap();
		void onBtnClickedSnapNoCb();
		void onBtnClickedSnapWithoutTempNoCb();
		void onBtnClickedSnapWithoutImgNoCb();
		void onBtnClickedSnapUniform();
		void onBtnClickedSetEmiss();
		void onBtnClickedSetDistance();
		void onBtnClickedSetHumidity();
		void onBtnClickedSetBackgroundTemp();
		void onBtnClickedSetBackgroundTempSwitch();
		void onBtnClickedGetEmiss();
		void onBtnClickedGetDistance();
		void onBtnClickedGetHumidity();
		void onBtnClickedGetBackgroundTemp();
		void onBtnClickedGetBackgroundTempSwitch();

		void onBtnClickedGetDeviceInfo();

		void onBtnClickedSetCalcParmB2();
		void onBtnClickedGetCalcParmB2();
		void onBtnClickedSetKf();
		void onBtnClickedGetKf();
		void onBtnClickedGetVskGear();
		void onBtnClickedSetVskGear();

		void onBtnClickedGetUartCheckAddress();
		void onBtnClickedGetUartRate();

		void onBtnClickedSetUartCheckAddress();
		void onBtnClickedSetUartRate();

		void onBtnClickedSaveParmB2();

		void onRaioBtnAdjustLightMode();

		void onBtnClickedGetContrast();
		void onBtnClickedSetContrast();
		void onBtnClickedGetBrightness();
		void onBtnClickedSetBrightness();

		void onBtnClickedGetSpanMin();
		void onBtnClickedSetSpanMin();
		void onBtnClickedGetSpanMax();
		void onBtnClickedSetSpanMax();

		void onCheckedDeviceAlarm(bool bChecked);
		void onBtnClickedGetDeviceAlarmState();
		void onBtnClickedSetDeviceAlarmMin();
		void onBtnClickedSetDeviceAlarmMax();

		void onLinkStateChange(int nState);

		void onChangePaletteShowInfo();
		void onCheckedShowPalette(bool bChecked);
		void onRadioBnClickedPaletteAlignmentChange();

		void onChangeExLevel();
		void onBtnClickedGetExLevelInfo();

		void onBtnClickedGetCmdPort();
		void onBtnClickedSetCmdPort();

		void onBtnTestAddPlp();
		void onBtnGetAllPlpInfo();
		void onBtnDelAllPlpInfo();
		void onBtnSyncPlpToLocal();
		void onBtnSyncPlpToDevice();

		void onCboPlpPoint();
		void onCboPlpLine();
		void onCboPlpRect();

		void onBtnAddPlpPoint();
		void onBtnAddPlpLine();
		void onBtnAddPlpRect();

		void onBtnUpdatePlpPoint();
		void onBtnUpdatePlpLine();
		void onBtnUpdatePlpRect();

		void onBtnDeletePlpPoint();
		void onBtnDeletePlpLine();
		void onBtnDeletePlpRect();

		void updateCurrentPlpCbo();

		void onBtnGetRgbTempMatrix();

		void onBtnGetArmPort();
		void onBtnSetArmPort();

		void onBtnGetFpgaPort();
		void onBtnSetFpgaPort();

		void onBtnGetY16Port();
		void onBtnSetY16Port();

		void onBtnGetDeviceState();


		void onY16Data(int nAlarmMinFlag, int nAlarmMaxFlag, float fSpanMin, float fSpanMax);
		void onShowImg(uchar * pData, int nWidth, int nHeight);
		void onY16Parm(void * pY16Parm2);
		//int y16CbFunObj(const GD_MTC_CALLBACK_Y16Info * pY16Info, void *pUser);
		int onSearchDeviceResult(GD_MTC_IRDeviceInfo * pDeviceInfo, int nDeviceInfoSize, void *pUser);
		int editSnapPicCbObj(unsigned char * pRgbData, int nRgbDataLen, int nRgbWidth, int nRgbHeight, bool bRgb, void * pUser);
		void getPaletteData();

		GD_MTC_IRDeviceType deviceType();
		void disableAllCtrls();
		void updataControlState();

		void tip(const QString & s1);

		bool isValidIp(const QString & s1);
		bool isValidIntAndTip(const QString & s1);
		bool isValidInt(const QString & s1);
		bool isValidFloatNumber(const QString & s1);

		int rgbCbFunS(const GD_MTC_CALLBACK_RGBInfo * RGBInfo, void *pUser);
		int y16CbFunS(const GD_MTC_CALLBACK_Y16Info * pY16Info, void *pUser);
		int linkStateCbFunS(const GD_MTC_CALLBACK_LinkStateInfo *LinkStateInfo, void *pUser);
		int serarchDeviceCbFunS(const GD_MTC_IRDeviceInfo * pDeviceInfo, int nDeviceInfoSize, void *pUser);
	protected:
		QString getIP(QLineEdit * p1, QLineEdit * p2, QLineEdit * p3, QLineEdit * p4);
		bool isValidIp(QLineEdit * p1, QLineEdit * p2, QLineEdit * p3, QLineEdit * p4, QString & strResult);
		QString configFilePath();
		bool readConfigFile();
		bool writeConfigFile();

		void setAddStringDefaultRect();

	private:
		Ui::Test3Class ui;
		QLabel * m_pLblShowPic = 0;
		int m_nAdjustLightMode = ADJUST_LIGHT_MODE_BRIGHTNESS_CONTRAST;

		bool m_bDataStreamIsOpen = false;
		int m_nSessionId;
		int m_nLoginId;
		QString m_strIp;
		bool m_bRecordingY16 = false;
		bool m_bRecordingAvi = false;

		char m_strDeviceVersion[128];

		int m_nPrePicWidth = 0;
		int m_nPrePicHeight = 0;

		int m_nResizeCount = 0;
		GD_MTC_IRDeviceType m_nDeviceType;
		PULL_DATA_TYPE m_nPullDataType;
		GD_MTC_IRDeviceInfo m_deviceInfo;

		unsigned char * m_pPaletteData = 0;
		int m_nPaletteDataLen = 0;
		std::string m_strPaletteFormat;
		bool m_bPaletteDataOk = false;
		int m_nPaletteDemension0 = 0;
		int m_nPaletteDemension1 = 0;
		int m_nPaletteDemension2 = 0;
		int m_nPaletteIndex = 0;

		QJsonObject m_joConfig;
		bool m_bConfigOk = false;

		bool m_bSetSpanInitValue = false;

		QDateTime m_dtPreOpenDevice;

		//NetWidget * g_pTest3 = 0;
		GD_MTC_TempPointInfo g_ptCenter;
		GD_MTC_TempPointInfo g_ptCursor;
		GD_MTC_TempPointInfo g_ptHigh;
		GD_MTC_TempPointInfo g_ptLow;
		GD_MTC_TempPointInfo g_ptAvg;
		GD_MTC_RectangleInfo g_Rect;
		GD_MTC_TempPointInfo g_ptRectHigh;
		GD_MTC_TempPointInfo g_ptRectLow;
		GD_MTC_TempPointInfo g_ptRectAvg;

		short  g_ptCenterAd = 0;
		short  g_ptCursorAd = 0;
		short  g_ptHighAd = 0;
		short  g_ptLowAd = 0;

		std::vector<GD_MTC_PLP_PointInfo> m_vectPlpPointInfo;
		std::vector<GD_MTC_PLP_LineInfo> m_vectPlpLineInfo;
		std::vector<GD_MTC_PLP_RectangleInfo> m_vectPlpRectInfo;

		float g_fCursorTemp = 0.0f;
		bool g_bSnapPicNextFrame = false;

		bool g_bFocusing = false;
		bool g_bInRgbCb = false;
		bool g_bInY16Cb = false;

		int m_nTestAddPlpCount = 0;

		static const int m_nPlpObjSize = 16;
	protected:
		virtual void closeEvent(QCloseEvent *event) override;

		//friend class ExpertWidget;
	};
}