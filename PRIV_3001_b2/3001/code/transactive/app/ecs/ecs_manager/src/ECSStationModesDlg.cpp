/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/ECSStationModesDlg.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class is a derivation of TransActiveDialog and implementation of the IECSStationModesView 
  * interface. It provides the actual ECS Station Modes user interface, but defers the responsibility 
  * of it's logic and data to the ECSStationModes member (which manipulates this object via 
  * IECSStationModesView).
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ECSManager.h"
#include "app/ecs/ecs_manager/src/ECSStationModesDlg.h"
#include "app/ecs/ecs_manager/src/ECSStationModes.h"
#include "app/ecs/ecs_manager/src/ECSColourAccessor.h"

#include "bus/generic_gui/src/HelpLauncher.h" //TD16730

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include  <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_STATIONMODE_DESELECTED               (WM_APP+1)
#define WM_SET_CURRENT_STATIONMODE              (WM_APP+2)
#define WM_SET_CURRENT_STATIONMODE_INPROGRESS   (WM_APP+3)
#define WM_ENABLE_APPLY_STATIONMODE             (WM_APP+4)
#define WM_SET_STATIONMODE_PROPOSALSTATE        (WM_APP+5)
#define WM_INITIALISE                           (WM_APP+6)
#define WM_SET_CURRENT_STATIONMODE_LOCK			(WM_APP+7)
#define WM_SET_INBOUND_STATION_LOCK				(WM_APP+8)
#define WM_SET_OUTBOUND_STATION_LOCK			(WM_APP+9)

using TA_Base_Core::RunParams;
using TA_Base_Bus::TransActiveDialog;
using TA_Base_Bus::HelpLauncher;

/////////////////////////////////////////////////////////////////////////////
// ECSStationModesDlg dialog

namespace TA_IRS_App
{
    const int MINIMUM_DIALOG_WIDTH              = 465;
    const int MINIMUM_DIALOG_HEIGHT             = 435;
    const std::string SM_DIALOG_LABEL           = "ECS Station Mode";
    const std::string STATION_MODE_LABEL        = "Station Mode";
    const std::string OPERATION_LABEL           = "Operation";
    const std::string TRACK_LABEL               = "Track";
    const std::string CONDITION_LABEL           = "Operating Condition";
    const int STATION_MODE_WIDTH                = 100;
	const int OPERATION_WIDTH                   = 220;
    const int TRACK_WIDTH                       = 90;
    const int CONDITION_WIDTH                   = 150;


    //
    // Constructor
    //
    ECSStationModesDlg::ECSStationModesDlg( TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/ ): 
	TransActiveDialog( genericGUICallback, ECSStationModesDlg::IDD, pParent ), 
	m_pECSStationModes( 0 ),
    m_rightRelativeOffset( 0 ), 
	m_bottomRelativeOffset( 0 ), 
	m_areOffsetsSetup( false ), 
	m_flashToggle( false ),
    m_currentStationModeDisplayColour( ::GetSysColor( COLOR_BTNFACE ) ),
	m_isInProgressBlinking(false)
    {
        FUNCTION_ENTRY( "Constructor" );

        //{{AFX_DATA_INIT(ECSStationModesDlg)
        //}}AFX_DATA_INIT

        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );

        // create the smart object which performs all the logic for this view.
        //m_pECSStationModes = std::auto_ptr< ECSStationModes >( new ECSStationModes( *this ) );

        FUNCTION_EXIT;
    }


    //
    // DoDataExchange
    //
    void ECSStationModesDlg::DoDataExchange( CDataExchange* pDX )
    {
        TransActiveDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(ECSStationModesDlg)
        DDX_Control(pDX, IDC_STATUS_EDIT, m_statusEditControl);
        DDX_Control(pDX, IDC_CURRENTSTATIONMODE_DISPLAY, m_currentStationModeDisplay);
        DDX_Control(pDX, IDC_STATIONMODESELECTION_LISTCTRL, m_stationModeSelectionListCtrl);
        DDX_Control(pDX, IDC_OUTBOUNDLOCATION_DISPLAY, m_outboundLocationDisplay);
        DDX_Control(pDX, IDC_INBOUNDLOCATION_DISPLAY, m_inboundLocationDisplay);
        DDX_Control(pDX, IDC_LOCATIONSELECTION_COMBO, m_locationSelectionCombo);
        DDX_Control(pDX, IDC_APPLYSTATIONMODE_BUTTON, m_applyButton);
		DDX_Control(pDX, IDC_CLOSE_BUTTON, m_closeButton);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP( ECSStationModesDlg, TransActiveDialog )
        //{{AFX_MSG_MAP(ECSStationModesDlg)
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, onCloseButton)
    ON_CBN_SELCHANGE(IDC_LOCATIONSELECTION_COMBO, onLocationSelectionChanged)
    ON_MESSAGE_VOID(WM_STATIONMODE_DESELECTED, onStationModeDeselected)
    ON_MESSAGE_VOID(WM_INITIALISE, onInitialise)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_STATIONMODESELECTION_LISTCTRL, onStationModeSelectionItemChanged)
    ON_MESSAGE(WM_SET_CURRENT_STATIONMODE, onSetCurrentStationMode)
    ON_MESSAGE(WM_SET_CURRENT_STATIONMODE_INPROGRESS, onSetCurrentStationModeInProgress)
    ON_MESSAGE(WM_ENABLE_APPLY_STATIONMODE, onEnableApplyStationModeControl)
	ON_MESSAGE(WM_ENABLE_CLOSE,onEnableCloseButton)
    ON_MESSAGE(WM_SET_STATIONMODE_PROPOSALSTATE, onSetStationModeProposalState)
	ON_MESSAGE(WM_SET_CURRENT_STATIONMODE_LOCK, onSetCurrentStationModeLock)
	ON_MESSAGE(WM_SET_INBOUND_STATION_LOCK, onSetInboundStationLock)
	ON_MESSAGE(WM_SET_OUTBOUND_STATION_LOCK, onSetOutboundStationLock)
    ON_COMMAND(ID_APP_ABOUT, onAppAbout)
	ON_COMMAND(ID_HELP_INDEX, OnAppHelp)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_APPLYSTATIONMODE_BUTTON, onApplyStationModeButton)
	ON_BN_CLICKED(ID_HELP, OnHelpButton)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // ECSStationModesDlg message handlers

    //
    // OnInitDialog
    //
    BOOL ECSStationModesDlg::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        TA_Base_Bus::ResizingProperties rp;
        rp.canMaximise = false;
        rp.maxWidth = 1680;
        rp.maxHeight = 1050;
        CRect rtClient;
        GetClientRect(rtClient);
        rp.minWidth = rtClient.Width();
        rp.minHeight = rtClient.Height();
        setResizingProperties(rp);

        setupOffsets();

        TransActiveDialog::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog

        SetIcon( m_hIcon, TRUE );           // Set big icon

        SetWindowText(  SM_DIALOG_LABEL.c_str() );
        m_stationModeSelectionListCtrl.InsertColumn
            ( 0, _T( STATION_MODE_LABEL.c_str() ), LVCFMT_CENTER, STATION_MODE_WIDTH );
        m_stationModeSelectionListCtrl.InsertColumn
            ( 1, _T( OPERATION_LABEL.c_str() ), LVCFMT_CENTER, OPERATION_WIDTH );
        m_stationModeSelectionListCtrl.InsertColumn
            ( 2, _T( TRACK_LABEL.c_str() ), LVCFMT_CENTER, TRACK_WIDTH );
        m_stationModeSelectionListCtrl.InsertColumn
            ( 3, _T( CONDITION_LABEL.c_str() ), LVCFMT_CENTER, CONDITION_WIDTH );

        // Set up column struct for setting format of first column (CListCtrl always sets first column
        // to LVCFMT_LEFT despite what is specified during InsertColumn).
        LVCOLUMN lvCol;
        ::ZeroMemory(&lvCol, sizeof(LVCOLUMN));
        lvCol.mask = LVCF_FMT;
        lvCol.fmt = LVCFMT_CENTER;
        m_stationModeSelectionListCtrl.SetColumn( 0, &lvCol );

        m_stationModeSelectionListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES );

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
    void ECSStationModesDlg::onInitialise()
    {
        FUNCTION_ENTRY( "onInitialise" );

        m_pECSStationModes->initialise();
		
        FUNCTION_EXIT;
    }

	void ECSStationModesDlg::init()
	{
		// create the smart object which performs all the logic for this view.
        m_pECSStationModes = std::auto_ptr< ECSStationModes >( new ECSStationModes( *this ) );

		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/09/01 11:00:00

		//PostMessage( WM_INITIALISE );
		m_thread_hd = CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadFunction, this, 0, &m_thread_id );
		CloseHandle(m_thread_hd);
				
		//Launch Manager/////////////////////////////////////////////////////
	}
	
	/////////////////////////////////////////////////////////////////////
	//Launch Manager modified by: zhangjunsheng 2008/09/01 11:00:00
	
	DWORD WINAPI ECSStationModesDlg::ThreadFunction(LPVOID lpThreadParameter)
	{
		ECSStationModesDlg* ECSSMD = (ECSStationModesDlg*) lpThreadParameter;
		ECSSMD->onInitialise();
        ECSSMD->PostMessage( WM_ENABLE_CLOSE, static_cast<WPARAM>(TRUE) );
		return TRUE;
	}
				
	//Launch Manager/////////////////////////////////////////////////////

    //
    // OnSize
    //
    void ECSStationModesDlg::OnSize( UINT nType, int cx, int cy ) 
    {
        FUNCTION_ENTRY( "OnSize" );

        // Let the CDialog base class do any re-sizing processing it needs
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

            p_resizeWnd = GetDlgItem( IDC_STATIONMODESELECTION_GROUPBOX );
            p_resizeWnd->GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
        
            // Use Station Mode selection GroupBox offsets as relative guide 
            // for all adjustments.
            int rightAdjust = resizeRect.right - ( thisRect.right - m_rightRelativeOffset );
            int bottomAdjust = resizeRect.bottom - ( thisRect.bottom - m_bottomRelativeOffset );
			CRect listRect;
			m_stationModeSelectionListCtrl.GetWindowRect(&listRect);

            // Station Mode selection GroupBox.
            resizeRect.right = resizeRect.right - rightAdjust;
            resizeRect.bottom = resizeRect.bottom - bottomAdjust;
            p_resizeWnd->MoveWindow( resizeRect );

            // Station Mode Selection List Ctrl.
			CRect satusModeListRect;
            m_stationModeSelectionListCtrl.GetWindowRect( &satusModeListRect );
            ScreenToClient( &satusModeListRect );
            satusModeListRect.right = satusModeListRect.right - rightAdjust;
            satusModeListRect.bottom = satusModeListRect.bottom - bottomAdjust;
            m_stationModeSelectionListCtrl.MoveWindow( &satusModeListRect );
			// Station Mode Status Edit Control
			m_statusEditControl.GetWindowRect(& resizeRect);
			ScreenToClient( &resizeRect );
			resizeRect.top = resizeRect.top - bottomAdjust;
			resizeRect.right = resizeRect.right - rightAdjust;
			resizeRect.bottom = resizeRect.bottom -  bottomAdjust;
			m_statusEditControl.MoveWindow(&resizeRect);

			// Station Mode Status Edit Value
            CWnd* p_resizeStatusGroupWnd;
            p_resizeStatusGroupWnd = GetDlgItem( IDC_STATUS_GROUPBOX3 );
            p_resizeStatusGroupWnd->GetWindowRect( &resizeRect );
       		ScreenToClient( &resizeRect );

			resizeRect.top = resizeRect.top - bottomAdjust;
			resizeRect.right = resizeRect.right - rightAdjust;
			resizeRect.bottom = resizeRect.bottom -  bottomAdjust;
			p_resizeStatusGroupWnd->MoveWindow(&resizeRect);		

            // The buttons along the bottom.
            p_resizeWnd = GetDlgItem( IDC_CLOSE_BUTTON );
            p_resizeWnd->GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
            p_resizeWnd->MoveWindow( ( resizeRect.left - rightAdjust ),
                ( resizeRect.top - bottomAdjust ), resizeRect.Width(), resizeRect.Height(), FALSE );        

            m_applyButton.GetWindowRect( &resizeRect );
            ScreenToClient( &resizeRect );
            m_applyButton.MoveWindow( ( resizeRect.left - rightAdjust ),
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

        FUNCTION_EXIT;
    }


    //
    // setLocations
    //
    void ECSStationModesDlg::setLocations( const LocationNames& locations )
    {
        FUNCTION_ENTRY( "setLocations" );

        //
        // It is important to retain order of Locations, as this is how we indicate the which location
        // the user selects back to our smart class (i.e. by index).
        // Therefore as we don't store the list in this class, must ensure ComboBox is not of type sort.
        LocationNames::const_iterator locationNamesIt = locations.begin();
        int index = 0;
        while ( locationNamesIt != locations.end() )
        {
            m_locationSelectionCombo.InsertString( index, _T( ( *locationNamesIt ).c_str() ) );
            index++;
            locationNamesIt++;
        }

        FUNCTION_EXIT;
    }


    //
    // setSelectedLocationIndex
    //
    void ECSStationModesDlg::setSelectedLocationIndex( unsigned int selectedLocationIndex )
    {
        FUNCTION_ENTRY( "setSelectedLocationIndex" );

        m_locationSelectionCombo.SetCurSel( selectedLocationIndex );

        FUNCTION_EXIT;
    }


    //
    // enableLocationSelectionControl
    //
    void ECSStationModesDlg::enableLocationSelectionControl( bool doEnableLocationSelectionControl )
    {
        FUNCTION_ENTRY( "enableLocationSelectionControl" );

        m_locationSelectionCombo.EnableWindow( doEnableLocationSelectionControl );

        FUNCTION_EXIT;
    }


    //
    // enableApplyStationModeControl
    //
    void ECSStationModesDlg::enableApplyStationModeControl( bool doEnableApplyStationModeControl )
    {
        FUNCTION_ENTRY( "enableApplyStationModeControl" );

        WPARAM wParam = static_cast< WPARAM >( doEnableApplyStationModeControl );
        PostMessage( WM_ENABLE_APPLY_STATIONMODE, wParam, 0 );

        FUNCTION_EXIT;
    }


    //
    // setStationModeDetails
    //
    void ECSStationModesDlg::setStationModeDetails( const StationModeDetails& stationModeDetails )
    {
        FUNCTION_ENTRY( "setStationModeDetails" );

        m_stationModeSelectionListCtrl.populateStationModeDetails( stationModeDetails );

        FUNCTION_EXIT;
    }


    //
    // setCurrentStationMode
    //
    void ECSStationModesDlg::setCurrentStationMode( const std::string& stationMode, 
        ECurrentStationModeType currentStationModeType )
    {
        FUNCTION_ENTRY( "setCurrentStationMode" );

        // This can be set from alternate thread so to be safe we post a message to ourself with the
        // pointer to the Station Mode string, and the ECurrentStationModeType passed in as params.
        std::string* p_stationMode = new std::string( stationMode );
        WPARAM wParam = reinterpret_cast< WPARAM >( p_stationMode );
        LPARAM lParam = static_cast< LPARAM >( currentStationModeType );
        PostMessage( WM_SET_CURRENT_STATIONMODE, wParam, lParam );

        FUNCTION_EXIT;
    }


    //
    // setCurrentStationModeInProgress
    //
    void ECSStationModesDlg::setCurrentStationModeInProgress( bool doShowAsInProgress )
    {
        FUNCTION_ENTRY( "setCurrentStationModeInProgress" );

        WPARAM wParam = static_cast< WPARAM >( doShowAsInProgress );
        PostMessage( WM_SET_CURRENT_STATIONMODE_INPROGRESS, wParam, 0 );

        FUNCTION_EXIT;
    }


	void ECSStationModesDlg::setCurrentStationVisualLock( bool isStationLocked, unsigned int index )
	{
        FUNCTION_ENTRY( "setCurrentStationVisualLock" );

        // Post the update 
		WPARAM wParam = static_cast<WPARAM>(isStationLocked);
		LPARAM lParam = static_cast<LPARAM>(index);
		PostMessage( WM_SET_CURRENT_STATIONMODE_LOCK, wParam, lParam);

        FUNCTION_EXIT;
	}


    //
    // setStationModeProposalState
    //
    void ECSStationModesDlg::setStationModeProposalState( unsigned int stationModeIndex, EModeProposalState stationModeProposalState )
    {
        FUNCTION_ENTRY( "setStationModeProposalState" );

        WPARAM wParam = static_cast< WPARAM >( stationModeIndex );
        LPARAM lParam = static_cast< LPARAM >( stationModeProposalState );
        PostMessage( WM_SET_STATIONMODE_PROPOSALSTATE, wParam, lParam );

        FUNCTION_EXIT;
    }


    //
    // setInboundLocation
    //
    void  ECSStationModesDlg::setInboundLocation( const std::string& innerboundLocation )
    {
        FUNCTION_ENTRY( "setInboundLocation" );

        m_inboundLocationDisplay.SetWindowText( _T( innerboundLocation.c_str() ) ); 

        FUNCTION_EXIT;
    }


    //
    // setOutboundLocation
    //
    void  ECSStationModesDlg::setOutboundLocation( const std::string& outerboundLocation )
    {
        FUNCTION_ENTRY( "setOutboundLocation" );

        m_outboundLocationDisplay.SetWindowText( _T( outerboundLocation.c_str() ) );

        FUNCTION_EXIT;
    }


	void ECSStationModesDlg::setInboundStationLock(bool isStationLocked)
	{
        FUNCTION_ENTRY( "setInboundStationLock" );

        // Post the update 
		WPARAM wParam = static_cast<WPARAM>(isStationLocked);
		PostMessage( WM_SET_INBOUND_STATION_LOCK, wParam, 0);

        FUNCTION_EXIT;		
	}


	void ECSStationModesDlg::setOutboundStationLock(bool isStationLocked)
	{
        FUNCTION_ENTRY( "setOutboundStationLock" );

        // Post the update 
		WPARAM wParam = static_cast<WPARAM>(isStationLocked);
		PostMessage( WM_SET_OUTBOUND_STATION_LOCK, wParam, 0);

        FUNCTION_EXIT;		
	}


    //
    // onCloseButton
    //
    void ECSStationModesDlg::onCloseButton() 
    {
        FUNCTION_ENTRY( "onCloseButton" );

        PostMessage( WM_CLOSE );

        FUNCTION_EXIT;
    }


    //
    // onLocationSelectionChnaged
    //
    void ECSStationModesDlg::onLocationSelectionChanged() 
    {
        FUNCTION_ENTRY( "onLocationSelectionChanged" );

		m_isInProgressBlinking = false;

		//zn++ CL-20717: clear the index when changing location
		m_stationModeSelectionListCtrl.setCurrentAppliedMode(-1, false);
		//when station change, should invalidate the index
		m_stationModeSelectionListCtrl.setCurrentSelection(-1);
		//++zn

        m_pECSStationModes->locationSelectionChanged( m_locationSelectionCombo.GetCurSel() );

        FUNCTION_EXIT;
    }


    //
    // onStationModeSelectionItemChanged
    //
    void ECSStationModesDlg::onStationModeSelectionItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        NM_LISTVIEW* pNMListView = ( NM_LISTVIEW* )pNMHDR;
        
        if ( pNMListView->uChanged & LVIF_STATE )
        {
            if ( pNMListView->uNewState & LVIS_SELECTED )
            {
                // Let the smart class know new Station Mode has been selected.
				//POSITION pos = m_stationModeSelectionListCtrl.GetFirstSelectedItemPosition();
				//int selectedStationModeIndex = m_stationModeSelectionListCtrl.GetNextSelectedItem( pos );

				// The if statement is required to stop the list control from sending the 
				// OnItemListChanged repeatedly. Because border selection on the list control is a hack,
				// we only want the control to send the update if selection is different.
				if(pNMListView->iItem != m_stationModeSelectionListCtrl.getCurrentSelection())
				{
					m_pECSStationModes->stationModeSelectionChanged( pNMListView->iItem );
					Invalidate();
				}
				
                
            }
            else if ( pNMListView->uOldState & LVIS_SELECTED )
            {
                // WM_STATIONMODE_DESELECTED handler will verify if deselection was due to alternate 
                // item being selected, or no item being any longer selected (if the latter case then
                // need to inform smart class).
                PostMessage( WM_STATIONMODE_DESELECTED );
            }
        }

        *pResult = 0;
    }


    //
    // onStationModeDeselected
    //
    void ECSStationModesDlg::onStationModeDeselected()
    {
        POSITION pos = m_stationModeSelectionListCtrl.GetFirstSelectedItemPosition();
        int currentSelection = m_stationModeSelectionListCtrl.GetNextSelectedItem( pos );
        if ( -1 == currentSelection ) // If deselection notification was NOT due to new selection.
        {
            m_pECSStationModes->stationModeSelectionChanged( currentSelection );
        }
    }


    //
    // onEnableApplyStationModeControl
    //
    LRESULT ECSStationModesDlg::onEnableApplyStationModeControl( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onEnableApplyStationModeControl" );

        BOOL doEnableStationModeControl = static_cast< BOOL >( wParam );        
        m_applyButton.EnableWindow( doEnableStationModeControl );

        FUNCTION_EXIT;
        return 0;
    }
LRESULT ECSStationModesDlg::onEnableCloseButton( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onEnableApplyStationModeControl" );

        BOOL doEnableStationModeControl = static_cast< BOOL >( wParam );        
        m_closeButton.EnableWindow( TRUE );

        FUNCTION_EXIT;
        return 0;
    }
	

    //
    // onSetCurrentStationMode
    //
    LRESULT ECSStationModesDlg::onSetCurrentStationMode( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onSetCurrentStationMode" );

        std::string* p_currentStationModeStr = reinterpret_cast< std::string* >( wParam );
        ECurrentStationModeType currentStationModeType = static_cast< ECurrentStationModeType >( lParam );

		if (currentStationModeType == TA_IRS_App::CURRENT_STATION_MODE_FAILED)
		{
			m_currentStationModeDisplay.setTextColour(0xFFFFFF);
		}
		else
		{
			m_currentStationModeDisplay.setTextColour(0x000000);
		}

        m_currentStationModeDisplay.SetWindowText( _T( p_currentStationModeStr->c_str() ) );            

		// marvin++
		if(*p_currentStationModeStr != "#")
		{
			//zn++ CL-20840
			//unsigned int modeIndex = atoi(p_currentStationModeStr->c_str());
			int modeIndex = atoi(p_currentStationModeStr->c_str());
			//++zn
			m_currentStationModeDisplayColour = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeIndex);
		}
		else
		{
			m_currentStationModeDisplayColour = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(currentStationModeType);
		}
		// ++marvin
		
        m_currentStationModeDisplay.setBackgroundColour( m_currentStationModeDisplayColour );
        

// 		// CL17285++
// 		std::string modeString = *p_currentStationModeStr;
// 
// 		if ( modeString != "#" )
// 		{
// 			m_stationModeSelectionListCtrl.setCurrentStationMode( modeString );
// 		}
// 		// ++CL17285
		
		delete p_currentStationModeStr;

        FUNCTION_EXIT;
        return 0;
    }


    //
    // onSetCurrentStationModeInProgress
    //
    LRESULT ECSStationModesDlg::onSetCurrentStationModeInProgress( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onSetCurrentStationModeInProgress" );

        BOOL doSetStationModeInProgress = static_cast<BOOL>(wParam);

        if (TRUE == doSetStationModeInProgress && false == m_isInProgressBlinking)
        {
            SetTimer( FLASH_TIMER_ID, FLASH_INTERVAL, NULL );
			m_isInProgressBlinking = true;
        }
        else if (FALSE == doSetStationModeInProgress && true == m_isInProgressBlinking)
        {
            KillTimer( FLASH_TIMER_ID );
			m_isInProgressBlinking = false;

            if( true == m_flashToggle )
            {
                m_currentStationModeDisplay.setBackgroundColour( m_currentStationModeDisplayColour );
                m_flashToggle = !m_flashToggle;
            }
        }

		Invalidate();

        FUNCTION_EXIT;
        return 0;
    }


    //
    // onSetStationModeProposalState
    //
    LRESULT ECSStationModesDlg::onSetStationModeProposalState( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY( "onSetStationModeProposalState" );

        unsigned int stationModeIndex = static_cast< unsigned int >( wParam );
        EModeProposalState stationModeProposalState = 
            static_cast< EModeProposalState >( lParam );
        m_stationModeSelectionListCtrl.setStationModeProposalState( stationModeIndex, stationModeProposalState );

        FUNCTION_EXIT;
        return 0;
    }
 

	LRESULT ECSStationModesDlg::onSetCurrentStationModeLock( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onSetCurrentStationModeLock" );
		
		unsigned int wparam = static_cast<unsigned int>(wParam);
		bool isStationLocked = (wparam != 0);
		unsigned int index = static_cast<unsigned int>(lParam);

		if(isStationLocked)
		{
			COLORREF colour = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(TA_IRS_App::CURRENT_STATION_LOCK);
			m_locationSelectionCombo.SetBackgroundColor(colour);
			m_locationSelectionCombo.MapIndexToColour(index, colour);
		}
		else // FALSE
		{
			COLORREF colour = RGB(255, 255, 255);
			m_locationSelectionCombo.SetBackgroundColor(colour); // White background?
			m_locationSelectionCombo.MapIndexToColour(index, colour);
		}

		Invalidate();

		FUNCTION_EXIT;
		return 0;
	}


	LRESULT ECSStationModesDlg::onSetInboundStationLock( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onSetCurrentStationModeLock" );
		
		unsigned int wparam = static_cast<unsigned int>(wParam);
		bool isStationLocked = (wparam != 0);

		if(isStationLocked)
		{
			m_inboundLocationDisplay.setBackgroundColour(TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(TA_IRS_App::CURRENT_STATION_LOCK));
		}
		else // FALSE
		{
			m_inboundLocationDisplay.setBackgroundColour(::GetSysColor(COLOR_3DFACE)); // White background?
		}

		Invalidate();

		FUNCTION_EXIT;
		return 0;
	}


	LRESULT ECSStationModesDlg::onSetOutboundStationLock( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY( "onSetOutboundStationLock" );
		
		unsigned int wparam = static_cast<unsigned int>(wParam);
		bool isStationLocked = (wparam != 0);

		if(isStationLocked)
		{
			m_outboundLocationDisplay.setBackgroundColour(TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(TA_IRS_App::CURRENT_STATION_LOCK));
		}
		else // FALSE
		{
			m_outboundLocationDisplay.setBackgroundColour(::GetSysColor(COLOR_3DFACE)); // White background?
		}

		Invalidate();

		FUNCTION_EXIT;
		return 0;
	}
	

	void ECSStationModesDlg::setColourMap(unsigned int index, COLORREF colour)
	{
		m_locationSelectionCombo.MapIndexToColour(index, colour);
	}

    //
    // onAppAbout
    //
    void ECSStationModesDlg::onAppAbout()
    {
        FUNCTION_ENTRY( "onAppAbout" );

        // Display the about box
        HelpLauncher::getInstance().displayAboutBox();  

        FUNCTION_EXIT;
    }


    //
    // terminateApplication
    //
    void ECSStationModesDlg::terminateApplication()
    {
        FUNCTION_ENTRY( "terminateApplication" );

        PostMessage( WM_CLOSE );

        FUNCTION_EXIT;
    }

    
    //
    // displayErrorMessage
    //
    void ECSStationModesDlg::displayErrorMessage( const std::string& errorMessage )
    {
        FUNCTION_ENTRY( "displayErrorMessage" );

        MessageBox( _T( errorMessage.c_str() ), RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_ICONERROR | MB_OK );

        FUNCTION_EXIT;
    }


    //
    // displayWarningMessage
    //
    void ECSStationModesDlg::displayWarningMessage( const std::string& warningMessage )
    {
        FUNCTION_ENTRY( "displayWarningMessage" );

        MessageBox( _T( warningMessage.c_str() ), RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_ICONWARNING | MB_OK );

        FUNCTION_EXIT;
    }


    //
    // setupOffsets
    //
    void ECSStationModesDlg::setupOffsets()
    {
        FUNCTION_ENTRY( "setupOffsets" );

        CRect thisRect;
        GetWindowRect(&thisRect);

        CRect stationModeSelectionRect;
        GetDlgItem( IDC_STATIONMODESELECTION_GROUPBOX )->GetWindowRect( &stationModeSelectionRect );

        m_rightRelativeOffset = thisRect.right - stationModeSelectionRect.right;
        m_bottomRelativeOffset = thisRect.bottom - stationModeSelectionRect.bottom;

        m_areOffsetsSetup = true;

        FUNCTION_EXIT;
    }


    //
    // onApplyStationModeButton
    //
    void ECSStationModesDlg::onApplyStationModeButton() 
    {
        FUNCTION_ENTRY( "onApplyStationModeButton" );

        POSITION pos = m_stationModeSelectionListCtrl.GetFirstSelectedItemPosition();
        int selectedStationModeIndex = m_stationModeSelectionListCtrl.GetNextSelectedItem( pos );
        if ( -1 != selectedStationModeIndex )
        {
            m_pECSStationModes->applyStationMode( selectedStationModeIndex );
		}
        m_stationModeSelectionListCtrl.SetItemState(m_stationModeSelectionListCtrl.getCurrentSelection(), 0 , LVIS_SELECTED);
        FUNCTION_EXIT;
    }


	//
    // aquireConfirmation
    //
	bool ECSStationModesDlg::aquireConfirmation( const std::string& confirmationMessage )
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


	void ECSStationModesDlg::addStatusLine(const std::string& statusText)
	{
		
		while (m_statusEditControl.GetLimitText() < (m_statusEditControl.GetWindowTextLength() + strlen(statusText.c_str()) + 2))
		{
			int pos = m_statusEditControl.LineIndex(m_statusEditControl.GetLineCount()-2);
			m_statusEditControl.SetSel(pos, -1, FALSE);
			m_statusEditControl.ReplaceSel("");
		}
		
		// Make a new line for the new status text
		m_statusEditControl.SetSel(0,0);
		m_statusEditControl.ReplaceSel("\r\n");

		// Insert the new status text
		m_statusEditControl.SetSel(0,0);
		m_statusEditControl.ReplaceSel(statusText.c_str());

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"addStatusLine() - %s", statusText.c_str());
	}


    //
    // OnTimer
    //
    void ECSStationModesDlg::OnTimer(UINT nIDEvent) 
    {
        switch ( nIDEvent )
        {
        case FLASH_TIMER_ID:
            {
                if ( m_flashToggle )
                {
                    m_currentStationModeDisplay.setBackgroundColour(m_currentStationModeDisplayColour);
                }
                else
                {
                    m_currentStationModeDisplay.setBackgroundColour(::GetSysColor( COLOR_WINDOW )); // sets to default.
                }
                m_flashToggle = !m_flashToggle;
            }
            break;
        default:
            break;
        }
        
        TransActiveDialog::OnTimer(nIDEvent);
    }


    //
    // onClose
    //
    void ECSStationModesDlg::OnClose() 
    {
        FUNCTION_ENTRY( "OnClose" );

        m_pECSStationModes->prepareForClose();

		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();

        FUNCTION_EXIT;
    }
	
	// 7196-Start
	void ECSStationModesDlg::refreshHealthPollInterval()
	{
	    FUNCTION_ENTRY( "refreshHealthPollInterval" );
		m_pECSStationModes->refreshHealthPollInterval();
        FUNCTION_EXIT;

	}	
	// 7196-End

	//TD16730
	void ECSStationModesDlg::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}

	void ECSStationModesDlg::OnAppHelp() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
	}

	void ECSStationModesDlg::setCurrentAppliedMode(int index, bool isOverriding)
	{
		m_stationModeSelectionListCtrl.setCurrentAppliedMode(index, isOverriding);
	}


} // namespace TA_IRS_App
