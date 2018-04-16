#include "stdafx.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include "app/cctv/video_display_manager/src/QcapDlg.h"
//#include "app/cctv/QCAP/INC/QCAP.H"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
	IMPLEMENT_DYNAMIC(QcapDlg, CDialog)

	QcapDlg::QcapDlg(UINT dlgId, CWnd* pParent /*=NULL*/)
		: CDialog(dlgId, pParent),
		  m_hwHandle(NULL),
		  m_playWndHandle(NULL)
	{

	}

	QcapDlg::~QcapDlg()
	{
	}

	BOOL QcapDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();
		//CoInitialize(NULL);
		return TRUE;
	}

	void QcapDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
	}

	void QcapDlg::setPlayWindowHandle(HWND playWnd)
	{
		TA_ASSERT(m_playWndHandle == NULL, "");
		m_playWndHandle = playWnd;
	}

	void QcapDlg::initHw()
	{
		/*
#ifndef LOCAL_TEST
		TA_ASSERT(m_playWndHandle != NULL, "");
		if(m_hwHandle != NULL)
		{
			return;
		}
		QRESULT r = QCAP_CREATE( CAPTURE_CARD_DEVICE_NAME, 0, m_playWndHandle, &m_hwHandle);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "QCAP_CREATE return value:%d", r);
		if(m_hwHandle != NULL)
		{
			r = QCAP_SET_VIDEO_INPUT(m_hwHandle, QCAP_INPUT_TYPE_AUTO);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "QCAP_SET_VIDEO_INPUT return value:%d", r);
		}
#endif
		*/
	}

	void QcapDlg::uninitHw()
	{/*
#ifndef LOCAL_TEST
		if(m_hwHandle == NULL)
		{
			return;
		}
		QCAP_DESTROY(m_hwHandle);
		m_hwHandle = NULL;
#endif*/
	}

	void QcapDlg::play()
	{/*
#ifndef LOCAL_TEST
		initHw();
		if(m_hwHandle != NULL)
		{
			QCAP_RUN(m_hwHandle);
		}
#endif
		ShowWindow(SW_SHOW);*/
	}

	void QcapDlg::stop()
	{/*
		ShowWindow(SW_HIDE);
#ifndef LOCAL_TEST
		if(m_hwHandle != NULL)
		{
			QCAP_STOP(m_hwHandle);
			uninitHw();
		}
#endif*/
	}

	BEGIN_MESSAGE_MAP(QcapDlg, CDialog)
		ON_WM_DESTROY()
		ON_WM_CLOSE()
	END_MESSAGE_MAP()

	void QcapDlg::OnClose()
	{
		stop();
	}

	void QcapDlg::OnDestroy()
	{
		uninitHw();
		//CoUninitialize();
		CDialog::OnDestroy();
	}

    BOOL QcapDlg::PreTranslateMessage(MSG* pMsg)
    {
		if(WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
        {
			if(IsZoomed())
			{
				ModifyStyle(0, WS_CAPTION);
				ShowWindow(SW_SHOWNORMAL);
			}
			return TRUE;
        }
		return CDialog::PreTranslateMessage(pMsg);
    }
}

