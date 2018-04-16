/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "app/pa/PAManager/src/stdafx.h"

#include "core/exceptions/src/ApplicationException.h"

#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/application_types/src/apptypes.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/EmergencyDVAMsgListPage.h"
#include "app/pa/PAManager/src/NormalDVAMsgListPage.h"
#include "app/pa/PAManager/src/OtherDVAMsgListPage.h"
#include "app/pa/PAManager/src/GwxDisplayWrapper.h"
#include "app/pa/PAManager/src/GroupNameEditDlg.h"
#include "app/pa/PAManager/src/PAScheduleInDayDlg.h"
#include "app/pa/PAManager/src/RunParamListener.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaAgentInterface.h"
#include "app/pa/PAManager/src/SchematicDecideModel.h"
#include "app/pa/PAManager/src/PACorbaMessageSubscriber.h"
#include "app/pa/PAManager/src/BackupKeyMonitor.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PAManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// PAManagerDlg dialog

PAManagerDlg::PAManagerDlg(TA_Base_Bus::IGUIAccess& controlClass) 
: 
TransActiveDialog(controlClass, PAManagerDlg::IDD, NULL),
ICurSelDVAMsgObserver(),
IDVAMsgShortcutConfigObserver(),
m_oBroadcastModel(),
m_pEmergencyListPage( new EmergencyDVAMsgListPage( m_oBroadcastModel )),
m_pNormalListPage( new NormalDVAMsgListPage( m_oBroadcastModel )),
m_pOtherListPage( new OtherDVAMsgListPage( m_oBroadcastModel )),
m_pScheduleBroadcastInDayDlg( NULL ),
m_pPACorbaMessageSubscriber( NULL ),
m_pBackupKeyMonitor( NULL ),
m_hLockMode( NULL ),
m_hUnlockMode( NULL ),
m_hUnknownlockMode( NULL ),
m_nBroadcastMode( DVA_BROADCAST_MODE ),
m_fntList(),
m_indexInplayMsgListCtrl(-1)
{
    FUNCTION_ENTRY( "PAManagerDlg" );

    m_fntList.CreatePointFont( LIST_CTRL_FONT_SIZE * 10, PA_MANAGER_FONT_FACE_NAME );

    m_pScheduleBroadcastInDayDlg = new PAScheduleInDayDlg( this );

    m_oBroadcastModel.registerCurSelDVAMsgObserver( EMERGENCY_DVA_MSG, this );
    m_oBroadcastModel.registerCurSelDVAMsgObserver( NORMAL_DVA_MSG, this );
    m_oBroadcastModel.registerCurSelDVAMsgObserver( OTHER_DVA_MSG, this );

    PaDataManager::getInstance().registerDVAMsgShortcutObserver( this );

    m_oBroadcastModel.registerAddDVAMsgListObserver( this );
	
    PaErrorHelper::registerMsgBoxParentWnd( this );

    FUNCTION_EXIT;
}

PAManagerDlg::~PAManagerDlg()
{
    FUNCTION_ENTRY( "~PAManagerDlg" );

    m_fntList.DeleteObject();

    PaErrorHelper::deregisterMsgBoxParentWnd( this );

    PaDataManager::getInstance().deregisterDVAMsgShortcutObserver( this );

    m_oBroadcastModel.deregisterCurSelDVAMsgObserver( OTHER_DVA_MSG, this );
    m_oBroadcastModel.deregisterCurSelDVAMsgObserver( NORMAL_DVA_MSG, this );
    m_oBroadcastModel.deregisterCurSelDVAMsgObserver( EMERGENCY_DVA_MSG, this );

    if ( NULL != m_pBackupKeyMonitor )
    {
        delete m_pBackupKeyMonitor;
        m_pBackupKeyMonitor = NULL;
    }
    if ( NULL != m_pPACorbaMessageSubscriber )
    {
        delete m_pPACorbaMessageSubscriber;
        m_pPACorbaMessageSubscriber = NULL;
    }
    if ( NULL != m_pScheduleBroadcastInDayDlg )
    {
        delete m_pScheduleBroadcastInDayDlg;
        m_pScheduleBroadcastInDayDlg = NULL;
    }
    if ( NULL != m_pOtherListPage )
    {
        delete m_pOtherListPage;
        m_pOtherListPage = NULL;
    }
    if ( NULL != m_pNormalListPage )
    {
        delete m_pNormalListPage;
        m_pNormalListPage = NULL;
    }
    if ( NULL != m_pEmergencyListPage )
    {
        delete m_pEmergencyListPage;
        m_pEmergencyListPage = NULL;
    }
    m_oBroadcastModel.deregisterAddDVAMsgListObserver( this );

    FUNCTION_EXIT;
}

void PAManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    TransActiveDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(PAManagerDlg)
    DDX_Control(pDX, IDC_PIC_BACKUP_STATUS, m_picBackupStatus);
    DDX_Control(pDX, IDC_CHECK_CYCLE, m_ckCycleBroadcast);
    DDX_Control(pDX, IDC_BTN_SCHEDULE, m_btnSchedule);
    DDX_Control(pDX, IDC_COMBO_GROUP, m_cbGroupNameList);
    DDX_Control(pDX, IDC_EDIT_CUR_BROADCAST_NAME, m_edBroadcastRecordName);
    DDX_Control(pDX, IDC_LIST_SELED_ZONES, m_lbSeledZones);
    DDX_Control(pDX, IDC_EDIT_CYCLE_TIMES, m_edCycleTimes);
    DDX_Control(pDX, IDC_EDIT_INTERVAL_TIMES, m_edDwellTime);
    DDX_Control(pDX, IDC_BTN_BROADCAST_START, m_btnDVABroadcastStart);
    DDX_Control(pDX, IDC_BTN_BROADCAST_STOP, m_btnDVABroadcastStop);
    DDX_Control(pDX, IDC_BTN_GROUP_SAVE, m_btnSaveZoneGroup);
    DDX_Control(pDX, IDC_BTN_GROUP_DELETE, m_btnDelZoneGroup);
    DDX_Control(pDX, IDC_BTN_TASTE_DVA, m_btnDVAMsgPlayback);
    DDX_Control(pDX, IDC_BTN_MONITOR_START, m_btnMonitorStart);//added by lixiaoxia
    DDX_Control(pDX, IDC_BTN_MONITOR_STOP, m_btnMonitorStop);//added by lixiaoxia
    DDX_Control(pDX, IDC_CHECK_DVA_BROADCAST, m_ckDVABroadcastMode);
    DDX_Control(pDX, IDC_CHECK_LIVE_BROADCAST, m_ckLiveBroadcastMode);
	DDX_Control(pDX, IDC_CHECK_BACKGROUND_MUSIC, m_ckBackgroundMusicMode);
    DDX_Control(pDX, IDC_CHECK_SCHEDULE_IN_DAY, m_ckScheduleInDay);
    DDX_Control(pDX, IDC_TIME_START_IN_DAY, m_dtStartTimeInDay);
    DDX_Control(pDX, IDC_TAB_DVA_MSG, m_tabDvaMsg);
    DDX_Control(pDX, IDC_BTN_SCHEDULE_MANAGER, m_ckSchedulemanager);
    DDX_Control(pDX, IDC_LIST_SELECTED_DVA_MESSAGES, m_playMsgListCtrl);
    //}}AFX_DATA_MAP

    FUNCTION_EXIT;
}

BEGIN_MESSAGE_MAP(PAManagerDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(PAManagerDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BTN_BROADCAST_START, OnBnClickedBtnBroadcastStart)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_STOP, OnBnClickedBtnBroadcastStop)
    ON_BN_CLICKED(IDC_BTN_HELP, OnBnClickedBtnHelp)
    ON_BN_CLICKED(IDC_BTN_GROUP_SAVE, OnBnClickedBtnGroupSave)
    ON_BN_CLICKED(IDC_BTN_GROUP_DELETE, OnBnClickedBtnGroupDelete)
    ON_BN_CLICKED(IDC_BTN_SCHEDULE, OnBnClickedBtnSchedule)
    ON_BN_CLICKED(IDC_CHECK_CYCLE, OnBnClickedCheckCycle)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_A, OnBnClickedQuickBroadcastA)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_B, OnBnClickedQuickBroadcastB)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_C, OnBnClickedQuickBroadcastC)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_D, OnBnClickedQuickBroadcastD)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_E, OnBnClickedQuickBroadcastE)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_F, OnBnClickedQuickBroadcastF)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_G, OnBnClickedQuickBroadcastG)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_H, OnBnClickedQuickBroadcastH)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_I, OnBnClickedQuickBroadcastI)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_J, OnBnClickedQuickBroadcastJ)
    ON_BN_CLICKED(IDC_BTN_BROADCAST_K, OnBnClickedQuickBroadcastK)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_L, OnBnClickedQuickBroadcastL)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_M, OnBnClickedQuickBroadcastM)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_N, OnBnClickedQuickBroadcastN)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_O, OnBnClickedQuickBroadcastO)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_P, OnBnClickedQuickBroadcastP)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_Q, OnBnClickedQuickBroadcastQ)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_R, OnBnClickedQuickBroadcastR)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_S, OnBnClickedQuickBroadcastS)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_T, OnBnClickedQuickBroadcastT)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_U, OnBnClickedQuickBroadcastU)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_V, OnBnClickedQuickBroadcastV)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_W, OnBnClickedQuickBroadcastW)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_X, OnBnClickedQuickBroadcastX)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_Y, OnBnClickedQuickBroadcastY)
	ON_BN_CLICKED(IDC_BTN_BROADCAST_Z, OnBnClickedQuickBroadcastZ)
    ON_BN_CLICKED(IDC_CHECK_DVA_BROADCAST, OnBnClickedCheckDvaBroadcast)
    ON_BN_CLICKED(IDC_CHECK_LIVE_BROADCAST, OnBnClickedCheckLiveBroadcast)
	ON_BN_CLICKED(IDC_CHECK_BACKGROUND_MUSIC, OnBnClickedCheckBackgroundMusic)
    ON_BN_CLICKED(IDC_CHECK_SCHEDULE_IN_DAY, OnBnClickedCheckScheduleInDay)
    ON_BN_CLICKED(IDC_BTN_TASTE_DVA, OnBnClickedBtnTasteDva)
    ON_BN_CLICKED(IDC_BTN_MONITOR_START, OnBnClickedBtnMonitorStart)//added by lixiaoxia
    ON_BN_CLICKED(IDC_BTN_MONITOR_STOP, OnBnClickedBtnMonitorStop)//added by lixiaoxia
    ON_BN_CLICKED(IDC_BTN_SCHEDULE_MANAGER, OnClickedCheckScheduleManager)
    ON_BN_CLICKED(IDC_BTN_EXIT, OnBnClickedBtnExit)
    ON_BN_CLICKED(IDC_BTN_MESSAGE_UP, OnBnClickedBtnPlayMsgUp)
    ON_BN_CLICKED(IDC_BTN_MESSAGE_DOWN, OnBnClickedBtnPlayMsgDown)
    ON_BN_CLICKED(IDC_BTN_MESSAGE_DELETE, OnBnClickedBtnPlayMsgDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SELECTED_DVA_MESSAGES, OnLvnItemchangedPlayMsgList)
    ON_CBN_SELCHANGE(IDC_COMBO_GROUP, OnCbnSelchangeComboGroup)
    ON_MESSAGE(WM_UPDATE_DVA_MSG_LIST, OnUpdateDVAMsgList)
    ON_MESSAGE(WM_UPDATE_ZONE_GROUP_LIST, OnUpdateZoneGroupList)
    ON_MESSAGE(WM_POPUP_MESSAGE_BOX, OnPopupMessageBox)
    ON_MESSAGE(WM_PA_STN_HMI_ZONE_SEL_CHANGE, OnUpdateZoneSelection)
    ON_MESSAGE(WM_UPDATE_DVA_MSG_SHORTCUT_BTN_TEXT, OnUpdateDVAMsgShortcutBtnText)
    ON_MESSAGE(WM_UPDATE_OPERATION_SCOPE_GUI, OnUpdateOpScopeGUI)
    ON_MESSAGE(WM_UPDATE_BACKUP_KEY_STATUS, OnUpdateBackupKeyStatus)
    ON_MESSAGE(WM_SCHEDULE_BROADCAST_UPDATE, OnScheduleBroadcastUpdate)
    ON_MESSAGE(WM_SCHEDULE_IN_DAY_DLG_HIDE, OnScheduleInDayDlgHide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL PAManagerDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    TA_Base_Bus::ResizingProperties rp;
    rp.canMaximise = false;
    rp.maxWidth = 1440;
    rp.maxHeight = 900;
    CRect rtClient;
    GetClientRect(rtClient);
    rp.minWidth = rtClient.Width();
    rp.minHeight = rtClient.Height();
    setResizingProperties(rp);
    TransActiveDialog::OnInitDialog();

    if ( NULL == m_hLockMode )
    {
        m_hLockMode = static_cast<HICON>(::LoadImage( ::AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(IDB_LOCKMODE), IMAGE_ICON, BACKUP_KEY_ICON_SIZE, BACKUP_KEY_ICON_SIZE, 0 ));
    }
    if ( NULL == m_hUnlockMode )
    {
        m_hUnlockMode = static_cast<HICON>(::LoadImage( ::AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(IDB_UNLOCKMODE), IMAGE_ICON, BACKUP_KEY_ICON_SIZE, BACKUP_KEY_ICON_SIZE, 0 ));
    }
    if ( NULL == m_hUnknownlockMode )
    {
        m_hUnknownlockMode = static_cast<HICON>(::LoadImage( ::AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(IDB_UNKNOWNLOCKMODE), IMAGE_ICON, BACKUP_KEY_ICON_SIZE, BACKUP_KEY_ICON_SIZE, 0 ));
    }

    m_ckCycleBroadcast.SetCheck( DEFAULT_CYCLE_BROADCAST_CHECK );
    OnBnClickedCheckCycle();
    m_ckScheduleInDay.SetCheck( DEFAULT_SCHEDULE_BROADCAST_IN_DAY_CHECK );
    OnBnClickedCheckScheduleInDay();

    m_picBackupStatus.SetIcon( m_hUnknownlockMode );

    m_lbSeledZones.SetFont( &m_fntList );

    m_cbGroupNameList.PostMessage( WM_CB_SETMINVISIBLE, static_cast<WPARAM>(ZONE_GROUP_SHOW_MAX_ITEMS), 0 ); 

    m_btnDelZoneGroup.EnableWindow( FALSE );
    m_btnDVABroadcastStart.EnableWindow( FALSE );
    m_btnDVAMsgPlayback.EnableWindow( FALSE );
    m_ckDVABroadcastMode.EnableWindow( FALSE );
    m_ckDVABroadcastMode.SetCheck( CHECKBOX_CHECKED );

    m_dtStartTimeInDay.SetFormat( TIME_FORMAT_HH_MM_SS );

    m_tabDvaMsg.addPage( m_pNormalListPage, m_pNormalListPage->getTemplateId(), m_pNormalListPage->getCaption() );
    m_tabDvaMsg.addPage( m_pOtherListPage, m_pOtherListPage->getTemplateId(), m_pOtherListPage->getCaption() );
    m_tabDvaMsg.addPage( m_pEmergencyListPage, m_pEmergencyListPage->getTemplateId(), m_pEmergencyListPage->getCaption() );
    m_tabDvaMsg.showPage( 0 );

    m_oBroadcastModel.registerZoneSelectionNotify( GetSafeHwnd() );

	//
    m_playMsgListCtrl.SetFont( &m_fntList );
	CRect rect;
	m_playMsgListCtrl.GetClientRect(&rect);
	int nColInterval = rect.Width()/5;

	m_playMsgListCtrl.InsertColumn( DVA_MSG_LIST_ID_INDEX, DVA_MSG_LIST_COL_ID, LVCFMT_LEFT, nColInterval*1 );
	m_playMsgListCtrl.InsertColumn( DVA_MSG_LIST_NAME_INDEX, DVA_MSG_LIST_COL_NAME, LVCFMT_LEFT, nColInterval*4 );
	m_playMsgListCtrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);   
	
    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void PAManagerDlg::OnPaint()
{
    FUNCTION_ENTRY( "OnPaint" );

    TransActiveDialog::OnPaint();
    GwxDisplayWrapper::getInstance().refreshSchematic();

    FUNCTION_EXIT;
}

void PAManagerDlg::init()
{
    FUNCTION_ENTRY( "init" );

    if ( NULL != m_pScheduleBroadcastInDayDlg )
    {
        m_pScheduleBroadcastInDayDlg->Create( PAScheduleInDayDlg::IDD, this );
    }
    
    CWnd* marker = GetDlgItem( IDC_ZONE_AREA_MARKER );

    SchematicDecideModel sdmSchematicDecide;
    std::string strSchematicFile = sdmSchematicDecide.getSchematicFileName();
    int nRet= GwxDisplayWrapper::getInstance().loadSchematic( strSchematicFile, marker->GetSafeHwnd(), GetSafeHwnd() );
    PaErrorHelper::checkError( nRet );

    PaDataManager::getInstance().updateDVAMsgMap();
    PostMessage( WM_UPDATE_DVA_MSG_LIST, 0, 0 );
    PaDataManager::getInstance().updateDVAMsgShortcuts();
    PostMessage( WM_UPDATE_DVA_MSG_SHORTCUT_BTN_TEXT, 0, 0 );
    PaDataManager::getInstance().updateZoneGroups();
    PostMessage( WM_UPDATE_ZONE_GROUP_LIST, 0, 0 );

    nRet = PaDataManager::getInstance().updateDataFromAgent();
    if ( !PaErrorHelper::checkError( nRet ))
    {
        PostMessage( WM_UPDATE_SIG_BROADCAST_SETTING, 0, 0 );
    }

    PostMessage( WM_UPDATE_OPERATION_SCOPE_GUI, 0, 0 );

    if ( NULL == m_pPACorbaMessageSubscriber )
    {
        m_pPACorbaMessageSubscriber = new PACorbaMessageSubscriber( this );
    }

    if ( NULL == m_pBackupKeyMonitor )
    {
        m_pBackupKeyMonitor = new BackupKeyMonitor( CachedConfig::getInstance()->getPABackEntityKey() );
        m_pBackupKeyMonitor->registerObserver( GetSafeHwnd() );
    }

    FUNCTION_EXIT;
}

BOOL PAManagerDlg::DestroyWindow()
{
    FUNCTION_ENTRY( "DestroyWindow" );

    try
    {
        GwxDisplayWrapper::getInstance().closeSchematic();
        if ( NULL != m_pBackupKeyMonitor )
        {
            m_pBackupKeyMonitor->deregisterObserver();
        }
    }
    catch ( ... )
    {
    }

    FUNCTION_EXIT;
    return TransActiveDialog::DestroyWindow();
}

LRESULT PAManagerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "WindowProc" );

    LRESULT result = 0;

    try
    {
        result = TransActiveDialog::WindowProc(message, wParam, lParam);
    }
    catch( ... )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred" );
    }

    FUNCTION_EXIT;
    return result;
}

void PAManagerDlg::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    try
    {
        ::DeleteObject( m_hLockMode );
        ::DeleteObject( m_hUnlockMode );
        ::DeleteObject( m_hUnknownlockMode );

        if ( NULL != m_pScheduleBroadcastInDayDlg )
        {
            m_pScheduleBroadcastInDayDlg->DestroyWindow();
        }

        if ( NULL != m_pPACorbaMessageSubscriber )
        {
            delete m_pPACorbaMessageSubscriber;
            m_pPACorbaMessageSubscriber = NULL;
        }
    }
    catch (...)
    {
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::OnSize(UINT nType, int cx, int cy)
{
    FUNCTION_ENTRY( "OnSize" );

    TransActiveDialog::OnSize( nType, cx, cy );

    if ( SIZE_RESTORED == nType )
    {
//         GwxDisplayWrapper::getInstance().refreshSchematic();
    }

    FUNCTION_EXIT;
}

void PAManagerDlg::OnMove(int x, int y)
{
    FUNCTION_ENTRY( "OnMove" );

    TransActiveDialog::OnMove(x, y);

    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Window moved to (%d,%d)", x, y);

    //if (x == -32000 && y == -32000)
    //{
    //    // This is a special case situation that occurs in response
    //    // to a WM_MINIMIZE notification
    //    FUNCTION_EXIT;
    //    return;
    //}

    //CRect screen;
    //::GetClientRect(::GetDesktopWindow(), screen);
    //CRect rc;
    //GetWindowRect(rc);

    //enum { BUFFER_DISTANCE = 8 };

    //int adjustedX = x;
    //int adjustedY = y;
    //if (adjustedY < BUFFER_DISTANCE)
    //{
    //    // Don't allow movement into negative on Y (at all)
    //    adjustedY = BUFFER_DISTANCE;
    //}
    //// Don't allow to move completely below bottom of screen
    //if (adjustedY > screen.Height() - BUFFER_DISTANCE)
    //{
    //    adjustedY = screen.Height() - BUFFER_DISTANCE;
    //}

    //// Allow at least BUFFER_DISTANCE pixel inside of screen either side
    //if (adjustedX + rc.Width() < BUFFER_DISTANCE)
    //{
    //    adjustedX = -rc.Width() + BUFFER_DISTANCE;
    //}
    //if (adjustedX > screen.Width() - BUFFER_DISTANCE)
    //{
    //    adjustedX = screen.Width() - BUFFER_DISTANCE;
    //}

    //static bool overridingOnMove = false;
    //// To avoid recursion, confirm this OnMove request is not in response
    //// To an onMove override
    //if (!overridingOnMove)
    //{
    //    if (x != adjustedX || y != adjustedY)
    //    {
    //        CRect client;
    //        GetClientRect(client);
    //        ClientToScreen(client);
    //        adjustedY -= (client.top - rc.top);     // Subtract height of menu
    //        adjustedX -= (client.left - rc.left);   // Subtract width of left border

    //        overridingOnMove = true;

    //        // Reposition as necessary
    //        ::MoveWindow(m_hWnd, adjustedX, adjustedY, rc.Width(), rc.Height(), TRUE);

    //        overridingOnMove = false;

    //        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Window outside screen, repositioned to (%d,%d)", adjustedX, adjustedY);
    //    }
    //}

    if (( NULL != m_pScheduleBroadcastInDayDlg ) && ( ::IsWindow( m_pScheduleBroadcastInDayDlg->GetSafeHwnd() ) ))
    {
        RECT stClient = { 0, 0, 0, 0 };
        GetWindowRect( &stClient );
        RECT stScheduleDlgRect = { 0, 0, 0, 0 };
        m_pScheduleBroadcastInDayDlg->GetWindowRect( &stScheduleDlgRect );
        m_pScheduleBroadcastInDayDlg->MoveWindow( stClient.right, stClient.top, 
            stScheduleDlgRect.right - stScheduleDlgRect.left, stScheduleDlgRect.bottom- stScheduleDlgRect.top );
    }

//     GwxDisplayWrapper::getInstance().refreshSchematic();

    FUNCTION_EXIT;
}

void PAManagerDlg::updateBackupStatusImage( int nBackupKey )
{
    HICON hToBeSet = NULL;
    switch( nBackupKey )
    {
        case BACKUP_KEY_LOCK_MODE:
        {
            hToBeSet = m_hLockMode;
            break;
        }
        case BACKUP_KEY_UNKNOWN_STATUS:
        {
            // Currently, no unknown is need, set to unlock mode
            //hToBeSet = m_hUnknownlockMode();
            hToBeSet = m_hUnlockMode;
            break;
        }
        case BACKUP_KEY_UNLOCK_MODE:
        {
            hToBeSet = m_hUnlockMode;
            break;
        }
        default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, VariableConvertor::intergerToString( nBackupKey ).c_str() );
            hToBeSet = m_hUnknownlockMode;
            break;
        }
    }

    if ( NULL != hToBeSet )
    {
        m_picBackupStatus.SetIcon( hToBeSet );
    }
}

void PAManagerDlg::OnBnClickedBtnBroadcastStart()
{
    // Check rights first
    if ( !PaRightsManager::getInstance().hasRightToBroadcastDva() )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_BROADCAST_STATION_DVA_MSG );
        return;
    }

    if ( m_oBroadcastModel.isSelectedZoneEmpty() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_NOT_SELECTED );
        return;
    }

	int nDwellTime = 0;
    if ( 0 == m_edDwellTime.GetWindowTextLength() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_DWELL_TIME_IS_NOT_SET );
        m_edDwellTime.SetFocus();
        return;
    }
    char strDwellTime[DWELL_TIME_MAX_CHARS] = { 0 };
    m_edDwellTime.GetWindowText( strDwellTime, DWELL_TIME_MAX_CHARS );
    nDwellTime = VariableConvertor::stringToInterger( std::string(strDwellTime) );
    if ( !m_oBroadcastModel.checkCyclicTimesRange( nDwellTime ))
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_DWELL_TIME_OUT_OF_RANGE );
        m_edDwellTime.SetFocus();
        return;
    }

    int nRepeatTimes = 0;
    // Get cyclic times
    if ( CHECKBOX_CHECKED == m_ckCycleBroadcast.GetCheck() )
    {
        if ( 0 == m_edCycleTimes.GetWindowTextLength() )
        {
            PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_CYCLIC_TIMES_IS_NOT_SET );
            m_edCycleTimes.SetFocus();
            return;
        }
        char strRepeatTimes[REPEAT_TIME_MAX_CHARS] = { 0 };
        m_edCycleTimes.GetWindowText( strRepeatTimes, REPEAT_TIME_MAX_CHARS );
        nRepeatTimes = VariableConvertor::stringToInterger( std::string(strRepeatTimes) );
        if ( !m_oBroadcastModel.checkCyclicTimesRange( nRepeatTimes ))
        {
            PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_CYCLIC_TIMES_OUT_OF_RANGE );
            m_edCycleTimes.SetFocus();
            return;
        }
    }

    // Get start time in day
    time_t tmStartTimeInDay = 0;
    if ( CHECKBOX_CHECKED == m_ckScheduleInDay.GetCheck() )
    {
        CTime ctmStartTime;
        m_dtStartTimeInDay.GetTime( ctmStartTime );
        tmStartTimeInDay = static_cast<time_t>( ctmStartTime.GetTime() );
        time_t tmCurrentTime = time( NULL );
        if ( tmCurrentTime > tmStartTimeInDay )
        {
            PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_SCHEDULE_TIME_IN_PAST );
            return;
        }
    }

    if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_START_DVA_BROADCAST ))
    {
        return;
    }

    // Get zone keys
    std::vector<unsigned long> vecZones;
    m_oBroadcastModel.getCurZones( vecZones );
    
    // Get DVA message key
    //unsigned long ulDVAMsgKey = m_oBroadcastModel.getActiveSelDVAMsgKey();
    std::vector<unsigned long> vecMsgKeys;
	m_oBroadcastModel.getPlayDVAMsgList(vecMsgKeys);
    // Call agent interface to action
    std::string strBroadcastID = "";
    int nRet = PaAgentInterface::startDVABroadcast( strBroadcastID, vecZones, vecMsgKeys, nRepeatTimes, nDwellTime, tmStartTimeInDay );

    if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
    {
        return;
    }

    // Success and popup a message box
    PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_DVA_BROADCAST_START );
}

void PAManagerDlg::OnBnClickedBtnBroadcastStop()
{
    // Check rights first
    if ( !PaRightsManager::getInstance().hasRightToStopDva() )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_STOP_STATION_DVA_MSG );
        return;
    }

    // GF-248 deleted by requirement changing
    //if ( m_oBroadcastModel.isSelectedZoneEmpty() )
    //{
    //    PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_NOT_SELECTED );
    //    return;
    //}

    if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_STOP_DVA_BROADCAST ))
    {
        return;
    }

    // Get zone keys
    std::vector<unsigned long> vecZones;
    m_oBroadcastModel.getCurZones( vecZones );
    
    // Call agent interface to action
    int nRet = PaAgentInterface::stopDVABroadcast( vecZones );

    if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
    {
        return;
    }

    // Success and popup a message box
    PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_DVA_BROADCAST_STOP );
}

void PAManagerDlg::OnBnClickedBtnHelp()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
}

void PAManagerDlg::OnBnClickedBtnGroupSave()
{
    // Check rights first
    if ( !PaRightsManager::getInstance().hasRightToModifyZoneGroup() )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_MODIFY_ZONE_GROUP );
        return;
    }

    if ( m_oBroadcastModel.isSelectedZoneEmpty() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_NOT_SELECTED );
        return;
    }

    // Check zone groups count limit
    if ( PaDataManager::getInstance().isZoneGroupsCountOverLimit() )
    {
        PaDataManager::getInstance().updateZoneGroups();
        PostMessage( WM_UPDATE_ZONE_GROUP_LIST, 0, 0 );
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_GROUP_COUNT_MAX );
        return;
    }

    char strGroupName[GROUP_NAME_BUFFER_SIZE] = { 0 };
    m_cbGroupNameList.GetWindowText( strGroupName, GROUP_NAME_BUFFER_SIZE );
    GroupNameEditDlg dlgGroupNameEdit( m_oBroadcastModel, std::string(strGroupName), this );

    int nOperatorSel = dlgGroupNameEdit.DoModal();
    if ( IDCANCEL == nOperatorSel )
    {
        return;
    }

    // Avoid insert string before "Current selection"
    m_cbGroupNameList.DeleteString( 0 );
    int nInserIndex = m_cbGroupNameList.AddString( dlgGroupNameEdit.getGroupName().c_str() );
    m_cbGroupNameList.SetItemData( nInserIndex, dlgGroupNameEdit.getNewGroupKey() );
    m_cbGroupNameList.InsertString( 0, GROUP_NAME_CUR_SEL );
    ++nInserIndex;
    m_cbGroupNameList.SetCurSel( nInserIndex );
    OnCbnSelchangeComboGroup();
}

void PAManagerDlg::OnBnClickedBtnGroupDelete()
{
    // Check rights first
    if ( !PaRightsManager::getInstance().hasRightToModifyZoneGroup() )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_MODIFY_ZONE_GROUP );
        return;
    }

    int nCurSel = m_cbGroupNameList.GetCurSel();
    if ( -1 == nCurSel )
    {
        return;
    }

    if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_DELETE_ZONE_GROUP ))
    {
        return;
    }

    unsigned long ulZoneGroupKey = static_cast<unsigned long>(m_cbGroupNameList.GetItemData( nCurSel ));
    int nRet = PaDataManager::getInstance().deleteZoneGroupByGroupKey( ulZoneGroupKey );
    if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
    {
        return;
    }

    m_cbGroupNameList.DeleteString( nCurSel );
    m_cbGroupNameList.SetCurSel( 0 );
    OnCbnSelchangeComboGroup();
}

void PAManagerDlg::OnBnClickedBtnSchedule()
{
    bool bLaunchSuccess = true;

    RECT newRect = { 0, 0, 0, 0};
    try
    {
        CPoint pt = GetMessagePos();
        newRect = TA_Base_Bus::AppLauncher::getInstance().getRect(
            TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, pt.x );
    }
    catch ( TA_Base_Core::ApplicationException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
        bLaunchSuccess = false;
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "AppLauncher::getRect" );
        bLaunchSuccess = false;
    }

    std::string commandLine = "--job-info=Plan:PlanPath/";
    try
    {
        // Run the Plan Scheduler
        TA_Base_Bus::AppLauncher::getInstance().launchApplication( SCHEDULING_MANAGER, commandLine,
            TA_Base_Bus::TA_GenericGui::POS_BOUNDED, TA_Base_Bus::TA_GenericGui::ALIGN_FIT, NULL, &newRect );
    }
    catch( TA_Base_Core::ApplicationException& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
        bLaunchSuccess = false;
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "AppLauncher::launchApplication" );
        bLaunchSuccess = false;
    }

    if ( !bLaunchSuccess )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_FAILED_TO_RUN_SCHEDULE_MANAGER );
    }
}

void PAManagerDlg::OnBnClickedCheckCycle()
{
    int nCheck = m_ckCycleBroadcast.GetCheck();
    BOOL bEnable = ( CHECKBOX_UNCHECKED == nCheck ) ? FALSE : TRUE;
    m_edCycleTimes.EnableWindow( bEnable );
}

void PAManagerDlg::OnBnClickedCheckScheduleInDay()
{
    int nCheck = m_ckScheduleInDay.GetCheck();
    BOOL bEnableScheduleInDay = ( CHECKBOX_CHECKED == nCheck ) ? TRUE : FALSE;
    if ( bEnableScheduleInDay )
    {
        m_dtStartTimeInDay.SetTime( &(CTime::GetCurrentTime()) );
    }
    m_dtStartTimeInDay.EnableWindow( bEnableScheduleInDay );
}

void PAManagerDlg::OnClickedCheckScheduleManager()
{
    if (( NULL == m_pScheduleBroadcastInDayDlg ) || !(::IsWindow(m_pScheduleBroadcastInDayDlg->GetSafeHwnd())) )
    {
        return;
    }

    int nCurCheck = m_ckSchedulemanager.GetCheck();
    if ( CHECKBOX_UNCHECKED == nCurCheck )
    {
        m_pScheduleBroadcastInDayDlg->ShowWindow( SW_HIDE );
    }
    else
    {
		RECT stMyRect = { 0, 0, 0, 0 };
		GetWindowRect( &stMyRect );
		RECT stScheduleRect = { 0, 0, 0, 0 };
		m_pScheduleBroadcastInDayDlg->GetWindowRect( &stScheduleRect );

		POINT stMyWnd = { stMyRect.right, stMyRect.top };
		HMONITOR hCurMonitor = ::MonitorFromPoint( stMyWnd, MONITOR_DEFAULTTONEAREST );
		if ( NULL != hCurMonitor )
		{
			MONITORINFO stCurMonInfo = { sizeof(MONITORINFO), { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0u };
			::GetMonitorInfo( hCurMonitor, &stCurMonInfo );

			long lDiffer = stMyRect.right + stScheduleRect.right - stScheduleRect.left - stCurMonInfo.rcMonitor.right;
			if ( 0 < lDiffer )
			{
				stMyRect.left -= lDiffer;
				stMyRect.right -= lDiffer;
				MoveWindow( &stMyRect );
			}
		}

		PostMessage( WM_MOVE, 0, MAKEWORD( stMyRect.left, stMyRect.top ) );
		m_pScheduleBroadcastInDayDlg->ShowWindow( SW_SHOWNOACTIVATE );
    }
}

void PAManagerDlg::processDVAMsgSelChanged( int nType )
{
    updateBroadcastBtnsStatus();

    if ( CachedConfig::getInstance()->isAtOcc() )
    {
        BOOL bEnablePlayback = ( NULL == m_oBroadcastModel.getCurSelDVAMsg( nType ) ) ? FALSE : TRUE;
        m_btnDVAMsgPlayback.EnableWindow( bEnablePlayback );
    }

    m_edBroadcastRecordName.SetWindowText( m_oBroadcastModel.getCurDVAMsgName( nType ).c_str() );
}

void PAManagerDlg::processDVAMsgShortcutChanged( unsigned long ulShortcutIndex )
{
    std::string strBtnText = EMPTY_STRING;
    bool bShortcutBinded = PaDataManager::getInstance().getDVAMsgShortcutNameByIndex( strBtnText, ulShortcutIndex );
    if ( !bShortcutBinded )
    {
        return;
    }

    switch (ulShortcutIndex)
    {
    case 1u:
        GetDlgItem( IDC_BTN_BROADCAST_A )->SetWindowText( strBtnText.c_str() );
    	break;
    case 2u:
        GetDlgItem( IDC_BTN_BROADCAST_B )->SetWindowText( strBtnText.c_str() );
    	break;
    case 3u:
        GetDlgItem( IDC_BTN_BROADCAST_C )->SetWindowText( strBtnText.c_str() );
        break;
    case 4u:
        GetDlgItem( IDC_BTN_BROADCAST_D )->SetWindowText( strBtnText.c_str() );
        break;
    case 5u:
        GetDlgItem( IDC_BTN_BROADCAST_E )->SetWindowText( strBtnText.c_str() );
    	break;
    case 6u:
        GetDlgItem( IDC_BTN_BROADCAST_F )->SetWindowText( strBtnText.c_str() );
    	break;
    case 7u:
        GetDlgItem( IDC_BTN_BROADCAST_G )->SetWindowText( strBtnText.c_str() );
        break;
    case 8u:
        GetDlgItem( IDC_BTN_BROADCAST_H )->SetWindowText( strBtnText.c_str() );
        break;
    case 9u:
        GetDlgItem( IDC_BTN_BROADCAST_I )->SetWindowText( strBtnText.c_str() );
        break;
    case 10u:
        GetDlgItem( IDC_BTN_BROADCAST_J )->SetWindowText( strBtnText.c_str() );
        break;
	case 11u:
		GetDlgItem( IDC_BTN_BROADCAST_K )->SetWindowText( strBtnText.c_str() );
		break;
	case 12u:
		GetDlgItem( IDC_BTN_BROADCAST_L )->SetWindowText( strBtnText.c_str() );
		break;
	case 13u:
		GetDlgItem( IDC_BTN_BROADCAST_M )->SetWindowText( strBtnText.c_str() );
		break;
	case 14u:
		GetDlgItem( IDC_BTN_BROADCAST_N )->SetWindowText( strBtnText.c_str() );
		break;
	case 15u:
		GetDlgItem( IDC_BTN_BROADCAST_O )->SetWindowText( strBtnText.c_str() );
		break;
	case 16u:
		GetDlgItem( IDC_BTN_BROADCAST_P )->SetWindowText( strBtnText.c_str() );
		break;
	case 17u:
		GetDlgItem( IDC_BTN_BROADCAST_Q )->SetWindowText( strBtnText.c_str() );
		break;
	case 18u:
		GetDlgItem( IDC_BTN_BROADCAST_R )->SetWindowText( strBtnText.c_str() );
		break;
	case 19u:
		GetDlgItem( IDC_BTN_BROADCAST_S )->SetWindowText( strBtnText.c_str() );
		break;
	case 20u:
		GetDlgItem( IDC_BTN_BROADCAST_T )->SetWindowText( strBtnText.c_str() );
		break;
	case 21u:
		GetDlgItem( IDC_BTN_BROADCAST_U )->SetWindowText( strBtnText.c_str() );
		break;
	case 22u:
		GetDlgItem( IDC_BTN_BROADCAST_V )->SetWindowText( strBtnText.c_str() );
		break;
	case 23u:
		GetDlgItem( IDC_BTN_BROADCAST_W )->SetWindowText( strBtnText.c_str() );
		break;
	case 24u:
		GetDlgItem( IDC_BTN_BROADCAST_X )->SetWindowText( strBtnText.c_str() );
		break;
	case 25u:
		GetDlgItem( IDC_BTN_BROADCAST_Y )->SetWindowText( strBtnText.c_str() );
		break;
	case 26u:
		GetDlgItem( IDC_BTN_BROADCAST_Z )->SetWindowText( strBtnText.c_str() );
		break;
    default:
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        break;
    }
}

void PAManagerDlg::processDVAMsgListChanged(unsigned long ulDVAMsgKey)
{
	std::string strID, strName;
	m_oBroadcastModel.getPlayDVAMsgInfo(ulDVAMsgKey,strID,strName);

	//int nNewPos = m_playMsgListCtrl.InsertItem( DVA_MSG_LIST_ID_INDEX, strID.c_str() );
	//m_playMsgListCtrl.SetItemText( nNewPos, DVA_MSG_LIST_NAME_INDEX, strName.c_str() );	
	
    LVITEM lv;
    lv.iItem = m_playMsgListCtrl.GetItemCount();
    lv.iSubItem = 0;
    lv.pszText = (LPSTR) strID.c_str();
    lv.mask = LVIF_TEXT;

    int nNewPos = m_playMsgListCtrl.InsertItem(&lv);
	m_playMsgListCtrl.SetItemText( nNewPos, DVA_MSG_LIST_NAME_INDEX, strName.c_str() );	
	
}

void PAManagerDlg::OnBnClickedQuickBroadcastA()
{
    selectQuickBroadcastDVAMsg( 1u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastB()
{
    selectQuickBroadcastDVAMsg( 2u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastC()
{
    selectQuickBroadcastDVAMsg( 3u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastD()
{
    selectQuickBroadcastDVAMsg( 4u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastE()
{
    selectQuickBroadcastDVAMsg( 5u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastF()
{
    selectQuickBroadcastDVAMsg( 6u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastG()
{
    selectQuickBroadcastDVAMsg( 7u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastH()
{
    selectQuickBroadcastDVAMsg( 8u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastI()
{
    selectQuickBroadcastDVAMsg( 9u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastJ()
{
    selectQuickBroadcastDVAMsg( 10u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastK()
{
	selectQuickBroadcastDVAMsg( 11u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastL()
{
	selectQuickBroadcastDVAMsg( 12u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastM()
{
	selectQuickBroadcastDVAMsg( 13u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastN()
{
	selectQuickBroadcastDVAMsg( 14u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastO()
{
	selectQuickBroadcastDVAMsg( 15u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastP()
{
	selectQuickBroadcastDVAMsg( 16u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastQ()
{
	selectQuickBroadcastDVAMsg( 17u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastR()
{
	selectQuickBroadcastDVAMsg( 18u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastS()
{
	selectQuickBroadcastDVAMsg( 19u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastT()
{
	selectQuickBroadcastDVAMsg( 20u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastU()
{
	selectQuickBroadcastDVAMsg( 21u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastV()
{
	selectQuickBroadcastDVAMsg( 22u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastW()
{
	selectQuickBroadcastDVAMsg( 23u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastX()
{
	selectQuickBroadcastDVAMsg( 24u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastY()
{
	selectQuickBroadcastDVAMsg( 25u );
}

void PAManagerDlg::OnBnClickedQuickBroadcastZ()
{
	selectQuickBroadcastDVAMsg( 26u );
}

void PAManagerDlg::OnBnClickedBtnPlayMsgUp()
{
	m_playMsgListCtrl.SetFocus();

	// determine whether it is the first row or no row was selected.
	if (0 == m_indexInplayMsgListCtrl || -1 == m_indexInplayMsgListCtrl)
	{
		return;
	}

	// Backup the field value.
	CString tempField1, tempField2;
	tempField1 = m_playMsgListCtrl.GetItemText(m_indexInplayMsgListCtrl, 0);
	tempField2 = m_playMsgListCtrl.GetItemText(m_indexInplayMsgListCtrl, 1);

	// delete the selected item.
	m_playMsgListCtrl.DeleteItem(m_indexInplayMsgListCtrl);

	// Insert the item before the selected item.
	int tempItem;
	tempItem = m_playMsgListCtrl.InsertItem(m_indexInplayMsgListCtrl-1, _T(tempField1));
	m_playMsgListCtrl.SetItemText(tempItem, 1, _T(tempField2));

	m_oBroadcastModel.upDVAMsgInList(m_indexInplayMsgListCtrl);	
	m_indexInplayMsgListCtrl--;

	UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
	m_playMsgListCtrl.SetItemState(m_indexInplayMsgListCtrl, flag, flag);	
}

void PAManagerDlg::OnBnClickedBtnPlayMsgDown()
{
	m_playMsgListCtrl.SetFocus();

	// determine whether it is the first row or no row was selected.
	if ((m_playMsgListCtrl.GetItemCount()-1) == m_indexInplayMsgListCtrl || -1 == m_indexInplayMsgListCtrl)
	{
		return;
	}

	// Backup the field value.
	CString tempField1, tempField2;
	tempField1 = m_playMsgListCtrl.GetItemText(m_indexInplayMsgListCtrl, 0);
	tempField2 = m_playMsgListCtrl.GetItemText(m_indexInplayMsgListCtrl, 1);

	// delete the selected item.
	m_playMsgListCtrl.DeleteItem(m_indexInplayMsgListCtrl);

	// Insert the item before the selected item.
	int tempItem;
	tempItem = m_playMsgListCtrl.InsertItem(m_indexInplayMsgListCtrl+1, _T(tempField1));
	m_playMsgListCtrl.SetItemText(tempItem, 1, _T(tempField2));
	m_oBroadcastModel.downDVAMsgInList(m_indexInplayMsgListCtrl);	
	m_indexInplayMsgListCtrl++;

	UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
	m_playMsgListCtrl.SetItemState(m_indexInplayMsgListCtrl, flag, flag);		
}

void PAManagerDlg::OnBnClickedBtnPlayMsgDelete()
{
	m_playMsgListCtrl.SetFocus();

	if (m_indexInplayMsgListCtrl == -1)
		return;

	CString tempField1, tempField2;
	tempField1 = m_playMsgListCtrl.GetItemText(m_indexInplayMsgListCtrl, 0);
	tempField2 = m_playMsgListCtrl.GetItemText(m_indexInplayMsgListCtrl, 1);

	m_playMsgListCtrl.DeleteItem(m_indexInplayMsgListCtrl);
	m_oBroadcastModel.deleteDVAMsgInList(m_indexInplayMsgListCtrl);	

	if (m_indexInplayMsgListCtrl != 0)
	{
		m_indexInplayMsgListCtrl--;
	}

	UINT flag = LVIS_SELECTED|LVIS_FOCUSED;
	m_playMsgListCtrl.SetItemState(m_indexInplayMsgListCtrl, flag, flag);	
}

void PAManagerDlg::OnLvnItemchangedPlayMsgList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POSITION pos;
	pos = m_playMsgListCtrl.GetFirstSelectedItemPosition();

	// get the index of the selected item.
	int indexInFieldList = m_playMsgListCtrl.GetNextSelectedItem(pos);

	if (indexInFieldList != -1)
	{
		m_indexInplayMsgListCtrl = indexInFieldList;
	}
	*pResult = 0;	
}

void PAManagerDlg::OnBnClickedBtnTasteDva()
{
    // Check rights
    if ( !PaRightsManager::getInstance().hasRightToPlaybackDvaMessage() )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_BROADCAST_DVA_PLAYBACK );
        return;
    }

    // Popup confirm dialog
    if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_START_DVA_MSG_PLAYBACK ))
    {
        return;
    }

    // Get DVA message key
    unsigned long ulDVAMsgKey = m_oBroadcastModel.getActiveSelDVAMsgKey();
    
    // Call agent interface
    std::string strBroadcastId = "";
    int nRet = PaAgentInterface::startDVAMsgPlayback( strBroadcastId, ulDVAMsgKey );

    if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
    {
        return;
    }

    // Success and popup a message box
    PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_DVA_MSG_PLAYBACK );
}

void PAManagerDlg::OnBnClickedCheckDvaBroadcast()
{
    m_ckDVABroadcastMode.SetCheck( CHECKBOX_UNCHECKED );
    if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_SWITCH_TO_DVA_BROADCAST_MODE ))
    {
        return;
    }

    m_oBroadcastModel.lockZoneList( false );
    m_ckDVABroadcastMode.EnableWindow( FALSE );
    m_ckLiveBroadcastMode.SetCheck( CHECKBOX_UNCHECKED );
	m_ckLiveBroadcastMode.EnableWindow(TRUE);

    m_cbGroupNameList.EnableWindow( TRUE );
    if ( 0 != m_cbGroupNameList.GetCurSel() )
    {
        m_btnDelZoneGroup.EnableWindow( TRUE );
    }
    else
    {
        m_btnSaveZoneGroup.EnableWindow( TRUE );
    }
    m_ckDVABroadcastMode.SetCheck( CHECKBOX_CHECKED );
    m_nBroadcastMode = DVA_BROADCAST_MODE;
    updateBroadcastBtnsStatus();
    updateLiveBroadcastCheckBtnStatus();

    if ( 0 == m_cbGroupNameList.GetCurSel() )
    {
        OnUpdateZoneSelection( 0, 0 );
    }
}

void PAManagerDlg::OnBnClickedCheckLiveBroadcast()
{
	//Get button new status
	int nBtnNewState = m_ckLiveBroadcastMode.GetCheck();

	//Revert to the original state of the button
	m_ckLiveBroadcastMode.SetCheck( (CHECKBOX_UNCHECKED == nBtnNewState) ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED );

    // Check rights
    if ( !PaRightsManager::getInstance().hasRightToBroadcastLive() )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_BROADCAST_STATION_LIVE );
        return;
    }

    if ( m_oBroadcastModel.isSelectedZoneEmpty() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_NOT_SELECTED );
        return;
    }

	if( CHECKBOX_CHECKED == nBtnNewState )
	{
		startLiveBroadcast();
	}
	else
	{
		stopLiveBroadcast();
	}
}

void PAManagerDlg::OnBnClickedCheckBackgroundMusic()
{
	//Get button new status
	int nBtnNewState = m_ckBackgroundMusicMode.GetCheck();

	//Revert to the original state of the button
	m_ckBackgroundMusicMode.SetCheck( (CHECKBOX_UNCHECKED == nBtnNewState) ? CHECKBOX_CHECKED : CHECKBOX_UNCHECKED );

	// Check rights
	if ( !PaRightsManager::getInstance().hasRightToBackgroundMusic() )
	{
	    PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_BROADCAST_STATION_BACKGROUND_MUSIC );
	    return;
	}

	if ( m_oBroadcastModel.isSelectedZoneEmpty() )
	{
		PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_NOT_SELECTED );
		return;
	}

	if( CHECKBOX_CHECKED == nBtnNewState )
	{
        startBackgroundMusicBroadcast();
	}
	else
	{
		stopBackgroundMusicBroadcast();
	}
}

void PAManagerDlg::selectQuickBroadcastDVAMsg( unsigned int unIndex )
{
    unsigned long ulDVAMsgKey = PaDataManager::getInstance().getDVAMsgKeyByShortcutIndex( unIndex );
    if ( INVALID_DVA_MSG_KEY == ulDVAMsgKey )
    {
        // The DVA message shortcut is not configured yet
        return;
    }

    int nType = PaDataManager::getInstance().getDVAMsgTypeByKey( ulDVAMsgKey );
    CPropertyPage* pActiveTab = NULL;
    switch( nType )
    {
        case EMERGENCY_DVA_MSG:
        {
            pActiveTab = m_pEmergencyListPage;
            break;
        }
        case NORMAL_DVA_MSG:
        {
            pActiveTab = m_pNormalListPage;
            break;
        }
        case OTHER_DVA_MSG:
        {
            pActiveTab = m_pOtherListPage;
            break;
        }
        default:
            break;
    }

    if ( NULL == pActiveTab )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }
    m_tabDvaMsg.showPage( pActiveTab );
    // Should not block here, do not use SendMessage
    pActiveTab->PostMessage( WM_SEL_QUICK_BROADCAST_DVA_MSG, static_cast<WPARAM>(ulDVAMsgKey), 0 );
	m_oBroadcastModel.addToPlayDVAMsgList(nType,ulDVAMsgKey);
	
}

LRESULT PAManagerDlg::OnUpdateDVAMsgList( WPARAM wParam, LPARAM lParam )
{
    if ( NULL != m_pEmergencyListPage )
    {
        m_pEmergencyListPage->PostMessage( WM_UPDATE_DVA_MSG_LIST, wParam, lParam );
    }
    if ( NULL != m_pNormalListPage )
    {
        m_pNormalListPage->PostMessage( WM_UPDATE_DVA_MSG_LIST, wParam, lParam );
    }
    if ( NULL != m_pOtherListPage )
    {
        m_pOtherListPage->PostMessage( WM_UPDATE_DVA_MSG_LIST, wParam, lParam );
    }

    return 0;
}

LRESULT PAManagerDlg::OnUpdateZoneGroupList( WPARAM wParam, LPARAM lParam )
{
    std::map<unsigned long, std::string>& mapZoneGroups = PaDataManager::getInstance().getAllZoneGroups();

    m_cbGroupNameList.ResetContent();

    int nInsertPos = 0;
    for ( std::map<unsigned long, std::string>::iterator itLoop = mapZoneGroups.begin();
          mapZoneGroups.end() != itLoop; ++itLoop )
    {
        nInsertPos = m_cbGroupNameList.AddString( itLoop->second.c_str() );
        m_cbGroupNameList.SetItemData( nInsertPos, itLoop->first );
    }

    m_cbGroupNameList.InsertString( 0, GROUP_NAME_CUR_SEL );
    m_cbGroupNameList.SetCurSel( 0 );

    return 0;
}

LRESULT PAManagerDlg::OnScheduleInDayDlgHide( WPARAM wParam, LPARAM lParam )
{
    m_ckSchedulemanager.SetCheck( CHECKBOX_UNCHECKED );

    return 0;
}

LRESULT PAManagerDlg::OnUpdateZoneSelection( WPARAM wParam, LPARAM lParam )
{
    // User can't modify zone selection in live broadcast mode
    if ( LIVE_BROADCAST_MODE == m_nBroadcastMode )
    {
        if ( static_cast<int>(wParam) == ZONE_SYNCHRONISE_SELECTED )
        {
            PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_SELECTING_IN_LIVE_BROADCAST_MODE );
        }

        // In live mode, no zone update
        return 0;
    }

    // If current group selection is not "Active selection", switch to Active selection
    if ( 0 != m_cbGroupNameList.GetCurSel() )
    {
        m_cbGroupNameList.SetCurSel( 0 );
        OnCbnSelchangeComboGroup();
    }

    std::vector<std::string> vecSelectedZones;
    m_oBroadcastModel.getActiveSelZoneNames( vecSelectedZones );
    m_lbSeledZones.ResetContent();

    for ( std::vector<std::string>::iterator itLoop = vecSelectedZones.begin(); vecSelectedZones.end() != itLoop; ++itLoop )
    {
        m_lbSeledZones.AddString( itLoop->c_str() );
    }

    updateBroadcastBtnsStatus();
    updateZoneMonitoringBtnsStatus();//added by lixiaoxia

    return 0;
}

LRESULT PAManagerDlg::OnPopupMessageBox( WPARAM wParam, LPARAM lParam )
{
    const char* const strMsg = reinterpret_cast<const char* const>(wParam);
    const char* const strCaption = reinterpret_cast<const char* const>(lParam);
    if (( NULL == strMsg ) || ( NULL == strCaption ))
    {
        return 0;
    }

    UINT unBoxType = MB_OK;
    if ( strCaption == WARNING_TITLE )
    {
        unBoxType |= MB_ICONWARNING;
    }
    else if ( strCaption == ERROR_TITLE )
    {
        unBoxType |= MB_ICONERROR;
    }

    MessageBox( strMsg, strCaption, unBoxType );

    return 0;
}

void PAManagerDlg::OnCbnSelchangeComboGroup()
{
    int nCurSel = m_cbGroupNameList.GetCurSel();

    std::vector<std::string> vecZoneNames;
    if ( 0 != nCurSel )
    {
        unsigned long ulGroupKey = m_cbGroupNameList.GetItemData( nCurSel );
        vecZoneNames = PaDataManager::getInstance().getZoneNamesByGroupKey( ulGroupKey );
        m_oBroadcastModel.updateCurSelZoneGroupKey( ulGroupKey );
    }
    else
    {
        m_oBroadcastModel.getActiveSelZoneNames( vecZoneNames );
        m_oBroadcastModel.updateCurSelZoneGroupKey( ACTIVE_CUR_SEL_ZONE_GROUP_KEY );
    }

    m_lbSeledZones.ResetContent();

    for ( std::vector<std::string>::iterator itLoop = vecZoneNames.begin();
          vecZoneNames.end() != itLoop; ++itLoop )
    {
        m_lbSeledZones.AddString( itLoop->c_str() );
    }
    vecZoneNames.clear();

    // If current selection is "active selection"
    BOOL bEnableBtnSaveGroup = ( 0 == nCurSel ) ? TRUE : FALSE;
    m_btnSaveZoneGroup.EnableWindow( bEnableBtnSaveGroup );
    BOOL bEnableBtnDelGroup = ( 0 == nCurSel ) ? FALSE : TRUE;
    m_btnDelZoneGroup.EnableWindow( bEnableBtnDelGroup );
    updateBroadcastBtnsStatus();
    updateZoneMonitoringBtnsStatus();//added by lixiaoxia
}

void PAManagerDlg::updateOpScopeGUI()
{
    bool bIsAtOcc = CachedConfig::getInstance()->isAtOcc();

    if ( bIsAtOcc )
    {
        SetWindowText( OCC_PA_DLG_TITTLE );
		m_ckBackgroundMusicMode.EnableWindow(FALSE);
    }
    else
    {
        SetWindowText( STATION_PA_DLG_TITTLE );
		m_ckBackgroundMusicMode.EnableWindow(TRUE);
    }

	m_btnMonitorStart.EnableWindow(FALSE);
    m_btnMonitorStop.EnableWindow( TRUE );
}

LRESULT PAManagerDlg::OnUpdateDVAMsgShortcutBtnText( WPARAM wParam, LPARAM lParam )
{
    std::map<unsigned long, unsigned long>& mapShortcuts = PaDataManager::getInstance().getExistShortcutNos();

    for ( std::map<unsigned long, unsigned long>::iterator itLoop = mapShortcuts.begin();
          mapShortcuts.end() != itLoop; ++itLoop )
    {
        processDVAMsgShortcutChanged( itLoop->first );
    }

    return 0;
}

LRESULT PAManagerDlg::OnUpdateOpScopeGUI( WPARAM wParam, LPARAM lParam )
{
    updateOpScopeGUI();
    return 0;
}

LRESULT PAManagerDlg::OnUpdateBackupKeyStatus( WPARAM wParam, LPARAM lParam )
{
    int nBackupKey = static_cast<int>(wParam);

    updateBackupStatusImage( nBackupKey );

    updateBroadcastBtnsStatus();
    updateLiveBroadcastCheckBtnStatus();
    updateZoneMonitoringBtnsStatus();//added by lixiaoxia

    return 0;
}

LRESULT PAManagerDlg::OnScheduleBroadcastUpdate( WPARAM wParam, LPARAM lParam )
{
    if (( NULL != m_pScheduleBroadcastInDayDlg ) && ::IsWindow(m_pScheduleBroadcastInDayDlg->GetSafeHwnd()) )
    {
        m_pScheduleBroadcastInDayDlg->PostMessage( WM_SCHEDULE_BROADCAST_UPDATE, wParam, lParam );
    }

    return 0;
}

void PAManagerDlg::updateBroadcastBtnsStatus()
{
    bool bEnableDvaStart = true;
    // Enable Button of Start DVA when DVA broadcast mode (not live broadcast mode, to be modified)
    bEnableDvaStart &= ( DVA_BROADCAST_MODE == m_nBroadcastMode );
    // and when Active selected DVA message is not null
    bEnableDvaStart &= ( NULL != m_oBroadcastModel.getActiveSelDVAMsg() );
    // and when at least one zone is selected
    bEnableDvaStart &= ( !m_oBroadcastModel.isSelectedZoneEmpty() );
    // and when backup key status is not backup mode
    if ( NULL != m_pBackupKeyMonitor )
    {
        bEnableDvaStart &= ( !m_pBackupKeyMonitor->isBackupLockMode() );
    }
    else
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
    }

    BOOL bEnableDvaStartBtn = bEnableDvaStart ? TRUE : FALSE;
    m_btnDVABroadcastStart.EnableWindow( bEnableDvaStartBtn );

    bool bEnableDvaStop = true;
    // Enable Button of Start DVA when DVA broadcast mode (not live broadcast mode, to be modified)
    bEnableDvaStop &= ( DVA_BROADCAST_MODE == m_nBroadcastMode );
    // and when at least one zone is selected
    // GF-248 deleted by requirement changing
    //bEnableDvaStop &= ( !m_oBroadcastModel.isSelectedZoneEmpty() );
    // and when backup key status is not backup mode
    if ( NULL != m_pBackupKeyMonitor )
    {
        bEnableDvaStop &= ( !m_pBackupKeyMonitor->isBackupLockMode() );
    }
    else
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
    }

    BOOL bEnableDvaStopBtn = bEnableDvaStop ? TRUE : FALSE;
    m_btnDVABroadcastStop.EnableWindow( bEnableDvaStopBtn );
}

//added by lixiaoxia
void PAManagerDlg::updateZoneMonitoringBtnsStatus()
{
    bool bEnableMonitoringStart = true;
    // when at least one zone is selected
    bEnableMonitoringStart &= ( !m_oBroadcastModel.isSelectedZoneEmpty() );
    // and when backup key status is not backup mode
    if ( NULL != m_pBackupKeyMonitor )
    {
        bEnableMonitoringStart &= ( !m_pBackupKeyMonitor->isBackupLockMode() );
    }
    else
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
    }

    BOOL bEnableMonitoringStartBtn = bEnableMonitoringStart ? TRUE : FALSE;
    m_btnMonitorStart.EnableWindow( bEnableMonitoringStartBtn );
}

void PAManagerDlg::OnBnClickedBtnExit()
{
    OnClose();
}

void PAManagerDlg::updateLiveBroadcastCheckBtnStatus()
{
	bool bEnableLive = true;
    if ( NULL != m_pBackupKeyMonitor )
    {
        bEnableLive &= ( !m_pBackupKeyMonitor->isBackupLockMode() );
    }
    else
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
    }

    BOOL bEnableLiveBtn = bEnableLive ? TRUE : FALSE;
    m_ckLiveBroadcastMode.EnableWindow( bEnableLiveBtn );
}

void PAManagerDlg::updateBackgroundMusicCheckBtnStatus()
{
	bool bEnableBackgroundMusic = true;
	if ( NULL != m_pBackupKeyMonitor )
	{
		bEnableBackgroundMusic &= ( !m_pBackupKeyMonitor->isBackupLockMode() );
	}
	else
	{
		PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
	}

	BOOL bEnableBackgroundMusicBtn = bEnableBackgroundMusic ? TRUE : FALSE;
	m_ckBackgroundMusicMode.EnableWindow( bEnableBackgroundMusicBtn );
}

//begin-added by lixiaoxia 
void PAManagerDlg::OnBnClickedBtnMonitorStart()
{
	// Check rights first
	if ( !PaRightsManager::getInstance().hasRightToStartMonitor() )
	{
		PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_START_MONITOR_ZONE);
		return;
	}

	if ( m_oBroadcastModel.isSelectedZoneEmpty() )
	{
		PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_NOT_SELECTED );
		return;
	}

	// Get zone keys
	std::vector<unsigned long> vecZones;
	m_oBroadcastModel.getCurZones( vecZones );
	if (vecZones.size() > 1)
	{
		PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_SELECTED_MANY );
		return;
	}

	if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_START_MONITOR_BROADCAST ))
	{
		return;
	}

	// Call agent interface to action
	std::string strBroadcastID = "";
	int nRet = PaAgentInterface::startBroadcastMonitor( vecZones);

	if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
	{
		return;
	}

	// Success and popup a message box
	PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_MONITOR_BROADCAST_START );
}

void PAManagerDlg::OnBnClickedBtnMonitorStop()
{
	// Check rights first
	if ( !PaRightsManager::getInstance().hasRightToStopMonitor() )
	{
		PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_STOP_MONITOR_ZONE);
		return;
	}

	if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_STOP_MONITOR_BROADCAST ))
	{
		return;
	}

	// Get zone keys
	std::vector<unsigned long> vecZones;
	m_oBroadcastModel.getCurZones( vecZones );
	// Call agent interface to action
	int nRet = PaAgentInterface::stopBroadcastMonitor( vecZones );

	if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
	{
		return;
	}

	// Success and popup a message box
	PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_MONITOR_BROADCAST_STOP );
}
//end-added by lixiaoxia

void PAManagerDlg::startLiveBroadcast()
{
	if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_SWITCH_TO_LIVE_BROADCAST_MODE ))
	{
		return;
	}

	// Get zone keys
	std::vector<unsigned long> vecZones;
	m_oBroadcastModel.getCurZones( vecZones );

	// Start live broadcast
	std::string strBroadcastId = "";
	int nRet = PaAgentInterface::startLiveBroadcast( strBroadcastId, vecZones );

	// Check live broadcast result
	if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
	{
		return;
	}

	// Update GUI
	m_oBroadcastModel.lockZoneList();
	m_ckDVABroadcastMode.SetCheck( CHECKBOX_UNCHECKED );
	m_ckDVABroadcastMode.EnableWindow( FALSE );
	m_cbGroupNameList.EnableWindow( FALSE );
	if ( 0 != m_cbGroupNameList.GetCurSel() )
	{
		m_btnDelZoneGroup.EnableWindow( FALSE );
	}
	else
	{
		m_btnSaveZoneGroup.EnableWindow( FALSE );
	}
	m_ckLiveBroadcastMode.SetCheck( CHECKBOX_CHECKED );
	m_nBroadcastMode = LIVE_BROADCAST_MODE;
	updateBroadcastBtnsStatus();
	updateLiveBroadcastCheckBtnStatus();

	// Success and popup a message box
	PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_LIVE_BROADCAST_START );
}

void PAManagerDlg::stopLiveBroadcast()
{
	// Get zone keys
	std::vector<unsigned long> vecZones;
	m_oBroadcastModel.getCurZones( vecZones );

	// Start live broadcast
	std::string strBroadcastId = "";
	int nRet = PaAgentInterface::stopLiveBroadcast( vecZones );

	// Check live broadcast result
	if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
	{
		return;
	}

	// Success and popup a message box
	PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_LIVE_BROADCAST_STOP );

	// Update GUI
	m_oBroadcastModel.lockZoneList(false);
	m_ckDVABroadcastMode.SetCheck( CHECKBOX_CHECKED );
	m_ckDVABroadcastMode.EnableWindow( TRUE );
	m_cbGroupNameList.EnableWindow( TRUE );
	if ( 0 != m_cbGroupNameList.GetCurSel() )
	{
		m_btnDelZoneGroup.EnableWindow( TRUE );
	}
	else
	{
		m_btnSaveZoneGroup.EnableWindow( TRUE );
	}
	m_ckLiveBroadcastMode.SetCheck( CHECKBOX_UNCHECKED );
	m_nBroadcastMode = DVA_BROADCAST_MODE;
	updateBroadcastBtnsStatus();
	updateLiveBroadcastCheckBtnStatus();
}

void PAManagerDlg::startBackgroundMusicBroadcast()
{
	if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_SWITCH_TO_BACKGROUND_MUSIC_BROADCAST_MODE ))
	{
		return;
	}

	// Get zone keys
	std::vector<unsigned long> vecZones;
	m_oBroadcastModel.getCurZones( vecZones );

	int nRet = PaAgentInterface::startBackgroundMusicBroadcast( vecZones );

	// Check background music broadcast result
	if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
	{
		return;
	}

	// Update GUI
	m_oBroadcastModel.lockZoneList();
	m_ckBackgroundMusicMode.SetCheck( CHECKBOX_CHECKED );
	m_nBroadcastMode = BACKGROUND_MUSIC_BROADCAST_MODE;
	updateBackgroundMusicCheckBtnStatus();

	// Success and popup a message box
	PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_BACKGROUND_MUSIC_START );
}

void PAManagerDlg::stopBackgroundMusicBroadcast()
{
	// Get zone keys
	std::vector<unsigned long> vecZones;
	m_oBroadcastModel.getCurZones( vecZones );

	int nRet = PaAgentInterface::stopBackgroundMusicBroadcast( vecZones );

	// Check background music broadcast result
	if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
	{
		return;
	}

	// Success and popup a message box
	PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_BACKGROUND_MUSIC_STOP );

	// Update GUI
	m_oBroadcastModel.lockZoneList(false);
	m_ckBackgroundMusicMode.SetCheck( CHECKBOX_UNCHECKED );
	m_nBroadcastMode = DVA_BROADCAST_MODE;
	updateBackgroundMusicCheckBtnStatus();
}

