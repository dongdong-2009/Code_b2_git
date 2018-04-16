// TunnelModeManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManager.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelModeBaseDialog.h"
#include "app/bas/TunnelModeManager/src/TunnelFireModeDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelCongestionModeDlg.h"
#include "app/bas/TunnelModeManager/src/TunnelModeSelectDlg.h"
#include "bus/generic_gui/src/AppLauncher.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace TA_IRS_App
{
	CTunnelModeManagerDlg::CTunnelModeManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent /*= NULL*/)
		: TA_Base_Bus::TransActiveDialog(controlClass, CTunnelModeManagerDlg::IDD, pParent)
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		m_implementDialog = NULL;
	}

	void CTunnelModeManagerDlg::DoDataExchange(CDataExchange* pDX)
	{
		TA_Base_Bus::TransActiveDialog::DoDataExchange(pDX);
	}

	BEGIN_MESSAGE_MAP(CTunnelModeManagerDlg, TA_Base_Bus::TransActiveDialog)
		ON_WM_SYSCOMMAND()
		ON_WM_PAINT()
		ON_WM_SIZE()
		ON_WM_QUERYDRAGICON()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	// CTunnelModeManagerDlg message handlers

	BOOL CTunnelModeManagerDlg::OnInitDialog()
	{
		InitializeImplement();
		setResizingProperties(m_implementDialog->getResizingProperties());
		TA_Base_Bus::TransActiveDialog::OnInitDialog();

		// Set the icon for this dialog.  The framework does this automatically
		//  when the application's main window is not a dialog
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon

		// TODO: Add extra initialization here
		ShowImplementDialog();
		return TRUE;  // return TRUE  unless you set the focus to a control
	}

	// If you add a minimize button to your dialog, you will need the code below
	//  to draw the icon.  For MFC applications using the document/view model,
	//  this is automatically done for you by the framework.

	void CTunnelModeManagerDlg::OnPaint()
	{
		if (IsIconic())
		{
			CPaintDC dc(this); // device context for painting

			SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

			// Center icon in client rectangle
			int cxIcon = GetSystemMetrics(SM_CXICON);
			int cyIcon = GetSystemMetrics(SM_CYICON);
			CRect rect;
			GetClientRect(&rect);
			int x = (rect.Width() - cxIcon + 1) / 2;
			int y = (rect.Height() - cyIcon + 1) / 2;

			// Draw the icon
			dc.DrawIcon(x, y, m_hIcon);
		}
		else
		{
			CDialog::OnPaint();
		}
	}

	// The system calls this function to obtain the cursor to display while the user drags
	//  the minimized window.
	HCURSOR CTunnelModeManagerDlg::OnQueryDragIcon()
	{
		return static_cast<HCURSOR>(m_hIcon);
	}

	void CTunnelModeManagerDlg::InitializeImplement()
	{
		FUNCTION_ENTRY("InitializeImplement");
		using namespace TA_Base_Core;
		if (RunParams::getInstance().isSet(RPARAM_TUNNELMODE_TYPE))
		{
			std::string strTimeTableTypeValue = RunParams::getInstance().get(RPARAM_TUNNELMODE_TYPE);
			if(0 == strTimeTableTypeValue.compare(RPARAM_TUNNELMODE_TYPE_VALUE_SELECTION))
			{
				m_implementDialog = new CTunnelModeSelectDlg(this);
				m_implementDialog->Create(CTunnelModeSelectDlg::IDD, this);
			}
			else if (0 == strTimeTableTypeValue.compare(RPARAM_TUNNELMODE_TYPE_VALUE_FIRE))
			{
				m_implementDialog = new CTunnelFireModeDlg(this);
				m_implementDialog->Create(CTunnelFireModeDlg::IDD, this);
			}
			else if (0 == strTimeTableTypeValue.compare(RPARAM_TUNNELMODE_TYPE_VALUE_CONGESTION))
			{
				m_implementDialog = new CTunnelCongestionModeDlg(this);
				m_implementDialog->Create(CTunnelCongestionModeDlg::IDD, this);
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Please Check the Run Parameter : tunnel mode Invalid.");
				return;
			}
			m_implementDialog->ShowWindow(FALSE);
		}

		FUNCTION_EXIT;
	}

	void CTunnelModeManagerDlg::ShowImplementDialog()
	{
        DlgInitializeSize rpSize = m_implementDialog->getDlgInitSize();
        setResizingProperties( m_implementDialog->getResizingProperties() );
        m_implementDialog->ShowWindow(TRUE);
        SetWindowText(m_implementDialog->getApplicationCaption().c_str());

        RECT clientRect;
        GetClientRect( &clientRect );
        ClientToScreen( &clientRect );
        int nResizeWidth = (( rpSize.nWidth - clientRect.right + clientRect.left ) / 2 );
        int nResizeHeight = (( rpSize.nHeight - clientRect.bottom + clientRect.top ) / 2 );
        clientRect.left -= nResizeWidth;
        clientRect.top -= nResizeHeight;
        clientRect.right += nResizeWidth;
        clientRect.bottom += nResizeHeight;
        MoveWindow( &clientRect, TRUE );
	}

	void CTunnelModeManagerDlg::OnSize(UINT nType, int cx, int cy)
	{
		TA_Base_Bus::TransActiveDialog::OnSize(nType, cx, cy);
		if (NULL != m_implementDialog && IsWindow(m_implementDialog->GetSafeHwnd()))
		{
			CRect clientRect;
			GetClientRect(&clientRect);
			m_implementDialog->MoveWindow(&clientRect);
		}
	}

	void CTunnelModeManagerDlg::OnClose()
	{
		if(m_implementDialog->canCloseWindow())
		{
			m_implementDialog->SendMessage(WM_QUIT, 0, 0);
			PostMessage(WM_QUIT, 0, 0);
		}
		else
		{
			MessageBox("程序正在执行重要步骤暂时不能退出，请稍后再试！");
		}
	}
}
