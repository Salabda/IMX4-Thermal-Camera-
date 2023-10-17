#include "NetWidget.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QImage>
#include <QApplication>
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QPainter>
#include <QFileDialog>
#include <string>
#include <utility>
#include <chrono>
//#include "MeasureCoreNet.h"
#include "DeviceFunctionSupport.h"
//#include "FtAll.h"
#include "ftBasic.h"

#include "Global.h"
#pragma comment(lib, "MeasureCoreNet.lib")

#include "GD_MTC_ERROR_CODE.h"

namespace ft
{

	int CALLBACK rgbCbFun(const GD_MTC_CALLBACK_RGBInfo * RGBInfo, void *pUser)
	{
		NetWidget * pWidget = (NetWidget *)pUser;
		if (pWidget)
		{
			return pWidget->rgbCbFunS(RGBInfo, pUser);
		}


		return 0;
	}

	int CALLBACK y16CbFun(const GD_MTC_CALLBACK_Y16Info * pY16Info, void *pUser)
	{
		NetWidget * pWidget = (NetWidget *)pUser;
		if (pWidget)
		{
			return pWidget->y16CbFunS(pY16Info, pUser);
		}



		return 0;
	}

	int CALLBACK linkStateCbFun(const GD_MTC_CALLBACK_LinkStateInfo *LinkStateInfo, void *pUser)
	{
		NetWidget * pWidget = (NetWidget *)pUser;
		if (pWidget)
		{
			pWidget->linkStateCbFunS(LinkStateInfo, pUser);
		}

		return 0;
	}

	int CALLBACK serarchDeviceCbFun(const GD_MTC_IRDeviceInfo * pDeviceInfo, int nDeviceInfoSize, void *pUser)
	{
		NetWidget * pWidget = (NetWidget *)pUser;

		if (pWidget)
		{
			return pWidget->serarchDeviceCbFunS(pDeviceInfo, nDeviceInfoSize, pUser);
		}

		return 1;
	}

	int CALLBACK editSnapPicCb(unsigned char * pRgbData, int nRgbDataLen, int nRgbWidth, int nRgbHeight, void * pUser)
	{
		NetWidget * pWidget = (NetWidget *)pUser;
		if (pWidget)
		{
			//pWidget->editSnapPicCbObj(pRgbData, nRgbDataLen, nRgbWidth, nRgbHeight, false, pUser);
			pWidget->editSnapPicCbObj(pRgbData, nRgbDataLen, nRgbWidth, nRgbHeight, true, pUser);
		}

		// 	for (int i = 50; i < 100 ; i++)
		// 	{
		// 		for (int j = 50; j < 100; j++)
		// 		{
		// 			pRgbData[(nRgbWidth * i + j) * 3 + 0] = 255;
		// 			pRgbData[(nRgbWidth * i + j) * 3 + 1] = 0;
		// 			pRgbData[(nRgbWidth * i + j) * 3 + 2] = 0;
		// 		}
		// 	}

		return 1;
	}

	NetWidget::NetWidget(QWidget *parent)
		: QWidget(parent)
	{
		ui.setupUi(this);

		m_nSessionId = -1;
		m_nLoginId = -1;

		qRegisterMetaType<QImage>("QImage");
		qRegisterMetaType<QPixmap>("QPixmap");
		qRegisterMetaType<GD_MTC_IRDeviceType>("GD_MTC_IRDeviceType");
		qRegisterMetaType<GD_MTC_IRDeviceInfo>("GD_MTC_IRDeviceInfo");


		connect(this, SIGNAL(showImg(uchar *, int, int)), this, SLOT(onShowImg(uchar *, int, int)), Qt::QueuedConnection);
		connect(this, &NetWidget::y16Parm, this, &NetWidget::onY16Parm, Qt::QueuedConnection);
		connect(this, &NetWidget::y16Data, this, &NetWidget::onY16Data, Qt::QueuedConnection);
		connect(this, &NetWidget::searchDeviceResult, this, &NetWidget::onSearchDeviceResult, Qt::QueuedConnection);
		connect(this, &NetWidget::linkStateChange, this, &NetWidget::onLinkStateChange, Qt::QueuedConnection);
		//connect(this, SIGNAL(searchDeviceResult(GD_MTC_IRDeviceInfo *, int , void *)), this, SLOT(onSearchDeviceResult(GD_MTC_IRDeviceInfo *, int , void *)), Qt::QueuedConnection);



		ui.lineEdit_49->setText("192");
		ui.lineEdit_50->setText("168");
		ui.lineEdit_51->setText("1");
		ui.lineEdit_52->setText("168");

		ui.lineEdit_37->setText("255");
		ui.lineEdit_38->setText("255");
		ui.lineEdit_39->setText("255");
		ui.lineEdit_40->setText("0");

		ui.lineEdit_41->setText("192");
		ui.lineEdit_42->setText("168");
		ui.lineEdit_43->setText("1");
		ui.lineEdit_44->setText("254");

		ui.lineEdit_45->setText("192");
		ui.lineEdit_46->setText("168");
		ui.lineEdit_47->setText("1");
		ui.lineEdit_48->setText("1");


		ui.lineEdit_35->setText("192");
		ui.lineEdit_36->setText("168");
		ui.lineEdit_53->setText("1");
		ui.lineEdit_54->setText("168");

		ui.lineEdit_55->setText("255");
		ui.lineEdit_56->setText("255");
		ui.lineEdit_57->setText("255");
		ui.lineEdit_58->setText("0");

		ui.lineEdit_59->setText("192");
		ui.lineEdit_60->setText("168");
		ui.lineEdit_61->setText("1");
		ui.lineEdit_62->setText("1");

		ui.lineEdit_63->setText("192");
		ui.lineEdit_64->setText("168");
		ui.lineEdit_65->setText("1");
		ui.lineEdit_66->setText("1");

		ui.lineEdit_81->setText("192");
		ui.lineEdit_82->setText("168");
		ui.lineEdit_83->setText("1");
		ui.lineEdit_84->setText("168");


		this->setMouseTracking(true);
		m_pLblShowPic = ui.labelShowPic;

		m_pLblShowPic->installEventFilter(this);
		m_pLblShowPic->setMouseTracking(true);
		m_pLblShowPic->setAlignment(Qt::AlignLeft | Qt::AlignTop);

		for (int i = 0; i < 12; i++)
		{
			ui.comboBox->addItem(QString("%1").arg(i));
		}
		ui.comboBox->setCurrentIndex(0);

		for (int i = 0; i < 25; i++)
		{
			ui.comboBox_4->addItem(QString("%1").arg(i + 1));
		}
		ui.comboBox_4->setCurrentIndex(24);

		{
			QString sVect[7] =
			{
				QString(u8"最高温"),
				QString(u8"最低温"),
				QString(u8"平均温"),
				QString(u8"最高温+最低温"),
				QString(u8"最高温+平均温"),
				QString(u8"最低温+平均温"),
				QString(u8"最高温+最低温+平均温")
			};


			for (int i = 0; i < 7; i++)
			{
				ui.comboBox_6->addItem(QString("%1: %2").arg(i).arg(sVect[i]));
			}

			for (int i = 0; i < 7; i++)
			{
				ui.comboBox_7->addItem(QString("%1: %2").arg(i).arg(sVect[i]));
			}
		}


		ui.scrollAreaWidgetContents->setFixedWidth(272);
		ui.pushButton_10->hide();
		ui.groupBox_17->hide();

		ui.lineEdit_16->setReadOnly(true);
		ui.lineEdit_17->setReadOnly(true);
		ui.lineEdit_22->setReadOnly(true);
		ui.lineEdit_25->setReadOnly(true);

		updataControlState();

		// OR机型, 不支持动态IP
		//ui.radioButton_2->setEnabled(false);

		// 从配置文件中读取IP到当前控件
		{
			if (false == this->readConfigFile())
			{
				this->writeConfigFile();
				this->readConfigFile();
			}
			if (true == m_bConfigOk)
			{
				auto strIp = m_joConfig["ip"].toString();
				auto nMode = m_joConfig["mode"].toString().toInt();

				if (0 == nMode)
				{
					ui.radioButton_8->setChecked(true);
				}
				else if (1 == nMode)
				{
					ui.radioButton_9->setChecked(true);
				}
				else if (2 == nMode)
				{
					ui.radioButton_10->setChecked(true);
				}

				QStringList partList = strIp.split(tr("."));
				if (4 == partList.size())
				{
					ui.lineEdit_49->setText(partList.at(0));
					ui.lineEdit_50->setText(partList.at(1));
					ui.lineEdit_51->setText(partList.at(2));
					ui.lineEdit_52->setText(partList.at(3));
				}
				else
				{
					ft::logError("Ip from cfg file is not 4 parts.");
				}

				int n1 = 0;
			}
		}

		//ui.radioButton_7->setChecked(false);

		this->setAddStringDefaultRect();

		// 	{
		// 		const auto & v = typeid(HWND);
		// 		QString strMsg = QString("%1,%2,%3").arg(v.name()).arg(v.raw_name()).arg(sizeof(HWND));
		// 		this->setWindowTitle(strMsg);
		// 	}
	}

	NetWidget::~NetWidget()
	{
		if (m_pPaletteData)
		{
			delete[] m_pPaletteData;
			m_pPaletteData = 0;
		}
	}

	bool NetWidget::eventFilter(QObject *watched, QEvent *e)
	{
		if (watched == m_pLblShowPic)
		{
			auto nType = e->type();

			if (nType == QEvent::MouseMove)
			{
				auto event = (QMouseEvent *)e;

				if (Qt::NoButton == event->buttons())
				{
					auto x = event->x();
					auto y = event->y();
					g_ptCursor.PointX = x;
					g_ptCursor.PointY = y;

					// 				{
					// 					ft::log(QString("x=%1, y=%2, buttons=%3").arg(x).arg(y).arg(pTrueEvent->buttons()));
					// 				}
				}
			}
		}

		// 发现在Linux平台下ScrollWidget收不到其中包含的禁用状态控件的鼠标滚轮滚动事件, Windows平台下可正常收到. 为在两个平台都可正常滚动此子窗口添加如下处理
		{
			auto nType = e->type();

			if (nType == QEvent::Wheel)
			{
				auto event = (QWheelEvent *)e;

				QWidget * w = (QWidget *)watched;

				if (ui.scrollAreaWidgetContents != w)
				{
					if (ui.scrollAreaWidgetContents->isAncestorOf(w))
					{
						if (false == w->isEnabled())
						{
							QWheelEvent * e2 = new QWheelEvent(*event);
							QCoreApplication::postEvent(ui.scrollAreaWidgetContents, e2);

							e->setAccepted(true);
						}
					}
				}
			}
		}

		return false;
	}

	void NetWidget::closeEvent(QCloseEvent *event)
	{
		this->onCloseDevice();
	}

	void NetWidget::onSearchDevice()
	{
		GD_MTC_SDK_StartSearchDevice(serarchDeviceCbFun, (int *)this);
	}

	void NetWidget::onOpenDevice()
	{
		int n1 = 0;

		if (true == m_dtPreOpenDevice.isValid())
		{
			if (m_dtPreOpenDevice.msecsTo(QDateTime::currentDateTime()) < 3000)
			{
				ft::log("OpenDevice too often");
				return;
			}
		}
		m_dtPreOpenDevice = QDateTime::currentDateTime();


		if (m_bDataStreamIsOpen)
		{
			this->OnBnClickedButtonCloseStream();
		}

		{
			QString strResult;
			if (false == this->isValidIp(ui.lineEdit_49, ui.lineEdit_50, ui.lineEdit_51, ui.lineEdit_52, strResult))
			{
				QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
				return;
			}
		}

		m_strIp = getIP(ui.lineEdit_49, ui.lineEdit_50, ui.lineEdit_51, ui.lineEdit_52);
		if (m_strIp.isEmpty())
		{
			return;
		}


		{
			auto ba = m_strIp.toLatin1();
			GD_MTC_IRDeviceType nDeviceType;
			if (GD_MTC_SDK_GetDeviceType(ba.data(), nDeviceType))
			{
				m_nDeviceType = nDeviceType;
				int n1 = 0;
				if (GD_MTC_SDK_SupportFunction(nDeviceType, PULL_DATA_TYPE_Y16_ONLY, DFS_Y16))
				{
					int n1 = 0;
				}
				else
				{
					int n1 = 0;
				}
				if (GD_MTC_SDK_SupportFunction(nDeviceType, PULL_DATA_TYPE_H264_ONLY, DFS_Y8))
				{
					int n1 = 0;
				}
				else
				{
					int n1 = 0;
				}
				if (GD_MTC_SDK_SupportFunction(nDeviceType, PULL_DATA_TYPE_H264_AND_Y16, DFS_Y8_AND_Y16))
				{
					int n1 = 0;
				}
				else
				{
					int n1 = 0;
				}


				if (ui.radioButton_8->isChecked())
				{
					this->OnBnClickedButtonH264Only();
				}
				else if (ui.radioButton_9->isChecked())
				{
					this->OnBnClickedButtonY16Only();
				}
				else if (ui.radioButton_10->isChecked())
				{
					this->OnBnClickedButtonH264AndY16();
					int n1 = 0;
				}
			}
			else
			{
				QMessageBox::information(this, u8"提示", u8"获取设备类型失败, 可能没有连接设备");
				int n1 = 0;
				ft::logError("GD_MTC_SDK_GetDeviceType return failed.");
			}
		}
	}

	void NetWidget::onCloseDevice()
	{
		int n1 = 0;

		this->OnBnClickedButtonCloseStream();
	}

	void NetWidget::onCloseShutter()
	{
		int n1 = GD_MTC_SDK_ShutterClose(m_nLoginId, 0);
		n1 = n1;
	}

	void NetWidget::onOpenShutter()
	{
		int n1 = GD_MTC_SDK_ShutterOpen(m_nLoginId, 0);
		n1 = n1;
	}

	void NetWidget::onShutter()
	{
		int n1 = 0;
		this->OnBnClickedButtonShutter();
	}

	void NetWidget::onChangePalette()
	{
		int n1 = 0;
		this->OnBnClickedButtonSetPalette();

	}

	void NetWidget::OnBnClickedButtonH264Only()
	{
		auto nDeviceType = deviceType();
		openStreamDlg(nDeviceType, PULL_DATA_TYPE::PULL_DATA_TYPE_H264_ONLY);
	}

	void NetWidget::openStreamDlg(GD_MTC_IRDeviceType nDeviceType, PULL_DATA_TYPE nPullDataType)
	{
		GD_MTC_IRDeviceInfo DeviceInfo;

		auto ba = m_strIp.toLatin1();
		strcpy(DeviceInfo.IpAddress, ba.data());

		//DeviceInfo.DevType = nDeviceType;
		//DeviceInfo.DevType = DEVICE_TYPE_UNKNOWN;
		//DeviceInfo.DevType = m_nDeviceType = NET384x288NOCLIPHS;

		DeviceInfo.DevType = m_nDeviceType;
		//DeviceInfo.DevType = GD_MTC_IRDeviceType::OR2;

		auto nLogId = GD_MTC_SDK_Login(DeviceInfo, 0);
		ft::log(tr("nLogId=%1").arg(nLogId));
		if (ERROR_LOGIN_FAIL != nLogId)
		{
			m_nLoginId = nLogId;

			GD_MTC_IRLinkInfo IRLinkInfo;
			memset(&IRLinkInfo, 0, sizeof(IRLinkInfo));

			IRLinkInfo.RGBCB = rgbCbFun;
			IRLinkInfo.Y16CB = y16CbFun;
			IRLinkInfo.LinkStateCB = linkStateCbFun;

			IRLinkInfo.nPullDataType = nPullDataType;

			int nPaletteIndex = 0;
			int nFps = 12;

			if (m_bConfigOk)
			{
				nPaletteIndex = m_joConfig["palette"].toString().toInt();
				nFps = m_joConfig["fps"].toString().toInt();
			}

			//auto nSessionId = GD_MTC_SDK_OpenStream(nLogId, &IRLinkInfo, 0, nFps, 0);
			auto nSessionId = GD_MTC_SDK_OpenStream(nLogId, &IRLinkInfo, nPaletteIndex, nFps, (int *)this);
			ft::log(tr("nSessionId=%1").arg(nSessionId));
			if (ERROR_OPENSTREAM_FAIL != nSessionId)
			{
				m_nPaletteIndex = nPaletteIndex;

				m_bDataStreamIsOpen = true;
				m_bSetSpanInitValue = false;
				m_nPullDataType = nPullDataType;
				m_nSessionId = nSessionId;


				ui.radioButton->setChecked(true);
				this->onRaioBtnAdjustLightMode();
				ui.comboBox->setCurrentIndex(nPaletteIndex);
				ui.comboBox_4->setCurrentIndex(nFps - 1);

				// Test
				// 			{
				// 				m_nSessionId = nSessionId;
				// 				return;
				// 			}

				GD_MTC_IRDeviceInfo deviceInfo;
				ft::log("GD_MTC_SDK_GetDeviceInfo");
				int nRtn = GD_MTC_SDK_GetDeviceInfo(m_nLoginId, &deviceInfo, 0);
				if (ERROR_GETDEVICEINFO_FAIL != nRtn)
				{
					m_deviceInfo = deviceInfo;

					m_nDeviceType = deviceInfo.DevType;
					this->updataControlState();
					{
						ft::log(QString("deviceInfo.DevType=%1").arg(deviceInfo.DevType));
					}
					this->setWindowTitle(QString::fromStdString(deviceTypeToStr(deviceInfo.DevType)));
					int n1 = 0;
					m_nResizeCount = 2;

					m_joConfig["ip"] = m_strIp;
					m_joConfig["mode"] = QString::number((int)nPullDataType);
					m_joConfig["fps"] = QString::number(nFps);
					m_joConfig["palette"] = QString::number(nPaletteIndex);
					this->writeConfigFile();

					// 设置电子变焦的中心坐标为图像中心点
					{
						ui.lineEdit_75->setText(QString::number(deviceInfo.ImgWidth / 2));
						ui.lineEdit_76->setText(QString::number(deviceInfo.ImgHeight / 2));
					}

					// 获取色带信息
					this->getPaletteData();

					m_nPullDataType = nPullDataType;
					this->updataControlState();
					{
						ft::log(QString("deviceInfo.DevType=%1").arg(m_nDeviceType));
					}
					this->setWindowTitle(QString::fromStdString(deviceTypeToStr(m_nDeviceType)));
					m_nResizeCount = 2;

					//m_nSessionId = nSessionId;
				}
				else
				{
					ft::log("GD_MTC_SDK_GetDeviceInfo failed.");
					int n1 = 0;
					this->tip(QString(u8"获取设备信息失败"));


					this->OnBnClickedButtonCloseStream();
				}
			}
			else
			{
				ft::logError("openStream failed.");

				GD_MTC_SDK_Logout(m_nLoginId);
			}
		}
		else
		{
			ft::logError("Login failed.");
		}
	}

	void NetWidget::OnBnClickedButtonY16Only()
	{
		auto nDeviceType = deviceType();
		openStreamDlg(nDeviceType, PULL_DATA_TYPE::PULL_DATA_TYPE_Y16_ONLY);
	}

	void NetWidget::OnBnClickedButtonH264AndY16()
	{
		auto nDeviceType = deviceType();
		openStreamDlg(nDeviceType, PULL_DATA_TYPE::PULL_DATA_TYPE_H264_AND_Y16);
	}

	void NetWidget::OnBnClickedButtonCloseStream()
	{
		// TODO: 在此添加控件通知处理程序代码

		if (true == m_bRecordingAvi)
		{
			onBtnClickedAviRecord();
		}

		if (true == m_bRecordingY16)
		{
			onBtnClickedY16Record();
		}

		if (-1 != m_nSessionId)
		{
			int n1 = GD_MTC_SDK_CloseStream(m_nSessionId);
			if (n1 != NO_ERROR)
			{
				ft::logError(QString("Call GD_MTC_SDK_CloseStream return %1").arg(n1));
			}
			else
			{
				ft::log(QString("Call GD_MTC_SDK_CloseStream return OK"));
			}
			m_nSessionId = -1;
		}

		if (-1 != m_nLoginId)
		{
			int n1 = GD_MTC_SDK_Logout(m_nLoginId);
			if (n1 != NO_ERROR)
			{
				ft::logError(QString("Call GD_MTC_SDK_Logout return %1").arg(n1));
			}
			else
			{
				ft::log(QString("Call GD_MTC_SDK_Logout return OK"));
			}
			m_nLoginId = -1;
		}

		//GD_MTC_SDK_Release();
		m_bDataStreamIsOpen = false;

		this->updataControlState();

		this->onLinkStateChange(2);

		QThread::msleep(1000);
		GD_MTC_LogMemInfo();
	}


	void NetWidget::OnBnClickedButtonShutter()
	{
		int n1 = GD_MTC_SDK_Shutter(m_nLoginId, 0);
		n1 = n1;
	}


	void NetWidget::OnBnClickedEnableShutter()
	{
		int nRtn = GD_MTC_SDK_EnableShutter(m_nLoginId, 0);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::OnBnClickedDisableShutter()
	{
		int nRtn = GD_MTC_SDK_DisableShutter(m_nLoginId, 0);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::OnBnClickedSetShutterTime()
	{
		QString strValue = ui.lineEdit_15->text();
		if (true == isValidIntAndTip(strValue))
		{
			bool bOk = false;
			int nValue = strValue.toInt(&bOk);
			if (false == bOk)
			{
				this->tip(u8"自动快门时间值无效");
			}

			int nMin = m_deviceInfo.nAutoShutterLimitMinuteMin;
			int nMax = m_deviceInfo.nAutoShutterLimitMinuteMax;
			if (nValue >= nMin && nValue <= nMax)
			{
				int nRtn = GD_MTC_SDK_SetShutterTime(m_nLoginId, nValue, 0);
				if (NO_ERROR == nRtn)
				{
					int n1 = 0;
				}
				else
				{
					int n1 = 0;
					this->tip(u8"设置自动快门时间失败");
				}
			}
			else
			{
				this->tip(QString(u8"自动快门时间间隔必须在[%1, %2]范围内").arg(nMin).arg(nMax));
			}
		}
	}

	int NetWidget::getMaxMinAvgType()
	{
		int t = 0;
		if (ui.radioButton_11->isChecked())
		{
			t = 0;
		}
		if (ui.radioButton_12->isChecked())
		{
			t = 1;
		}
		if (ui.radioButton_16->isChecked())
		{
			t = 2;
		}
		if (ui.radioButton_13->isChecked())
		{
			t = 3;
		}
		if (ui.radioButton_14->isChecked())
		{
			t = 4;
		}
		if (ui.radioButton_15->isChecked())
		{
			t = 5;
		}
		if (ui.radioButton_7->isChecked())
		{
			t = 6;
		}
		return t;
	}

	void NetWidget::onBnClickedAddAnalyser()
	{
		QString strX = ui.lineEdit_18->text();
		QString strY = ui.lineEdit_19->text();
		QString strW = ui.lineEdit_20->text();
		QString strH = ui.lineEdit_21->text();


		int x = strX.toInt();
		int y = strY.toInt();
		int w = strW.toInt();
		int h = strH.toInt();
		int t = getMaxMinAvgType();

		// 验证数据的有效性
		{
			if (false == this->isValidInt(strX))
			{
				this->tip(QString(u8"输入的X值无效"));
				return;
			}
			if (false == this->isValidInt(strY))
			{
				this->tip(QString(u8"输入的Y值无效"));
				return;
			}
			if (false == this->isValidInt(strW))
			{
				this->tip(QString(u8"输入的Width值无效"));
				return;
			}
			if (false == this->isValidInt(strH))
			{
				this->tip(QString(u8"输入的Height值无效"));
				return;
			}


			int nWidth = m_deviceInfo.ImgWidth;
			int nHeight = m_deviceInfo.ImgHeight;

			if (x < 0 || x >= nWidth)
			{
				this->tip(QString(u8"矩形左边框超出[%1, %2)范围").arg(0).arg(nWidth));
				return;
			}
			if (x + w < 0 || x + w >= nWidth)
			{
				this->tip(QString(u8"矩形右边框超出[%1, %2)范围").arg(0).arg(nWidth));
				return;
			}
			if (y < 0 || y >= nHeight)
			{
				this->tip(QString(u8"矩形上边框超出[%1, %2)范围").arg(0).arg(nHeight));
				return;
			}
			if (y + h < 0 || y + h >= nHeight)
			{
				this->tip(QString(u8"矩形下边框超出[%1, %2)范围").arg(0).arg(nHeight));
				return;
			}
		}

		int nRtn = GD_MTC_SDK_AddRectAnalyser(m_nLoginId, x, y, w, h, t);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}

		// 海思平台机芯要求添加矩形对象后再下发一次切换显示类型
		//this->onRadioBnClickedMaxMinType();
	}

	void NetWidget::onRadioBnClickedMaxMinType()
	{
		int t = getMaxMinAvgType();

		int nRtn = GD_MTC_SDK_ChangeAnalyserTemperatureShowType(m_nLoginId, t);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::onCheckedDigitalTempWithCursor()
	{
		int nRtn = 0;
		if (true == ui.checkBox_3->isChecked())
		{
			nRtn = GD_MTC_SDK_EnableRectAnalyserDigitalTempWithCursor(m_nSessionId);
		}
		else
		{
			nRtn = GD_MTC_SDK_DisableRectAnalyserDigitalTempWithCursor(m_nSessionId);
		}

		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBnClickedDelAnalyser()
	{
		this->setAddStringDefaultRect();

		int nRtn = GD_MTC_SDK_DeleteRectAnalyser(m_nLoginId);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBnClickedAddString()
	{
		QString strX = ui.lineEdit_24->text();
		QString strY = ui.lineEdit_23->text();
		QString s = ui.lineEdit_26->text();
		bool bOk = false;
		int x = strX.toInt(&bOk);
		int y = strY.toInt();


		// 验证数据的有效性
		{
			if (false == this->isValidInt(strX))
			{
				this->tip(QString(u8"输入的X值无效"));
				return;
			}
			if (false == this->isValidInt(strY))
			{
				this->tip(QString(u8"输入的Y值无效"));
				return;
			}

			int nWidth = m_deviceInfo.ImgWidth;
			int nHeight = m_deviceInfo.ImgHeight;

			if (x < 0 || x >= nWidth)
			{
				this->tip(QString(u8"X超出[%1, %2)范围").arg(0).arg(nWidth));
				return;
			}
			if (y < 0 || y >= nHeight)
			{
				this->tip(QString(u8"Y超出[%1, %2)范围").arg(0).arg(nHeight));
				return;
			}

			int nLenLimit = m_deviceInfo.nAddStringMaxLen;

			if (s.size() > nLenLimit)
			{
				this->tip(QString(u8"字符长度超过%1个").arg(nLenLimit));
				return;
			}
		}

		//QTextCodec* pCodec = QTextCodec::codecForName("gb2312");
		//if (pCodec)
		{
			//auto ba = pCodec->fromUnicode(s);

			//int nRtn = GD_MTC_SDK_AddStringToStream(m_nLoginId, x, y, ba.data());
			int nRtn = GD_MTC_SDK_AddStringToStream(m_nLoginId, x, y, s.unicode(), s.size() * 2, "utf16");
			if (NO_ERROR == nRtn)
			{
				int n1 = 0;
			}
			else
			{
				int n1 = 0;
				this->tip(QString(u8"添加字符失败, 可能字符过长或超出屏幕区域"));
			}
		}
	}

	void NetWidget::onBnClickedEnableAddString()
	{
		int nRtn = GD_MTC_SDK_EnableAddStringToStream(m_nLoginId);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBnClickedDisableAddString()
	{
		int nRtn = GD_MTC_SDK_DisableAddStringToStream(m_nLoginId);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBnClickedGetSerialNumber()
	{
		char cBuf[200] = {};
		int nRtn = GD_MTC_SDK_GetDeviceSerialNumber(m_nLoginId, cBuf, 0);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
			ui.lineEdit_25->setText(QString(cBuf));
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBnClickedChangeIp()
	{
		int n1 = 0;

		QString strNewIP;
		QString strNetMask;
		QString strNewGateWay;
		QString strNewDNS;
		QString strOriIP;

		strNewIP = "192.168.1.169";
		strNetMask = "255.255.255.0";
		strNewGateWay = "192.168.1.254";
		strNewDNS = "192.168.1.1";
		strOriIP = "192.168.1.168";

		strNewIP = getIP(ui.lineEdit_30, ui.lineEdit_32, ui.lineEdit_31, ui.lineEdit_29);
		strNetMask = getIP(ui.lineEdit_37, ui.lineEdit_38, ui.lineEdit_39, ui.lineEdit_40);
		strNewGateWay = getIP(ui.lineEdit_41, ui.lineEdit_42, ui.lineEdit_43, ui.lineEdit_44);
		strNewDNS = getIP(ui.lineEdit_45, ui.lineEdit_46, ui.lineEdit_47, ui.lineEdit_48);
		strOriIP = getIP(ui.lineEdit_49, ui.lineEdit_50, ui.lineEdit_51, ui.lineEdit_52);


		auto baNewIP = strNewIP.toLatin1();
		auto baNetMask = strNetMask.toLatin1();
		auto baNewGateWay = strNewGateWay.toLatin1();
		auto baNewDNS = strNewDNS.toLatin1();
		auto baOriIP = strOriIP.toLatin1();

		int nRtn = GD_MTC_SDK_ChangeDeviceAddress(m_nSessionId, baNewIP.data(), baNetMask.data(), baNewGateWay.data(), baNewDNS.data(), baOriIP.data());
		if (NO_ERROR == nRtn)
		{
			QMessageBox::information(this, "Tip", "Change IP OK.");
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBnClickedChangeIpWithoutConnection()
	{
		int n1 = 0;

		bool bDhcp = false;
		if (true == ui.radioButton_3->isChecked())
		{
			bDhcp = false;
		}
		else if (true == ui.radioButton_2->isChecked())
		{
			bDhcp = true;
		}

		if (false == bDhcp)
		{
			{
				QString strResult;
				if (false == this->isValidIp(ui.lineEdit_67, ui.lineEdit_68, ui.lineEdit_69, ui.lineEdit_70, strResult))
				{
					QMessageBox::information(this, u8"提示", QString(u8"新IP[%1]格式不正确").arg(strResult));
					return;
				}
				if (false == this->isValidIp(ui.lineEdit_55, ui.lineEdit_56, ui.lineEdit_57, ui.lineEdit_58, strResult))
				{
					QMessageBox::information(this, u8"提示", QString(u8"子网掩码[%1]格式不正确").arg(strResult));
					return;
				}
				if (false == this->isValidIp(ui.lineEdit_59, ui.lineEdit_60, ui.lineEdit_61, ui.lineEdit_62, strResult))
				{
					QMessageBox::information(this, u8"提示", QString(u8"网关[%1]格式不正确").arg(strResult));
					return;
				}
				if (false == this->isValidIp(ui.lineEdit_63, ui.lineEdit_64, ui.lineEdit_65, ui.lineEdit_66, strResult))
				{
					QMessageBox::information(this, u8"提示", QString(u8"DNS[%1]格式不正确").arg(strResult));
					return;
				}
				if (false == this->isValidIp(ui.lineEdit_35, ui.lineEdit_36, ui.lineEdit_53, ui.lineEdit_54, strResult))
				{
					QMessageBox::information(this, u8"提示", QString(u8"原IP[%1]格式不正确").arg(strResult));
					return;
				}
			}

			QString strNewIP;
			QString strNetMask;
			QString strNewGateWay;
			QString strNewDNS;
			QString strOriIP;

			strNewIP = "192.168.1.169";
			strNetMask = "255.255.255.0";
			strNewGateWay = "192.168.1.254";
			strNewDNS = "192.168.1.1";
			strOriIP = "192.168.1.168";

			strNewIP = getIP(ui.lineEdit_67, ui.lineEdit_68, ui.lineEdit_69, ui.lineEdit_70);
			strNetMask = getIP(ui.lineEdit_55, ui.lineEdit_56, ui.lineEdit_57, ui.lineEdit_58);
			strNewGateWay = getIP(ui.lineEdit_59, ui.lineEdit_60, ui.lineEdit_61, ui.lineEdit_62);
			strNewDNS = getIP(ui.lineEdit_63, ui.lineEdit_64, ui.lineEdit_65, ui.lineEdit_66);
			strOriIP = getIP(ui.lineEdit_35, ui.lineEdit_36, ui.lineEdit_53, ui.lineEdit_54);


			QByteArray baNewIP = strNewIP.toLatin1();
			QByteArray baNetMask = strNetMask.toLatin1();
			QByteArray baNewGateWay = strNewGateWay.toLatin1();
			QByteArray baNewDNS = strNewDNS.toLatin1();
			QByteArray baOriIP = strOriIP.toLatin1();

			int nRtn = GD_MTC_SDK_ChangeDeviceAddressWithoutConnection(baNewIP.data(), baNetMask.data(), baNewGateWay.data(), baNewDNS.data(), baOriIP.data(), bDhcp);
			if (NO_ERROR == nRtn)
			{
				QMessageBox::information(this, "Tip", "Change IP OK.");
			}
			else
			{
				int n1 = 0;
				QMessageBox::information(this, "Tip", "Change IP failed.");
			}
		}
		else
		{
			{
				QString strResult;
				if (false == this->isValidIp(ui.lineEdit_35, ui.lineEdit_36, ui.lineEdit_53, ui.lineEdit_54, strResult))
				{
					QMessageBox::information(this, u8"提示", QString(u8"原IP[%1]格式不正确").arg(strResult));
					return;
				}
			}

			QString strNewIP;
			QString strNetMask;
			QString strNewGateWay;
			QString strNewDNS;
			QString strOriIP;

			strOriIP = "192.168.1.168";
			strOriIP = getIP(ui.lineEdit_35, ui.lineEdit_36, ui.lineEdit_53, ui.lineEdit_54);

			QByteArray baNewIP = strNewIP.toLatin1();
			QByteArray baNetMask = strNetMask.toLatin1();
			QByteArray baNewGateWay = strNewGateWay.toLatin1();
			QByteArray baNewDNS = strNewDNS.toLatin1();
			QByteArray baOriIP = strOriIP.toLatin1();

			int nRtn = GD_MTC_SDK_ChangeDeviceAddressWithoutConnection(baNewIP.data(), baNetMask.data(), baNewGateWay.data(), baNewDNS.data(), baOriIP.data(), bDhcp);
			if (NO_ERROR == nRtn)
			{
				QMessageBox::information(this, "Tip", "Change IP OK.");
			}
			else
			{
				int n1 = 0;
				QMessageBox::information(this, "Tip", "Change IP failed.");
			}
		}
	}

	void NetWidget::onRadioBtnIpTypeChanged()
	{
		bool bStaticIp = true;
		if (true == ui.radioButton_3->isChecked())
		{
			bStaticIp = true;
		}
		else if (true == ui.radioButton_2->isChecked())
		{
			bStaticIp = false;
		}

		ui.lineEdit_67->setEnabled(bStaticIp);
		ui.lineEdit_68->setEnabled(bStaticIp);
		ui.lineEdit_69->setEnabled(bStaticIp);
		ui.lineEdit_70->setEnabled(bStaticIp);

		ui.lineEdit_55->setEnabled(bStaticIp);
		ui.lineEdit_56->setEnabled(bStaticIp);
		ui.lineEdit_57->setEnabled(bStaticIp);
		ui.lineEdit_58->setEnabled(bStaticIp);

		ui.lineEdit_59->setEnabled(bStaticIp);
		ui.lineEdit_60->setEnabled(bStaticIp);
		ui.lineEdit_61->setEnabled(bStaticIp);
		ui.lineEdit_62->setEnabled(bStaticIp);

		ui.lineEdit_63->setEnabled(bStaticIp);
		ui.lineEdit_64->setEnabled(bStaticIp);
		ui.lineEdit_65->setEnabled(bStaticIp);
		ui.lineEdit_66->setEnabled(bStaticIp);
	}

	void NetWidget::onBtnClickedY16Record()
	{
		int n1 = 0;

		if (false == m_bRecordingY16)
		{
			ui.pushButton_6->setText(u8"停止Y16录像");
			QString strFilePath = ft::exeDir();
			strFilePath.replace("/", "\\");
			auto ba = strFilePath.toLatin1();
			n1 = GD_MTC_SDK_StartRecord(m_nSessionId, ba.data());

			m_bRecordingY16 = true;
		}
		else
		{
			ui.pushButton_6->setText(u8"Y16录像");
			n1 = GD_MTC_SDK_StopRecord(m_nSessionId);

			m_bRecordingY16 = false;
		}
	}

	void NetWidget::onBtnClickedAviRecord()
	{
		if (false == m_bRecordingAvi)
		{
			ui.pushButton_5->setText(u8"停止avi录像");
			QString strFilePath = QString("%1\\%2.avi").arg(ft::exeDir()).arg(ft::currentDateTimeStringMs());
			strFilePath.replace("/", "\\");
			auto ba = strFilePath.toLatin1();
			GD_MTC_SDK_StartRecordAvi(m_nSessionId, ba.data());

			m_bRecordingAvi = true;
		}
		else
		{
			ui.pushButton_5->setText(u8"avi录像");
			int n1 = GD_MTC_SDK_StopRecordAvi(m_nSessionId);
			if (NO_ERROR != n1)
			{
				ft::logError(QString("GD_MTC_SDK_StopRecordAvi return %1").arg(n1));
			}
			else
			{
				ft::log(QString("GD_MTC_SDK_StopRecordAvi return %1").arg(n1));
			}

			m_bRecordingAvi = false;
		}
	}

	void NetWidget::onBtnClickedSnap()
	{
		int n1 = 0;
		g_bSnapPicNextFrame = true;

	}

	void NetWidget::onBtnClickedSnapNoCb()
	{
		QString strFilePath = ft::exeDir();
		auto ba = strFilePath.toLatin1();
		GD_MTC_SDK_SnapPictureNoCb(m_nSessionId, ba.data());
	}

	void NetWidget::onBtnClickedSnapWithoutTempNoCb()
	{
		QString strFilePath = ft::exeDir();
		auto ba = strFilePath.toLatin1();
		GD_MTC_SDK_SnapPictureWithoutTempNoCb(m_nSessionId, ba.data());
	}

	void NetWidget::onBtnClickedSnapWithoutImgNoCb()
	{
		QString strFilePath = ft::exeDir();
		auto ba = strFilePath.toLatin1();
		GD_MTC_SDK_SnapPictureWithoutImgNoCb(m_nSessionId, ba.data());
	}

	void NetWidget::onBtnClickedSnapUniform()
	{
		//QByteArray baPath = QString("%1/%2.jpg").arg(ft::exeDir()).arg(ft::currentDateTimeStringMs()).toLatin1();
		QByteArray baPath = ft::exeDir().toLatin1();
		//if (NO_ERROR == GD_MTC_SDK_SnapPictureUniform(m_nSessionId, baPath, false, true, true, editSnapPicCb, (int *)this))
		if (NO_ERROR == GD_MTC_SDK_SnapPictureUniform(m_nSessionId, baPath, false, true, true, 0, (int *)0))
		{
			ft::log(QString("call %1 return OK").arg("onBtnClickedSnapUniform"));
		}
		else
		{
			ft::logError(QString("call %1 return fail").arg("onBtnClickedSnapUniform"));
		}
	}

	void NetWidget::onBtnClickedSetEmiss()
	{
		{
			QString strText = ui.lineEdit_6->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		float fEmiss = ui.lineEdit_6->text().toFloat();
		//if (fEmiss >= m_deviceInfo.fEmissMin && fEmiss <= m_deviceInfo.fEmissMax)
		if (fEmiss > m_deviceInfo.fEmissMin && fEmiss < m_deviceInfo.fEmissMax)
		{
			if (NO_ERROR != GD_MTC_SDK_setEmiss(m_nLoginId, fEmiss, 0))
			{
				this->tip(QString(u8"设置发射率失败"));
			}
		}
		else
		{
			this->tip(QString(u8"发射率必须在[%1, %2]范围内").arg(m_deviceInfo.fEmissMin).arg(m_deviceInfo.fEmissMax));
		}
	}

	void NetWidget::onBtnClickedSetDistance()
	{
		{
			QString strText = ui.lineEdit_8->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		float fDistance = ui.lineEdit_8->text().toFloat();
		if (fDistance >= m_deviceInfo.fDistanceMin && fDistance <= m_deviceInfo.fDistanceMax)
		{
			if (NO_ERROR != GD_MTC_SDK_setDistance(m_nLoginId, fDistance, 0))
			{
				this->tip(QString(u8"设置距离失败"));
			}
		}
		else
		{
			this->tip(QString(u8"距离必须在[%1, %2]范围内").arg(m_deviceInfo.fDistanceMin).arg(m_deviceInfo.fDistanceMax));
		}
	}

	void NetWidget::onBtnClickedSetHumidity()
	{
		{
			QString strText = ui.lineEdit_9->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		int nHumidity = ui.lineEdit_9->text().toInt();
		if (nHumidity > m_deviceInfo.nHumidityMin && nHumidity <= m_deviceInfo.nHumidityMax)
		{
			if (NO_ERROR != GD_MTC_SDK_setHumidity(m_nLoginId, nHumidity, 0))
			{
				this->tip(QString(u8"设置湿度失败"));
			}
		}
		else
		{
			this->tip(QString(u8"湿度必须在[%1, %2]范围内").arg(m_deviceInfo.nHumidityMin).arg(m_deviceInfo.nHumidityMax));
		}
	}

	void NetWidget::onBtnClickedSetBackgroundTemp()
	{
		{
			QString strText = ui.lineEdit_10->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		float fBackTemp = ui.lineEdit_10->text().toFloat();
		GD_MTC_SDK_setBackgroundTemp(m_nLoginId, fBackTemp, 0);
	}

	void NetWidget::onBtnClickedSetBackgroundTempSwitch()
	{
		int nSwitch = ui.checkBox->isChecked() ? 1 : 0;
		GD_MTC_SDK_setBackgroundTempSwitch(m_nLoginId, nSwitch, 0);
	}

	void NetWidget::onBtnClickedGetEmiss()
	{
		float nData = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_getEmiss(m_nLoginId, &nData, 0))
		{
			ui.lineEdit_6->setText(QString::number(nData));
		}
		//return;

		// 	{
		// 		GD_MTC_ADD_STRING_INFO measureParmInfo;
		// 		if (NO_ERROR == GD_MTC_SDK_GetAddStringInfo(m_nLoginId, &measureParmInfo))
		// 		{
		// 			QString s;
		//
		// 			if (measureParmInfo.nStrByteLen > 0)
		// 			{
		// 				s = QString((QChar *)measureParmInfo.strData, measureParmInfo.nStrByteLen / 2);
		// 			}
		// 			else
		// 			{
		// 				ft::log(QString("nStrByteLen=%1").arg(measureParmInfo.nStrByteLen));
		// 			}
		//
		// 			ft::log(QString("nEnabled=%1, x=%2, y=%3, s=%4, nStrByteLen=%5")
		// 				.arg(measureParmInfo.nEnabled)
		// 				.arg(measureParmInfo.x)
		// 				.arg(measureParmInfo.y)
		// 				.arg(s)
		// 				.arg(measureParmInfo.nStrByteLen)
		// 			);
		// 			int n1 = 0;
		// 		}
		// 		else
		// 		{
		// 			ft::logError("Call GD_MTC_SDK_GetAddStringInfo failed");
		// 		}
		//
		// 		return;
		// 	}
		//
		// 	{
		// 		GD_MTC_RECT_ANALYSER_INFO measureParmInfo;
		// 		if (NO_ERROR == GD_MTC_SDK_GetRectAnalyserInfo(m_nLoginId, &measureParmInfo))
		// 		{
		// 			ft::log(QString("nAnalyseMode=%1, nCursorMode=%2, x=%3, y=%4, nWidth=%5, nHeight=%6")
		// 				.arg(measureParmInfo.nAnalyseMode)
		// 				.arg(measureParmInfo.nCursorMode)
		// 				.arg(measureParmInfo.x)
		// 				.arg(measureParmInfo.y)
		// 				.arg(measureParmInfo.nWidth)
		// 				.arg(measureParmInfo.nHeight)
		// 			);
		// 			int n1 = 0;
		// 		}
		// 		else
		// 		{
		// 			ft::logError("Call GD_MTC_SDK_GetRectAnalyserInfo failed");
		// 		}
		//
		// 		return;
		// 	}

		GD_MTC_MeasureParamInfo measureParmInfo;
		if (NO_ERROR == GD_MTC_SDK_getMeasureParamInfo(m_nLoginId, &measureParmInfo))
		{
			ft::log(QString("fEmiss=%1, fDistance=%2, fBackgroundTemp=%3, nBackgroundTempEnable=%4, nHumidity=%5, fB2=%6, nKf=%7, nVskGear=%8")
				.arg(measureParmInfo.fEmiss)
				.arg(measureParmInfo.fDistance)
				.arg(measureParmInfo.fBackgroundTemp)
				.arg(measureParmInfo.nBackgroundTempEnable)
				.arg(measureParmInfo.nHumidity)
				.arg(measureParmInfo.fB2)
				.arg(measureParmInfo.nKf)
				.arg(measureParmInfo.nVskGear)

			);
			int n1 = 0;
		}
		else
		{
			ft::logError("Call GD_MTC_SDK_getMeasureParamInfo failed");
		}
	}

	void NetWidget::onBtnClickedGetDistance()
	{
		float nData = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_getDistance(m_nLoginId, &nData, 0))
		{
			ui.lineEdit_8->setText(QString::number(nData));
		}
	}

	void NetWidget::onBtnClickedGetHumidity()
	{
		int nData = 0;
		if (NO_ERROR == GD_MTC_SDK_getHumidity(m_nLoginId, &nData, 0))
		{
			ui.lineEdit_9->setText(QString::number(nData));
		}
	}

	void NetWidget::onBtnClickedGetBackgroundTemp()
	{
		float nData = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_getBackgroundTemp(m_nLoginId, &nData, 0))
		{
			ui.lineEdit_10->setText(QString::number(nData));
		}
	}

	void NetWidget::onBtnClickedGetBackgroundTempSwitch()
	{
		int nData = 0;
		if (NO_ERROR == GD_MTC_SDK_getBackgroundTempSwitch(m_nLoginId, &nData, 0))
		{
			ui.checkBox->setChecked(nData > 0 ? true : false);
		}
	}

	void NetWidget::onBtnClickedGetDeviceInfo()
	{
		GD_MTC_IRDeviceInfo deviceInfo;
		int nRtn = GD_MTC_SDK_GetDeviceInfo(m_nLoginId, &deviceInfo, 0);
		if (ERROR_GETDEVICEINFO_FAIL != nRtn)
		{
			int n1 = 0;
			auto strList = QString(deviceInfo.DeviceVersion).split(" ");
			if (2 == strList.size())
			{
				ui.lineEdit_16->setText(strList[0]);
				ui.lineEdit_17->setText(strList[1]);
			}
			else if (1 == strList.size())
			{
				ui.lineEdit_16->setText(strList[0]);
			}
		}
		else
		{
			int n1 = 0;
		}

		char cBuf[200] = {};
		if (NO_ERROR == GD_MTC_SDK_GetSDKVersion(0, cBuf, 0))
		{
			ui.lineEdit_22->setText(QString(cBuf));
		}
	}

	void NetWidget::onBtnClickedSetCalcParmB2()
	{
		{
			QString strText = ui.lineEdit_7->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		float fB2 = ui.lineEdit_7->text().toFloat();
		if (NO_ERROR == GD_MTC_SDK_setB2(m_nLoginId, fB2, 0))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBtnClickedGetCalcParmB2()
	{
		float fB2 = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_getB2(m_nLoginId, &fB2, 0))
		{
			ui.lineEdit_7->setText(QString::number(fB2));
		}
	}

	void NetWidget::onBtnClickedSetKf()
	{
		{
			QString strText = ui.lineEdit_13->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		int nData = ui.lineEdit_13->text().toInt();
		if (NO_ERROR == GD_MTC_SDK_SetGearKf(m_nSessionId, nData))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBtnClickedGetKf()
	{
		int nData = 0;
		if (NO_ERROR == GD_MTC_SDK_GetGearKf(m_nSessionId, &nData))
		{
			ui.lineEdit_13->setText(QString::number(nData));
		}
	}

	void NetWidget::onBtnClickedGetVskGear()
	{
		int nData = 0;
		if (NO_ERROR == GD_MTC_SDK_GetVskGear(m_nSessionId, &nData))
		{
			ui.lineEdit_71->setText(QString::number(nData));
		}
	}

	void NetWidget::onBtnClickedSetVskGear()
	{
		QString strText = ui.lineEdit_71->text();
		int nData = ui.lineEdit_71->text().toInt();

		if (false == this->isValidFloatNumber(strText))
		{
			this->tip(QString(u8"输入的数值无效"));
			return;
		}

		int nGearMin = m_deviceInfo.nVskGearMin;
		int nGearMax = m_deviceInfo.nVskGearMax;
		if (nData < nGearMin || nData > nGearMax)
		{
			this->tip(QString(u8"输入的档位不在[%1, %2]范围中").arg(nGearMin).arg(nGearMax));
			return;
		}

		if (NO_ERROR == GD_MTC_SDK_SetVskGear(m_nSessionId, nData))
		{
			int n1 = 0;
		}
		else
		{
			this->tip(u8"设置档位失败");
		}
	}

	void NetWidget::onBtnClickedGetUartCheckAddress()
	{
		int nData = 0;
		if (NO_ERROR == GD_MTC_SDK_GetUartCheckAddress(m_nLoginId, &nData))
		{
			int n1 = 0;
			ui.lineEdit_72->setText(QString::number(nData));
		}
		else
		{
			this->tip(u8"获取UART校验地址失败");
		}
	}

	void NetWidget::onBtnClickedGetUartRate()
	{
		int nData = 0;
		if (NO_ERROR == GD_MTC_SDK_GetUartRate(m_nLoginId, &nData))
		{
			int n1 = 0;
			ui.lineEdit_73->setText(QString::number(nData));
		}
		else
		{
			this->tip(u8"获取UART传输速率失败");
		}
	}

	void NetWidget::onBtnClickedSetUartCheckAddress()
	{
		QString strText = ui.lineEdit_72->text();
		int nData = ui.lineEdit_72->text().toInt();

		if (false == this->isValidInt(strText))
		{
			this->tip(QString(u8"输入的数值无效"));
			return;
		}

		if (NO_ERROR == GD_MTC_SDK_SetUartCheckAddress(m_nLoginId, nData))
		{
			int n1 = 0;
		}
		else
		{
			this->tip(u8"设置UART校验地址失败");
		}
	}

	void NetWidget::onBtnClickedSetUartRate()
	{
		QString strText = ui.lineEdit_73->text();
		int nData = ui.lineEdit_73->text().toInt();

		if (false == this->isValidInt(strText))
		{
			this->tip(QString(u8"输入的数值无效"));
			return;
		}

		if (NO_ERROR == GD_MTC_SDK_SetUartRate(m_nLoginId, nData))
		{
			int n1 = 0;
		}
		else
		{
			this->tip(u8"设置UART传输速率失败");
		}
	}

	void NetWidget::onBtnClickedSaveParmB2()
	{
		int res = GD_MTC_SDK_saveB2(m_nLoginId, 0);
		if (NO_ERROR == res)
		{
			if (GD_MTC_IRDeviceType::NETHS == m_nDeviceType ||
				GD_MTC_IRDeviceType::NET800x600HS == m_nDeviceType ||
				GD_MTC_IRDeviceType::NET640x480HS == m_nDeviceType ||
				GD_MTC_IRDeviceType::NET384x288NOCLIPHS == m_nDeviceType ||
				GD_MTC_IRDeviceType::ORHS == m_nDeviceType ||
				GD_MTC_IRDeviceType::ZU07A384x288 == m_nDeviceType ||
				GD_MTC_IRDeviceType::NET384x288HAOBOHS == m_nDeviceType ||
				GD_MTC_IRDeviceType::ZU02C384x288HS == m_nDeviceType ||
				GD_MTC_IRDeviceType::ZS02B640x480HS == m_nDeviceType ||
				GD_MTC_IRDeviceType::NET640x480HAOBOHS == m_nDeviceType ||
				GD_MTC_IRDeviceType::NETHS == m_nDeviceType)
			{
				this->tip(u8"保存参数成功");
			}
			else
			{
				QMessageBox::warning(this, QString(u8"警告"), QString(u8"保存中,2分钟内请不要关机"));
			}
		}
	}

	void NetWidget::onRaioBtnAdjustLightMode()
	{
		int nMode = 0;
		if (true == ui.radioButton->isChecked())
		{
			nMode = ADJUST_LIGHT_MODE_BRIGHTNESS_CONTRAST;
		}
		else if (true == ui.radioButton_4->isChecked())
		{
			nMode = ADJUST_LIGHT_MODE_TEMPERATURE_MANUAL;
		}
		else if (true == ui.radioButton_5->isChecked())
		{
			nMode = ADJUST_LIGHT_MODE_TEMPERATURE_AUTO;
		}

		m_nAdjustLightMode = nMode;

		if (ADJUST_LIGHT_MODE_BRIGHTNESS_CONTRAST == nMode)
		{
			ui.groupBox_10->setEnabled(true);
			ui.groupBox_19->setEnabled(false);
		}
		else if (ADJUST_LIGHT_MODE_TEMPERATURE_MANUAL == nMode)
		{
			ui.groupBox_10->setEnabled(false);
			ui.groupBox_19->setEnabled(true);
		}
		else
		{
			ui.groupBox_10->setEnabled(false);
			ui.groupBox_19->setEnabled(false);
		}

		if (NO_ERROR == GD_MTC_SDK_SetAdjustLightMode(m_nSessionId, nMode))
		{
			int n1 = 0;
			if (ADJUST_LIGHT_MODE_TEMPERATURE_MANUAL == nMode)
			{
				this->onBtnClickedSetSpanMax();
			}
		}
	}

	void NetWidget::onBtnClickedGetContrast()
	{
		int nValue = 0;
		if (NO_ERROR == GD_MTC_SDK_GetContrast(m_nSessionId, &nValue))
		{
			ui.lineEdit_12->setText(QString::number(nValue));
		}
	}

	void NetWidget::onBtnClickedSetContrast()
	{
		int nValue = ui.lineEdit_12->text().toInt();
		auto strX = ui.lineEdit_12->text();
		// 验证数据的有效性
		{
			if (false == this->isValidInt(strX))
			{
				this->tip(QString(u8"输入的对比度无效"));
				return;
			}

			int nLow = 1;
			int nHigh = 100;

			auto x = nValue;

			if (x < nLow || x > nHigh)
			{
				this->tip(QString(u8"对比度超出[%1, %2]范围").arg(nLow).arg(nHigh));
				return;
			}
		}
		if (NO_ERROR == GD_MTC_SDK_SetContrast(m_nSessionId, nValue))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBtnClickedGetBrightness()
	{
		int nValue = 0;
		if (NO_ERROR == GD_MTC_SDK_GetBrightness(m_nSessionId, &nValue))
		{
			ui.lineEdit_11->setText(QString::number(nValue));
		}
	}

	void NetWidget::onBtnClickedSetBrightness()
	{
		int nValue = ui.lineEdit_11->text().toInt();
		auto strX = ui.lineEdit_11->text();
		// 验证数据的有效性
		{
			if (false == this->isValidInt(strX))
			{
				this->tip(QString(u8"输入的亮度值无效"));
				return;
			}

			int nLow = 1;
			int nHigh = 100;

			auto x = nValue;

			if (x < nLow || x > nHigh)
			{
				this->tip(QString(u8"亮度值超出[%1, %2]范围").arg(nLow).arg(nHigh));
				return;
			}
		}
		if (NO_ERROR == GD_MTC_SDK_SetBrightness(m_nSessionId, nValue))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBtnClickedGetSpanMin()
	{
		float f = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_GetSpanMin(m_nSessionId, &f))
		{
			ui.lineEdit_27->setText(QString::number(f));
		}
	}

	void NetWidget::onBtnClickedSetSpanMin()
	{

	}

	void NetWidget::onBtnClickedGetSpanMax()
	{
		float f = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_GetSpanMax(m_nSessionId, &f))
		{
			ui.lineEdit_28->setText(QString::number(f));
		}

		this->onBtnClickedGetSpanMin();
	}

	void NetWidget::onBtnClickedSetSpanMax()
	{
		float fMin = 0.0f;
		float fMax = 0.0f;
		{
			{
				QString strText = ui.lineEdit_28->text();
				if (false == this->isValidFloatNumber(strText))
				{
					this->tip(QString(u8"输入的数值无效"));
					return;
				}
			}
			auto nValue = ui.lineEdit_28->text().toFloat();
			fMax = nValue;

		}

		{
			{
				QString strText = ui.lineEdit_27->text();
				if (false == this->isValidFloatNumber(strText))
				{
					this->tip(QString(u8"输入的数值无效"));
					return;
				}
			}
			auto nValue = ui.lineEdit_27->text().toFloat();
			fMin = nValue;
		}

		if (fMax < fMin)
		{
			this->tip(QString(u8"高值不能比低值小"));
			return;
		}

		if (NO_ERROR == GD_MTC_SDK_SetSpanMax(m_nSessionId, fMax))
		{
			int n1 = 0;
		}
		if (NO_ERROR == GD_MTC_SDK_SetSpanMin(m_nSessionId, fMin))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onCheckedDeviceAlarm(bool bChecked)
	{
		int nState = 0;
		if (false == ui.checkBox_4->isChecked() && false == ui.checkBox_5->isChecked())
		{
			nState = 0;
		}
		else if (true == ui.checkBox_4->isChecked() && false == ui.checkBox_5->isChecked())
		{
			nState = 1;
		}
		else if (false == ui.checkBox_4->isChecked() && true == ui.checkBox_5->isChecked())
		{
			nState = 2;
		}
		else if (true == ui.checkBox_4->isChecked() && true == ui.checkBox_5->isChecked())
		{
			nState = 3;
		}
		if (NO_ERROR == GD_MTC_SDK_SetDeviceAlarmState(m_nSessionId, nState))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBtnClickedGetDeviceAlarmState()
	{
		int nState = 0;
		if (NO_ERROR == GD_MTC_SDK_GetDeviceAlarmState(m_nSessionId, &nState))
		{
			if (0 == nState)
			{
				ui.checkBox_4->setChecked(false);
				ui.checkBox_5->setChecked(false);
			}
			else if (1 == nState)
			{
				ui.checkBox_4->setChecked(true);
				ui.checkBox_5->setChecked(false);
			}
			else if (2 == nState)
			{
				ui.checkBox_4->setChecked(false);
				ui.checkBox_5->setChecked(true);
			}
			else if (3 == nState)
			{
				ui.checkBox_4->setChecked(true);
				ui.checkBox_5->setChecked(true);
			}
		}

		float fMax = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_GetDeviceAlarmMax(m_nSessionId, &fMax))
		{
			ui.lineEdit_33->setText(QString::number(fMax));
		}

		float fMin = 0.0f;
		if (NO_ERROR == GD_MTC_SDK_GetDeviceAlarmMin(m_nSessionId, &fMin))
		{
			ui.lineEdit_34->setText(QString::number(fMin));
		}
	}

	void NetWidget::onBtnClickedSetDeviceAlarmMin()
	{
		{
			QString strText = ui.lineEdit_34->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		auto fValue = ui.lineEdit_34->text().toFloat();
		if (NO_ERROR == GD_MTC_SDK_SetDeviceAlarmMin(m_nSessionId, fValue))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onBtnClickedSetDeviceAlarmMax()
	{
		{
			QString strText = ui.lineEdit_33->text();
			if (false == this->isValidFloatNumber(strText))
			{
				this->tip(QString(u8"输入的数值无效"));
				return;
			}
		}
		auto fValue = ui.lineEdit_33->text().toFloat();
		if (NO_ERROR == GD_MTC_SDK_SetDeviceAlarmMax(m_nSessionId, fValue))
		{
			int n1 = 0;
		}
	}

	void NetWidget::onLinkStateChange(int nState)
	{
		if (1 == nState)
		{
			ui.label_54->setText(u8"已连接");
		}
		else if (2 == nState)
		{
			ui.label_54->setText(u8"已关闭");
		}
		else
		{
			ui.label_54->setText(u8"正在重连...");
		}
	}

	void NetWidget::onChangePaletteShowInfo()
	{
		bool bChecked = ui.checkBox_2->isChecked();

		GD_MTC_SHOW_PALETTE_INFO showPaletteInfo;

		showPaletteInfo.bShow = bChecked;

		if (false == bChecked)
		{
			ui.radioButton_19->setEnabled(false);
			ui.radioButton_18->setEnabled(false);
		}
		else
		{
			ui.radioButton_19->setEnabled(true);
			ui.radioButton_18->setEnabled(true);
		}

		if (ui.radioButton_19->isChecked())
		{
			showPaletteInfo.nAlignment = GD_MTC_ALIGNMENT_FLAG_ALIGNLEFT;
		}
		if (ui.radioButton_18->isChecked())
		{
			showPaletteInfo.nAlignment = GD_MTC_ALIGNMENT_FLAG_ALIGNRIGHT;
		}

		if (NO_ERROR == GD_MTC_SDK_SetShowPaletteInfo(m_nSessionId, showPaletteInfo))
		{
			int n1 = 0;
			ft::log(QString("showPaletteInfo.bShow=%1, showPaletteInfo.nAlignment=%2").arg(showPaletteInfo.bShow).arg(showPaletteInfo.nAlignment));
		}
		else
		{
			ft::log("Call GD_MTC_SDK_SetShowPaletteInfo failed");
		}
	}

	void NetWidget::onCheckedShowPalette(bool bChecked)
	{

	}

	void NetWidget::onRadioBnClickedPaletteAlignmentChange()
	{

	}

	void NetWidget::onChangeExLevel()
	{
		GD_MTC_EX nExLevel = GD_MTC_EX_1;

		if (ui.radioButton_24->isChecked())
		{
			nExLevel = GD_MTC_EX_1;
		}
		else if (ui.radioButton_23->isChecked())
		{
			nExLevel = GD_MTC_EX_2;
		}
		else if (ui.radioButton_22->isChecked())
		{
			nExLevel = GD_MTC_EX_4;
		}

		auto strCenterX = ui.lineEdit_75->text();
		auto strCenterY = ui.lineEdit_76->text();

		if (false == isValidInt(strCenterX))
		{
			this->tip(QString(u8"放大中心X坐标无效"));
			return;
		}
		if (false == isValidInt(strCenterY))
		{
			this->tip(QString(u8"放大中心Y坐标无效"));
			return;
		}

		int nCenterX = strCenterX.toInt();
		int nCenterY = strCenterY.toInt();
		if (NO_ERROR != GD_MTC_SDK_SetExLevel(m_nSessionId, nExLevel, nCenterX, nCenterY))
		{
			auto strTip = QString(u8"电子变焦失败, 原因可能是变焦中心超出范围, nExLevel=%1, nCenterX=%2, nCenterY=%3").arg(nExLevel).arg(nCenterX).arg(nCenterY);
			this->tip(strTip);
			ft::logError(strTip);
		}
	}

	void NetWidget::onBtnClickedGetExLevelInfo()
	{
		GD_MTC_EX_INFO exInfo;
		if (NO_ERROR == GD_MTC_SDK_GetExLevel(m_nSessionId, &exInfo))
		{
			if (GD_MTC_EX_1 == exInfo.nExLevel)
			{
				ui.radioButton_24->setChecked(true);
			}
			else if (GD_MTC_EX_2 == exInfo.nExLevel)
			{
				ui.radioButton_23->setChecked(true);
			}
			else if (GD_MTC_EX_4 == exInfo.nExLevel)
			{
				ui.radioButton_22->setChecked(true);
			}

			ui.lineEdit_75->setText(QString::number(exInfo.nCenterX));
			ui.lineEdit_76->setText(QString::number(exInfo.nCenterY));
		}
		else
		{
			this->tip(QString(u8"获取电子变焦状态失败"));
		}
	}

	void NetWidget::onBtnClickedSetCmdPort()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP;

		strOriIP = "192.168.1.168";

		strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);


		QByteArray baOriIP = strOriIP.toLatin1();

		QString strPort = ui.lineEdit_74->text();
		bool bOk = false;
		int nPort = strPort.toInt(&bOk);
		if (true == bOk && nPort > 0 && nPort < 0xFFFF)
		{
			int nRtn = GD_MTC_SDK_SetCmdPort(baOriIP.data(), nPort);
			if (NO_ERROR == nRtn)
			{
				QMessageBox::information(this, "Tip", "Change cmd port OK.");
			}
			else
			{
				QMessageBox::information(this, "Tip", "Change cmd port failed. Call GD_MTC_SDK_SetCmdPort return failure.");
			}
		}
		else
		{
			QMessageBox::information(this, "Tip", "Change cmd port failed. Port number is invalid");
		}
	}

	void NetWidget::onBtnTestAddPlp()
	{
		++m_nTestAddPlpCount;
		m_nTestAddPlpCount %= 5;

		if (0 == m_nTestAddPlpCount)
		{
			GD_MTC_SDK_DeleteAllPlp(m_nLoginId);
		}
		else if (1 == m_nTestAddPlpCount)
		{
			GD_MTC_SDK_DeleteAllPlp(m_nLoginId);

			GD_MTC_PLP_PointInfo Point;
			Point.PointX = 100;
			Point.PointY = 100;
			GD_MTC_SDK_AddPlpPoint(m_nLoginId, &Point);


			GD_MTC_PLP_LineInfo Line;
			Line.nCursorType = CURSOR_MODE_MAX_MIN;
			auto & pt1 = Line.PointBegin;
			auto & pt2 = Line.PointEnd;
			pt1.PointX = 150;
			pt1.PointY = 50;
			pt2.PointX = pt1.PointX + 150;
			pt2.PointY = pt1.PointY + 100;

			GD_MTC_SDK_AddPlpLine(m_nLoginId, &Line);

			pt1.PointX = 300;
			pt1.PointY = 50;
			pt2.PointX = pt1.PointX;
			pt2.PointY = pt1.PointY + 150;
			GD_MTC_SDK_AddPlpLine(m_nLoginId, &Line);

			GD_MTC_PLP_RectangleInfo Rect;
			Rect.nCursorType = CURSOR_MODE_MAX_MIN;

			Rect.X = 200;
			Rect.Y = 200;
			Rect.Width = 50;
			Rect.Height = 50;
			GD_MTC_SDK_AddPlpRect(m_nLoginId, &Rect);

			Rect.X = 300;
			Rect.Y = 200;
			Rect.Width = 50;
			Rect.Height = 50;
			GD_MTC_SDK_AddPlpRect(m_nLoginId, &Rect);
		}
		else if (2 == m_nTestAddPlpCount)
		{
			GD_MTC_SDK_DeleteAllPlp(m_nLoginId);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					GD_MTC_PLP_PointInfo Point;
					Point.PointX = 30 + 70 * j;
					Point.PointY = 30 + 70 * i;
					GD_MTC_SDK_AddPlpPoint(m_nLoginId, &Point);
				}
			}
		}
		else if (3 == m_nTestAddPlpCount)
		{
			GD_MTC_SDK_DeleteAllPlp(m_nLoginId);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					GD_MTC_PLP_LineInfo Line;
					Line.nCursorType = CURSOR_MODE_MAX;
					auto & pt1 = Line.PointBegin;
					auto & pt2 = Line.PointEnd;
					pt1.PointX = 15 + 70 * j;
					pt1.PointY = 15 + 70 * i;
					pt2.PointX = pt1.PointX + 60;
					pt2.PointY = pt1.PointY + 60;

					GD_MTC_SDK_AddPlpLine(m_nLoginId, &Line);
				}
			}

		}
		else if (4 == m_nTestAddPlpCount)
		{
			GD_MTC_SDK_DeleteAllPlp(m_nLoginId);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					GD_MTC_PLP_RectangleInfo Rect;
					Rect.nCursorType = CURSOR_MODE_MAX;

					Rect.X = 15 + 70 * j;
					Rect.Y = 15 + 70 * i;
					Rect.Width = 50;
					Rect.Height = 50;
					GD_MTC_SDK_AddPlpRect(m_nLoginId, &Rect);
				}
			}
		}
		else
		{
			GD_MTC_SDK_DeleteAllPlp(m_nLoginId);
		}
	}


	void NetWidget::onBtnGetAllPlpInfo()
	{
		m_vectPlpPointInfo.clear();
		m_vectPlpLineInfo.clear();
		m_vectPlpRectInfo.clear();

		{

			auto pInfo = new GD_MTC_PLP_PointInfo[m_nPlpObjSize];
			int nNum = 0;

			if (0 == GD_MTC_SDK_GetPlpPoint(m_nLoginId, pInfo, &nNum))
			{
				if (nNum > 0)
				{
					m_vectPlpPointInfo = std::vector<GD_MTC_PLP_PointInfo>(pInfo, pInfo + nNum);
				}
			}

			delete[] pInfo;
			pInfo = 0;
		}

		{

			auto pInfo = new GD_MTC_PLP_LineInfo[m_nPlpObjSize];
			int nNum = 0;

			if (0 == GD_MTC_SDK_GetPlpLine(m_nLoginId, pInfo, &nNum))
			{
				if (nNum > 0)
				{
					m_vectPlpLineInfo = std::vector<GD_MTC_PLP_LineInfo>(pInfo, pInfo + nNum);
				}
			}

			delete[] pInfo;
			pInfo = 0;
		}


		{

			auto pInfo = new GD_MTC_PLP_RectangleInfo[m_nPlpObjSize];
			int nNum = 0;

			if (0 == GD_MTC_SDK_GetPlpRect(m_nLoginId, pInfo, &nNum))
			{
				if (nNum > 0)
				{
					m_vectPlpRectInfo = std::vector<GD_MTC_PLP_RectangleInfo>(pInfo, pInfo + nNum);
				}
			}

			delete[] pInfo;
			pInfo = 0;
		}


		this->updateCurrentPlpCbo();
	}

	void NetWidget::onBtnDelAllPlpInfo()
	{
		GD_MTC_SDK_DeleteAllPlp(m_nLoginId);

		m_vectPlpPointInfo.clear();
		m_vectPlpLineInfo.clear();
		m_vectPlpRectInfo.clear();

		this->updateCurrentPlpCbo();
	}

	void NetWidget::onBtnSyncPlpToLocal()
	{
		GD_MTC_SDK_SyncPlpToLocal(m_nLoginId, 1);
	}

	void NetWidget::onBtnSyncPlpToDevice()
	{
		GD_MTC_SDK_SyncPlpToLocal(m_nLoginId, 0);
	}

	void NetWidget::onCboPlpPoint()
	{
		int n1 = ui.comboBox_2->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpPointInfo.size())
		{
			auto & v = m_vectPlpPointInfo[n1];

			ui.checkBox_7->setChecked(v.nHide);

			ui.lineEdit_77->setText(QString("%1").arg(v.PointX));
			ui.lineEdit_78->setText(QString("%1").arg(v.PointY));
		}
	}

	void NetWidget::onCboPlpLine()
	{
		int n1 = ui.comboBox_3->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpLineInfo.size())
		{
			auto & v = m_vectPlpLineInfo[n1];

			ui.comboBox_6->setCurrentIndex(v.nCursorType);

			ui.checkBox_6->setChecked(v.nHide);

			ui.lineEdit_79->setText(QString("%1").arg(v.PointBegin.PointX));
			ui.lineEdit_80->setText(QString("%1").arg(v.PointBegin.PointY));
			ui.lineEdit_85->setText(QString("%1").arg(v.PointEnd.PointX));
			ui.lineEdit_86->setText(QString("%1").arg(v.PointEnd.PointY));
		}
	}

	void NetWidget::onCboPlpRect()
	{
		int n1 = ui.comboBox_5->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpRectInfo.size())
		{
			auto & v = m_vectPlpRectInfo[n1];

			ui.comboBox_7->setCurrentIndex(v.nCursorType);

			ui.checkBox_8->setChecked(v.nHide);

			ui.lineEdit_87->setText(QString("%1").arg(v.X));
			ui.lineEdit_88->setText(QString("%1").arg(v.Y));
			ui.lineEdit_89->setText(QString("%1").arg(v.Width));
			ui.lineEdit_90->setText(QString("%1").arg(v.Height));
		}
	}

	void NetWidget::onBtnAddPlpPoint()
	{
		int x = ui.lineEdit_77->text().toInt();
		int y = ui.lineEdit_78->text().toInt();
		int nHide = ui.checkBox_7->isChecked();

		GD_MTC_PLP_PointInfo info;
		info.PointX = x;
		info.PointY = y;
		info.nHide = nHide;
		if (NO_ERROR == GD_MTC_SDK_AddPlpPoint(m_nLoginId, &info))
		{
			m_vectPlpPointInfo.push_back(info);
			this->updateCurrentPlpCbo();

			ui.comboBox_2->setCurrentIndex(m_vectPlpPointInfo.size() - 1);
		}
		else
		{
			ft::logError("call GD_MTC_SDK_AddPlpPoint return failed.");
		}
	}

	void NetWidget::onBtnAddPlpLine()
	{
		int x = ui.lineEdit_79->text().toInt();
		int y = ui.lineEdit_80->text().toInt();
		int x2 = ui.lineEdit_85->text().toInt();
		int y2 = ui.lineEdit_86->text().toInt();
		int nHide = ui.checkBox_6->isChecked();
		int nCursorType = ui.comboBox_6->currentIndex();
		if (nCursorType < 0)
		{
			nCursorType = 0;
		}

		GD_MTC_PLP_LineInfo info;
		info.PointBegin.PointX = x;
		info.PointBegin.PointY = y;
		info.PointEnd.PointX = x2;
		info.PointEnd.PointY = y2;
		info.nHide = nHide;
		info.nCursorType = nCursorType;
		if (NO_ERROR == GD_MTC_SDK_AddPlpLine(m_nLoginId, &info))
		{
			m_vectPlpLineInfo.push_back(info);
			this->updateCurrentPlpCbo();

			ui.comboBox_3->setCurrentIndex(m_vectPlpLineInfo.size() - 1);
		}
		else
		{
			ft::logError("call GD_MTC_SDK_AddPlpLine return failed.");
		}
	}

	void NetWidget::onBtnAddPlpRect()
	{
		int x = ui.lineEdit_87->text().toInt();
		int y = ui.lineEdit_88->text().toInt();
		int w = ui.lineEdit_89->text().toInt();
		int h = ui.lineEdit_90->text().toInt();
		int nHide = ui.checkBox_8->isChecked();
		int nCursorType = ui.comboBox_7->currentIndex();
		if (nCursorType < 0)
		{
			nCursorType = 0;
		}

		GD_MTC_PLP_RectangleInfo info;
		info.X = x;
		info.Y = y;
		info.Width = w;
		info.Height = h;
		info.nHide = nHide;
		info.nCursorType = nCursorType;
		if (NO_ERROR == GD_MTC_SDK_AddPlpRect(m_nLoginId, &info))
		{
			m_vectPlpRectInfo.push_back(info);
			this->updateCurrentPlpCbo();

			ui.comboBox_5->setCurrentIndex(m_vectPlpRectInfo.size() - 1);
		}
		else
		{
			ft::logError("call GD_MTC_SDK_AddPlpRect return failed.");
		}
	}

	void NetWidget::onBtnUpdatePlpPoint()
	{
		int n1 = ui.comboBox_2->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpPointInfo.size())
		{
			int x = ui.lineEdit_77->text().toInt();
			int y = ui.lineEdit_78->text().toInt();
			int nHide = ui.checkBox_7->isChecked();

			auto & v = m_vectPlpPointInfo[n1];

			v.PointX = x;
			v.PointY = y;
			v.nHide = nHide;
			if (NO_ERROR != GD_MTC_SDK_UpdatePlpPoint(m_nLoginId, &v))
			{
				ft::logError("call GD_MTC_SDK_UpdatePlpPoint return failed.");
			}
		}
	}

	void NetWidget::onBtnUpdatePlpLine()
	{
		int n1 = ui.comboBox_3->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpLineInfo.size())
		{
			int x = ui.lineEdit_79->text().toInt();
			int y = ui.lineEdit_80->text().toInt();
			int x2 = ui.lineEdit_85->text().toInt();
			int y2 = ui.lineEdit_86->text().toInt();

			int nHide = ui.checkBox_6->isChecked();

			int nCursorType = ui.comboBox_6->currentIndex();
			if (nCursorType < 0)
			{
				nCursorType = 0;
			}

			auto & v = m_vectPlpLineInfo[n1];

			v.PointBegin.PointX = x;
			v.PointBegin.PointY = y;
			v.PointEnd.PointX = x2;
			v.PointEnd.PointY = y2;
			v.nHide = nHide;
			v.nCursorType = nCursorType;
			if (NO_ERROR != GD_MTC_SDK_UpdatePlpLine(m_nLoginId, &v))
			{
				ft::logError("call GD_MTC_SDK_UpdatePlpLine return failed.");
			}
		}
	}

	void NetWidget::onBtnUpdatePlpRect()
	{
		int n1 = ui.comboBox_5->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpRectInfo.size())
		{
			int x = ui.lineEdit_87->text().toInt();
			int y = ui.lineEdit_88->text().toInt();
			int w = ui.lineEdit_89->text().toInt();
			int h = ui.lineEdit_90->text().toInt();

			int nHide = ui.checkBox_8->isChecked();

			int nCursorType = ui.comboBox_7->currentIndex();
			if (nCursorType < 0)
			{
				nCursorType = 0;
			}

			auto & v = m_vectPlpRectInfo[n1];

			v.X = x;
			v.Y = y;
			v.Width = w;
			v.Height = h;
			v.nHide = nHide;
			v.nCursorType = nCursorType;
			if (NO_ERROR != GD_MTC_SDK_UpdatePlpRect(m_nLoginId, &v))
			{
				ft::logError("call GD_MTC_SDK_UpdatePlpRect return failed.");
			}
		}
	}

	void NetWidget::onBtnDeletePlpPoint()
	{
		int n1 = ui.comboBox_2->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpPointInfo.size())
		{
			int nPlpId = m_vectPlpPointInfo[n1].nPlpId;

			GD_MTC_SDK_DeletePlp(m_nLoginId, nPlpId);

			auto f = [&](GD_MTC_PLP_PointInfo & v) { return v.nPlpId == nPlpId; };
			auto itEnd = std::remove_if(m_vectPlpPointInfo.begin(), m_vectPlpPointInfo.end(), f);
			m_vectPlpPointInfo.erase(itEnd, m_vectPlpPointInfo.end());

			this->updateCurrentPlpCbo();
		}
	}

	void NetWidget::onBtnDeletePlpLine()
	{
		int n1 = ui.comboBox_3->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpLineInfo.size())
		{
			int nPlpId = m_vectPlpLineInfo[n1].nPlpId;

			GD_MTC_SDK_DeletePlp(m_nLoginId, nPlpId);

			auto f = [&](GD_MTC_PLP_LineInfo & v) { return v.nPlpId == nPlpId; };
			auto itEnd = std::remove_if(m_vectPlpLineInfo.begin(), m_vectPlpLineInfo.end(), f);
			m_vectPlpLineInfo.erase(itEnd, m_vectPlpLineInfo.end());

			this->updateCurrentPlpCbo();
		}
	}

	void NetWidget::onBtnDeletePlpRect()
	{
		int n1 = ui.comboBox_5->currentIndex();
		if (n1 >= 0 && n1 < m_vectPlpRectInfo.size())
		{
			int nPlpId = m_vectPlpRectInfo[n1].nPlpId;

			GD_MTC_SDK_DeletePlp(m_nLoginId, nPlpId);

			auto f = [&](GD_MTC_PLP_RectangleInfo & v) { return v.nPlpId == nPlpId; };
			auto itEnd = std::remove_if(m_vectPlpRectInfo.begin(), m_vectPlpRectInfo.end(), f);
			m_vectPlpRectInfo.erase(itEnd, m_vectPlpRectInfo.end());

			this->updateCurrentPlpCbo();
		}
	}

	void NetWidget::updateCurrentPlpCbo()
	{
		ui.comboBox_2->clear();
		ui.comboBox_3->clear();
		ui.comboBox_5->clear();
		{
			int i = 0;
			for (auto & v : m_vectPlpPointInfo)
			{
				ui.comboBox_2->addItem(QString("%1: id=%2").arg(i).arg(v.nPlpId));
				++i;
			}

			i = 0;
			for (auto & v : m_vectPlpLineInfo)
			{
				ui.comboBox_3->addItem(QString("%1: id=%2").arg(i).arg(v.nPlpId));
				++i;
			}

			i = 0;
			for (auto & v : m_vectPlpRectInfo)
			{
				ui.comboBox_5->addItem(QString("%1: id=%2").arg(i).arg(v.nPlpId));
				++i;
			}
		}
	}

	void NetWidget::onBtnGetRgbTempMatrix()
	{
		int w = m_deviceInfo.ImgWidth;
		int h = m_deviceInfo.ImgHeight;

		unsigned char * pRgbData = new unsigned char[w * h * 3];
		int nRgbWidth = 0;
		int nRgbHeight = 0;
		float * pTempMatrix = new float[w * h];
		int nMatrixWidth;
		int nMatrixHeight;
		GD_MTC_TempPointInfo tpTempMax;
		GD_MTC_TempPointInfo tpTempMin;
		GD_MTC_TempPointInfo tpTempAvg;
		auto nRtn = GD_MTC_SDK_GetRgbTempMatrixMaxMinAvgTemp(m_nSessionId, pRgbData, &nRgbWidth, &nRgbHeight, pTempMatrix, &nMatrixWidth, &nMatrixHeight, &tpTempMax, &tpTempMin, &tpTempAvg);
		if (0 == nRtn)
		{
			auto s1 = QString("nRgbWidth=%1, nRgbHeight=%2, nMatrixWidth=%3, nMatrixHeight=%4").arg(nRgbWidth).arg(nRgbHeight).arg(nMatrixWidth).arg(nMatrixHeight);
			auto s2 = QString("tpTempMax.PointX=%1, tpTempMax.PointY=%2, tpTempMax.PointTemp=%3").arg(tpTempMax.PointX).arg(tpTempMax.PointY).arg(tpTempMax.PointTemp);
			auto s3 = QString("tpTempMin.PointX=%1, tpTempMin.PointY=%2, tpTempMin.PointTemp=%3").arg(tpTempMin.PointX).arg(tpTempMin.PointY).arg(tpTempMin.PointTemp);
			auto s4 = QString("tpTempAvg.PointX=%1, tpTempAvg.PointY=%2, tpTempAvg.PointTemp=%3").arg(tpTempAvg.PointX).arg(tpTempAvg.PointY).arg(tpTempAvg.PointTemp);
			ft::log(s1);
			ft::log(s2);
			ft::log(s3);
			ft::log(s4);

			this->tip(QString("%1\r\n%2\r\n%3\r\n%4\r\n").arg(s1).arg(s2).arg(s3).arg(s4));

			QImage img = QImage(pRgbData, w, h, QImage::Format_RGB888);
			img = img.rgbSwapped();

			QString strMs = ft::currentDateTimeStringMs();
			img.save(QString("%1_%2").arg(strMs).arg("1.bmp"));


			// 将温度矩阵也映射成图像
			{
				for (int i = 0; i < h; i++)
				{
					for (int j = 0; j < w; j++)
					{
						int n1 = (pTempMatrix[w * i + j] - tpTempMin.PointTemp) / (tpTempMax.PointTemp - tpTempMin.PointTemp) * 255;

						pRgbData[(w * i + j) * 3 + 0] = n1;
						pRgbData[(w * i + j) * 3 + 1] = n1;
						pRgbData[(w * i + j) * 3 + 2] = n1;

					}
				}

				QImage img = QImage(pRgbData, w, h, QImage::Format_RGB888);
				img = img.rgbSwapped();
				img.save(QString("%1_%2").arg(strMs).arg("2.bmp"));
			}
		}

		delete[] pRgbData;
		pRgbData = 0;

		delete[] pTempMatrix;
		pTempMatrix = 0;
	}

	void NetWidget::onBtnGetArmPort()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);

		QByteArray baOriIP = strOriIP.toLatin1();

		int nPort = 0;
		int nRtn = GD_MTC_SDK_GetArmPort(baOriIP.data(), &nPort);
		if (NO_ERROR == nRtn)
		{
			ui.lineEdit_100->setText(QString::number(nPort));
		}
		else
		{
			QMessageBox::information(this, "Tip", "Get arm port failed.");
		}
	}

	void NetWidget::onBtnSetArmPort()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);


		QByteArray baOriIP = strOriIP.toLatin1();

		QString strPort = ui.lineEdit_100->text();
		bool bOk = false;
		int nPort = strPort.toInt(&bOk);
		if (true == bOk && nPort > 0 && nPort < 0xFFFF)
		{
			int nRtn = GD_MTC_SDK_SetArmPort(baOriIP.data(), nPort);
			if (NO_ERROR == nRtn)
			{
				QMessageBox::information(this, "Tip", "Change arm port OK.");
			}
			else
			{
				QMessageBox::information(this, "Tip", "Change arm port failed. Call GD_MTC_SDK_SetArmPort return failure.");
			}
		}
		else
		{
			QMessageBox::information(this, "Tip", "Change arm port failed. Port number is invalid");
		}
	}

	void NetWidget::onBtnGetFpgaPort()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);

		QByteArray baOriIP = strOriIP.toLatin1();

		int nPort = 0;
		int nRtn = GD_MTC_SDK_GetFpgaPort(baOriIP.data(), &nPort);
		if (NO_ERROR == nRtn)
		{
			ui.lineEdit_101->setText(QString::number(nPort));
		}
		else
		{
			QMessageBox::information(this, "Tip", "Get fpga port failed.");
		}
	}

	void NetWidget::onBtnSetFpgaPort()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);


		QByteArray baOriIP = strOriIP.toLatin1();

		QString strPort = ui.lineEdit_101->text();
		bool bOk = false;
		int nPort = strPort.toInt(&bOk);
		if (true == bOk && nPort > 0 && nPort < 0xFFFF)
		{
			int nRtn = GD_MTC_SDK_SetFpgaPort(baOriIP.data(), nPort);
			if (NO_ERROR == nRtn)
			{
				QMessageBox::information(this, "Tip", "Change fpga port OK.");
			}
			else
			{
				QMessageBox::information(this, "Tip", "Change fpga port failed. Call GD_MTC_SDK_SetFpgaPort return failure.");
			}
		}
		else
		{
			QMessageBox::information(this, "Tip", "Change fpga port failed. Port number is invalid");
		}
	}

	void NetWidget::onBtnGetY16Port()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);

		QByteArray baOriIP = strOriIP.toLatin1();

		int nPort = 0;
		int nRtn = GD_MTC_SDK_GetY16Port(baOriIP.data(), &nPort);
		if (NO_ERROR == nRtn)
		{
			ui.lineEdit_104->setText(QString::number(nPort));
		}
		else
		{
			QMessageBox::information(this, "Tip", "Get y16 port failed.");
		}
	}

	void NetWidget::onBtnSetY16Port()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);


		QByteArray baOriIP = strOriIP.toLatin1();

		QString strPort = ui.lineEdit_104->text();
		bool bOk = false;
		int nPort = strPort.toInt(&bOk);
		if (true == bOk && nPort > 0 && nPort < 0xFFFF)
		{
			int nRtn = GD_MTC_SDK_SetY16Port(baOriIP.data(), nPort);
			if (NO_ERROR == nRtn)
			{
				QMessageBox::information(this, "Tip", "Change y16 port OK.");
			}
			else
			{
				QMessageBox::information(this, "Tip", "Change y16 port failed. Call GD_MTC_SDK_SetY16Port return failure.");
			}
		}
		else
		{
			QMessageBox::information(this, "Tip", "Change y16 port failed. Port number is invalid");
		}
	}

	void NetWidget::onBtnGetDeviceState()
	{
		int nLen = 0;
		int nState = 4;
		if (NO_ERROR == GD_MTC_SDK_GetDeviceState(m_nSessionId, &nState, &nLen))
		{
			QString s1;
			if (0 == nState)
			{
				s1 += QString(u8"设备状态正常, 状态码%1\r\n").arg(nState);
			}
			else
			{
				s1 += QString(u8"设备状态异常, 状态码%1\r\n").arg(nState);

				if (nState & DEVICE_STATE_ARM_ERROR_NO_CURVE_FILE)
				{
					s1 += QString(u8"ARM错误, 没有曲线\r\n");
				}
				if (nState & DEVICE_STATE_ARM_ERROR_Y16_ABNORMAL)
				{
					s1 += QString(u8"ARM错误, Y16异常\r\n");
				}
				if (nState & DEVICE_STATE_ARM_ERROR_NO_MEASURE_PARAM)
				{
					s1 += QString(u8"ARM错误, 没有测温参数\r\n");
				}
				if (nState & DEVICE_STATE_ARM_ERROR_FONT_MISSING)
				{
					s1 += QString(u8"ARM错误, 字体库缺失\r\n");
				}

				if (nState & DEVICE_STATE_FPGA_ERROR_INVALID_SENSOR_CONFIG)
				{
					s1 += QString(u8"FPGA错误, 无效的探测器配置\r\n");
				}
				if (nState & DEVICE_STATE_FPGA_ERROR_LOAD_FLASH_PARAM_FAILED)
				{
					s1 += QString(u8"FPGA错误, 加载FLASH参数失败\r\n");
				}
				if (nState & DEVICE_STATE_FPGA_ERROR_DEVICE_WORK_TEMP_OVER_RANGE)
				{
					s1 += QString(u8"FPGA错误, 设备工作温度超出范围\r\n");
				}
			}

			QMessageBox::information(this, "Tip", s1);
			ft::log(s1);
			return;
		}

		QMessageBox::information(this, "Tip", u8"获取设备状态失败");
	}

	void NetWidget::onBtnClickedGetCmdPort()
	{
		QString strResult;
		if (false == this->isValidIp(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84, strResult))
		{
			QMessageBox::information(this, u8"提示", QString(u8"IP[%1]格式不正确").arg(strResult));
			return;
		}

		QString strOriIP;

		strOriIP = "192.168.1.168";

		strOriIP = getIP(ui.lineEdit_81, ui.lineEdit_82, ui.lineEdit_83, ui.lineEdit_84);


		QByteArray baOriIP = strOriIP.toLatin1();

		int nPort = 0;
		int nRtn = GD_MTC_SDK_GetCmdPort(baOriIP.data(), &nPort);
		if (NO_ERROR == nRtn)
		{
			ui.lineEdit_74->setText(QString::number(nPort));
			//QMessageBox::information(this, "Tip", "Change IP OK.");
		}
		else
		{
			int n1 = 0;
			QMessageBox::information(this, "Tip", "Get cmd port failed.");
		}
	}

	void NetWidget::onY16Data(int nAlarmMinFlag, int nAlarmMaxFlag, float fSpanMin, float fSpanMax)
	{
		if (false == m_bSetSpanInitValue)
		{
			// 		if (PULL_DATA_TYPE::PULL_DATA_TYPE_Y16_ONLY == m_nPullDataType &&
			// 			ADJUST_LIGHT_MODE_TEMPERATURE_MANUAL == m_nAdjustLightMode)
			if (PULL_DATA_TYPE::PULL_DATA_TYPE_Y16_ONLY == m_nPullDataType)
			{
				ui.lineEdit_27->setText(QString::number(g_ptLow.PointTemp));
				ui.lineEdit_28->setText(QString::number(g_ptHigh.PointTemp));
				m_bSetSpanInitValue = true;
			}
		}

		ui.pushButton_10->setText(QString("%1, %2").arg(nAlarmMaxFlag).arg(nAlarmMinFlag));

		if (ADJUST_LIGHT_MODE_TEMPERATURE_AUTO == m_nAdjustLightMode)
		{
			ui.lineEdit_27->setText(QString::number(fSpanMin));
			ui.lineEdit_28->setText(QString::number(fSpanMax));
		}

		g_bInY16Cb = false;
	}

	void NetWidget::onShowImg(uchar * pData, int nWidth, int nHeight)
	{
		// 	{
		// 		int nLen = nWidth * nHeight * 3;
		// 		this->editSnapPicCbObj(pData, nLen, nWidth, nHeight, true, 0);
		// 	}

		QImage img = QImage(pData, nWidth, nHeight, QImage::Format_RGB888);
		img = img.rgbSwapped();

		m_pLblShowPic->setFixedSize(nWidth, nHeight);

		ui.horizontalSpacer_2->changeSize(nWidth, 10, QSizePolicy::Fixed, QSizePolicy::Fixed);

		// 自动调焦时显示方框
		if (PULL_DATA_TYPE_Y16_ONLY == m_nPullDataType && true == g_bFocusing)
		{
			const int nWidth = 95;
			const int nHeight = 95;
			const int nLineLen = 10;
			int nPicWidth = m_deviceInfo.ImgWidth;
			int nPicHeight = m_deviceInfo.ImgHeight;

			QPainter painter(&img);
			painter.setPen(Qt::white);

			QVector<QPoint> pointPairs;
			QPoint pt1((nPicWidth - nWidth) / 2, (nPicHeight - nHeight) / 2);
			QPoint pt2(pt1.x() + nWidth, pt1.y());
			QPoint pt3(pt1.x(), pt1.y() + nHeight);
			QPoint pt4(pt3.x() + nWidth, pt3.y());
			QPoint ptX1;
			QPoint pt;
			QPoint ptX2;

			auto fun = [&]()
			{
				pointPairs.clear();
				pointPairs.push_back(ptX1);
				pointPairs.push_back(pt);
				pointPairs.push_back(pt);
				pointPairs.push_back(ptX2);
				painter.drawLines(pointPairs);
			};


			pt = pt1;
			ptX1 = QPoint(pt.x(), pt.y() + nLineLen);
			ptX2 = QPoint(pt.x() + nLineLen, pt.y());
			fun();

			pt = pt2;
			ptX1 = QPoint(pt.x() - nLineLen, pt.y());
			ptX2 = QPoint(pt.x(), pt.y() + nLineLen);
			fun();

			pt = pt3;
			ptX1 = QPoint(pt.x(), pt.y() - nLineLen);
			ptX2 = QPoint(pt.x() + nLineLen, pt.y());
			fun();

			pt = pt4;
			ptX1 = QPoint(pt.x() - nLineLen, pt.y());
			ptX2 = QPoint(pt.x(), pt.y() - nLineLen);
			fun();
		}

		m_pLblShowPic->setMinimumSize(nWidth, nHeight);

		m_pLblShowPic->setPixmap(QPixmap::fromImage(std::move(img)));

		int nPtLen = 2;

		ui.label_51->setText(tr("[%1, %2, %3, %4]")
			.arg(u8"X坐标")
			.arg(u8"Y坐标")
			.arg(u8"温度(℃)")
			.arg(u8"Y16")
		);

		ui.label_10->setText(tr("[%1, %2, %3, %4]")
			.arg(g_ptCursor.PointX, 3, 10, QChar('0'))
			.arg(g_ptCursor.PointY, 3, 10, QChar('0'))
			.arg(g_ptCursor.PointTemp, 5, 'f', nPtLen, QChar('0'))
			.arg(g_ptCursorAd)
		);

		ui.label_12->setText(tr("[%1, %2, %3, %4]")
			.arg(g_ptHigh.PointX, 3, 10, QChar('0'))
			.arg(g_ptHigh.PointY, 3, 10, QChar('0'))
			.arg(g_ptHigh.PointTemp, 5, 'f', nPtLen, QChar('0'))
			.arg(g_ptHighAd)
		);

		ui.label_8->setText(tr("[%1, %2, %3, %4]")
			.arg(g_ptLow.PointX, 3, 10, QChar('0'))
			.arg(g_ptLow.PointY, 3, 10, QChar('0'))
			.arg(g_ptLow.PointTemp, 5, 'f', nPtLen, QChar('0'))
			.arg(g_ptLowAd)
		);

		// 	ui.label_6->setText(tr("[%1, %2, %3]")
		// 		.arg(g_ptAvg.PointX, 3, 10, QChar('0'))
		// 		.arg(g_ptAvg.PointY, 3, 10, QChar('0'))
		// 		.arg(g_ptAvg.PointTemp, 5, 'f', nPtLen, QChar('0')));

		ui.label_6->setText(tr("[%1, %2, %3]")
			.arg("NA")
			.arg("NA")
			.arg(g_ptAvg.PointTemp, 5, 'f', nPtLen, QChar('0')));

		ui.label_18->setText(tr("[%1, %2, %3, %4]")
			.arg(g_ptCenter.PointX, 3, 10, QChar('0'))
			.arg(g_ptCenter.PointY, 3, 10, QChar('0'))
			.arg(g_ptCenter.PointTemp, 5, 'f', nPtLen, QChar('0'))
			.arg(g_ptCenterAd)
		);

		//ft::log(tr("%1").arg(g_ptCenter.PointTemp, 5, 'f', 1, QChar('0')));


		if (m_nPrePicWidth != nWidth || m_nPrePicHeight != nHeight)
		{
			//if (this->sizeHint() != this->size())
			{
				m_nResizeCount = 2;
			}
			m_nPrePicWidth = nWidth;
			m_nPrePicHeight = nHeight;
		}

		if (m_nResizeCount)
		{
			--m_nResizeCount;
			this->resize(this->sizeHint());

			QDesktopWidget* desktop = QApplication::desktop();
			move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
		}

		delete[] pData;
		pData = 0;

		g_bInRgbCb = false;
	}

	void NetWidget::onY16Parm(void * pY16Parm2)
	{

	}

	// int NetWidget::y16CbFunObj(const GD_MTC_CALLBACK_Y16Info * pY16Info, void *pUser)
	// {
	// 	int nW = pY16Info->ImgWidth;
	// 	int nH = pY16Info->ImgHeight;
	//
	// 	void * pOpque = pY16Info->pOpque;
	// 	if (0 == GD_MTC_SDK_GetPointTemp(pOpque, &g_ptCursor))
	// 	{
	// 		;
	// 	}
	//
	// 	g_ptCursorAd = pY16Info->Y16Data[nW * g_ptCursor.PointY + g_ptCursor.PointX];
	//
	// 	g_ptCenter.PointX = pY16Info->ImgWidth / 2;
	// 	g_ptCenter.PointY = pY16Info->ImgHeight / 2;
	// 	if (0 == GD_MTC_SDK_GetPointTemp(pOpque, &g_ptCenter))
	// 	{
	// 		;
	// 	}
	//
	// 	g_ptCenterAd = pY16Info->Y16Data[nW * g_ptCenter.PointY + g_ptCenter.PointX];
	//
	// 	float *pTempMatrix = new float[pY16Info->ImgWidth * pY16Info->ImgHeight];
	// 	if (0 == GD_MTC_SDK_GetTempMatrix(pY16Info->pOpque, pTempMatrix))
	// 	{
	// 		int n1 = 0;
	// 	}
	// 	else
	// 	{
	// 		int n1 = 0;
	// 	}
	//
	// 	delete[] pTempMatrix;
	// 	pTempMatrix = 0;
	//
	//
	// 	if (0 == GD_MTC_SDK_GetHighTemp(pOpque, &g_ptHigh))
	// 	{
	// 		;
	// 	}
	// 	if (0 == GD_MTC_SDK_GetLowTemp(pOpque, &g_ptLow))
	// 	{
	// 		;
	// 	}
	// 	if (0 == GD_MTC_SDK_GetAvegTemp(pOpque, &g_ptAvg))
	// 	{
	// 		;
	// 	}
	//
	// 	if (false == m_bSetSpanInitValue)
	// 	{
	// 		// 		if (PULL_DATA_TYPE::PULL_DATA_TYPE_Y16_ONLY == m_nPullDataType &&
	// 		// 			ADJUST_LIGHT_MODE_TEMPERATURE_MANUAL == m_nAdjustLightMode)
	// 		if (PULL_DATA_TYPE::PULL_DATA_TYPE_Y16_ONLY == m_nPullDataType)
	// 		{
	// 			ui.lineEdit_27->setText(QString::number(g_ptLow.PointTemp));
	// 			ui.lineEdit_28->setText(QString::number(g_ptHigh.PointTemp));
	// 			m_bSetSpanInitValue = true;
	// 		}
	// 	}
	//
	// 	g_ptHighAd = pY16Info->Y16Data[nW * g_ptHigh.PointY + g_ptHigh.PointX];
	// 	g_ptLowAd = pY16Info->Y16Data[nW * g_ptLow.PointY + g_ptLow.PointX];
	//
	//
	//
	// 	g_Rect.X = 50;
	// 	g_Rect.Y = 50;
	// 	g_Rect.Width = 100;
	// 	g_Rect.Height = 100;
	// 	if (0 == GD_MTC_SDK_GetRectHighTemp(pOpque, g_Rect, &g_ptRectHigh))
	// 	{
	// 		;
	// 	}
	// 	if (0 == GD_MTC_SDK_GetRectLowTemp(pOpque, g_Rect, &g_ptRectLow))
	// 	{
	// 		;
	// 	}
	// 	if (0 == GD_MTC_SDK_GetRectAvegTemp(pOpque, g_Rect, &g_ptRectAvg))
	// 	{
	// 		;
	// 	}
	//
	// 	if (g_bSnapPicNextFrame)
	// 	{
	// 		QString strFilePath = ft::exeDir();
	// 		auto ba = strFilePath.toLatin1();
	// 		GD_MTC_SDK_SnapPicture(pOpque, ba.data());
	//
	// 		g_bSnapPicNextFrame = false;
	// 	}
	//
	// 	int nAlarmMaxFlag = pY16Info->y16OutputInfo.nDeviceRealTimeMaxAlarmFlag;
	// 	int nAlarmMinFlag = pY16Info->y16OutputInfo.nDeviceRealTimeMinAlarmFlag;
	// 	ui.pushButton_10->setText(QString("%1, %2").arg(nAlarmMaxFlag).arg(nAlarmMinFlag));
	// 	//ui.pushButton_10->setText(QString("%1").arg(nAlarmMaxFlag));
	//
	// 	if (ADJUST_LIGHT_MODE_TEMPERATURE_AUTO == m_nAdjustLightMode)
	// 	{
	// 		float fSpanMin = pY16Info->y16OutputInfo.fTemperatureAdjustLightSpanMin;
	// 		float fSpanMax = pY16Info->y16OutputInfo.fTemperatureAdjustLightSpanMax;
	// 		ui.lineEdit_27->setText(QString::number(fSpanMin));
	// 		ui.lineEdit_28->setText(QString::number(fSpanMax));
	// 	}
	//
	// 	return 0;
	// }

	int NetWidget::onSearchDeviceResult(GD_MTC_IRDeviceInfo * pDeviceInfo, int nDeviceInfoSize, void *pUser)
	{
		if (nDeviceInfoSize > 0)
		{
			QString strAll;
			for (int i = 0; i < nDeviceInfoSize; i++)
			{
				const GD_MTC_IRDeviceInfo * p = pDeviceInfo + i;
				auto strMsg = QString("IP:%1, GateWay:%2, SubMask:%3, Mac:%4, Port:%5")
					.arg(p->IpAddress)
					.arg(p->GateWay)
					.arg(p->SubMask)
					.arg(p->MacAddress)
					.arg(p->Port);

				strAll += strMsg;
				strAll += "\r\n";
			}

			// 目前取第一条数据的IP显示到界面
			{
				QString strIp = pDeviceInfo[0].IpAddress;
				auto strList = strIp.split(".");
				if (4 == strList.size())
				{
					ui.lineEdit_49->setText(strList[0]);
					ui.lineEdit_50->setText(strList[1]);
					ui.lineEdit_51->setText(strList[2]);
					ui.lineEdit_52->setText(strList[3]);
				}
			}

			this->tip(strAll);


			delete[] pDeviceInfo;
			pDeviceInfo = 0;
		}
		else if (0 == nDeviceInfoSize)
		{
			this->tip(u8"没有搜索到设备");
		}


		return 1;
	}

	int NetWidget::editSnapPicCbObj(unsigned char * pRgbData, int nRgbDataLen, int nRgbWidth, int nRgbHeight, bool bRgb, void * pUser)
	{
		if (this->m_bPaletteDataOk)
		{
			if (m_nPaletteIndex >= 0 && m_nPaletteIndex < m_nPaletteDemension0)
			{
				int y = (nRgbHeight - m_nPaletteDemension1) / 2;

				int nLabelWidth = 10;

				int nOnePaletteLen = m_nPaletteDataLen / m_nPaletteDemension0;

				if (false == bRgb)
				{
					for (int i = y; i < m_nPaletteDemension1 + y; i++)
					{
						for (int j = nRgbWidth - nLabelWidth; j < nRgbWidth; j++)
						{
							int nPixIndex = i - y;
							nPixIndex = m_nPaletteDemension1 - 1 - nPixIndex;
							pRgbData[(nRgbWidth * i + j) * 3 + 0] = m_pPaletteData[nOnePaletteLen * m_nPaletteIndex + m_nPaletteDemension2 * nPixIndex + 2];
							pRgbData[(nRgbWidth * i + j) * 3 + 1] = m_pPaletteData[nOnePaletteLen * m_nPaletteIndex + m_nPaletteDemension2 * nPixIndex + 1];
							pRgbData[(nRgbWidth * i + j) * 3 + 2] = m_pPaletteData[nOnePaletteLen * m_nPaletteIndex + m_nPaletteDemension2 * nPixIndex + 0];
						}
					}
				}
				else
				{
					for (int i = y; i < m_nPaletteDemension1 + y; i++)
					{
						for (int j = nRgbWidth - nLabelWidth; j < nRgbWidth; j++)
						{
							int nPixIndex = i - y;
							nPixIndex = m_nPaletteDemension1 - 1 - nPixIndex;
							pRgbData[(nRgbWidth * i + j) * 3 + 0] = m_pPaletteData[nOnePaletteLen * m_nPaletteIndex + m_nPaletteDemension2 * nPixIndex + 0];
							pRgbData[(nRgbWidth * i + j) * 3 + 1] = m_pPaletteData[nOnePaletteLen * m_nPaletteIndex + m_nPaletteDemension2 * nPixIndex + 1];
							pRgbData[(nRgbWidth * i + j) * 3 + 2] = m_pPaletteData[nOnePaletteLen * m_nPaletteIndex + m_nPaletteDemension2 * nPixIndex + 2];
						}
					}
				}
			}
		}


		// 	for (int i = 50; i < 100 ; i++)
		// 	{
		// 		for (int j = 50; j < 100; j++)
		// 		{
		// 			pRgbData[(nRgbWidth * i + j) * 3 + 0] = 255;
		// 			pRgbData[(nRgbWidth * i + j) * 3 + 1] = 0;
		// 			pRgbData[(nRgbWidth * i + j) * 3 + 2] = 0;
		// 		}
		// 	}

		return 1;
	}

	void NetWidget::getPaletteData()
	{
		m_bPaletteDataOk = false;

		int nPaletteDataSize = 0;
		if (NO_ERROR == GD_MTC_SDK_GetPaletteDataSize(m_nSessionId, &nPaletteDataSize))
		{
			m_nPaletteDataLen = nPaletteDataSize;

			if (0 == m_pPaletteData)
			{
				m_pPaletteData = new unsigned char[nPaletteDataSize];
				memset(m_pPaletteData, 0, nPaletteDataSize);
			}
			char cBuf[20] = { 0 };
			if (NO_ERROR == GD_MTC_SDK_GetPaletteData(
				m_nSessionId,
				m_pPaletteData,
				&m_nPaletteDemension0,
				&m_nPaletteDemension1,
				&m_nPaletteDemension2,
				cBuf))
			{
				m_strPaletteFormat = cBuf;
				m_bPaletteDataOk = true;
			}
			else
			{
				ft::logError("Get palette data failed.");
			}
		}
		else
		{
			ft::logError("Get palette data size failed.");
		}
	}

	GD_MTC_IRDeviceType NetWidget::deviceType()
	{
		//GD_MTC_IRDeviceType nDeviceType = GD_MTC_IRDeviceType::OR2;
		//GD_MTC_IRDeviceType nDeviceType = GD_MTC_IRDeviceType::NET;
		GD_MTC_IRDeviceType nDeviceType = m_nDeviceType;

		return nDeviceType;
	}

	void NetWidget::disableAllCtrls()
	{
		// 	// Test
		// 	{
		// 		return;
		// 	}

		ui.lineEdit_49->setEnabled(true);
		ui.lineEdit_50->setEnabled(true);
		ui.lineEdit_51->setEnabled(true);
		ui.lineEdit_52->setEnabled(true);
		ui.pushButton_57->setEnabled(true);

		ui.radioButton_8->setEnabled(true);
		ui.radioButton_9->setEnabled(true);
		ui.radioButton_10->setEnabled(true);


		ui.pushButton->setEnabled(true);
		ui.pushButton_2->setEnabled(false);


		bool b = false;

		ui.label_10->setVisible(b);
		ui.label_12->setVisible(b);
		ui.label_8->setVisible(b);
		ui.label_6->setVisible(b);
		ui.label_18->setVisible(b);


		ui.pushButton_4->setEnabled(b);

		ui.pushButton_3->setEnabled(b);

		ui.pushButton_49->setEnabled(b);

		ui.pushButton_50->setEnabled(b);

		ui.pushButton_12->setEnabled(b);

		ui.pushButton_58->setEnabled(b);

		ui.pushButton_5->setEnabled(b);

		ui.pushButton_6->setEnabled(b);

		ui.pushButton_9->setEnabled(b);
		ui.comboBox->setEnabled(b);

		ui.pushButton_32->setEnabled(b);
		ui.comboBox_4->setEnabled(b);

		ui.pushButton_29->setEnabled(false);
		ui.pushButton_30->setEnabled(false);
		ui.pushButton_55->setEnabled(false);
		ui.pushButton_56->setEnabled(false);
		ui.pushButton_89->setEnabled(false);

		ui.groupBox_11->setEnabled(false);
		ui.groupBox_12->setEnabled(false);
		ui.groupBox_21->setEnabled(false);
		ui.groupBox_22->setEnabled(false);
		ui.groupBox->setEnabled(false);
		ui.groupBox_14->setEnabled(false);
		ui.groupBox_15->setEnabled(false);
		ui.groupBox_13->setEnabled(false);
		ui.groupBox_16->setEnabled(false);
		ui.groupBox_17->setEnabled(false);
		ui.groupBox_18->setEnabled(true);
		ui.groupBox_23->setEnabled(true);
		ui.groupBox_8->setEnabled(false);
		ui.groupBox_10->setEnabled(false);
		ui.groupBox_9->setEnabled(false);
	}

	void NetWidget::updataControlState()
	{
		// Test
		// 	{
		// 		return;
		// 	}

		if (false == m_bDataStreamIsOpen)
		{
			disableAllCtrls();
			ui.scrollArea->ensureVisible(0, 0);

			// 关闭数据流时初始化界面上部分控制的数值
			{
				ui.checkBox_2->setChecked(false);
				ui.radioButton_19->setChecked(true);

				ui.radioButton_19->setEnabled(false);
				ui.radioButton_18->setEnabled(false);

				ui.radioButton->setChecked(true);
				ui.lineEdit_11->setText("50");
				ui.lineEdit_12->setText("50");
			}
			return;
		}
		else
		{
			ui.lineEdit_49->setEnabled(false);
			ui.lineEdit_50->setEnabled(false);
			ui.lineEdit_51->setEnabled(false);
			ui.lineEdit_52->setEnabled(false);
			ui.pushButton_57->setEnabled(false);

			ui.radioButton_8->setEnabled(false);
			ui.radioButton_9->setEnabled(false);
			ui.radioButton_10->setEnabled(false);

			ui.pushButton->setEnabled(false);
			ui.pushButton_2->setEnabled(true);
		}

		auto nDviceType = m_nDeviceType;
		auto nPullDataType = m_nPullDataType;

		bool b = false;

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_GET_Y16_MATRIX_TEMP);
		ui.label_10->setVisible(b);
		ui.label_12->setVisible(b);
		ui.label_8->setVisible(b);
		ui.label_6->setVisible(b);
		ui.label_18->setVisible(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SHUTTER);
		ui.pushButton_4->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SNAP);
		ui.pushButton_3->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SNAP_NO_CB);
		ui.pushButton_49->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SNAP_WITHOUT_TEMP_NO_CB);
		ui.pushButton_50->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SNAP_WITHOUT_IMG_NO_CB);
		ui.pushButton_12->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SNAP_UNIFORM);
		ui.pushButton_58->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_AVI_RECORD);
		ui.pushButton_5->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_Y16_RECORD);
		ui.pushButton_6->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_PALETTE);
		ui.pushButton_9->setEnabled(b);
		ui.comboBox->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_FRAME_RATE);
		ui.pushButton_32->setEnabled(b);
		ui.comboBox_4->setEnabled(b);

		ui.pushButton_29->setEnabled(true);
		ui.pushButton_30->setEnabled(true);
		ui.pushButton_55->setEnabled(true);
		ui.pushButton_56->setEnabled(true);
		ui.pushButton_89->setEnabled(true);

		ui.groupBox_11->setEnabled(true);
		ui.groupBox_12->setEnabled(true);
		ui.groupBox_21->setEnabled(true);
		ui.groupBox_14->setEnabled(true);
		ui.groupBox_15->setEnabled(true);
		ui.groupBox_13->setEnabled(true);
		ui.groupBox_16->setEnabled(true);
		ui.groupBox_17->setEnabled(true);
		ui.groupBox_18->setEnabled(false);
		ui.groupBox_23->setEnabled(false);
		ui.groupBox_8->setEnabled(true);
		ui.groupBox_10->setEnabled(true);
		ui.groupBox_9->setEnabled(true);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_NEAR_FOCUS);
		ui.pushButton_34->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_FAR_FOCUS);
		ui.pushButton_39->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_NEAR_FOCUS_FINE);
		ui.pushButton_65->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_FAR_FOCUS_FINE);
		ui.pushButton_66->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_AUTO_FOCUS);
		ui.pushButton_38->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_GET_MOTOR_POS);
		ui.pushButton_40->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_MOTOR_POS);
		ui.pushButton_37->setEnabled(b);
		ui.lineEdit_14->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_DISABLE_AUTO_SHUTTER);
		ui.pushButton_35->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_ENABLE_AUTO_SHUTTER);
		ui.pushButton_36->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SHUTTER);
		ui.pushButton_42->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SHUTTER_CLOSE);
		ui.pushButton_59->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SHUTTER_OPEN);
		ui.pushButton_60->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_AUTO_SHUTTER_TIME);
		ui.pushButton_41->setEnabled(b);
		ui.lineEdit_15->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_EX_LEVEL);
		ui.groupBox_22->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_PLP_ANALYZE);
		ui.groupBox->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_ADD_RECT_ANALYSER);
		ui.groupBox_14->setEnabled(b);
		if (true == b)
		{
			b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_ENABLE_DIGITAL_TEMP_WITH_CURSOR);
			ui.checkBox_3->setEnabled(b);
		}


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_ADD_STRING);
		ui.groupBox_15->setEnabled(b);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_ENABLE_ADD_STRING);
		ui.pushButton_46->setEnabled(b);


		//ui.groupBox_13->setEnabled(true);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_GET_DEVICE_SERIAL_NUMBER);
		ui.groupBox_16->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_CHANGE_IP);
		ui.groupBox_17->setEnabled(b);

		//ui.groupBox_18->setEnabled(false);

		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_EMISS);
		ui.groupBox_8->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_GET_B2);
		ui.pushButton_8->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_B2);
		ui.lineEdit_7->setEnabled(b);
		ui.pushButton_7->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_VSK_GEAR);
		ui.lineEdit_71->setEnabled(b);
		ui.pushButton_61->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_UART_CHECK_ADDRESS);
		ui.lineEdit_72->setEnabled(b);
		ui.pushButton_69->setEnabled(b);
		ui.pushButton_68->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_UART_RATE);
		ui.lineEdit_73->setEnabled(b);
		ui.pushButton_71->setEnabled(b);
		ui.pushButton_70->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SAVE_ALL_PARM);
		ui.pushButton_11->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_GET_BRIGHTNESS);
		ui.pushButton_17->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_BRIGHTNESS);
		ui.lineEdit_11->setEnabled(b);
		ui.pushButton_16->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_GET_CONTRAST);
		ui.pushButton_19->setEnabled(b);
		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_SET_CONTRAST);
		ui.lineEdit_12->setEnabled(b);
		ui.pushButton_18->setEnabled(b);

		ui.lineEdit_27->setEnabled(b);
		ui.lineEdit_28->setEnabled(b);
		ui.pushButton_64->setEnabled(b);
		ui.pushButton_63->setEnabled(b);

		ui.radioButton->setEnabled(b);
		ui.radioButton_4->setEnabled(b);
		ui.radioButton_5->setEnabled(b);


		b = GD_MTC_SDK_SupportFunction(nDviceType, nPullDataType, DFS_GET_DEVICE_ALARM_STATE);
		ui.groupBox_9->setEnabled(b);

		ui.scrollArea->ensureVisible(0, 0);

		this->onRaioBtnAdjustLightMode();
	}

	void NetWidget::tip(const QString & s1)
	{
		QMessageBox::information(this, u8"提示", s1);
	}

	bool NetWidget::isValidIp(const QString & s1)
	{
		QString strMatch0_255 = tr("(([0-9]{1,2})|([0-1][0-9]{2,2}|((2[0-4][0-9])|(25[0-5]))))");
		QString strAllPatern = tr("^%1\\.%2\\.%3\\.%4$").arg(strMatch0_255, strMatch0_255, strMatch0_255, strMatch0_255);

		QRegExp RegExp(strAllPatern);

		if (true == RegExp.exactMatch(s1))
		{
			return true;
		}

		return false;
	}

	bool NetWidget::isValidIp(QLineEdit * p1, QLineEdit * p2, QLineEdit * p3, QLineEdit * p4, QString & strResult)
	{
		QString strIp;

		strIp = QString("%1.%2.%3.%4")
			.arg(p1->text())
			.arg(p2->text())
			.arg(p3->text())
			.arg(p4->text());

		strResult = strIp;

		return this->isValidIp(strIp);
	}

	QString NetWidget::configFilePath()
	{
		return QString("%1/%2").arg(ft::exeDir()).arg("Config.dat");
	}

	bool NetWidget::readConfigFile()
	{
		QString strFilaPath = configFilePath();
		QByteArray ba;
		if (true == ft::readFile(strFilaPath, ba))
		{
			QJsonDocument jd = QJsonDocument::fromJson(ba);
			m_joConfig = jd.object();
			m_bConfigOk = true;
			return true;
		}
		else
		{
			ft::logError(QString("Read file %1 failed.").arg(strFilaPath));
		}
		return false;
	}

	bool NetWidget::writeConfigFile()
	{
		// 将当前成功连上设备的IP地址写入文件
		if (true == m_bConfigOk)
		{
			QString strFilaPath = configFilePath();
			if (false == ft::saveToJsonFile(strFilaPath, m_joConfig))
			{
				ft::logError(QString("Write file %1 failed.").arg(strFilaPath));
				return false;
			}
		}
		else
		{
			QJsonObject jo;
			jo.insert("ip", "192.168.1.168");
			jo.insert("mode", "2");
			jo.insert("fps", "12");
			jo.insert("palette", "0");

			QString strFilaPath = configFilePath();
			if (false == ft::saveToJsonFile(strFilaPath, jo))
			{
				ft::logError(QString("Write file %1 failed.").arg(strFilaPath));
				return false;
			}
		}
		return true;
	}

	void NetWidget::setAddStringDefaultRect()
	{
		ui.lineEdit_18->setText("0");
		ui.lineEdit_19->setText("0");
		ui.lineEdit_20->setText("0");
		ui.lineEdit_21->setText("0");
	}

	bool NetWidget::isValidIntAndTip(const QString & s1)
	{
		int nDegitsMin = 1;
		//int nDegitsMax = 20;
		int nDegitsMax = 10;

		QString strMatch0_255 = tr("([0-9]{%1,%2})").arg(nDegitsMin).arg(nDegitsMax);
		QString strAllPatern = tr("^%1$").arg(strMatch0_255);

		QRegExp RegExp(strAllPatern);

		if (true == RegExp.exactMatch(s1))
		{
			return true;
		}
		else
		{
			this->tip(QString(u8"%1 不是一个%2到%3位的整数").arg(s1).arg(nDegitsMin).arg(nDegitsMax));
		}
		return false;
	}

	bool NetWidget::isValidInt(const QString & s1)
	{
		int nDegitsMin = 1;
		//int nDegitsMax = 20;
		int nDegitsMax = 10;

		QString strMatch0_255 = tr("([0-9]{%1,%2})").arg(nDegitsMin).arg(nDegitsMax);
		QString strAllPatern = tr("^%1$").arg(strMatch0_255);

		QRegExp RegExp(strAllPatern);

		if (true == RegExp.exactMatch(s1))
		{
			return true;
		}
		else
		{
			//this->tip(QString(u8"%1 不是一个%2到%3位的整数").arg(s1).arg(nDegitsMin).arg(nDegitsMax));
		}
		return false;
	}

	bool NetWidget::isValidFloatNumber(const QString & s1)
	{
		QString strAllPatern = QString("^(-?\\d+)(\\.\\d+)?$");

		QRegExp RegExp(strAllPatern);

		if (true == RegExp.exactMatch(s1))
		{
			return true;
		}

		return false;
	}

	int NetWidget::rgbCbFunS(const GD_MTC_CALLBACK_RGBInfo * RGBInfo, void *pUser)
	{
		// Test
		// 	{
		// 		static unsigned int nCount = 0;
		// 		nCount++;
		// 		if (0 != nCount % 5)
		// 		{
		// 			return 0;
		// 		}
		// 	}

		if (true == g_bInRgbCb)
		{
			ft::log("In rgb callback, new data ignored");
			return 0;
		}

		int n1 = 0;
		RGBInfo->ImgWidth;
		RGBInfo->ImgHeight;

		//if (pWidget)
		{
			int nLen = RGBInfo->ImgWidth * RGBInfo->ImgHeight * 3;
			uchar * pData = new uchar[nLen];
			memcpy(pData, RGBInfo->RGBData, nLen);

			// 		{
			// 			NetWidget * pWidet = (NetWidget *)pUser;
			// 			pWidet->editSnapPicCbObj(pData, nLen, RGBInfo->ImgWidth, RGBInfo->ImgHeight, true, 0);
			// 		}

			g_bInRgbCb = true;
			emit this->showImg(pData, RGBInfo->ImgWidth, RGBInfo->ImgHeight);
		}

		return 0;
	}

	int NetWidget::y16CbFunS(const GD_MTC_CALLBACK_Y16Info * pY16Info, void *pUser)
	{
		if (true == g_bInY16Cb)
		{
			ft::log("In Y16 callback, new data ignored");
			return 0;
		}

		// 	{
		// 		ft::log(QString("fCurrentShutterTemp=%1").arg(pY16Info->y16OutputInfo.fCurrentShutterTemp));
		// 	}

		int nW = pY16Info->ImgWidth;
		int nH = pY16Info->ImgHeight;

		void * pOpque = pY16Info->pOpque;
		if (0 == GD_MTC_SDK_GetPointTemp(pOpque, &g_ptCursor))
		{
			;
		}

		// 	{
		// 		ft::log(QString("g_ptCursor.PointX=%1, g_ptCursor.PointY=%2").arg(g_ptCursor.PointX).arg(g_ptCursor.PointY));
		// 	}

		g_ptCursorAd = pY16Info->Y16Data[nW * g_ptCursor.PointY + g_ptCursor.PointX];

		g_ptCenter.PointX = pY16Info->ImgWidth / 2;
		g_ptCenter.PointY = pY16Info->ImgHeight / 2;
		if (0 == GD_MTC_SDK_GetPointTemp(pOpque, &g_ptCenter))
		{
			;
		}

		g_ptCenterAd = pY16Info->Y16Data[nW * g_ptCenter.PointY + g_ptCenter.PointX];

		float *pTempMatrix = new float[pY16Info->ImgWidth * pY16Info->ImgHeight];
		if (0 == GD_MTC_SDK_GetTempMatrix(pY16Info->pOpque, pTempMatrix))
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
		}

		delete[] pTempMatrix;
		pTempMatrix = 0;


		if (0 == GD_MTC_SDK_GetHighTemp(pOpque, &g_ptHigh))
		{
			;
		}
		if (0 == GD_MTC_SDK_GetLowTemp(pOpque, &g_ptLow))
		{
			;
		}
		if (0 == GD_MTC_SDK_GetAvegTemp(pOpque, &g_ptAvg))
		{
			;
		}


		// 	{
		// 		unsigned int x0 = 0;
		// 		unsigned int y0 = 479;
		// 		unsigned int x1 = 639;
		// 		unsigned int y1 = 0;
		// 		int info[5] = {0};
		//
		//
		//
		// 		if (0 == GD_MTC_SDK_GetRectTemperatureInfoE1(pOpque, x0, y0, x1, y1, info));
		// 		{
		// 			ft::log("GD_MTC_SDK_GetRectTemperatureInfoE1 return OK");
		//
		// 			g_ptLow.PointX = info[3] % nW;
		// 			g_ptLow.PointY = info[3] / nW;
		//
		// 			g_ptHigh.PointX = info[4] % nW;
		// 			g_ptHigh.PointY = info[4] / nW;
		//
		//
		// // 			g_ptLow.PointTemp = *(float *)&info[0];
		// // 			g_ptHigh.PointTemp = *(float *)&info[1];
		// // 			g_ptAvg.PointTemp = *(float *)&info[2];
		// 			g_ptLow.PointTemp = info[0] / 100.0f;
		// 			g_ptHigh.PointTemp = info[1] / 100.0f;
		// 			g_ptAvg.PointTemp = info[2] / 100.0f;
		// 		}
		//  	}



		g_ptHighAd = pY16Info->Y16Data[nW * g_ptHigh.PointY + g_ptHigh.PointX];
		g_ptLowAd = pY16Info->Y16Data[nW * g_ptLow.PointY + g_ptLow.PointX];



		g_Rect.X = 50;
		g_Rect.Y = 50;
		g_Rect.Width = 100;
		g_Rect.Height = 100;
		if (0 == GD_MTC_SDK_GetRectHighTemp(pOpque, g_Rect, &g_ptRectHigh))
		{
			;
		}
		if (0 == GD_MTC_SDK_GetRectLowTemp(pOpque, g_Rect, &g_ptRectLow))
		{
			;
		}
		if (0 == GD_MTC_SDK_GetRectAvegTemp(pOpque, g_Rect, &g_ptRectAvg))
		{
			;
		}

		if (g_bSnapPicNextFrame)
		{
			QString strFilePath = ft::exeDir();
			auto ba = strFilePath.toLatin1();
			GD_MTC_SDK_SnapPicture(pOpque, ba.data());

			g_bSnapPicNextFrame = false;
		}

		int nAlarmMaxFlag = pY16Info->y16OutputInfo.nDeviceRealTimeMaxAlarmFlag;
		int nAlarmMinFlag = pY16Info->y16OutputInfo.nDeviceRealTimeMinAlarmFlag;


		float fSpanMin = pY16Info->y16OutputInfo.fTemperatureAdjustLightSpanMin;
		float fSpanMax = pY16Info->y16OutputInfo.fTemperatureAdjustLightSpanMax;

		//if (pWidget)
		{
			g_bInY16Cb = true;
			emit this->y16Data(nAlarmMinFlag, nAlarmMaxFlag, fSpanMin, fSpanMax);
		}

		g_bFocusing = pY16Info->y16OutputInfo.nFocusing ? true : false;

		// 	// 
		// 	{
		// 		auto & v = pY16Info->y16OutputInfo;
		// 		ft::log(QString("fCurrentShutterTemp=%1,fPreShutterTemp=%2,fJpmTemp=%3,fLenTemp=%4").arg(v.fCurrentShutterTemp).arg(v.fPreShutterTemp).arg(v.fJpmTemp).arg(v.fLenTemp));
		// 	}

		return 0;
	}

	int NetWidget::linkStateCbFunS(const GD_MTC_CALLBACK_LinkStateInfo *LinkStateInfo, void *pUser)
	{
		int n1 = 0;
		auto n2 = LinkStateInfo->LinkState;
		if (-1 == n2)
		{
			// 未连接
			n1 = n1;
			emit this->linkStateChange(0);
		}
		else if (0 == n2)
		{
			// 已连接
			n1 = n1;
			emit this->linkStateChange(1);
		}
		n1 = n1;

		return 0;
	}

	int NetWidget::serarchDeviceCbFunS(const GD_MTC_IRDeviceInfo * pDeviceInfo, int nDeviceInfoSize, void *pUser)
	{
		if (nDeviceInfoSize > 0)
		{
			GD_MTC_IRDeviceInfo * pDeviceInfo2 = new GD_MTC_IRDeviceInfo[nDeviceInfoSize];
			memcpy(pDeviceInfo2, pDeviceInfo, sizeof(GD_MTC_IRDeviceInfo) * nDeviceInfoSize);
			emit this->searchDeviceResult(pDeviceInfo2, nDeviceInfoSize, pUser);
		}
		else if (0 == nDeviceInfoSize)
		{
			emit this->searchDeviceResult((GD_MTC_IRDeviceInfo *)0, 0, pUser);
		}

		return 0;
	}

	QString NetWidget::getIP(QLineEdit * p1, QLineEdit * p2, QLineEdit * p3, QLineEdit * p4)
	{
		QString strIp;
		auto fun = [](int n1) { return n1 >= 0 && n1 <= 255; };
		if (fun(p1->text().toInt()) &&
			fun(p2->text().toInt()) &&
			fun(p3->text().toInt()) &&
			fun(p4->text().toInt()))
		{
			strIp = QString("%1.%2.%3.%4")
				.arg(QString::number(p1->text().toInt()))
				.arg(QString::number(p2->text().toInt()))
				.arg(QString::number(p3->text().toInt()))
				.arg(QString::number(p4->text().toInt()));

			return strIp;
		}
		return "";
	}

	void NetWidget::OnBnClickedButtonSetPalette()
	{
		int n1 = ui.comboBox->currentIndex();
		m_nPaletteIndex = n1;
		if (NO_ERROR == GD_MTC_SDK_SetPalette(m_nSessionId, n1, 0))
		{
			m_joConfig["palette"] = QString::number(m_nPaletteIndex);
			this->writeConfigFile();
		}
		else
		{
			this->tip(u8"设置色带失败,可能当前正在irgd录像");
		}
	}

	void NetWidget::OnBnClickedButtonFrameRate()
	{
		int n1 = ui.comboBox_4->currentIndex();
		n1 += 1;
		if (NO_ERROR == GD_MTC_SDK_setY16FramePerSecond(m_nSessionId, n1, 0))
		{
			m_joConfig["fps"] = QString::number(n1);
			this->writeConfigFile();
		}
		else
		{
			this->tip(u8"设置帧率失败,可能当前正在irgd录像");
		}
	}

	void NetWidget::OnBnClickedButtonPicCenterTemp()
	{
		QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ft::exeDir(), tr("Image Files(*.jpg)"));
		if (path.length() > 0)
		{
			int nWidth = 0;
			int nHeight = 0;
			auto ba = path.toLatin1();
			if (NO_ERROR == GD_MTC_SDK_GetTempPictureDemension(ba.data(), &nWidth, &nHeight))
			{
				float fTemp = 0.0f;
				int nX = nWidth / 2;
				int nY = nHeight / 2;
				if (NO_ERROR == GD_MTC_SDK_GetPointTemperatureFromPicture(ba.data(), nX, nY, &fTemp))
				{
					QMessageBox::information(NULL, tr("Path"), tr("Center point temperature is [%1,%2]=%3").arg(nX).arg(nY).arg(fTemp));
				}
				else
				{
					this->tip(u8"获取图片中心温失败");
				}
			}
			else
			{
				this->tip(u8"获取图片中心温失败");
			}
		}
	}

	void NetWidget::OnBnClickedButtonVideoCenterTemp()
	{
		QString path = QFileDialog::getOpenFileName(this, tr("Open IRGD"), ft::exeDir(), tr("IRGD Files(*.irgd)"));
		if (path.length() > 0)
		{
			int nWidth = 0;
			int nHeight = 0;
			float fTemp = 0.0f;
			auto ba = path.toLatin1();
			if (NO_ERROR == GD_MTC_SDK_GetTempVideoFristFrameTemp(ba.data(), &nWidth, &nHeight, &fTemp))
			{
				int nX = nWidth;
				int nY = nHeight;

				QMessageBox::information(NULL, tr("Path"), tr("Center point temperature is [%1,%2]=%3").arg(nX).arg(nY).arg(fTemp));
			}
			else
			{
				this->tip(u8"获取视频中心温失败");
			}
		}
	}

	void NetWidget::OnBnClickedButtonPicTempMatrix()
	{
		QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ft::exeDir(), tr("Image Files(*.jpg)"));
		if (path.length() > 0)
		{
			int nWidth = 0;
			int nHeight = 0;
			auto ba = path.toLatin1();
			if (NO_ERROR == GD_MTC_SDK_GetTempPictureDemension(ba.data(), &nWidth, &nHeight))
			{
				float fTemp = 0.0f;
				int nX = nWidth / 2;
				int nY = nHeight / 2;
				float* imageT = new float[nWidth * nHeight];
				if (NO_ERROR == GD_MTC_SDK_GetGlobalTemperatureFromPicture(ba.data(), imageT))
				{
					fTemp = imageT[nWidth * nY + nX];

					QMessageBox::information(NULL, tr("Path"), tr("Center point temperature is [%1,%2]=%3").arg(nX).arg(nY).arg(fTemp));

					float fMax = *std::max_element(imageT, imageT + nWidth * nHeight);
					float fMin = *std::min_element(imageT, imageT + nWidth * nHeight);
					float fAvg = std::accumulate(imageT, imageT + nWidth * nHeight, 0.0f) / (nWidth * nHeight);
					QMessageBox::information(NULL, tr("Path"), tr("fMax=%1, fMin=%2, fAvg=%3").arg(fMax).arg(fMin).arg(fAvg));
				}
				else
				{
					this->tip(u8"获取图片温度矩阵失败");
				}
				delete[] imageT;
				imageT = 0;
			}
			else
			{
				this->tip(u8"获取图片温度矩阵失败");
			}
		}
	}

	void NetWidget::OnBnClickedButtonPicTempMax()
	{
		QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ft::exeDir(), tr("Image Files(*.jpg)"));
		if (path.length() > 0)
		{
			int nWidth = 0;
			int nHeight = 0;
			auto ba = path.toLatin1();
			if (NO_ERROR == GD_MTC_SDK_GetTempPictureDemension(ba.data(), &nWidth, &nHeight))
			{
				POINT_INFO maxInfo;
				POINT_INFO minInfo;
				float avgT;

				if (NO_ERROR == GD_MTC_SDK_GetRectTemperatureFromPicture(ba.data(), 0, 0, nWidth, nHeight, &maxInfo, &minInfo, &avgT))
				{
					float fMax = maxInfo.temp;
					float fMin = minInfo.temp;
					float fAvg = avgT;
					QMessageBox::information(NULL, tr("Path"), tr("fMax=%1, fMin=%2, fAvg=%3").arg(fMax).arg(fMin).arg(fAvg));
				}
				else
				{
					this->tip(u8"获取图片矩形温失败");
				}


				// 新接口验证
				{

					unsigned int x0 = 0;
					unsigned int y0 = nHeight;
					unsigned int x1 = nWidth;
					unsigned int y1 = 0;
					int info[5] = { 0 };

					if (0 == GD_MTC_SDK_GetRectTemperatureFromPictureE1(ba.data(), x0, y0, x1, y1, info))
					{
						ft::log("Call GD_MTC_SDK_GetRectTemperatureFromPictureE1 return OK");

						GD_MTC_TempPointInfo tpMax;
						GD_MTC_TempPointInfo tpMin;
						GD_MTC_TempPointInfo tpAvg;

						// 					tpMin.PointTemp = *(float *)&info[0];
						// 					tpMax.PointTemp = *(float *)&info[1];
						// 					tpAvg.PointTemp = *(float *)&info[2];
						tpMin.PointTemp = info[0] / 100.0f;
						tpMax.PointTemp = info[1] / 100.0f;
						tpAvg.PointTemp = info[2] / 100.0f;


						tpMin.PointX = info[3] % nWidth;
						tpMin.PointY = info[3] / nWidth;

						tpMax.PointX = info[4] % nWidth;
						tpMax.PointY = info[4] / nWidth;

						ft::log(QString("Min[x,y,f]=[%1,%2,%3], Max[x,y,f]=[%4,%5,%6], Avg[x,y,f]=[%7,%8,%9]")
							.arg(tpMin.PointX).arg(tpMin.PointY).arg(tpMin.PointTemp)
							.arg(tpMax.PointX).arg(tpMax.PointY).arg(tpMax.PointTemp)
							.arg(tpAvg.PointX).arg(tpAvg.PointY).arg(tpAvg.PointTemp)
						);
					}
				}
			}
			else
			{
				this->tip(u8"获取图片矩形温失败");
			}
		}
	}

	bool NetWidget::startLink()
	{
		GD_MTC_IRDeviceInfo DeviceInfo;
		QString ipAddress = "192.168.1.169";
		if (ipAddress.isEmpty())
		{
			return false;
		}

		QByteArray c = ipAddress.toLocal8Bit();
		strcpy(DeviceInfo.IpAddress, c.data());
		DeviceInfo.DevType = DEVICE_TYPE_UNKNOWN;

		int iniSdk = GD_MTC_SDK_Init();
		if (!iniSdk == NO_ERROR)
		{
			int n1 = 0;
			return false;
		}


		int iLoginID = GD_MTC_SDK_Login(DeviceInfo, 0);
		if (iLoginID == ERROR_LOGIN_FAIL)
		{
			int n1 = 0;
			return false;
		}

		int n1 = 0;
		return true;
	}

	void NetWidget::onBtnTest()
	{
		//
		{
			static int nCount = 0;
			static int nLogId = 0;
			static int nSessionId = 0;
			++nCount;
			if (1 == nCount)
			{
				GD_MTC_SDK_Test6("192.168.1.168", 4, &nLogId, &nSessionId);
			}
			else if (2 == nCount ||
				3 == nCount ||
				4 == nCount)
			{
				GD_MTC_SDK_Test8(nSessionId, "D:\\\\test3.jpg");
			}
			else if (5 == nCount)
			{
				GD_MTC_SDK_Test7(nLogId, nSessionId);
				nCount = 0;
			}
		}

		//GD_MTC_SDK_Test5("192.168.1.168", "D:\\\\test3.jpg", 4);

		//GD_MTC_SDK_Test4("192.168.1.168", );
		//GD_MTC_SDK_Test3();
// 		{
// 			auto nLogId = GD_MTC_SDK_Login2("192.168.1.168");
// 			ft::log(tr("nLogId=%1").arg(nLogId));
// 			if (ERROR_LOGIN_FAIL != nLogId)
// 			{
// 				auto nSessionId = GD_MTC_SDK_OpenStream3(nLogId);
// 				ft::log(tr("nSessionId=%1").arg(nSessionId));
// 				if (ERROR_OPENSTREAM_FAIL != nSessionId)
// 				{
// 					QThread::sleep(5);
// 					GD_MTC_SDK_SnapPictureUniform2(nSessionId, "D:\\\\test.jpg");
// 
// 					GD_MTC_SDK_CloseStream(nSessionId);
// 					GD_MTC_SDK_Logout(nLogId);
// 				}
// 				else
// 				{
// 					ft::logError("openStream failed.");
// 
// 					GD_MTC_SDK_Logout(nLogId);
// 				}
// 			}
// 			else
// 			{
// 				ft::logError("Login failed.");
// 			}
// 		}


// 		{
// 			int nState = 0;
// 			int nLen = 4;
// 			if (0 == GD_MTC_SDK_GetDeviceState(m_nSessionId, &nState, &nLen))
// 			{
// 				this->tip(QString("nState=%1").arg(nState));
// 			}
// 			else
// 			{
// 				this->tip("call GD_MTC_SDK_GetDeviceState return failed.");
// 			}
// 		}

		// 	{
		// 		static int nFlag = 0;
		// 		if (1 == nFlag)
		// 		{
		// 			GD_MTC_SDK_EnableRectAnalyserDigitalTempWithCursor(m_nSessionId);
		// 		}
		// 		else
		// 		{
		// 			GD_MTC_SDK_DisableRectAnalyserDigitalTempWithCursor(m_nSessionId);
		// 		}
		// 		
		// 		++nFlag;
		// 		nFlag %= 2;
		// 	}


		// 	{
		// 		static int nFlag = 0;
		// 		GD_MTC_SDK_SetImageFlip(m_nSessionId, nFlag);
		// 		++nFlag;
		// 		nFlag %= 3;
		// 	}


		// 	static int nCount = 1;
		// 	static int nLogId = -1;
		// 	static auto nPullDataType = PULL_DATA_TYPE_H264_ONLY;
		// 	if (1 == nCount)
		// 	{
		// 		GD_MTC_IRDeviceInfo DeviceInfo;
		//
		// 		m_strIp = "192.168.1.168";
		//
		// 		auto ba = m_strIp.toLatin1();
		// 		strcpy(DeviceInfo.IpAddress, ba.data());
		//
		// 		//DeviceInfo.DevType = nDeviceType;
		// 		DeviceInfo.DevType = DEVICE_TYPE_UNKNOWN;
		// 		//DeviceInfo.DevType = m_nDeviceType;
		// 		//DeviceInfo.DevType = GD_MTC_IRDeviceType::OR2;
		//
		// 		nLogId = GD_MTC_SDK_Login(DeviceInfo, 0);
		// 		ft::log(tr("nLogId=%1").arg(nLogId));
		// 		if (ERROR_LOGIN_FAIL != nLogId)
		// 		{
		// 			m_nLoginId = nLogId;
		//
		//
		// 		}
		// 		else
		// 		{
		// 			ft::logError("Login failed.");
		// 		}
		// 	}
		// 	else if (2 == nCount || 4 == nCount)
		// 	{
		// 		GD_MTC_IRLinkInfo IRLinkInfo;
		// 		memset(&IRLinkInfo, 0, sizeof(IRLinkInfo));
		//
		// 		IRLinkInfo.RGBCB = rgbCbFun;
		// 		IRLinkInfo.Y16CB = y16CbFun;
		// 		IRLinkInfo.LinkStateCB = linkStateCbFun;
		//
		// 		IRLinkInfo.nPullDataType = nPullDataType;
		//
		// 		int nPaletteIndex = 0;
		// 		int nFps = 12;
		//
		// 		if (m_bConfigOk)
		// 		{
		// 			nPaletteIndex = m_joConfig["palette"].toString().toInt();
		// 			nFps = m_joConfig["fps"].toString().toInt();
		// 		}
		//
		// 		//auto nSessionId = GD_MTC_SDK_OpenStream(nLogId, &IRLinkInfo, 0, nFps, 0);
		// 		auto nSessionId = GD_MTC_SDK_OpenStream(nLogId, &IRLinkInfo, nPaletteIndex, nFps, (int *)this);
		// 		ft::log(tr("nSessionId=%1").arg(nSessionId));
		// 		if (ERROR_OPENSTREAM_FAIL != nSessionId)
		// 		{
		// 			m_nPaletteIndex = nPaletteIndex;
		//
		// 			m_bDataStreamIsOpen = true;
		// 			m_bSetSpanInitValue = false;
		// 			m_nPullDataType = nPullDataType;
		// 			m_nSessionId = nSessionId;
		//
		//
		// 			ui.radioButton->setChecked(true);
		// 			this->onRaioBtnAdjustLightMode();
		// 			ui.comboBox->setCurrentIndex(nPaletteIndex);
		// 			ui.comboBox_4->setCurrentIndex(nFps - 1);
		//
		// 			// Test
		// 			// 			{
		// 			// 				m_nSessionId = nSessionId;
		// 			// 				return;
		// 			// 			}
		//
		// 			GD_MTC_IRDeviceInfo deviceInfo;
		// 			ft::log("GD_MTC_SDK_GetDeviceInfo");
		// 			int nRtn = GD_MTC_SDK_GetDeviceInfo(m_nLoginId, &deviceInfo, 0);
		// 			if (ERROR_GETDEVICEINFO_FAIL != nRtn)
		// 			{
		// 				m_deviceInfo = deviceInfo;
		//
		// 				m_nDeviceType = deviceInfo.DevType;
		// 				this->updataControlState();
		// 				{
		// 					ft::log(QString("deviceInfo.DevType=%1").arg(deviceInfo.DevType));
		// 				}
		// 				this->setWindowTitle(QString::fromStdString(deviceTypeToStr(deviceInfo.DevType)));
		// 				int n1 = 0;
		// 				m_nResizeCount = 2;
		//
		// 				m_joConfig["ip"] = m_strIp;
		// 				m_joConfig["mode"] = QString::number((int)nPullDataType);
		// 				m_joConfig["fps"] = QString::number(nFps);
		// 				m_joConfig["palette"] = QString::number(nPaletteIndex);
		// 				this->writeConfigFile();
		//
		// 				// 设置电子变焦的中心坐标为图像中心点
		// 				{
		// 					ui.lineEdit_75->setText(QString::number(deviceInfo.ImgWidth / 2));
		// 					ui.lineEdit_76->setText(QString::number(deviceInfo.ImgHeight / 2));
		// 				}
		//
		// 				// 获取色带信息
		// 				this->getPaletteData();
		//
		// 				m_nPullDataType = nPullDataType;
		// 				this->updataControlState();
		// 				{
		// 					ft::log(QString("deviceInfo.DevType=%1").arg(m_nDeviceType));
		// 				}
		// 				this->setWindowTitle(QString::fromStdString(deviceTypeToStr(m_nDeviceType)));
		// 				m_nResizeCount = 2;
		//
		// 				//m_nSessionId = nSessionId;
		// 			}
		// 			else
		// 			{
		// 				ft::log("GD_MTC_SDK_GetDeviceInfo failed.");
		// 				int n1 = 0;
		// 				this->tip(QString(u8"获取设备信息失败"));
		//
		//
		// 				this->OnBnClickedButtonCloseStream();
		// 			}
		// 		}
		// 		else
		// 		{
		// 			ft::logError("openStream failed.");
		//
		// 			GD_MTC_SDK_Logout(m_nLoginId);
		// 		}
		// 	}
		// 	else if (3 == nCount || 5 == nCount)
		// 	{
		// 		if (-1 != m_nSessionId)
		// 		{
		// 			int n1 = GD_MTC_SDK_CloseStream(m_nSessionId);
		// 			if (n1 != NO_ERROR)
		// 			{
		// 				ft::logError(QString("Call GD_MTC_SDK_CloseStream return %1").arg(n1));
		// 			}
		// 			m_nSessionId = -1;
		// 		}
		// 	}
		// 	else if (6 == nCount)
		// 	{
		// 		if (-1 != m_nLoginId)
		// 		{
		// 			int n1 = GD_MTC_SDK_Logout(m_nLoginId);
		// 			if (n1 != NO_ERROR)
		// 			{
		// 				ft::logError(QString("Call GD_MTC_SDK_Logout return %1").arg(n1));
		// 			}
		// 			m_nLoginId = -1;
		// 		}
		// 	}
		//
		// 	++nCount;
		//
		// 	if (7 == nCount)
		// 	{
		// 		nCount = 1;
		// 	}

		// 	static bool m_isStopConnect = false;
		// 	static bool m_isConnected = false;
		// 	int FailnCount = 0;
		//
		// 	while (!m_isStopConnect)
		// 	{
		// 		if (!m_isConnected && this->startLink())
		// 		{
		// 			int n1 = 0;
		//
		// 			m_isConnected = true;
		// 		}
		// 		else
		// 		{
		// 			if (m_isStopConnect)
		// 			{
		// 				return;
		// 			}
		// 			else
		// 			{
		// 				int n1 = 0;
		// 			}
		// 			QThread::sleep(1);
		//
		// 			++FailnCount;
		// 			//this->setWindowTitle(QString("FailnCount=%1").arg(FailnCount));
		// 			ft::log(QString("FailnCount=%1").arg(FailnCount));
		// 			if (FailnCount > 0x7FFFFFFF)
		// 			{
		// 				return;
		// 			}
		// 		}
		// 	}
	}

	void NetWidget::OnBnClickedBtnFocusAuto()
	{
		GD_MTC_SDK_SendNetWorkCmd(m_nSessionId, Focus, 3, 0);
	}

	void NetWidget::onBtnPressedNearFocus()
	{
		if (NO_ERROR != GD_MTC_SDK_SendNetWorkCmd(m_nSessionId, Focus, 2, 0))
		{
			ft::logError(QString("call %1 return fail").arg("onBtnPressedNearFocus"));
		}
		else
		{
			ft::log(QString("call %1 return OK").arg("onBtnPressedNearFocus"));
		}
	}

	void NetWidget::onBtnPressedFarFocus()
	{
		if (NO_ERROR != GD_MTC_SDK_SendNetWorkCmd(m_nSessionId, Focus, 1, 0))
		{
			ft::logError(QString("call %1 return fail").arg("onBtnPressedFarFocus"));
		}
		else
		{
			ft::log(QString("call %1 return OK").arg("onBtnPressedFarFocus"));
		}
	}

	void NetWidget::onBtnPressedNearFocusFine()
	{
		GD_MTC_SDK_SendNetWorkCmd(m_nSessionId, Focus, 5, 0);
	}

	void NetWidget::onBtnPressedFarFocusFine()
	{
		GD_MTC_SDK_SendNetWorkCmd(m_nSessionId, Focus, 4, 0);
	}

	void NetWidget::onBtnReleasedNearFocus()
	{
		if (NO_ERROR != GD_MTC_SDK_SendNetWorkCmd(m_nSessionId, Focus, 0, 0))
		{
			ft::logError(QString("call %1 return fail").arg("onBtnReleasedNearFocus"));
		}
		else
		{
			ft::log(QString("call %1 return OK").arg("onBtnReleasedNearFocus"));
		}
	}

	void NetWidget::onBtnReleasedFarFocus()
	{
		if (NO_ERROR != GD_MTC_SDK_SendNetWorkCmd(m_nSessionId, Focus, 0, 0))
		{
			ft::logError(QString("call %1 return fail").arg("onBtnReleasedFarFocus"));
		}
		else
		{
			ft::log(QString("call %1 return OK").arg("onBtnReleasedFarFocus"));
		}
	}

	void NetWidget::OnBnClickedBtnMotorposGet2()
	{
		int nPos = 0;
		int nRtn = GD_MTC_SDK_getMotorPostition(m_nLoginId, &nPos, 0);
		if (NO_ERROR == nRtn)
		{
			ui.lineEdit_14->setText(QString::number(nPos));
		}
		else
		{
			int n1 = 0;
		}
	}

	void NetWidget::OnBnClickedBtnMotorposSet2()
	{
		QString strPos = ui.lineEdit_14->text();
		bool bOk = false;
		int nPos = strPos.toInt(&bOk);

		// 数据有效性验证
		{
			if (false == this->isValidInt(strPos) || false == bOk)
			{
				this->tip(u8"输入的电机位置不是一个有效的整数");
				return;
			}

			int nMin = m_deviceInfo.nMotorPosMin;
			int nMax = m_deviceInfo.nMotorPosMax;

			if (nPos < nMin || nPos > nMax)
			{
				this->tip(QString(u8"电机位置超出[%1, %2]范围").arg(nMin).arg(nMax));
				return;
			}
		}

		int nRtn = GD_MTC_SDK_setMotorPostition(m_nLoginId, nPos, 0);
		if (NO_ERROR == nRtn)
		{
			int n1 = 0;
		}
		else
		{
			int n1 = 0;
			this->tip(QString(u8"设置电机位置失败"));
		}
	}

}

