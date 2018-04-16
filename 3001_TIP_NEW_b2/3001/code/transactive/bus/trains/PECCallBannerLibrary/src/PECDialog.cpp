/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #10 $
  *
  * Last modification: $2008-01-14$
  * Last modified by:  $LiangHua Yuan$
  *
  */

#include "bus/trains/PECCallBannerLibrary/src/PECDialog.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallEntry.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/HelpLauncher.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_gui/src/applauncher.h"
#include <windows.h>

#define DISABLE_BUTTON	FALSE
#define ENABLE_BUTTON	TRUE


namespace TA_IRS_Bus
{

    const std::string PECDialog::STATUS_BAR_CALL_LOST_MESSAGE = "PEC Audio Line has been disconnected.";
    const std::string PECDialog::STATUS_BAR_PEC_RESET_MESSAGE = "PEC has been reset.";
    const std::string PECDialog::STATUS_BAR_MPU_FAILURE_MESSAGE = "PEC has been reset due to MPU failure";
    const std::string PECDialog::STATUS_BAR_ANSWER_IN_PROGRESS_MESSAGE = "Answering PEC...";
    const std::string PECDialog::STATUS_BAR_ACTIVATING_CCTV_MESSAGE = "Activating CCTV...";
    const std::string PECDialog::STATUS_BAR_PEC_ANSWERED_MESSAGE = "PEC Answered";
	const std::string PECDialog::STATUS_CONNECTION_ERROR = "Error encountered while establishing Audio";
	const std::string PECDialog::PEC_MESSAGE_BOX_CAPTION = "PEC Call Error";
    const COLORREF PECDialog::STATUS_BAR_ERROR_COLOUR = RGB( 255, 128, 128 );
    const COLORREF PECDialog::STATUS_BAR_OK_COLOUR = GetSysColor( COLOR_3DFACE );
    const DWORD PECDialog::DISPLAY_PEC_STATUS = WM_USER + 1;
    const DWORD PECDialog::REFRESH_PEC_DETAILS = WM_USER + 2;
	HHOOK PECDialog::m_hhookCBTProc = 0;



    PECDialog::PECDialog( PECCallEntry* callEntry, CWnd* pParent )
            : CDialog( PECDialog::IDD, pParent ),
              m_pCallEntry( callEntry ),
              m_statusBarBackgroundBrush( NULL ),
			  m_bWasHoldReset(false)  ,
			  m_threadPoolManager(NULL)
    {
        FUNCTION_ENTRY( "PECDialog( callEntry, pParent )" );

        TA_ASSERT( m_pCallEntry != NULL, "Call Entry must not be NULL" );
		
		m_statusBarBackgroundBrush = new CBrush();
		m_statusBarBackgroundBrush->CreateSysColorBrush( STATUS_BAR_OK_COLOUR );
		
		Create(PECDialog::IDD, pParent);

		m_threadPoolManager = new TA_Base_Core::ThreadPoolManager(1,1);

        FUNCTION_EXIT;
    }


    PECDialog::~PECDialog()
    {
        FUNCTION_ENTRY( "~PECDialog" );

		if (m_threadPoolManager!=NULL)
		{
			delete m_threadPoolManager;
		}

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(PECDialog, CDialog)
        //{{AFX_MSG_MAP(PECDialog)
        ON_WM_CTLCOLOR() 
        ON_BN_CLICKED(IDHELP, onButtonHelp)
        ON_BN_CLICKED(IDC_PEC_BUTTON_CCTV, onButtonCctv)
        ON_BN_CLICKED(IDC_PEC_BUTTON_RESET, onButtonReset)
        ON_BN_CLICKED(IDC_PEC_BUTTON_RESET_ALL, onButtonResetAll)
		ON_BN_CLICKED(IDC_PEC_BUTTON_HOLD, onButtonHold)
        ON_MESSAGE(REFRESH_PEC_DETAILS, refreshFields)
        ON_MESSAGE(DISPLAY_PEC_STATUS , displayPecStatus)
		ON_MESSAGE(PEC_UPDATE_THREAD_STATUS, updateThreadStatus)
		ON_MESSAGE(PEC_CALL_TERMINATE_MSG, onShowCallTerminateMsg)
		ON_MESSAGE(PEC_ENABLE_WINDOW, doWindowEnable)
		ON_MESSAGE(PEC_DISABLE_WINDOW, doWindowDisable)
		ON_MESSAGE(PEC_HIDE_DLG, onHideDialog)
		ON_MESSAGE(PEC_SHOW_INFO_MSG, onShowInformationMessage)
		ON_MESSAGE(PEC_SHOW_ERR_MSG, onShowErrorMessage)
		ON_MESSAGE(PEC_SHOW_DLG, onShowDialog)
		ON_MESSAGE(PEC_ENABLE_BTNS, onEnableAllButtons)
		ON_MESSAGE(PEC_DISABLE_BTNS, onDisableAllButtons)
		ON_MESSAGE(PEC_SET_FORE_WINDOW, onSetForeWindow)
		ON_MESSAGE(PEC_CHANGE_OVER_FAIL_MSG, onShowChangeOverFailMsg)
		ON_WM_CLOSE()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL PECDialog::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        // Calls the parent implementation of this method
        CDialog::OnInitDialog();

        // refreshes the field with the PEC details
        // and initialises the status display
        updatePecDetails();
        setStatusInitial();

        // checks if CCTV is avaliable from the call entry
        // if not, disable the cctv button
        GetDlgItem( IDC_PEC_BUTTON_CCTV )->EnableWindow( m_pCallEntry->isCctvAvailable() );

        //Centers the window on the parent
        CenterWindow();
		
		// peter.wong, Activate hot key buttons, 0x12 = alt key
		::keybd_event(0x12, 0, 0, 0);
		::keybd_event(0x12, 0, KEYEVENTF_KEYUP, 0);
		
		m_bWasHoldReset = false;

		m_bIsBackgroundThreadBusy = FALSE;

        FUNCTION_EXIT;
        return TRUE;
    }

	void PECDialog::OnClose()
	{
		m_bIsBackgroundThreadBusy = FALSE;
		EndWaitCursor();
	}

	LRESULT PECDialog::updateThreadStatus(WPARAM wParam, LPARAM lParam)
	{

 		LOG_GENERIC (SourceInfo,
			TA_Base_Core::DebugUtil::DebugInfo,
			"PECDialog update Thread Status, Thread status is busy %ld, Show dialog %ld", (long)wParam, (long)lParam);
		
		m_bIsBackgroundThreadBusy = (long)wParam  ;

		EndWaitCursor();

		BOOL isHideDialog = (long)lParam ;
		if(isHideDialog)
		{
			this->hideDialog();
		}
		return 0;
	}


    void PECDialog::setCallEntry( PECCallEntry* callEntry )
    {
        FUNCTION_ENTRY( "setCallEntry" );

        // Sets m_pCallEntry
        m_pCallEntry = callEntry;

        // refreshes the field with the PEC details
        updatePecDetails();

        // and initialises the status display
        setStatusInitial();

        FUNCTION_EXIT;
    }


    PECCallEntry* PECDialog::getCallEntry()
    {
        FUNCTION_ENTRY( "getCallEntity" );
        FUNCTION_EXIT;
        return m_pCallEntry;
    }


    void PECDialog::updatePecDetails()
    {
        FUNCTION_ENTRY( "updatePecDetails" );

        // Posts a REFRESH_PEC_DETAILS message to the window
        PostMessage( REFRESH_PEC_DETAILS, 0, 0 );

        FUNCTION_EXIT;
    }


    void PECDialog::showDialog()
    {
		FUNCTION_ENTRY( "hideDialog" );

		::PostMessage(this->GetSafeHwnd(), PEC_SHOW_DLG,0,0);

		FUNCTION_EXIT;
    }

	LRESULT PECDialog::onShowDialog( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "showDialog" );

		//Use the ShowWindow call to show the window
		ShowWindow( SW_SHOW );
		m_bWasHoldReset = false;
		// Upon showing the dialog, check for coupled train
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Test Coupled Train : showWindow::setTvssButtonVisibility()");
		setTvssButtonVisibility ();

		FUNCTION_EXIT;
		return 0;
	}


    void PECDialog::hideDialog()
    {
        FUNCTION_ENTRY( "hideDialog" );

        // Use the ShowWindow call to hide the window
		::PostMessage(this->GetSafeHwnd(), PEC_HIDE_DLG,0,0);

        FUNCTION_EXIT;
    }

	LRESULT PECDialog::onHideDialog( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "hideDialog" );

		// Use the ShowWindow call to hide the window
		if (m_hWnd)
		{
			ShowWindow( SW_HIDE );
		}

		FUNCTION_EXIT;
		return 0;
	}


    bool PECDialog::isShown()
    {
        FUNCTION_ENTRY( "isShown" );

        // Use the IsWindowVisible function to check visibility

        FUNCTION_EXIT;
        return ( IsWindowVisible() == TRUE );
    }


    void PECDialog::enableDialog()
    {
        FUNCTION_ENTRY( "enableDialog" );

        // Use the MFC enable window call to enable the window
        ::PostMessage(this->GetSafeHwnd(), PEC_ENABLE_WINDOW,0,0);

        FUNCTION_EXIT;
    }

	LRESULT PECDialog::doWindowEnable( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "doWindowEnable" );

		// Use the MFC enable window call to enable the window
		EnableWindow( TRUE );

		FUNCTION_EXIT;
		return 0;
	}


    void PECDialog::disableDialog()
    {
        FUNCTION_ENTRY( "disableDialog" );

        // Use the MFC enable window call to disable the window
        ::PostMessage(this->GetSafeHwnd(), PEC_DISABLE_WINDOW,0,0);

        FUNCTION_EXIT;
    }

	LRESULT PECDialog::doWindowDisable( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "doWindowDisable" );

		// Use the MFC enable window call to disable the window
		EnableWindow( FALSE );

		FUNCTION_EXIT;
		return 0;
	}

	void PECDialog::disableAllButtons()
	{
		FUNCTION_ENTRY( "disableAllButtons" );

		LOG_GENERIC (SourceInfo,
			TA_Base_Core::DebugUtil::DebugInfo,
			"PECDialog disable all buttons");

		::PostMessage(this->GetSafeHwnd(), PEC_DISABLE_BTNS,0,0);

		FUNCTION_EXIT;
	}

	LRESULT PECDialog::onDisableAllButtons( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onDisableAllButtons" );
		LOG_GENERIC (SourceInfo,
			TA_Base_Core::DebugUtil::DebugInfo,
			"PECDialog disable all buttons");

		GetDlgItem( IDHELP )->EnableWindow(FALSE);
		GetDlgItem( IDC_PEC_BUTTON_CCTV )->EnableWindow(FALSE);
		GetDlgItem( IDC_PEC_BUTTON_HOLD )->EnableWindow(FALSE);
		GetDlgItem( IDC_PEC_BUTTON_HOLD )->EnableWindow(FALSE);
		GetDlgItem( IDC_PEC_BUTTON_RESET )->EnableWindow(FALSE);	
		GetDlgItem( IDC_PEC_BUTTON_RESET_ALL )->EnableWindow(FALSE);
		FUNCTION_EXIT;
		return 0;
	}

	void PECDialog::enableAllButtons()
	{
		FUNCTION_ENTRY( "enableAllButtons" );

		LOG_GENERIC (SourceInfo,
			TA_Base_Core::DebugUtil::DebugInfo,
			"PECDialog enable all buttons");
		
		::PostMessage(this->GetSafeHwnd(), PEC_ENABLE_BTNS,0,0);

		FUNCTION_EXIT;
	}

	LRESULT PECDialog::onEnableAllButtons( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onEnableAllButtons" );

		LOG_GENERIC (SourceInfo,
			TA_Base_Core::DebugUtil::DebugInfo,
			"PECDialog enable all buttons");

		GetDlgItem( IDHELP )->EnableWindow(TRUE);
		GetDlgItem( IDC_PEC_BUTTON_CCTV )->EnableWindow(TRUE);
		GetDlgItem( IDC_PEC_BUTTON_HOLD )->EnableWindow(TRUE);
		GetDlgItem( IDC_PEC_BUTTON_HOLD )->EnableWindow(TRUE);
		GetDlgItem( IDC_PEC_BUTTON_RESET )->EnableWindow(TRUE);	
		GetDlgItem( IDC_PEC_BUTTON_RESET_ALL )->EnableWindow(TRUE);
		
		FUNCTION_EXIT;

		return 0;
	}


    void PECDialog::setStatusInitial()
    {
        FUNCTION_ENTRY( "setStatusInitial" );

        setPecStatus( None );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusAnswerInProgress()
    {
        FUNCTION_ENTRY( "setStatusAnswerInProgress" );

        setPecStatus( AnsweringPec );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusActivatingCctv()
    {

        FUNCTION_ENTRY( "setStatusActivatingCctv" );

        setPecStatus( ActivatingCctv );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusPecAnswered()
    {
        FUNCTION_ENTRY( "setStatusPecAnswered" );

        setPecStatus( Answered );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusCallLost()
    {
        FUNCTION_ENTRY( "setStatusCallLost" );

        setPecStatus( CallLost );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusReset()
    {
        FUNCTION_ENTRY( "setStatusReset" );

        setPecStatus( Reset );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusMpuChangeover()
    {
        FUNCTION_ENTRY( "setStatusMpuChangeover" );

        setPecStatus( MpuFailure );

        FUNCTION_EXIT;
    }


    void PECDialog::showErrorMessage( const std::string& message )
    {
        FUNCTION_ENTRY( "showErrorMessage" );

		char * cstr = new char [message.length()+1];
		strcpy(cstr, message.c_str());

		::PostMessage(this->GetSafeHwnd(), PEC_SHOW_ERR_MSG, (WPARAM)cstr,LPARAM(0));

        FUNCTION_EXIT;
    }

	LRESULT PECDialog::onShowErrorMessage( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "showErrorMessage" );

		// Uses TransactiveMessage to create the given message
		// and load the resource string IDS_UE_020071.

		char* message = (char*)(wParam);

		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << message;
		CString errMsg = userMsg.constructMessage( IDS_UE_020071 );

		// to move the message box position
		m_hhookCBTProc = SetWindowsHookEx(WH_CBT, 
			(HOOKPROC)pfnCBTMsgBoxHook, 
			0, GetCurrentThreadId());

		// Displays the message using MessageBox.
		MessageBox( errMsg,
			PEC_MESSAGE_BOX_CAPTION.c_str(),
			MB_OK | MB_ICONSTOP );

		UnhookWindowsHookEx(m_hhookCBTProc);

		delete [] message;

		FUNCTION_EXIT;
		return 0;
	}


    void PECDialog::showInformationMessage( const std::string& message )
    {
        FUNCTION_ENTRY( "showInformationMessage" );

		char * cstr = new char [message.length()+1];
		strcpy(cstr, message.c_str());

        ::PostMessage(this->GetSafeHwnd(), PEC_SHOW_INFO_MSG, (WPARAM)cstr,LPARAM(0));
        FUNCTION_EXIT;
    }

	LRESULT PECDialog::onShowInformationMessage( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onShowInformationMessage" );

		char* message = (char*)(wParam);

		// Uses TransactiveMessage to create the given message
		// and load the resource string IDS_UI_050063.

		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << message;
		CString errMsg = userMsg.constructMessage( IDS_UI_050063 );


		// Displays the message using MessageBox.
		MessageBox( errMsg,
			TA_Base_Core::RunParams::getInstance().get( RPARAM_APPNAME ).c_str(),
			MB_OK );

		delete [] message;
		FUNCTION_EXIT;
		return 0;
	}


    LRESULT PECDialog::refreshFields( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "refreshFields" );

		try {
        // Re-sets the text for m_type, m_source, and m_destination
        m_type.SetWindowText( m_pCallEntry->getPECDetailString().c_str() );
        m_source.SetWindowText( m_pCallEntry->getTrainDetailString().c_str() );
        m_destination.SetWindowText( m_pCallEntry->getLocationDetailString().c_str() );
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "unknown location information" );
		}

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT PECDialog::displayPecStatus( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "displayPecStatus" );

        std::string progress;
        COLORREF colour = STATUS_BAR_OK_COLOUR;

        // Extract the status from the LPARAM
        EPecStatus status = static_cast< EPecStatus >( lParam ); //reinterpret_cast

        // Depending on the status, choose the
        // correct message and status bar colour

        switch ( status )
        {

            case None:
                progress = "";
                colour = STATUS_BAR_OK_COLOUR;
                break;

            case AnsweringPec:
                progress = STATUS_BAR_ANSWER_IN_PROGRESS_MESSAGE;
                colour = STATUS_BAR_OK_COLOUR;
                break;

            case ActivatingCctv:
                progress = STATUS_BAR_ACTIVATING_CCTV_MESSAGE;
                colour = STATUS_BAR_OK_COLOUR;
                break;

            case Answered:
                progress = STATUS_BAR_PEC_ANSWERED_MESSAGE;
                colour = STATUS_BAR_OK_COLOUR;
                break;

            case CallLost:
                progress = STATUS_BAR_CALL_LOST_MESSAGE;
                colour = STATUS_BAR_ERROR_COLOUR;
                break;

            case Reset:
                progress = STATUS_BAR_PEC_RESET_MESSAGE;
                colour = STATUS_BAR_ERROR_COLOUR;
                break;

            case MpuFailure:
                progress = STATUS_BAR_MPU_FAILURE_MESSAGE;
                colour = STATUS_BAR_ERROR_COLOUR;
                break;
			case ConnectionError:
				progress = STATUS_CONNECTION_ERROR;
				colour = STATUS_BAR_ERROR_COLOUR;
				break;
            default:
                TA_ASSERT( false, "Invalid EPecStatus given" );
        }

        // Then call setStatusBar with the message and colour
        setStatusBar( progress, colour );

        FUNCTION_EXIT;
        return 0;
    }

	LRESULT CALLBACK PECDialog::pfnCBTMsgBoxHook(int nCode, WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY("pfnCBTMsgBoxHook");

		if (nCode == HCBT_CREATEWND)
		{
			CREATESTRUCT *pcs = ((CBT_CREATEWND *)lParam)->lpcs;

			if ((pcs->style & WS_DLGFRAME) || (pcs->style & WS_POPUP))
			{
				HWND hMB = (HWND)wParam;

				CPoint   pt;   
				GetCursorPos(&pt);   

				CRect win;
				::GetWindowRect(hMB, win);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Moving error message box to center of the screen");

				RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
					TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
					pt.x);

				int centerX = ((boundary.right-boundary.left) - win.Width())/2;
				int centerY = ((boundary.bottom-boundary.top) - win.Height())/2;

				::MoveWindow(hMB, centerX, centerY, win.Width(), win.Height(),TRUE);
			}
		}

		FUNCTION_EXIT;
		return (CallNextHookEx(m_hhookCBTProc, nCode, wParam, lParam));
	}



    void PECDialog::onButtonCctv()
    {
        FUNCTION_ENTRY( "onButtonCctv" );

        // Notify the call entry the CCTV button was pressed
       //m_pCallEntry->dialogCctvButtonPressed();

		if(!m_bIsBackgroundThreadBusy)
		{
			PECCallWorkItemPtr callItem( new TA_IRS_Bus::PECCallWorkItem( m_pCallEntry, TA_IRS_Bus::PECCallActionType::CCTV) );
			m_bIsBackgroundThreadBusy = true;
			m_threadPoolManager->queueWorkItem(callItem);
		}
		else
		{
			this->showErrorMessage("PEC is in use, please wait...");

		}
		
        FUNCTION_EXIT;
    }


    void PECDialog::onButtonReset()
    {
        FUNCTION_ENTRY( "onButtonReset" );

		if(!m_bIsBackgroundThreadBusy)
		{
			// Notify the call entry the Reset button was pressed
			m_bWasHoldReset = true;

			BeginWaitCursor();
			PECCallWorkItemPtr callItem( new TA_IRS_Bus::PECCallWorkItem( m_pCallEntry, TA_IRS_Bus::PECCallActionType::RESET) );
			m_bIsBackgroundThreadBusy = true;
			m_threadPoolManager->queueWorkItem(callItem);
			//EndWaitCursor();
		}
		else
		{
			this->showErrorMessage("PEC is in use, please wait...");
		}

        FUNCTION_EXIT;
    }


    void PECDialog::onButtonResetAll()
    {
        FUNCTION_ENTRY( "onButtonResetAll" );

		if(!m_bIsBackgroundThreadBusy)
		{
			m_bWasHoldReset = true;
			// Notify the call entry the Reset All button was pressed
			BeginWaitCursor();
			PECCallWorkItemPtr callItem( new TA_IRS_Bus::PECCallWorkItem( m_pCallEntry, TA_IRS_Bus::PECCallActionType::RESETALL) );
			m_bIsBackgroundThreadBusy = true;
			m_threadPoolManager->queueWorkItem(callItem);
			//EndWaitCursor();
		}
		else
		{
			this->showErrorMessage("PEC is in use, please wait...");
		}

        FUNCTION_EXIT;
    }


    void PECDialog::onButtonHelp()
    {
        FUNCTION_ENTRY( "onButtonHelp" );

        // Use HelpLauncher to display the help window
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

        FUNCTION_EXIT;
    }

	void PECDialog::onButtonHold()
    {
        FUNCTION_ENTRY( "onButtonHold" );

		if(!m_bIsBackgroundThreadBusy)
		{
			// Notify the call entry the Reset All button was pressed
			m_bWasHoldReset = true;
			BeginWaitCursor();
			PECCallWorkItemPtr callItem( new TA_IRS_Bus::PECCallWorkItem( m_pCallEntry, TA_IRS_Bus::PECCallActionType::ONHOLD) );
			m_bIsBackgroundThreadBusy = true;
			m_threadPoolManager->queueWorkItem(callItem);
			//EndWaitCursor();
		}
		else
		{
			this->showErrorMessage("PEC is in use, please wait...");
		}
        FUNCTION_EXIT;
    }


    HBRUSH PECDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
    {
        HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );

        // Use the m_statusBackgroundBrush for the edit box

        if ( pWnd->GetDlgCtrlID() == IDC_PEC_EDIT_STATUS_BAR )
        {
            // Set the background mode for text to transparent
            // so the background will show through.
            pDC->SetBkMode( TRANSPARENT );

            hbr = ( HBRUSH )( m_statusBarBackgroundBrush->GetSafeHandle() );
        }

        return hbr;
    }

    void PECDialog::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(PECDialog)
        DDX_Control( pDX, IDC_PEC_EDIT_TYPE, m_type );
        DDX_Control( pDX, IDC_PEC_EDIT_SOURCE, m_source );
        DDX_Control( pDX, IDC_PEC_EDIT_DESTINATION, m_destination );
        DDX_Control( pDX, IDC_PEC_EDIT_STATUS_BAR, m_statusBar );
        //}}AFX_DATA_MAP
    }


    void PECDialog::setPecStatus( EPecStatus status )
    {
        FUNCTION_ENTRY( "setPecStatus" );

        // Posts a DISPLAY_PEC_STATUS  message to
        // the window, with the status as the LPARAM
        PostMessage( DISPLAY_PEC_STATUS, 0, LPARAM( status ) );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusBar( const std::string& message, COLORREF colour )
    {
        FUNCTION_ENTRY( "setStatusBar" );

        // Sets the status bar text
        m_statusBar.SetWindowText( message.c_str() );
        //then sets the status bar colour
        setStatusBarColour( colour );

        FUNCTION_EXIT;
    }


    void PECDialog::setStatusBarColour( COLORREF colour )
    {
        FUNCTION_ENTRY( "setStatusBarColour" );

        // Hide the edit, set its background color,
        m_statusBar.ShowWindow( false );

        delete m_statusBarBackgroundBrush;
        m_statusBarBackgroundBrush = new CBrush( colour );

        // then show it again.
        m_statusBar.ShowWindow( true );

        FUNCTION_EXIT;
    }


    void PECDialog::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );

        // do nothing

        FUNCTION_EXIT;
    }

	void PECDialog::setTvssButtonVisibility ()
	{
		FUNCTION_ENTRY( "setTvssButtonVisibility" );
		
		CWnd* pWnd = GetDlgItem(IDC_PEC_BUTTON_CCTV);

		if (m_pCallEntry->isCoupledTrain())
		{
			pWnd->EnableWindow(DISABLE_BUTTON);
			::MessageBox (NULL,"A PEC from a coupled train has been answered.\n Automatic activation of train image has been inhibited","PEC",MB_ICONWARNING);
		}
		else
		{
			pWnd->EnableWindow(ENABLE_BUTTON);
		}
		
		FUNCTION_EXIT;
	}

	void PECDialog::setConnectionError()
	{
		FUNCTION_ENTRY( "setStatusReset" );

		setPecStatus( ConnectionError );

		FUNCTION_EXIT;

	}

	bool PECDialog::getHoldResetStatus ()
	{
		FUNCTION_ENTRY( "getResetStatus" );
		FUNCTION_EXIT;
		return m_bWasHoldReset;
	}

	LRESULT PECDialog::onShowCallTerminateMsg( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onShowCallTerminateMsg" );
		::MessageBox(NULL,"Audio for this PEC has been terminated","PEC",MB_OK | MB_ICONEXCLAMATION);
		FUNCTION_EXIT;
		return 0;
	}

	LRESULT   PECDialog:: onSetForeWindow(WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onSetForeWindow" );
		SetForegroundWindow();
		FUNCTION_EXIT;
		return 0;
	}

	LRESULT   PECDialog:: onShowChangeOverFailMsg(WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onShowChangeOverFailMsg" );
		::MessageBox(NULL,"PEC failed due to Changeover","PEC",MB_OK | MB_ICONEXCLAMATION);
		FUNCTION_EXIT;
		return 0;
	}

}
