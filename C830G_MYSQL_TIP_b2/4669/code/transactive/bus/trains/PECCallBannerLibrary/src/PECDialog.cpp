/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #3 $
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


namespace TA_IRS_Bus
{

    const std::string PECDialog::STATUS_BAR_CALL_LOST_MESSAGE = "PEC Audio Line has been disconnected.";
    const std::string PECDialog::STATUS_BAR_PEC_RESET_MESSAGE = "PEC has been reset.";
    const std::string PECDialog::STATUS_BAR_MPU_FAILURE_MESSAGE = "PEC has been reset due to MPU failure";
    const std::string PECDialog::STATUS_BAR_ANSWER_IN_PROGRESS_MESSAGE = "Answering PEC...";
    const std::string PECDialog::STATUS_BAR_ACTIVATING_CCTV_MESSAGE = "Activating CCTV...";
    const std::string PECDialog::STATUS_BAR_PEC_ANSWERED_MESSAGE = "PEC Answered";
    const COLORREF PECDialog::STATUS_BAR_ERROR_COLOUR = RGB( 255, 128, 128 );
    const COLORREF PECDialog::STATUS_BAR_OK_COLOUR = GetSysColor( COLOR_3DFACE );
    const DWORD PECDialog::DISPLAY_PEC_STATUS = WM_USER + 1;
    const DWORD PECDialog::REFRESH_PEC_DETAILS = WM_USER + 2;
	const DWORD PECDialog::SHOW_ERROR_MESSAGE = WM_USER + 3;
	const DWORD PECDialog::ACTIVATE_CCTV = WM_USER + 4;
	


    PECDialog::PECDialog( PECCallEntry* callEntry, CWnd* pParent )
            : CDialog( PECDialog::IDD, pParent ),
              m_callEntry( callEntry ),
              m_statusBarBackgroundBrush( NULL )
    {
        FUNCTION_ENTRY( "PECDialog( callEntry, pParent )" );

        TA_ASSERT( m_callEntry != NULL, "Call Entry must not be NULL" );
		
		m_statusBarBackgroundBrush = new CBrush();
		m_statusBarBackgroundBrush->CreateSysColorBrush( STATUS_BAR_OK_COLOUR );
		
		Create(PECDialog::IDD, pParent);

        FUNCTION_EXIT;
    }


    PECDialog::~PECDialog()
    {
        FUNCTION_ENTRY( "~PECDialog" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(PECDialog, CDialog)
        //{{AFX_MSG_MAP(PECDialog)
        ON_WM_CTLCOLOR() 
        ON_BN_CLICKED(IDHELP, onButtonHelp)
        ON_BN_CLICKED(IDC_PEC_BUTTON_CCTV, onButtonCctv)
        ON_BN_CLICKED(IDC_PEC_BUTTON_RESET, onButtonReset)
        ON_BN_CLICKED(IDC_PEC_BUTTON_RESET_ALL, onButtonResetAll)
        ON_MESSAGE(REFRESH_PEC_DETAILS, refreshFields)
        ON_MESSAGE(DISPLAY_PEC_STATUS , displayPecStatus)
		ON_MESSAGE(SHOW_ERROR_MESSAGE , onErrorMessage)
		//ON_MESSAGE(ACTIVATE_CCTV , onButtonCctv)
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
        GetDlgItem( IDC_PEC_BUTTON_CCTV )->EnableWindow( m_callEntry->isCctvAvailable() );

        //Centers the window on the parent
        CenterWindow();

        FUNCTION_EXIT;
        return TRUE;
    }


    void PECDialog::setCallEntry( PECCallEntry* callEntry )
    {
        FUNCTION_ENTRY( "setCallEntry" );

        // Sets m_callEntry
        m_callEntry = callEntry;

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
        return m_callEntry;
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
        FUNCTION_ENTRY( "showDialog" );

        //U se the ShowWindow call to show the window
        ShowWindow( SW_SHOW );

        FUNCTION_EXIT;
    }


    void PECDialog::hideDialog()
    {
        FUNCTION_ENTRY( "hideDialog" );

        // Use the ShowWindow call to hide the window
        ShowWindow( SW_HIDE );

        FUNCTION_EXIT;
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
        EnableWindow( TRUE );

        FUNCTION_EXIT;
    }


    void PECDialog::disableDialog()
    {
        FUNCTION_ENTRY( "disableDialog" );

        // Use the MFC enable window call to disable the window
        EnableWindow( FALSE );

        FUNCTION_EXIT;
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

        // Uses TransactiveMessage to create the given message
        // and load the resource string IDS_UE_020071.
        TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << message;
		userMsg.showMsgBox(IDS_UE_020071);
        
        FUNCTION_EXIT;
    }


    void PECDialog::showInformationMessage( const std::string& message )
    {
        FUNCTION_ENTRY( "showInformationMessage" );

        // Uses TransactiveMessage to create the given message
        // and load the resource string IDS_UI_050063.
        TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << message;
		userMsg.showMsgBox(IDS_UI_050063);

        FUNCTION_EXIT;
    }


    LRESULT PECDialog::refreshFields( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "refreshFields" );

        // Re-sets the text for m_type, m_source, and m_destination
        m_type.SetWindowText( m_callEntry->getPECDetailString().c_str() );
        m_source.SetWindowText( m_callEntry->getTrainDetailString().c_str() );
        m_destination.SetWindowText( m_callEntry->getLocationDetailString().c_str() );

        FUNCTION_EXIT;
        return 0;
    }

	LRESULT PECDialog::onErrorMessage( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onErrorMessage" );
		std::string error_message=(const char *)( lParam );
        showErrorMessage(error_message);
        FUNCTION_EXIT;
        return 0;
    }
	void PECDialog::displayErrorMessage(const std::string message) 
	{
		SendMessage(SHOW_ERROR_MESSAGE,0,LPARAM(message.c_str()));
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

            default:
                TA_ASSERT( false, "Invalid EPecStatus given" );
        }

        // Then call setStatusBar with the message and colour
        setStatusBar( progress, colour );

        FUNCTION_EXIT;
        return 0;
    }


    void PECDialog::onButtonCctv()
    {
        FUNCTION_ENTRY( "onButtonCctv" );

        // Notify the call entry the CCTV button was pressed
        m_callEntry->dialogCctvButtonPressed();

        FUNCTION_EXIT;
    }


    void PECDialog::onButtonReset()
    {
        FUNCTION_ENTRY( "onButtonReset" );

        // Notify the call entry the Reset button was pressed
        BeginWaitCursor();
        m_callEntry->dialogResetButtonPressed();
        EndWaitCursor();

        FUNCTION_EXIT;
    }


    void PECDialog::onButtonResetAll()
    {
        FUNCTION_ENTRY( "onButtonResetAll" );

        // Notify the call entry the Reset All button was pressed
        BeginWaitCursor();
        m_callEntry->dialogResetAllButtonPressed();
        EndWaitCursor();

        FUNCTION_EXIT;
    }


    void PECDialog::onButtonHelp()
    {
        FUNCTION_ENTRY( "onButtonHelp" );

        // Use HelpLauncher to display the help window
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

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
}
