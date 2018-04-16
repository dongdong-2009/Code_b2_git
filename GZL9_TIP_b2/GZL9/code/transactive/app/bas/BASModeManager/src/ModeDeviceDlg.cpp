// src\ModeDeviceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASModeManager/src/BASManager.h"
#include "app/bas/BASModeManager/src/ModeDeviceDlg.h"

namespace TA_IRS_App
{
	#define Timer_FillWith_ListCtrl_Data 1
	#define Timer_FillWith_ListCtrl_Data_Time 300
	// CModeDeviceDlg dialog
	using namespace TA_Base_Core;
	IMPLEMENT_DYNAMIC(CModeDeviceDlg, CDialog)
	CModeDeviceDlg::CModeDeviceDlg(Mode_SmartPtr mode, CWnd* pParent /*=NULL*/)
		: CDialog(CModeDeviceDlg::IDD, pParent)
		,m_mode(mode),m_initDlgDataThread(NULL)
	{
		m_isStopLoading = false;
	}

	CModeDeviceDlg::~CModeDeviceDlg()
	{
	}

	void CModeDeviceDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_LIST_DEVICE, m_deviceList);
	}


	BEGIN_MESSAGE_MAP(CModeDeviceDlg, CDialog)
		ON_WM_SIZE()
		ON_WM_TIMER()
		ON_BN_CLICKED(IDCANCEL, &CModeDeviceDlg::OnBnClickedCancel)
	END_MESSAGE_MAP()


	// CModeDeviceDlg message handlers
	BOOL CModeDeviceDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();
		m_deviceList.Initialise();
		ModeStatusListenerManagerInstance::instance()->addListener(this);
		SetTimer(Timer_FillWith_ListCtrl_Data, Timer_FillWith_ListCtrl_Data_Time, NULL);
		m_initDlgDataThread = new BasManagerActionThread(this);
		m_initDlgDataThread->setAction(BasManagerAction_Initialize);
		m_initDlgDataThread->start();

		CString strCaption = "";
		strCaption.Format("%s - %s  设备列表", m_mode->getName().c_str(), m_mode->getDescription().c_str() );
		SetWindowText(strCaption);
		GetDlgItem(IDCANCEL)->SetWindowText(_T("退出"));
		PostMessage(WM_SIZE, 0 ,0);
		return TRUE;
	}

	void CModeDeviceDlg::OnTimer(UINT_PTR nIDEvent)
	{
		if (Timer_FillWith_ListCtrl_Data == nIDEvent)
		{
			KillTimer(Timer_FillWith_ListCtrl_Data);
			//do something
		}
		CDialog::OnTimer(nIDEvent);
	}

	void CModeDeviceDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialog::OnSize(nType, cx, cy);

		// TODO: Add your message handler code here
		if (FALSE != IsWindow(this->m_hWnd) && BAS_IsWindow(IDC_LIST_DEVICE))
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			CRect btnCancelRect;
			GetDlgItem(IDCANCEL)->GetWindowRect(&btnCancelRect);

			CRect deviceListRect;
			deviceListRect.left = clientRect.left + gDlgLeftBlank;
			deviceListRect.top = clientRect.top + gDlgTopBlank;
			deviceListRect.right = clientRect.right - gDlgRightBlank;
			deviceListRect.bottom = clientRect.bottom - 2 * gDlgBottomBlank - btnCancelRect.Height();
			GetDlgItem(IDC_LIST_DEVICE)->MoveWindow(deviceListRect, FALSE);
			BAS_MoveFixedSizeCtrl(IDCANCEL, 
				deviceListRect.right - btnCancelRect.Width(), deviceListRect.bottom + gDlgBottomBlank);
		
			Invalidate(TRUE);
		}
	}

	void CModeDeviceDlg::updateModeStatus(unsigned long modeKey, int status)
	{
		FUNCTION_ENTRY("updateModeStatus");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Receive a mode status update message modeKey=%d", modeKey);

		FUNCTION_EXIT;
	}

	void CModeDeviceDlg::OnBnClickedCancel()
	{
		// TODO: Add your control notification handler code here
		m_isStopLoading = true;
		ModeStatusListenerManagerInstance::instance()->removeListener(this);
		DeviceEquipmentManagerInstance::instance()->resetListener(NULL);
		KillTimer(Timer_FillWith_ListCtrl_Data);
		if (NULL != m_initDlgDataThread)
		{
			m_deviceList.stopFillwithModeDevice();
			//like++ GF-78; wait until initialize finished.
			CWaitCursor wc;
			m_initDlgDataThread->terminateAndWait();

			delete m_initDlgDataThread;
			m_initDlgDataThread = NULL;
			//unsigned long sleepTime = 100;
			//unsigned long loopCount = 0;
			//while( TA_Base_Core::Thread::THREAD_STATE_TERMINATED != m_initDlgDataThread->getCurrentState())
			//{
			//	Sleep(sleepTime);
			//	if( loopCount++ > 100) // most time out value is 100*100 millisecond
			//	{
			//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, " wait time out............");
			//		break;
			//	}
			//}
			//++like
		}
		CDialog::OnCancel();
	}

	void  CModeDeviceDlg::executeThreadFunction(BasManagerAction action)
	{
		FUNCTION_ENTRY("executeThreadFunction");
		switch(action)
		{
		case BasManagerAction_Initialize:
			InitializeDlgData();
			StartMonitorDevices();
			break;
		}
		FUNCTION_EXIT;

	}

	void CModeDeviceDlg::InitializeDlgData()
	{
		FUNCTION_ENTRY("InitializeDlgData");
		BAS_TRY
		{
			m_deviceList.fillwithModeDevice(m_mode->getLocationKey(), m_mode->getKey());
			if (!m_isStopLoading)
			{
				if (m_deviceList.GetItemCount() <= 0)
				{
					//MessageBox(_T("没有找到相应的设备列表，请检查数据库配置！"), MessageBox_Caption, MB_OK | MB_ICONERROR);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "There no found the mode device list,ModeKey:%d,ModeName:%s",
						m_mode->getKey(), m_mode->getName().c_str());
				}
			}
		}
		BAS_CATCH_UNDEPLOY_EXCEPTION;
		if (BAS_WhetherHaveError)
		{
			//MessageBox(_T("程序初始化过程中异常，管理器可能将无法正常工作！"), MessageBox_Caption, MB_OK | MB_ICONERROR);
		}

		FUNCTION_EXIT;
	}

	void CModeDeviceDlg::StartMonitorDevices()
	{
		FUNCTION_ENTRY("StartMonitorDevices");

		FUNCTION_EXIT;
	}
}

