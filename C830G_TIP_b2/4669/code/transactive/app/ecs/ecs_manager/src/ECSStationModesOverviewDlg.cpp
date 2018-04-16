/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class is a derivation of TransActiveDialog and implementation of the IECSStationModesOverviewView
  * interface. It provides the actual ECS Station Modes Overview user interface, but defers the responsibility
  * of it's logic and data to the ECSStationModesOverview member (which manipulates this object via
  * IECSStationModesOverviewView).
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER


#include "app/ecs/ecs_manager/src/stdafx.h"
#include "afxpriv.h"

#include "app/ecs/ecs_manager/src/ECSManager.h"
#include "app/ecs/ecs_manager/src/ECSStationModesOverview.h"
#include "app/ecs/ecs_manager/src/ECSStationModesOverviewDlg.h"
#include "app/ecs/ecs_manager/src/CongestionParameterDlg.h"
#include "app/ecs/ecs_manager/src/TimetableSelectionDlg.h"
#include "app/ecs/ecs_manager/src/DecisionDialog.h"

#include "bus/generic_gui/src/HelpLauncher.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_SET_CURRENT_STATIONMODE                  (WM_APP+1)
#define WM_SET_CURRENT_STATIONMODE_INPROGRESS       (WM_APP+2)
#define WM_SET_CURRENT_PROPOSED_STATIONMODE         (WM_APP+3)
#define WM_ENABLE_APPLY_STATIONMODE                 (WM_APP+4)
#define WM_ENABLE_APPLYALL_CONGESTEDSTATIONMODES    (WM_APP+5)
#define WM_INITIALISE                               (WM_APP+6)
#define WM_SET_CURRENT_STATIONMODE_LOCK             (WM_APP+7)
#define WM_SET_CURRENT_STATIONAUTOMODE              (WM_APP+8)
#define WM_SET_CURRENT_STATIONAUTOMODECONTROLSTATE  (WM_APP+9)
#define WM_ADD_STATUSLINE                           (WM_APP+10)
#define WM_SET_LOCATIONS                            (WM_APP+11)

using namespace TA_Base_Bus;
using namespace TA_Base_Core;

namespace TA_IRS_App
{
const char* const  OVERVIEW_DIALOG_CAPTION = "ECS Station Modes Overview";

// Grids static box
const int LEFT_STATIC_OFFSET = 14;
const int TOP_STATIC_OFFSET = 8;
const int STATIC_WIDTH = 852;

// Grid Control
const int leftGridControlOffSet = 32;
const int topGridControlOffSet = 30;
const int TOP_GRIDCTRL2_OFFSET = 210;
const int TOP_GRIDCTRL3_OFFSET = 390;
const int gridControlWidth = 816;

// Status static box
const int STATIC2_WIDTH = 852;
const int STATIC2_HEIGHT = 60;
const int TOP_STATIC2_OFFSET = 590;

// Status edit box
const int TOP_STATUSEDIT_OFFSET = 610;
const int STATUSEDIT_HEIGHT = 40;

// Buttons
const int VERT_OFFSET = 720;
const int TOP_BUTTON_OFFSET = 660;

const int HELP_BUTTON_WIDTH = 30;

const int LEFT_HELP_OFFSET = 8;
const int LEFT_APPLY_ALL_OFFSET = 406; //422
const int LEFT_APPLY_OFFSET = 638; //658
const int LEFT_CLOSE_OFFSET = 770;

//
// Constructor
//
ECSStationModesOverviewDlg::ECSStationModesOverviewDlg( TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/ )
    : TransActiveDialog( genericGUICallback, ECSStationModesOverviewDlg::IDD, pParent ), m_pECSStationModesOverview( 0 )
{
    FUNCTION_ENTRY( "Constructor" );

    //{{AFX_DATA_INIT(EventViewerDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );


    FUNCTION_EXIT;
}

void ECSStationModesOverviewDlg::init()
{
    // create the smart object which performs all the logic for this view.
    m_pECSStationModesOverview = std::auto_ptr< ECSStationModesOverview >( new ECSStationModesOverview( *this ) );

    // Reduce launching manager time
    //PostMessage( WM_INITIALISE );

    start();

}

//
// createGridCtrls
//
void ECSStationModesOverviewDlg::createGridCtrls()
{
    FUNCTION_ENTRY( "createGridCtrls" );

    // Use our dummy static controls to retrieve the positioning for the grids and then destroy them.
    CRect position1, position2, position3;

    GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_1 )->GetWindowRect( &position1 );
    GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_1 )->DestroyWindow();
    GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_2 )->GetWindowRect( &position2 );
    GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_2 )->DestroyWindow();
    GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_3 )->GetWindowRect( &position3 );
    GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_3 )->DestroyWindow();

    // Now convert to client position rather than on the whole screen
    ScreenToClient( &position1 );
    ScreenToClient( &position2 );
    ScreenToClient( &position3 );

    // Now create the subsystem grids at the positions given.
    m_stationModesOverviewGridCtrl1.Create( position1 , this, IDC_STATIONMODESOVERVIEW_GRIDCTRL_1,
                                            WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER );
    m_stationModesOverviewGridCtrl2.Create( position2 , this, IDC_STATIONMODESOVERVIEW_GRIDCTRL_2,
                                            WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER );
    m_stationModesOverviewGridCtrl3.Create( position3 , this, IDC_STATIONMODESOVERVIEW_GRIDCTRL_3,
                                            WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER );

    m_stationModesOverviewGridCtrl1.setUpGridCtrl();
    m_stationModesOverviewGridCtrl2.setUpGridCtrl();
    m_stationModesOverviewGridCtrl3.setUpGridCtrl();

    FUNCTION_EXIT;
}


//
// setLocations
//
void ECSStationModesOverviewDlg::setLocations( const LocationNames& locationNames )
{
    FUNCTION_ENTRY( "setLocations" );

    WPARAM wParam = reinterpret_cast<WPARAM>( new LocationNames( locationNames ) );
    PostMessage( WM_SET_LOCATIONS, wParam, 0 );

    FUNCTION_EXIT;
}


//
// terminateApplication
//
void ECSStationModesOverviewDlg::terminateApplication()
{
    FUNCTION_ENTRY( "terminateApplication" );

    PostMessage( WM_CLOSE );

    FUNCTION_EXIT;
}


//
// displayErrorMessage
//
void ECSStationModesOverviewDlg::displayErrorMessage( const std::string& errorMessage )
{
    FUNCTION_ENTRY( "displayErrorMessage" );

    MessageBox( _T( errorMessage.c_str() ), RunParams::getInstance().get( RPARAM_APPNAME ).c_str(), MB_ICONERROR | MB_OK );

    FUNCTION_EXIT;
}


//
// aquireConfirmation
//
bool ECSStationModesOverviewDlg::aquireConfirmation( const std::string& confirmationMessage )
{
    FUNCTION_ENTRY( "aquireConfirmation" );

    int confirmation = MessageBox( _T( confirmationMessage.c_str() ), RunParams::getInstance().get( RPARAM_APPNAME ).c_str(), MB_ICONQUESTION | MB_YESNO );
    if( confirmation == IDYES )
    {
        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


//
// aquireUserDecision
//
int ECSStationModesOverviewDlg::aquireUserDecision( const std::string& decisionMessage, const std::vector< std::string >& choices )
{
    FUNCTION_ENTRY( "aquireUserDecision" );

    DecisionDialog decisionDialog( this, decisionMessage, choices );

    FUNCTION_EXIT;
    return decisionDialog.DoModal();
}


//
// displayWarningMessage
//
void ECSStationModesOverviewDlg::displayWarningMessage( const std::string& warningMessage )
{
    FUNCTION_ENTRY( "displayWarningMessage" );

    MessageBox( _T( warningMessage.c_str() ), RunParams::getInstance().get( RPARAM_APPNAME ).c_str(), MB_ICONWARNING | MB_OK );

    FUNCTION_EXIT;
}


//
// enableApplyStationModeControl
//
void ECSStationModesOverviewDlg::enableApplyStationModeControl( bool doEnableApplyStationModeControl )
{
    FUNCTION_ENTRY( "enableApplyStationModeControl" );

    WPARAM wParam = static_cast< WPARAM >( doEnableApplyStationModeControl );
    PostMessage( WM_ENABLE_APPLY_STATIONMODE, wParam, 0 );

    FUNCTION_EXIT;
}


//
// enableApplyAllCongestedStationModesControl
//
void ECSStationModesOverviewDlg::enableApplyAllCongestedStationModesControl( bool doEnableApplyAllCongestedStationModesControl )
{
    FUNCTION_ENTRY( "enableApplyAllCongestedStationModesControl" );

    WPARAM wParam = static_cast< WPARAM >( doEnableApplyAllCongestedStationModesControl );
    PostMessage( WM_ENABLE_APPLYALL_CONGESTEDSTATIONMODES, wParam, 0 );

    FUNCTION_EXIT;
}


//
// setCurrentStationMode
//
void ECSStationModesOverviewDlg::setCurrentStationMode( int locationIndex, const std::string& stationMode, ECurrentStationModeType stationModeType )
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
// setCurrentStationAutoMode
//
void ECSStationModesOverviewDlg::setCurrentStationAutoMode( int locationIndex, bool autoModeStatus )
{
    FUNCTION_ENTRY( "setCurrentStationAutoMode" );

    WPARAM wParam = static_cast< WPARAM >( locationIndex );
    LPARAM lParam = static_cast< WPARAM >( autoModeStatus );

    PostMessage( WM_SET_CURRENT_STATIONAUTOMODE, wParam, lParam );

    FUNCTION_EXIT;
}

//
// setCurrentStationAutoModeControlState
//
void ECSStationModesOverviewDlg::setCurrentStationAutoModeControlState( int locationIndex, TA_Base_Bus::EDataPointControlState controlState )
{
    FUNCTION_ENTRY( "setCurrentStationAutoModeControlState" );

    WPARAM wParam = static_cast< WPARAM >( locationIndex );
    LPARAM lParam = static_cast< LPARAM >( controlState );

    PostMessage( WM_SET_CURRENT_STATIONAUTOMODECONTROLSTATE, wParam, lParam );

    FUNCTION_EXIT
}

//
// setCurrentStationModeInProgress
//
void ECSStationModesOverviewDlg::setCurrentStationModeInProgress( int locationIndex, bool doShowAsInProgress )
{
    FUNCTION_ENTRY( "setCurrentStationModeInProgress" );

    WPARAM wParam = static_cast< WPARAM >( locationIndex );
    LPARAM lParam = static_cast< WPARAM >( doShowAsInProgress );

    PostMessage( WM_SET_CURRENT_STATIONMODE_INPROGRESS, wParam, lParam );

    FUNCTION_EXIT;
}

void ECSStationModesOverviewDlg::setCurrentStationVisualLock( int locationIndex, bool isStationLocked )
{
    // Post update to self.
    FUNCTION_ENTRY( "setCurrentStationVisualLock" );

    WPARAM wParam = static_cast< WPARAM >( locationIndex );
    LPARAM lParam = static_cast< WPARAM >( isStationLocked );

    PostMessage( WM_SET_CURRENT_STATIONMODE_LOCK, wParam, lParam );

    FUNCTION_EXIT;
}


// liuyu++ 10745a
//
// setCurrentProposedStationMode
//
void ECSStationModesOverviewDlg::setCurrentProposedStationMode( int locationIndex, EProposedStationModeType proposedStationModeType,
                                                                const std::string& proposedStationMode, EModeProposalState proposalState )
{
    FUNCTION_ENTRY( "setCurrentProposedStationMode" );

    std::pair< int, EProposedStationModeType >* p_IndexTypePair =
        new std::pair< int, EProposedStationModeType >;

    ( *p_IndexTypePair ).first = locationIndex;
    ( *p_IndexTypePair ).second = proposedStationModeType;

    WPARAM wParam = reinterpret_cast< WPARAM >( p_IndexTypePair );

    std::pair< std::string, EModeProposalState >* p_ModeStatePair =
        new std::pair< std::string, EModeProposalState >;

    ( *p_ModeStatePair ).first = proposedStationMode;
    ( *p_ModeStatePair ).second = proposalState;

    LPARAM lParam = reinterpret_cast< LPARAM >( p_ModeStatePair );

    PostMessage( WM_SET_CURRENT_PROPOSED_STATIONMODE, wParam, lParam );

    FUNCTION_EXIT;
}
// ++liuyu 10745a

//
// DoDataExchange
//
void ECSStationModesOverviewDlg::DoDataExchange( CDataExchange* pDX )
{
    TransActiveDialog::DoDataExchange( pDX );
    //{{AFX_DATA_MAP(ECSStationModesOverviewDlg)
    DDX_Control( pDX, IDC_STATIONOVERVIEW_STATUS_EDIT, m_statusEditControl );
    DDX_Control( pDX, IDC_APPLYALL_CONGESTIONSTATIONMODES_BUTTON, m_applyAllCongestedStationModesButton );
    DDX_Control( pDX, IDC_APPLYSTATIONMODE_BUTTON, m_applyStationModeButton );
    DDX_Control( pDX, IDC_CLOSE_BUTTON, m_closeButton );
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( ECSStationModesOverviewDlg, TransActiveDialog )
    //{{AFX_MSG_MAP(ECSStationModesOverviewDlg)
    ON_MESSAGE_VOID( WM_INITIALISE, onInitialise )
    ON_NOTIFY( GVN_SELCHANGED, IDC_STATIONMODESOVERVIEW_GRIDCTRL_1, onStationModeOverviewGrid1SelectionChanged )
    ON_NOTIFY( GVN_SELCHANGED, IDC_STATIONMODESOVERVIEW_GRIDCTRL_2, onStationModeOverviewGrid2SelectionChanged )
    ON_NOTIFY( GVN_SELCHANGED, IDC_STATIONMODESOVERVIEW_GRIDCTRL_3, onStationModeOverviewGrid3SelectionChanged )
    ON_MESSAGE( WM_ENABLE_APPLY_STATIONMODE, onEnableApplyStationModeControl )
    ON_MESSAGE( WM_ENABLE_APPLYALL_CONGESTEDSTATIONMODES, onEnableApplyAllCongestedStationModesControl )
    ON_MESSAGE( WM_SET_CURRENT_STATIONMODE, onSetCurrentStationMode )
    ON_MESSAGE( WM_SET_CURRENT_STATIONMODE_INPROGRESS, onSetCurrentStationModeInProgress )
    ON_MESSAGE( WM_SET_CURRENT_PROPOSED_STATIONMODE, onSetCurrentProposedStationMode )
    ON_MESSAGE( WM_SET_CURRENT_STATIONMODE_LOCK, onSetCurrentStationModeLock )

    ON_MESSAGE( WM_ADD_STATUSLINE, onAddStatusLine )
    ON_MESSAGE( WM_SET_LOCATIONS, onSetLocations )

    ON_BN_CLICKED( IDC_CLOSE_BUTTON, onCloseButton )
    ON_COMMAND( ID_APP_ABOUT, onAppAbout )
    ON_COMMAND( ID_HELP_INDEX, OnAppHelp )
    ON_WM_DESTROY()
    ON_BN_CLICKED( IDC_APPLYSTATIONMODE_BUTTON, onApplyProposedStationModeButton )
    ON_BN_CLICKED( IDC_APPLYALL_CONGESTIONSTATIONMODES_BUTTON, onApplyAllProposedCongestionStationModesButton )
    ON_MESSAGE( WM_SET_CURRENT_STATIONAUTOMODE, onSetCurrentStationAutoMode )
    ON_BN_CLICKED( ID_HELP, OnHelpButton ) //TD16730
    ON_WM_SIZE()
    ON_MESSAGE( WM_SET_CURRENT_STATIONAUTOMODECONTROLSTATE, onSetCurrentStationAutoModeControlState )
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//ON_COMMAND(ID_FILE_1, OnFile1)
//ON_COMMAND(ID_FILE_2, OnFile2)

/////////////////////////////////////////////////////////////////////////////
// ECSStationModesOverviewDlg message handlers


//
// OnInitDialog
//
BOOL ECSStationModesOverviewDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    TA_Base_Bus::ResizingProperties rp;
    rp.canMaximise = true;
    rp.maxWidth = -1;
    rp.maxHeight = -1;
    CRect rtClient;
    GetWindowRect( rtClient );
    rp.minWidth = rtClient.Width();
    rp.minHeight = rtClient.Height();
    setResizingProperties( rp );

    TransActiveDialog::OnInitDialog();
    // Set the icon for this dialog.  The framework does this automatically
    // when the application's main window is not a dialog
    SetIcon( m_hIcon, TRUE );           // Set big icon

    // Set window caption here to ensure drawn correctly next to icon (doesn't work
    // if we just use resource file as GenGUI calls SetWindowPos
    SetWindowText( OVERVIEW_DIALOG_CAPTION );

    createGridCtrls();

    // Have to call redaw, else the Control Station Saved User Settings poitioning does not
    // draw properly.
    //RedrawWindow();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE  unless you set the focus to a control
}


//
// onInitialise
//
void ECSStationModesOverviewDlg::onInitialise()
{
    FUNCTION_ENTRY( "onInitialise" );

    m_pECSStationModesOverview->initialise();


    FUNCTION_EXIT;
}


//
// onStationModeOverviewGrid1SelectionChanged
//
void ECSStationModesOverviewDlg::onStationModeOverviewGrid1SelectionChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_GRIDVIEW* pNMGridView = ( NM_GRIDVIEW* )pNMHDR;

    //zn++ - CL-20790
    if( pNMGridView->iColumn == 0 )
    {
        enableApplyStationModeControl( true );
        return;
    }
    //++zn

    //      m_stationModesOverviewGridCtrl2.clearSelection();       //CL-20791
    //      m_stationModesOverviewGridCtrl3.clearSelection();       //CL-20791

    //10745q-Start
    m_stationModesOverviewGridCtrl1.ResetSelectedRow();
    //      m_stationModesOverviewGridCtrl2.ResetSelectedRow();     //CL-20791
    //      m_stationModesOverviewGridCtrl3.ResetSelectedRow();     //CL-20791
    //10745q-End

    //zn++ - CL-20790
    if( pNMGridView->iColumn == -1 && pNMGridView->iRow == -1 )
    {
        m_stationModesOverviewGridCtrl2.clearSelection();
        m_stationModesOverviewGridCtrl3.clearSelection();
        enableApplyStationModeControl( isSelectedAutoMode() );
        return;
    }
    //++zn

    LocationIndexAndProposedType locationIndexAndProposedType = m_stationModesOverviewGridCtrl1
                                                                .getProposedStationModeFromCellCoordinates( pNMGridView->iRow, pNMGridView->iColumn );

    m_pECSStationModesOverview->proposedStationModeSelected( locationIndexAndProposedType.first, locationIndexAndProposedType.second );

    if( pNMGridView->iRow == CURRENT_STATION_AUTO_MODE_ROW )
    {
        m_pECSStationModesOverview->autoModeSelected();
    }

    *pResult = 0;
}


//
// onStationModeOverviewGrid2SelectionChanged
//
void ECSStationModesOverviewDlg::onStationModeOverviewGrid2SelectionChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_GRIDVIEW* pNMGridView = ( NM_GRIDVIEW* )pNMHDR;

    //zn++ - CL-20790
    if( pNMGridView->iColumn == 0 )
    {
        enableApplyStationModeControl( true );
        return;
    }
    //++zn

    //      m_stationModesOverviewGridCtrl1.clearSelection();       //CL-20791
    //      m_stationModesOverviewGridCtrl3.clearSelection();       //CL-20791

    //10745q-Start
    //      m_stationModesOverviewGridCtrl1.ResetSelectedRow();     //CL-20791
    m_stationModesOverviewGridCtrl2.ResetSelectedRow();
    //      m_stationModesOverviewGridCtrl3.ResetSelectedRow();     //CL-20791
    //10745q-End

    //zn++ - CL-20790
    if( pNMGridView->iColumn == -1 && pNMGridView->iRow == -1 )
    {
        m_stationModesOverviewGridCtrl1.clearSelection();
        m_stationModesOverviewGridCtrl3.clearSelection();
        enableApplyStationModeControl( isSelectedAutoMode() );
        return;
    }
    //++zn

    LocationIndexAndProposedType locationIndexAndProposedType = m_stationModesOverviewGridCtrl2
                                                                .getProposedStationModeFromCellCoordinates( pNMGridView->iRow, pNMGridView->iColumn );

    if( locationIndexAndProposedType.first > -1 )
    {
        locationIndexAndProposedType.first += StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID;
    }
    m_pECSStationModesOverview->proposedStationModeSelected( locationIndexAndProposedType.first, locationIndexAndProposedType.second );
    //zn++
    if( pNMGridView->iRow == CURRENT_STATION_AUTO_MODE_ROW )
    {
        m_pECSStationModesOverview->autoModeSelected();
    }
    //++zn
    *pResult = 0;

}


//
// onStationModeOverviewGrid3SelectionChanged
//
void ECSStationModesOverviewDlg::onStationModeOverviewGrid3SelectionChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_GRIDVIEW* pNMGridView = ( NM_GRIDVIEW* )pNMHDR;

    //zn++ - CL-20790
    if( pNMGridView->iColumn == 0 )
    {
        enableApplyStationModeControl( true );
        return;
    }
    //++zn

    //      m_stationModesOverviewGridCtrl1.clearSelection();       //CL-20791
    //      m_stationModesOverviewGridCtrl2.clearSelection();       //CL-20791

    //10745q-Start
    //      m_stationModesOverviewGridCtrl1.ResetSelectedRow();     //CL-20791
    //      m_stationModesOverviewGridCtrl2.ResetSelectedRow();     //CL-20791
    m_stationModesOverviewGridCtrl3.ResetSelectedRow();
    //10745q-End

    //zn++ - CL-20790
    if( pNMGridView->iColumn == -1 && pNMGridView->iRow == -1 )
    {
        m_stationModesOverviewGridCtrl1.clearSelection();
        m_stationModesOverviewGridCtrl2.clearSelection();
        enableApplyStationModeControl( isSelectedAutoMode() );
        return;
    }
    //++zn

    LocationIndexAndProposedType locationIndexAndProposedType = m_stationModesOverviewGridCtrl3
                                                                .getProposedStationModeFromCellCoordinates( pNMGridView->iRow, pNMGridView->iColumn );

    if( locationIndexAndProposedType.first > -1 )
    {
        locationIndexAndProposedType.first += ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID );
    }

    m_pECSStationModesOverview->proposedStationModeSelected( locationIndexAndProposedType.first, locationIndexAndProposedType.second );
    //zn++
    if( pNMGridView->iRow == CURRENT_STATION_AUTO_MODE_ROW )
    {
        m_pECSStationModesOverview->autoModeSelected();
    }
    //++zn
    *pResult = 0;
}


//
// onCloseButton
//
void ECSStationModesOverviewDlg::onCloseButton()
{
    FUNCTION_ENTRY( "onCloseButton" );

    PostMessage( WM_CLOSE );

    FUNCTION_EXIT;
}


//
// onEnableApplyStationModeControl
//
LRESULT ECSStationModesOverviewDlg::onEnableApplyStationModeControl( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onEnableApplyStationModeControl" );

    BOOL doEnableApplyStationModeControl = static_cast< BOOL >( wParam );

    //zn++ - CL-20790
    m_applyStationModeButton.EnableWindow( doEnableApplyStationModeControl );
    //m_applyStationModeButton.EnableWindow(true);
    //++zn

    FUNCTION_EXIT;
    return 0;
}


//
// onEnableApplyAllCongested
//
LRESULT ECSStationModesOverviewDlg::onEnableApplyAllCongestedStationModesControl( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onEnableApplyAllCongestedStationModesControl" );
    BOOL doEnableApplyAllCongestedStationModesControl = static_cast< BOOL >( wParam );
    m_applyAllCongestedStationModesButton.EnableWindow( doEnableApplyAllCongestedStationModesControl );
    FUNCTION_EXIT;
    return 0;
}


LRESULT ECSStationModesOverviewDlg::onSetCurrentStationModeLock( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onSetCurrentStationModeLock" );

    int locationIndex = static_cast<int>( wParam );
    int lparam = static_cast<int>( lParam );
    bool isStationLocked = ( lparam != 0 );

    if( locationIndex < StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID )
    {
        m_stationModesOverviewGridCtrl1.setCurrentStationModeLock( locationIndex, isStationLocked );
    }
    else if( locationIndex < ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        m_stationModesOverviewGridCtrl2.setCurrentStationModeLock( ( locationIndex - StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) , isStationLocked );
    }
    else if( locationIndex < ( 3 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        m_stationModesOverviewGridCtrl3.setCurrentStationModeLock( ( locationIndex - ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) ), isStationLocked );
    }

    FUNCTION_EXIT;
    return 0;
}


//
// onSetCurrentStationMode
//
LRESULT ECSStationModesOverviewDlg::onSetCurrentStationMode( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onSetCurrentStationMode" );

    int locationIndex = static_cast< int >( wParam );
    std::pair< std::string, ECurrentStationModeType >* p_ModeTypePair =
        reinterpret_cast< std::pair< std::string, ECurrentStationModeType >* >( lParam );

    if( locationIndex < StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID )
    {
        m_stationModesOverviewGridCtrl1.setCurrentStationMode( locationIndex,
                                                               ( *p_ModeTypePair ).first, ( *p_ModeTypePair ).second );
    }
    else if( locationIndex < ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        m_stationModesOverviewGridCtrl2.setCurrentStationMode( ( locationIndex - StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ),
                                                               ( *p_ModeTypePair ).first, ( *p_ModeTypePair ).second );
    }
    else if( locationIndex < ( 3 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        m_stationModesOverviewGridCtrl3.setCurrentStationMode( ( locationIndex - ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) ),
                                                               ( *p_ModeTypePair ).first, ( *p_ModeTypePair ).second );
    }

    delete p_ModeTypePair;

    FUNCTION_EXIT;
    return 0;
}

// liuyu++ 20050517 Auto Mode Reset
//
// onSetCurrentStationAutoMode
//
LRESULT ECSStationModesOverviewDlg::onSetCurrentStationAutoMode( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onSetCurrentStationAutoMode" );

    int locationIndex = static_cast< int >( wParam );
    BOOL autoModeStatus = static_cast< BOOL >( lParam );

    if( locationIndex < StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID )
    {
        m_stationModesOverviewGridCtrl1.setCurrentStationAutoMode( locationIndex, autoModeStatus );
    }
    else if( locationIndex < ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        //m_stationModesOverviewGridCtrl2.setCurrentStationAutoMode( locationIndex, autoModeStatus );
        m_stationModesOverviewGridCtrl2.setCurrentStationAutoMode( locationIndex - StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID, autoModeStatus );
    }
    else if( locationIndex < ( 3 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        //m_stationModesOverviewGridCtrl3.setCurrentStationAutoMode( locationIndex, autoModeStatus );
        m_stationModesOverviewGridCtrl3.setCurrentStationAutoMode( locationIndex - ( StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID * 2 ), autoModeStatus );
    }

    FUNCTION_EXIT;
    return 0;
}
// ++liuyu 20050517 Auto Mode Reset

LRESULT ECSStationModesOverviewDlg::onSetCurrentStationAutoModeControlState( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onSetCurrentStationAutoMode" );

    int locationIndex = static_cast< int >( wParam );
    TA_Base_Bus::EDataPointControlState controlState = static_cast< TA_Base_Bus::EDataPointControlState >( lParam );


    ECurrentStationAutoModeType autoModeType;

    // Set autoModeType to off so that the cell will revert to blank cell.
    autoModeType = TA_IRS_App::CURRENT_STATION_AUTO_MODE_OFF;

    if( locationIndex < StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID )
    {
        m_stationModesOverviewGridCtrl1.setCurrentStationAutoModeControlState( locationIndex, autoModeType );
    }
    else if( locationIndex < ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        //zn++ CL-20755: Station Mode OverviewGUI will crash when launched with SER update
        //m_stationModesOverviewGridCtrl2.setCurrentStationAutoModeControlState( locationIndex, autoModeType );
        m_stationModesOverviewGridCtrl2.setCurrentStationAutoModeControlState( locationIndex - StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID, autoModeType );
        //++zn
    }
    else if( locationIndex < ( 3 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        //zn++ CL-20755
        //m_stationModesOverviewGridCtrl3.setCurrentStationAutoModeControlState( locationIndex, autoModeType );
        m_stationModesOverviewGridCtrl3.setCurrentStationAutoModeControlState( locationIndex - ( StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID * 2 ), autoModeType );
        //++zn
    }

    FUNCTION_EXIT;
    return 0;
}

//
// onSetCurrentStationModeInProgress
//
LRESULT ECSStationModesOverviewDlg::onSetCurrentStationModeInProgress( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onSetCurrentStationModeInProgress" );

    int locationIndex = static_cast< int >( wParam );
    BOOL doShowAsInProgress = static_cast< BOOL >( lParam );

    if( locationIndex < StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID )
    {
        m_stationModesOverviewGridCtrl1.setCurrentStationModeInProgress( locationIndex, doShowAsInProgress );
    }
    else if( locationIndex < ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        //zn++ CL-20755
        //m_stationModesOverviewGridCtrl2.setCurrentStationModeInProgress( locationIndex, doShowAsInProgress );
        m_stationModesOverviewGridCtrl2.setCurrentStationModeInProgress( locationIndex - StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID, doShowAsInProgress );
        //++zn
    }
    else if( locationIndex < ( 3 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        //zn++ CL-20755
        //m_stationModesOverviewGridCtrl3.setCurrentStationModeInProgress( locationIndex, doShowAsInProgress );
        m_stationModesOverviewGridCtrl3.setCurrentStationModeInProgress( locationIndex - ( StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID * 2 ), doShowAsInProgress );
        //++zn
    }

    FUNCTION_EXIT;
    return 0;
}


//
// onSetCurrentProposedStationMode
//
LRESULT ECSStationModesOverviewDlg::onSetCurrentProposedStationMode( WPARAM wParam, LPARAM lParam )
{
    FUNCTION_ENTRY( "onSetCurrentProposedStationMode" );

    std::pair< int, EProposedStationModeType >* p_IndexTypePair =
        reinterpret_cast< std::pair< int, EProposedStationModeType >* >( wParam );

    std::pair< std::string, EModeProposalState >* p_ModeStatePair =
        reinterpret_cast< std::pair< std::string, EModeProposalState >* >( lParam );

    int locationIndex = ( *p_IndexTypePair ).first;

    if( locationIndex < StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID )
    {
        m_stationModesOverviewGridCtrl1.setCurrentProposedStationMode( locationIndex,
                                                                       ( *p_IndexTypePair ).second, ( *p_ModeStatePair ).first, ( *p_ModeStatePair ).second );
    }
    else if( locationIndex < ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        m_stationModesOverviewGridCtrl2.setCurrentProposedStationMode( ( locationIndex - StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ),
                                                                       ( *p_IndexTypePair ).second, ( *p_ModeStatePair ).first, ( *p_ModeStatePair ).second );
    }
    else if( locationIndex < ( 3 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
    {
        m_stationModesOverviewGridCtrl3.setCurrentProposedStationMode( ( locationIndex - ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) ),
                                                                       ( *p_IndexTypePair ).second, ( *p_ModeStatePair ).first, ( *p_ModeStatePair ).second );
    }

    delete p_IndexTypePair;
    delete p_ModeStatePair;

    FUNCTION_EXIT;
    return 0;
}


//
// onAppAbout
//
void ECSStationModesOverviewDlg::onAppAbout()
{
    FUNCTION_ENTRY( "onAppAbout" );

    // Display the about box
    HelpLauncher::getInstance().displayAboutBox();

    FUNCTION_EXIT;
}


//
// OnDestroy
//
void ECSStationModesOverviewDlg::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    terminateAndWait();
    m_pECSStationModesOverview->prepareForClose();

    FUNCTION_EXIT;
}


//
// onApplyAllProposedCongestionStationModes
//
void ECSStationModesOverviewDlg::onApplyAllProposedCongestionStationModesButton()
{
    FUNCTION_ENTRY( "onApplyAllProposedCongestionStationModesButton" );

    m_pECSStationModesOverview->applyAllProposedCongestionStationModes();

    FUNCTION_EXIT;
}


//
// onApplyProposedStationModeButton
//
void ECSStationModesOverviewDlg::onApplyProposedStationModeButton()
{
    FUNCTION_ENTRY( "onApplyProposedStationModeButton" );

    // 10745q-Start
    StationModesOverviewGridCtrl* gridCtrlArray[3] =
    {
        &m_stationModesOverviewGridCtrl1,
        &m_stationModesOverviewGridCtrl2,
        &m_stationModesOverviewGridCtrl3
    };

    //zn++ CL-17276 when multi selection, should seperate station mode execution and auto mode execution
    //process auto mode first
    for( int i = 0; i < 3; i++ )
    {
        if( gridCtrlArray[i]->m_selectedrowAutoMode.empty() )   // no auto mode selected in this grid
        {
            continue;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "NO.%d Grid: whole row, 17276 %d auto mode should be executed", i, gridCtrlArray[i]->m_selectedrowAutoMode.size() );
        LocationIndexAndAutoModeMap::const_iterator iter;
        for( iter = gridCtrlArray[i]->m_selectedrowAutoMode.begin();
             iter != gridCtrlArray[i]->m_selectedrowAutoMode.end();
             ++iter )
        {
            m_pECSStationModesOverview->SetAutoModeStatusForLocation(
                ( *iter ).first + i * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID,
                ( *iter ).second );
        }

        gridCtrlArray[i]->m_selectedrowAutoMode.clear();
    }

    std::map<int, EProposedStationModeType> allProposedIndexAndType;

    // process station mode applying
    for( int iCt = 0; iCt < 3; iCt++ )
    {
        if( gridCtrlArray[iCt]->GetRowSelectedFlag() || gridCtrlArray[iCt]->GetMultiSelectedFlag() ) // CL-20791 add multi-selection
        {
            std::vector<LocationIndexAndProposedType>::const_iterator nvIter;
            for( nvIter = gridCtrlArray[iCt]->m_selectedrowProposedTypes.begin();
                 nvIter != gridCtrlArray[iCt]->m_selectedrowProposedTypes.end();
                 ++nvIter )
            {
                // Begin TD17268 -- check the condition to avoid add invalid mode(xxx/0) to request auto mode
                if( ( ( *nvIter ).second == PROPOSED_FOR_CONGESTION ) ||
                    //((*nvIter).second == PROPOSED_FOR_EMERGENCY) ||  //zn++ CL-17276
                    ( ( *nvIter ).second == PROPOSED_FOR_NORMAL ) ) // zn++ CL-17276 Multi Select Normal Mode
                    // End   TD17268
                {
                    LocationIndexAndProposedType locationIndexAndProposedType = *nvIter;
                    //m_pECSStationModesOverview->applySelectedProposedStationMode(locationIndexAndProposedType.first,
                    //                                  locationIndexAndProposedType.second);
                    //zn++ CL-17276 Multi Select Mode only available for the first grid
                    //allProposedIndexAndType[locationIndexAndProposedType.first] = locationIndexAndProposedType.second;
                    allProposedIndexAndType[locationIndexAndProposedType.first + iCt * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID] = locationIndexAndProposedType.second;
                    //++zn
                }
            }
            gridCtrlArray[iCt]->ResetSelectedRow();
        }
        else
        {
            LocationIndexAndProposedType locationAndProposedType;

            CCellID cellId = gridCtrlArray[iCt]->GetFocusCell();
            if( cellId.IsValid() == false || cellId.row == CURRENT_STATION_AUTO_MODE_ROW ) // either none selection or auto mode selection should be exclude from this process
            {
                continue;
            }
            else
            {
                locationAndProposedType = gridCtrlArray[iCt]->getProposedStationModeFromCellCoordinates
                                          ( cellId.row,  cellId.col );

                //m_pECSStationModesOverview->applySelectedProposedStationMode(locationAndProposedType.first,
                //  locationAndProposedType.second );
                // zn++ CL-20755 Only the first grid location can be applied becoz of the index
                // index get from grid control is the logic index, should change it to real location index
                //allProposedIndexAndType[locationAndProposedType.first] = locationAndProposedType.second;
                allProposedIndexAndType[locationAndProposedType.first + iCt * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID] = locationAndProposedType.second;
                // ++zn
            }
        }
    }
    // ++zn

    if( allProposedIndexAndType.size() > 0 )
    {
        m_pECSStationModesOverview->confirmApplyProposedStationMode( allProposedIndexAndType );

    }

    //zn++ - CL-20790
    m_stationModesOverviewGridCtrl1.clearSelection();
    m_stationModesOverviewGridCtrl2.clearSelection();
    m_stationModesOverviewGridCtrl3.clearSelection();

    enableApplyStationModeControl( FALSE );
    //++zn
    FUNCTION_EXIT;
}

void ECSStationModesOverviewDlg::addStatusLine( const std::string& statusText )
{
    WPARAM wParam = reinterpret_cast<WPARAM>( new std::string( statusText ) );
    PostMessage( WM_ADD_STATUSLINE, wParam, 0 );
}

void ECSStationModesOverviewDlg::OnSize( UINT nType, int cx, int cy )
{
    TransActiveDialog::OnSize( nType, cx, cy );

    // TODO: Add your message handler code here

    CRect thisRect;
    GetWindowRect( &thisRect );
    ScreenToClient( &thisRect );
    // Start by resizing the menu line
    CWnd* menuLine = GetDlgItem( IDC_MENU_SEPERATOR2 );
    if( NULL != menuLine )
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

    CRect resizeRect, resizeStaticRect, resizeHelpRect, resizeApplyAllRect, resizeApplyRect, resizeCloseRect;
    CRect resizeGridCtrl2Rect, resizeGridCtrl3Rect, resizeStatusRect, resizeStatusStaticRect;
    CWnd* p_staticWnd;
    CWnd* p_resizeWnd;
    CWnd* p_resizeHelpWnd;
    CWnd* p_resizeApplyAllWnd;
    CWnd* p_resizeApplyWnd;
    CWnd* p_resizeCloseWnd;
    CWnd* p_resizeGridCtrl2Wnd;
    CWnd* p_resizeGridCtrl3Wnd;
    CWnd* p_resizeStatusWnd;
    CWnd* p_resizeStatusStaticWnd;

    p_staticWnd = GetDlgItem( IDC_STATIC );
    p_resizeWnd = GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_1 );
    p_resizeHelpWnd = GetDlgItem( ID_HELP );
    p_resizeApplyAllWnd = GetDlgItem( IDC_APPLYALL_CONGESTIONSTATIONMODES_BUTTON );
    p_resizeApplyWnd = GetDlgItem( IDC_APPLYSTATIONMODE_BUTTON );
    p_resizeCloseWnd = GetDlgItem( IDC_CLOSE_BUTTON );
    p_resizeGridCtrl2Wnd = GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_2 );
    p_resizeGridCtrl3Wnd = GetDlgItem( IDC_STATIONMODESOVERVIEW_GRIDCTRL_3 );
    p_resizeStatusWnd = GetDlgItem( IDC_STATIONOVERVIEW_STATUS_EDIT );
    p_resizeStatusStaticWnd = GetDlgItem( IDC_STATIONOVERVIEW_STATUS_GROUPBOX3 );


    if( NULL != p_resizeWnd )
    {
        p_resizeWnd->GetWindowRect( &resizeRect );
        ScreenToClient( &resizeRect );

        p_resizeWnd->MoveWindow( leftGridControlOffSet, topGridControlOffSet,
                                 gridControlWidth + ( cx - 880 ), resizeRect.Height() );
    }

    if( p_resizeGridCtrl2Wnd )
    {
        p_resizeGridCtrl2Wnd->GetWindowRect( &resizeGridCtrl2Rect );
        ScreenToClient( &resizeGridCtrl2Rect );

        p_resizeGridCtrl2Wnd->MoveWindow( leftGridControlOffSet, TOP_GRIDCTRL2_OFFSET,
                                          gridControlWidth + ( cx - 880 ), resizeGridCtrl2Rect.Height() );

    }

    if( p_resizeGridCtrl3Wnd )
    {
        p_resizeGridCtrl3Wnd->GetWindowRect( &resizeGridCtrl3Rect );
        ScreenToClient( &resizeGridCtrl3Rect );

        p_resizeGridCtrl3Wnd->MoveWindow( leftGridControlOffSet, TOP_GRIDCTRL3_OFFSET,
                                          gridControlWidth + ( cx - 880 ), resizeGridCtrl3Rect.Height() );

    }

    if( p_staticWnd )
    {
        p_staticWnd->GetWindowRect( &resizeStaticRect );
        ScreenToClient( &resizeStaticRect );
        int width = resizeStaticRect.Width();
        p_staticWnd->MoveWindow( LEFT_STATIC_OFFSET, TOP_STATIC_OFFSET,
                                 STATIC_WIDTH + ( cx - 880 ),
                                 TOP_GRIDCTRL3_OFFSET + resizeGridCtrl3Rect.Height() + 15 - TOP_STATIC_OFFSET );

    }

    if( p_resizeStatusWnd )
    {
        p_resizeStatusWnd->GetWindowRect( &resizeStatusRect );
        ScreenToClient( &resizeStatusRect );

        p_resizeStatusWnd->MoveWindow( leftGridControlOffSet, TOP_STATUSEDIT_OFFSET,
                                       STATIC_WIDTH + ( cx - 920 ), STATUSEDIT_HEIGHT + ( cy - 726 ) );
    }

    if( p_resizeStatusStaticWnd )
    {
        p_resizeStatusStaticWnd->GetWindowRect( &resizeStatusStaticRect );
        ScreenToClient( &resizeStatusStaticRect );

        p_resizeStatusStaticWnd->MoveWindow( LEFT_STATIC_OFFSET, TOP_STATIC2_OFFSET,
                                             STATIC2_WIDTH + ( cx - 880 ), STATIC2_HEIGHT + ( cy - 718 ) );
    }


    if( p_resizeHelpWnd )
    {
        p_resizeHelpWnd->GetWindowRect( &resizeHelpRect );
        ScreenToClient( &resizeHelpRect );

        p_resizeHelpWnd->MoveWindow( LEFT_HELP_OFFSET, TOP_BUTTON_OFFSET + ( cy - VERT_OFFSET ),
                                     resizeHelpRect.Width(), resizeHelpRect.Height() );
    }

    if( p_resizeApplyAllWnd )
    {
        p_resizeApplyAllWnd->GetWindowRect( &resizeApplyAllRect );
        ScreenToClient( &resizeApplyAllRect );

        p_resizeApplyAllWnd->MoveWindow( LEFT_APPLY_ALL_OFFSET + ( cx - 880 ), TOP_BUTTON_OFFSET + ( cy - VERT_OFFSET ),
                                         resizeApplyAllRect.Width(), resizeApplyAllRect.Height() );

    }

    if( p_resizeApplyWnd )
    {
        p_resizeApplyWnd->GetWindowRect( &resizeApplyRect );
        ScreenToClient( &resizeApplyRect );

        p_resizeApplyWnd->MoveWindow( LEFT_APPLY_OFFSET + ( cx - 880 ), TOP_BUTTON_OFFSET + ( cy - VERT_OFFSET ),
                                      resizeApplyRect.Width(), resizeApplyRect.Height() );
    }

    if( p_resizeCloseWnd )
    {
        p_resizeCloseWnd->GetWindowRect( &resizeCloseRect );
        ScreenToClient( &resizeCloseRect );

        p_resizeCloseWnd->MoveWindow( LEFT_CLOSE_OFFSET + ( cx - 880 ), TOP_BUTTON_OFFSET + ( cy - VERT_OFFSET ),
                                      resizeCloseRect.Width(), resizeCloseRect.Height() );
    }

    // Redraw all the buttons at once to prevent artifacts
    RedrawWindow( thisRect );
    Invalidate();

}

// 7196-Start
void ECSStationModesOverviewDlg::refreshHealthPollInterval()
{
    m_pECSStationModesOverview->refreshHealthPollInterval();
}
// 7196-End


//TD16730
void ECSStationModesOverviewDlg::OnHelpButton()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp( true );
}

void ECSStationModesOverviewDlg::OnAppHelp()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}

//zn++ - CL-20790
bool ECSStationModesOverviewDlg::isSelectedAutoMode()
{
    StationModesOverviewGridCtrl* gridCtrlArray[3] =
    {
        &m_stationModesOverviewGridCtrl1,
        &m_stationModesOverviewGridCtrl2,
        &m_stationModesOverviewGridCtrl3
    };

    bool foundAutoMode = false;

    for( int i = 0; i < 3; i++ )
    {
        if( !gridCtrlArray[i]->m_selectedrowAutoMode.empty() )  // no auto mode selected in this grid
        {
            foundAutoMode = true;
        }
    }

    return foundAutoMode;
}

void ECSStationModesOverviewDlg::run()
{
    onInitialise();
    addStatusLine( "Finished loading station mode data." );
}

void ECSStationModesOverviewDlg::terminate()
{

}

LRESULT ECSStationModesOverviewDlg::onAddStatusLine( WPARAM wParam, LPARAM lParam )
{
    std::string* pStatusText = reinterpret_cast<std::string*>( wParam );
    if( NULL == pStatusText )
    {
        return 0;
    }
    const std::string& statusText = *pStatusText;
    while( m_statusEditControl.GetLimitText() < ( m_statusEditControl.GetWindowTextLength() + strlen( statusText.c_str() ) + 2 ) )
    {
        int pos = m_statusEditControl.LineIndex( m_statusEditControl.GetLineCount() - 2 );
        m_statusEditControl.SetSel( pos, -1, FALSE );
        m_statusEditControl.ReplaceSel( "" );
    }

    // Make a new line for the new status text
    m_statusEditControl.SetSel( 0, 0 );
    m_statusEditControl.ReplaceSel( "\r\n" );

    // Insert the new status text
    m_statusEditControl.SetSel( 0, 0 );
    m_statusEditControl.ReplaceSel( statusText.c_str() );

    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
         "addStatusLine() - %s", statusText.c_str() );
    delete pStatusText;
    return 0;
}

LRESULT ECSStationModesOverviewDlg::onSetLocations( WPARAM wParam, LPARAM lParam )
{
    LocationNames* pLocationNames = reinterpret_cast<LocationNames*>( wParam );
    if( NULL == pLocationNames )
    {
        return 0;
    }
    const LocationNames& locationNames = *pLocationNames;
    LocationNames::const_iterator locationNamesIt = locationNames.begin();
    LocationNames gridCtrl1Names, gridCtrl2Names, gridCtrl3Names;

    while( locationNamesIt != locationNames.end() )
    {
        if( ( locationNamesIt - locationNames.begin() ) < StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID )
        {
            gridCtrl1Names.push_back( ( *locationNamesIt ) );
        }
        else if( ( locationNamesIt - locationNames.begin() ) < ( 2 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
        {
            gridCtrl2Names.push_back( ( *locationNamesIt ) );
        }
        else if( ( locationNamesIt - locationNames.begin() ) < ( 3 * StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID ) )
        {
            gridCtrl3Names.push_back( ( *locationNamesIt ) );
        }

        locationNamesIt++;
    }

    m_stationModesOverviewGridCtrl1.setLocations( gridCtrl1Names );
    m_stationModesOverviewGridCtrl2.setLocations( gridCtrl2Names );
    m_stationModesOverviewGridCtrl3.setLocations( gridCtrl3Names );

    delete pLocationNames;
    return 0;
}

//++zn


} // namespace TA_IRS_App
