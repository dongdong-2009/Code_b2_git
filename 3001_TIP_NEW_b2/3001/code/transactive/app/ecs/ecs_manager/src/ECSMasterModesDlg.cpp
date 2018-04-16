/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ECSMasterModesDlg.cpp $
  * @author:  Robert Young
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2013/10/07 08:59:52 $
  * Last modified by:  $Author: huangjian $
  *
  * This class is a derivation of TransActiveDialog and implementation of the IECSMasterModesView 
  * interface. It provides the actual ECS Master Modes user interface, but defers the responsibility 
  * of it's logic and data to the ECSMasterModes member (which manipulates this object via 
  * IECSMasterModesView).
  *
  */

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ECSManager.h"
#include "app/ecs/ecs_manager/src/ECSMasterModesDlg.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"

#include "bus/generic_gui/src/HelpLauncher.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

#include  <afxpriv.h>

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_SET_CURRENT_STATIONMODE                  (WM_APP+1)
#define WM_SET_CURRENT_STATIONMODE_INPROGRESS       (WM_APP+2)
#define WM_SET_CURRENT_PROPOSED_MASTERMODE          (WM_APP+3)
#define WM_ENABLE_APPLY_MASTERMODE                  (WM_APP+4)
#define WM_INITIALISE                               (WM_APP+5)
#define WM_SET_CURRENT_STATION_LOCK					(WM_APP+6)

using namespace TA_Base_Core;

namespace TA_IRS_App
{
    const int NORMAL_START_HEIGHT = 1050;
    const int EMERGENCY_START_HEIGHT = 1050; 
    const int EMERGENCY_FOR_ZONE_START_HEIGHT = 1050; 
    const int NORMAL_START_WIDTH = 1680; 
    const int EMERGENCY_START_WIDTH = 1680;
    const int MASTER_MODE_DETAIL_START_ROW = 2;
    const char* const NORMAL_DIALOG_CAPTION     = "ECS Normal Master Mode";
    const char* const EMERGENCY_DIALOG_CAPTION  = "ECS Emergency Master Mode";
    const char* const EMERGENCY_FOR_ZONE_DIALOG_CAPTION    = "ECS Possible Emergency Master Modes For Zone";

    //
    // Constructor
    //
    ECSMasterModesDlg::ECSMasterModesDlg( TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/ )
        : TransActiveDialog( genericGUICallback, ECSMasterModesDlg::IDD, pParent ), m_areOffsetsSetup( false ), 
        m_rightRelativeOffset( 0 ), m_bottomRelativeOffset( 0 )
    {
        FUNCTION_ENTRY( "Constructor" );

        //{{AFX_DATA_INIT(EventViewerDlg)
            // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );

        FUNCTION_EXIT;
    }

	void ECSMasterModesDlg::init()
	{//TD18095, jianghp, to fix the performance of showing manager application
        // create the smart object which performs all the logic for this view.
        m_pECSMasterModes = std::auto_ptr< ECSMasterModes >( new ECSMasterModes( *this ) );
		
		// Reduce launching manager time
        // use this message handler to perform initialisation that 
        // should be done after dialog is shown (i.e after this method returns) 
        //PostMessage( WM_INITIALISE );	
				
		m_thread_hd = CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadFunction, this, 0, &m_thread_id );
		CloseHandle(m_thread_hd);
		// Reduce launching manager time
	}

	// Reduce launching manager time
	DWORD WINAPI ECSMasterModesDlg::ThreadFunction(LPVOID lpThreadParameter)
	{
		ECSMasterModesDlg* ECSMMD = (ECSMasterModesDlg*) lpThreadParameter;
		ECSMMD->onInitialise();
        ECSMMD->PostMessage( WM_ENABLE_CLOSE, static_cast<WPARAM>(TRUE) );
		return TRUE;
	}
	// Reduce launching manager time

    //
    // setLocations
    //
    void ECSMasterModesDlg::setLocations( const LocationNames& locationNames )
    {
        FUNCTION_ENTRY( "setLocations" );

        m_masterModesGridCtrl.setLocations( locationNames );

        FUNCTION_EXIT;
    }

    
    //
    // displayErrorMessage
    //
    void ECSMasterModesDlg::displayErrorMessage( const std::string& errorMessage )
    {
        FUNCTION_ENTRY( "displayErrorMessage" );

        MessageBox( _T( errorMessage.c_str() ), RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_ICONERROR | MB_OK );

        FUNCTION_EXIT;
    }


    //
    // displayWarningMessage
    //
    void ECSMasterModesDlg::displayWarningMessage( const std::string& warningMessage )
    {
        FUNCTION_ENTRY( "displayWarningMessage" );

        MessageBox( _T( warningMessage.c_str() ), RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_ICONWARNING | MB_OK );

        FUNCTION_EXIT;
    }


    //
    // aquireConfirmation
    //
    bool ECSMasterModesDlg::aquireConfirmation( const std::string& confirmationMessage )
    {
        FUNCTION_ENTRY( "aquireConfirmation" );

        int confirmation = MessageBox( _T( confirmationMessage.c_str() ), RunParams::getInstance().get( RPARAM_APPNAME ).c_str(), MB_ICONQUESTION | MB_YESNO );
        if ( confirmation == IDYES )
        {
            FUNCTION_EXIT;
            return true;
        }
        FUNCTION_EXIT;
        return false;
    }


    //
    // enableApplyMasterModeControl
    //
    void ECSMasterModesDlg::enableApplyMasterModeControl( bool doEnableApplyMasterModeControl )
    {
        FUNCTION_ENTRY( "enableApplyMasterModeControl" );

        WPARAM wParam = static_cast< WPARAM >( doEnableApplyMasterModeControl ); 
        PostMessage( WM_ENABLE_APPLY_MASTERMODE, wParam, 0 );

        FUNCTION_EXIT;
    }


    //
    // terminateApplication
    //
    void ECSMasterModesDlg::terminateApplication()
    {
        FUNCTION_ENTRY( "terminateApplication" );

        PostMessage( WM_CLOSE );

        FUNCTION_EXIT;
    }


    //
    // setMasterModeDetails
    //
    void ECSMasterModesDlg::setMasterModeDetails( const MasterModeDetails& masterModeDetails )
    {
        FUNCTION_ENTRY( "setMasterModeDetails" );

        m_masterModesGridCtrl.populateMasterModeDetails( masterModeDetails );

        FUNCTION_EXIT;
    }


    //
    // setCurrentStationMode
    //
    void ECSMasterModesDlg::setCurrentStationMode( int locationIndex, const std::string& stationMode, ECurrentStationModeType stationModeType )
    {
        FUNCTION_ENTRY( "setCurrentStationMode" );

        WPARAM wParam = static_cast< WPARAM >( locationIndex );
        std::pair< std::string, ECurrentStationModeType >* p_ModeTypePair =
            new std::pair< std::string, ECurrentStationModeType >;
        ( *p_ModeTypePair ).first = stationMode;
        ( *p_ModeTypePair ).second = stationModeType;
        LPARAM lParam = reinterpret_cast< WPARAM >( p_ModeTypePair );
        PostMessage( WM_SET_CURRENT_STATIONMODE, wParam, lParam );

        FUNCTION_EXIT;
    }


    //
    // setCurrentStationModeInProgress
    //
    void ECSMasterModesDlg::setCurrentStationModeInProgress( int locationIndex, bool doShowAsInProgress )
    {
        FUNCTION_ENTRY( "setCurrentStationModeInProgress" );

        WPARAM wParam = static_cast< WPARAM >( locationIndex );
        LPARAM lParam = static_cast< LPARAM >( doShowAsInProgress );

        PostMessage( WM_SET_CURRENT_STATIONMODE_INPROGRESS, wParam, lParam );

        FUNCTION_EXIT;
    }

	void ECSMasterModesDlg::setCurrentStationVisualLock( int locationIndex, bool isStationLocked )
	{
		// Post the updates.
        FUNCTION_ENTRY( "setCurrentStationVisualLock" );

        WPARAM wParam = static_cast< WPARAM >( locationIndex );
        LPARAM lParam = static_cast< LPARAM >( isStationLocked );

        PostMessage( WM_SET_CURRENT_STATION_LOCK, wParam, lParam );

        FUNCTION_EXIT;
	}


    //
    // setCurrentProposedMasterMode
    //
    void ECSMasterModesDlg::setCurrentProposedMasterMode( int masterModeIndex, EModeProposalState proposalState )
    {
        FUNCTION_ENTRY( "setCurrentProposedMasterMode" );

        WPARAM wParam = static_cast< WPARAM >( masterModeIndex );
        LPARAM lParam = static_cast< LPARAM >( proposalState );

        PostMessage( WM_SET_CURRENT_PROPOSED_MASTERMODE, wParam, lParam );

        FUNCTION_EXIT;
    }


    //
    // DoDataExchange
    //
    void ECSMasterModesDlg::DoDataExchange( CDataExchange* pDX )
    {
        TransActiveDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(ECSMasterModesDlg)
	    DDX_Control(pDX, IDC_MASTERMODES_STATUS_EDIT, m_masterModeEditControl);
        DDX_Control(pDX, IDC_APPLYMASTERMODE_BUTTON, m_applyMasterModeButton);
	    DDX_Control(pDX, IDC_CLOSE_BUTTON, m_closeButton);
    
	//}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP( ECSMasterModesDlg, TransActiveDialog )
        //{{AFX_MSG_MAP(ECSMasterModesDlg)
        ON_NOTIFY(GVN_SELCHANGED, IDC_MASTERMODES_GRIDCTRL, onMasterModesGridSelectionChanged)
        ON_MESSAGE(WM_SET_CURRENT_STATIONMODE, onSetCurrentStationMode)
        ON_MESSAGE(WM_SET_CURRENT_STATIONMODE_INPROGRESS, onSetCurrentStationModeInProgress)    
        ON_MESSAGE(WM_SET_CURRENT_PROPOSED_MASTERMODE, onSetCurrentProposedMasterMode)
        ON_MESSAGE(WM_ENABLE_APPLY_MASTERMODE, onEnableApplyMasterModeControl)
        ON_MESSAGE(WM_ENABLE_CLOSE, onEnableCloseButton)
		ON_MESSAGE_VOID(WM_INITIALISE, onInitialise)
		ON_MESSAGE(WM_SET_CURRENT_STATION_LOCK, onSetStationLockStatus)
        ON_BN_CLICKED(IDC_CLOSE_BUTTON, onCloseButton)
        ON_WM_CLOSE()
        ON_WM_SIZE()
        ON_COMMAND(ID_APP_ABOUT, onAppAbout)
		ON_COMMAND(ID_HELP_INDEX, OnAppHelp)
        ON_BN_CLICKED(IDC_APPLYMASTERMODE_BUTTON, onApplyMasterModeButton)
		ON_BN_CLICKED(ID_HELP, OnHelpButton) //TD16730
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // ECSMasterModesDlg message handlers

    //
    // OnInitDialog
    //
    BOOL ECSMasterModesDlg::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        // Have to calculate offsets before this dialog is resized.
        setupOffsets();

        createGridCtrl();

		// setup the icons for the scroll buttons
		CButton *p_leftButton = (CButton *)(GetDlgItem(IDC_BUTTON_LEFT));
		HICON iconL = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LEFT));
		p_leftButton->SetIcon(iconL);

		CButton *p_rightButton = (CButton *)(GetDlgItem(IDC_BUTTON_RIGHT));
		HICON iconR = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_RIGHT));
		p_rightButton->SetIcon(iconR);

        TA_Base_Bus::ResizingProperties rp;
        rp.canMaximise = false;
        rp.maxWidth = 1680;
        rp.maxHeight = 1050;
        CRect rtClient;
        GetClientRect(rtClient);
        rp.minWidth = rtClient.Width();
        rp.minHeight = rtClient.Height();
        setResizingProperties(rp);

        // Have to set initial size before superclass OnInitDialog so that
        // previous user-settings can override the default initial size this
        // helper method sets.
		try
		{
			setInitialSize();
		}
		catch ( ... )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,"Unknown", "Initialisation of Master Modes Dialog failed");
			return FALSE;
		}

        TransActiveDialog::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon( m_hIcon, TRUE );           // Set big icon

        // Have to call redaw, else the Control Station Saved User Settings poitioning does not 
        // draw properly.
        RedrawWindow();
		m_closeButton.EnableWindow(FALSE);
        FUNCTION_EXIT;
        return TRUE;  // return TRUE  unless you set the focus to a control
    }


    //
    // onInitialise
    //
    void ECSMasterModesDlg::onInitialise()
    {
        FUNCTION_ENTRY( "onInitialise" );

        m_pECSMasterModes->initialise();

        FUNCTION_EXIT;
    }


    //
    // setInitialSize
    //
    void ECSMasterModesDlg::setInitialSize()
    {
        FUNCTION_ENTRY( "setInitialSize" );

        // Set appropriate initial size depending on type of Master Modes dialog.
        // Note, previously saved user settings window placements will subsequently override
        // these during TransActiveDialog::OnInitDialog.
        CRect thisRect;
        GetWindowRect( &thisRect );
        switch ( ECSManagerConfigurationAccess::getInstance().getDialogType() )
        {
        case NORMAL_MASTER_MODES:
            {
                SetWindowPos( NULL, thisRect.left, thisRect.top, NORMAL_START_WIDTH, NORMAL_START_HEIGHT, 0 );
                SetWindowText( _T( NORMAL_DIALOG_CAPTION ) );
            }
            break;
        case EMERGENCY_MASTER_MODES:
            {
                SetWindowPos( NULL, thisRect.left, thisRect.top, EMERGENCY_START_WIDTH, EMERGENCY_START_HEIGHT, 0 );
                SetWindowText( _T( EMERGENCY_DIALOG_CAPTION ) );
            }
            break;
        case EMERGENCY_MASTER_MODES_FOR_ZONE:
            {
                SetWindowPos( NULL, thisRect.left, thisRect.top, EMERGENCY_START_WIDTH, EMERGENCY_FOR_ZONE_START_HEIGHT, 0 );
                SetWindowText( _T( EMERGENCY_FOR_ZONE_DIALOG_CAPTION ) );
            }
            break;
        default:
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected dialog type during initialisation of Master Modes Dialog." );
            TA_THROW( TA_Base_Core::TransactiveException("Unexpected dialog type" ));
        }

        FUNCTION_EXIT;
    }


    //
    // setupOffsets
    //
    void ECSMasterModesDlg::setupOffsets()
    {
        FUNCTION_ENTRY( "setupOffsets" );

        CRect thisRect;
        GetWindowRect( &thisRect );

        CRect masterModeSelectionRect;
		CRect statusBoxRect;
        GetDlgItem( IDC_MASTERMODES_SELECTION_GROUPBOX )->GetWindowRect( &masterModeSelectionRect );
		GetDlgItem( IDC_STATUS_GROUPBOX )->GetWindowRect(&statusBoxRect);

        m_rightRelativeOffset = thisRect.right - masterModeSelectionRect.right;
        m_bottomRelativeOffset = thisRect.bottom - masterModeSelectionRect.bottom;

        m_areOffsetsSetup = true;

        FUNCTION_EXIT;
    }


    //
    // createGridCtrl
    // 
    void ECSMasterModesDlg::createGridCtrl()
    {
        FUNCTION_ENTRY( "createGridCtrl" );

        // Use our dummy static control to retrieve the positioning for the grid and then destroy it.
        CRect position;

        GetDlgItem( IDC_MASTERMODES_GRIDCTRL )->GetWindowRect( &position );
        GetDlgItem( IDC_MASTERMODES_GRIDCTRL  )->DestroyWindow();

        // Now convert to client position rather than on the whole screen
        ScreenToClient(&position);

        // Now create the subsystem grids at the positions given.
        m_masterModesGridCtrl.Create( position ,this, IDC_MASTERMODES_GRIDCTRL, 
            WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER );
		
        m_masterModesGridCtrl.setUpGridCtrl();

        FUNCTION_EXIT;
    }


    //
    // onSetCurrentStationMode
    //
    LRESULT ECSMasterModesDlg::onSetCurrentStationMode( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onSetCurrentStationMode" );

        int locationIndex = static_cast< int >( wParam );
        std::pair< std::string, ECurrentStationModeType >* p_ModeTypePair =
            reinterpret_cast< std::pair< std::string, ECurrentStationModeType >* >( lParam );

        
        m_masterModesGridCtrl.setCurrentStationMode( locationIndex, 
            ( *p_ModeTypePair ).first, ( *p_ModeTypePair ).second ); 

        delete p_ModeTypePair;
        FUNCTION_EXIT;
        return 0;
    }


    //
    // onSetCurrentStationModeInProgress
    //
    LRESULT ECSMasterModesDlg::onSetCurrentStationModeInProgress( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onSetCurrentStationModeInProgress" );

        int locationIndex = static_cast< int >( wParam );
        BOOL doShowAsInProgress = static_cast< BOOL >( lParam );

        m_masterModesGridCtrl.setCurrentStationModeInProgress( locationIndex, doShowAsInProgress );
        
        FUNCTION_EXIT;
        return 0;
    }


    //
    // onEnableApplyMasterModeControl
    //
    LRESULT ECSMasterModesDlg::onEnableApplyMasterModeControl(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onEnableApplyMasterModeControl" );

        BOOL doEnableApplyMasterModeControl = static_cast< BOOL >( wParam );

        m_applyMasterModeButton.EnableWindow( doEnableApplyMasterModeControl );
        
        FUNCTION_EXIT;
        return 0;
    }
	LRESULT ECSMasterModesDlg::onEnableCloseButton(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onEnableApplyMasterModeControl" );

        BOOL doEnableApplyMasterModeControl = static_cast< BOOL >( wParam );

        m_closeButton.EnableWindow( TRUE );
        
        FUNCTION_EXIT;
        return 0;
    }
	

    //
    // onSetCurrentProposedMasterMode
    //
    LRESULT ECSMasterModesDlg::onSetCurrentProposedMasterMode(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onSetCurrentProposedMasterMode" );

        int masterModeIndex = static_cast< int >( wParam );
        EModeProposalState proposalState = static_cast< EModeProposalState >( lParam );

        m_masterModesGridCtrl.setCurrentProposedMasterMode( masterModeIndex, proposalState );

        FUNCTION_EXIT;
        return 0;
    }

	
	LRESULT ECSMasterModesDlg::onSetStationLockStatus(WPARAM wParam, LPARAM lParam)
	{
        FUNCTION_ENTRY( "onSetStationLockStatus" );

        int masterModeIndex = static_cast< int >( wParam );
		long lparam = static_cast<long>(lParam);
        bool isStationLocked = (lparam != 0);

        m_masterModesGridCtrl.setCurrentStationModeLock( masterModeIndex, isStationLocked );

        FUNCTION_EXIT;
        return 0;
	}

    //
    // onCloseButton
    //
    void ECSMasterModesDlg::onCloseButton() 
    {
        FUNCTION_ENTRY( "onCloseButton" );

        PostMessage( WM_CLOSE );    

        FUNCTION_EXIT;
    }


    //
    // OnClose
    //
    void ECSMasterModesDlg::OnClose() 
    {
        FUNCTION_ENTRY( "OnClose" );

        m_pECSMasterModes->prepareForClose();
        
		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();

        FUNCTION_EXIT;
    }


    //
    // onAppAbout
    //
    void ECSMasterModesDlg::onAppAbout() 
    {
        FUNCTION_ENTRY( "OnAppAbout" );

        // Display the about box
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

        FUNCTION_EXIT;
    }


    //
    // onMasterModesGridSelectionChanged
    //
    void ECSMasterModesDlg::onMasterModesGridSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        NM_GRIDVIEW* pNMGridView = ( NM_GRIDVIEW* )pNMHDR;

        if ( pNMGridView->iRow >= MASTER_MODE_DETAIL_START_ROW  )
        {
            m_pECSMasterModes->masterModeSelectionChanged( pNMGridView->iRow - MASTER_MODE_DETAIL_START_ROW );
        }
        else
        {
            m_pECSMasterModes->masterModeSelectionChanged( -1 );
        }

        *pResult = 0;
    }


    //
    // onApplyMasterModeButton
    //
    void ECSMasterModesDlg::onApplyMasterModeButton() 
    {
        FUNCTION_ENTRY( "onApplyMasterModeButton" );

        if ( m_masterModesGridCtrl.GetFocusCell().row >= MASTER_MODE_DETAIL_START_ROW  )
        {

            m_pECSMasterModes->applySelectedMasterMode( m_masterModesGridCtrl.GetFocusCell().row - MASTER_MODE_DETAIL_START_ROW );
        }
        else
        {
            m_pECSMasterModes->masterModeSelectionChanged( -1 );
        }
        m_masterModesGridCtrl.resetSelectionAfterApplyButtonPressed();
        FUNCTION_EXIT;
    }


    //
    // OnSize
    //
    void ECSMasterModesDlg::OnSize(UINT nType, int cx, int cy) 
    {
        // Let the CDialog base class do any re-sizing processing it needs.
        TransActiveDialog::OnSize(nType, cx, cy);

        if( m_areOffsetsSetup && nType != SIZE_MAXHIDE && nType != SIZE_MINIMIZED)
        {

            CRect thisRect;
            GetWindowRect( &thisRect );
            ScreenToClient( &thisRect );

            // Start by resizing the menu line
            CWnd* menuLine = GetDlgItem( IDC_MENU_SEPERATOR );
            if ( NULL != menuLine )
            {
                // Get the rectangle for the menu line
                CRect lineRect;
                menuLine->GetWindowRect( &lineRect );
                ScreenToClient( &lineRect );

                // Don't care about it's height - it's just a line. Set the line width bounds to 
                // the same as the main application window
                lineRect.right = ( lineRect.left + cx + 2 );
                menuLine->MoveWindow( &lineRect );
            }
		

            CRect resizeRect;
            CWnd* p_resizeWnd;

            p_resizeWnd = GetDlgItem( IDC_MASTERMODES_SELECTION_GROUPBOX );
            p_resizeWnd->GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
        
            // Use Station Mode selection GroupBox offsets as relative guide 
            // for all adjustments.
            int rightAdjust = resizeRect.right - ( thisRect.right - m_rightRelativeOffset );
            int bottomAdjust = resizeRect.bottom - ( thisRect.bottom - m_bottomRelativeOffset );

            // Station Mode selection GroupBox.
            resizeRect.right = resizeRect.right - rightAdjust;
            resizeRect.bottom = resizeRect.bottom - bottomAdjust;
            p_resizeWnd->MoveWindow( resizeRect );

			// Station Mode Status Edit Control
            CWnd* p_resizeStatusWnd;

            p_resizeStatusWnd = GetDlgItem( IDC_MASTERMODES_STATUS_EDIT );
            p_resizeStatusWnd->GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );

			resizeRect.top = resizeRect.top - bottomAdjust;
			resizeRect.right = resizeRect.right - rightAdjust;
			resizeRect.bottom = resizeRect.bottom -  bottomAdjust;
			p_resizeStatusWnd->MoveWindow(&resizeRect);

			// Station Mode Status Edit Value
            CWnd* p_resizeStatusGroupWnd;
            p_resizeStatusGroupWnd = GetDlgItem( IDC_MASTERMODES_STATUS_GROUPBOX3 );
            p_resizeStatusGroupWnd->GetWindowRect( &resizeRect );
       		ScreenToClient( &resizeRect );

			resizeRect.top = resizeRect.top - bottomAdjust;
			resizeRect.right = resizeRect.right - rightAdjust;
			resizeRect.bottom = resizeRect.bottom -  bottomAdjust;
			p_resizeStatusGroupWnd->MoveWindow(&resizeRect);

            // Master Modes Grid Ctrl.
            m_masterModesGridCtrl.GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
            resizeRect.right = resizeRect.right - rightAdjust;
            resizeRect.bottom = resizeRect.bottom - bottomAdjust;
            m_masterModesGridCtrl.MoveWindow( &resizeRect, FALSE );
            // Workaround to make sure GridCtrl sets its Horizontal scroll properly
            // (else it sometimes allows scrolling too far right of cells until the control resized slightly 
            // at least once).
            resizeRect.right++;
            m_masterModesGridCtrl.MoveWindow( &resizeRect, FALSE );
            resizeRect.right--;
            m_masterModesGridCtrl.MoveWindow( &resizeRect );

            // The buttons along the bottom.
            p_resizeWnd = GetDlgItem( IDC_CLOSE_BUTTON );
            p_resizeWnd->GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
            p_resizeWnd->MoveWindow( ( resizeRect.left - rightAdjust ),
                ( resizeRect.top - bottomAdjust ), resizeRect.Width(), resizeRect.Height(), FALSE );        

            p_resizeWnd = GetDlgItem( IDC_APPLYMASTERMODE_BUTTON );
            p_resizeWnd->GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
            p_resizeWnd->MoveWindow( ( resizeRect.left - rightAdjust ),
                ( resizeRect.top - bottomAdjust ), resizeRect.Width(), resizeRect.Height(), FALSE ); 
        
            // Only adjust vertical for Help button as its left aligned. 
            p_resizeWnd = GetDlgItem( ID_HELP );
            p_resizeWnd->GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
            p_resizeWnd->MoveWindow( resizeRect.left, ( resizeRect.top - bottomAdjust ), 
                resizeRect.Width(), resizeRect.Height(), FALSE );

            // Redraw all the buttons at once to prevent artifacts (these were 
            // appearing on Apply button sometimes during resizing)
            thisRect.top = __min( resizeRect.top, ( resizeRect.top - bottomAdjust ) );
            thisRect.bottom = __max( resizeRect.bottom, ( resizeRect.bottom - bottomAdjust ) );
            RedrawWindow( thisRect );

			Invalidate();
        }
    }

	void ECSMasterModesDlg::addStatusLine(const std::string& statusText)
	{
		
		while (m_masterModeEditControl.GetLimitText() < (m_masterModeEditControl.GetWindowTextLength() + strlen(statusText.c_str()) + 2))
		{
			int pos = m_masterModeEditControl.LineIndex(m_masterModeEditControl.GetLineCount()-2);
			m_masterModeEditControl.SetSel(pos, -1, FALSE);
			m_masterModeEditControl.ReplaceSel("");
		}
		
		// Make a new line for the new status text
		m_masterModeEditControl.SetSel(0,0);
		m_masterModeEditControl.ReplaceSel("\r\n");

		// Insert the new status text
		m_masterModeEditControl.SetSel(0,0);
		m_masterModeEditControl.ReplaceSel(statusText.c_str());

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"addStatusLine() - %s", statusText.c_str());

	}


	//TD16730
	void ECSMasterModesDlg::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}

	void ECSMasterModesDlg::OnAppHelp() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
	}

} // namespace TA_IRS_App
