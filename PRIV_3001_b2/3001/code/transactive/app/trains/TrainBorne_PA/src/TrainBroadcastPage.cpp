/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/TrainBroadcastPage.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * TrainBroadcastPage handles GUI side of train broadcast tab
  */

#include "app/trains/TrainBorne_PA/src/StdAfx.h"

#include "app/trains/TrainBorne_PA/src/CachedMapChangedWindowNotificationHandler.h"
#include "app/trains/TrainBorne_PA/src/LockControlRedraw.h"
#include "app/trains/TrainBorne_PA/src/PaErrorHandler.h"
#include "app/trains/TrainBorne_PA/src/CachedConfig.h"
#include "app/trains/TrainBorne_PA/src/TrainBroadcastModel.h"
#include "app/trains/TrainBorne_PA/src/TrainBroadcastPage.h"
#include "app/trains/TrainBorne_PA/src/TrainDvaStatusDlg.h"
#include "app/trains/TrainBorne_PA/src/TrainLiveStatusDlg.h"
#include "app/trains/TrainBorne_PA/src/CachedMapLoader.h"
#include "app/trains/TrainBorne_PA/src/PaRightsManager.h"

#include "bus/pa_4669/pa_agent/idl/src/IPAAgentCorbaDef.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"




using TA_Base_Bus::IPAAgentCorbaDef;
#pragma warning ( disable : 4267 4312 4311 )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_IRS_Bus;
using namespace TA_Base_Core;
using TA_Base_Bus::SettingsMgr;
using namespace TA_Base_Bus;

static const COleDateTime       NULL_TIME(2000,1,1,0,0,0);
static const COleDateTimeSpan   NULL_TIMESPAN(NULL_TIME);
static const int				MAX_TIME_INTERVAL = 65536;
static const DWORD COLOR_RED = RGB(255, 0, 0);

IMPLEMENT_DYNCREATE(TrainBroadcastPage, CPropertyPage)

const std::string TrainBroadcastPage::s_inServiceTabText = "Mainline";
const std::string TrainBroadcastPage::s_depotTabText = "Depot";

const std::string TrainBroadcastPage::s_enterNonZeroLengthNameMessage = 
            "Please enter a valid name.";
const std::string TrainBroadcastPage::s_dynamicGroupNameAlreadyExistsMessage = 
            "The name entered is already in use, overwrite existing group?";
const std::string TrainBroadcastPage::s_defaultDynamicGroupName = 
            "Unnamed Group";

const std::string TrainBroadcastPage::s_enterDynamicGroupNamePrompt =
            "Please enter a name for the new dynamic group";
const std::string TrainBroadcastPage::s_enterDynamicGroupNameWindowTitle = 
            "PA Manager - Add Dynamic Train Group";

TrainBroadcastPage::TrainBroadcastPage( CWnd* pParent ) 
: ITrainBorne(TrainBroadcastPage::IDD, pParent ),
  m_anyTrainsSelected(false),
  m_pageActive(false),
  m_hLiveIcon(0),
  m_hDvaIcon(0),
  m_trainDvaUpdateNotificationHandler(0),
  m_trainSelectorGUI(NULL),
  m_dvaMessageList(TDM_COL_MAX),
  m_isAtLeastTwoTrainsSelected(FALSE)
{
    FUNCTION_ENTRY( "TrainBroadcastPage" );

    //{{AFX_DATA_INIT(TrainBroadcastPage)
    //}}AFX_DATA_INIT
 //   TA_IRS_Bus::LocationCache::EStationType startMode;

	

 //   if (CachedConfig::getInstance()->getLocationType() ==
 //                                       CachedConfig::LOCATION_TYPE_DEPOT)
 //   {
 //       // If at depot, only the depot tab is available
 //       // (so we must display depot mode)
 //       startMode = TA_IRS_Bus::LocationCache::Depot;
 //   }
 //   else
 //   {
 //       // At either OCC or Station, OK to use In Service mode
 //       startMode = getAppropriateInServiceMode();
 //   }

	m_trainBroadcastModel = new TrainBroadcastModel(*this);

    FUNCTION_EXIT;
}


TrainBroadcastPage::~TrainBroadcastPage()
{
    FUNCTION_ENTRY("~TrainBroadcastPage");

    //TA_ASSERT(0 == m_trainDvaUpdateNotificationHandler, "Shouldn't be valid still");

    try
    {
        if (m_hLiveIcon)
        {
            DestroyIcon((HICON)m_hLiveIcon);
            m_hLiveIcon = 0;
        }

        if (m_hDvaIcon)
        {
            DestroyIcon((HICON)m_hDvaIcon);
            m_hDvaIcon = 0;
        }
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
    }

    FUNCTION_EXIT;
}


void TrainBroadcastPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    ITrainBorne::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(TrainBroadcastPage)
    //DDX_Control(pDX, IDC_BUTTONALLOUTBOUND, m_allOutboundButton);
    //DDX_Control(pDX, IDC_BUTTONALLINBOUND, m_allInboundButton);
    //DDX_Control(pDX, IDC_BUTTONCLEARALL, m_clearTrainSelectionButton);
    //DDX_Control(pDX, IDC_BUTTONALLTRAINS, m_selectAllTrainsButton);
    DDX_Control(pDX, IDC_DYNAMIC_GROUPS, m_dynamicGroupCombo);
    DDX_Control(pDX, IDC_STATIC_GROUPS, m_staticGroupCombo);
    DDX_Control(pDX, IDC_BUTTON_SAVE_GROUP_SEL, m_saveDynamicGroupButton);
    DDX_Control(pDX, IDC_BUTTON_DELETE_GROUP, m_deleteDynamicGroupButton);
    DDX_Control(pDX, IDC_TRN_CYCLICAL_STOP, m_period);
    DDX_Control(pDX, IDC_TRN_CYCLICAL_START, m_duration);
    DDX_Control(pDX, IDC_TRN_DVA_MESSAGES_LST, m_dvaMessageList);
    //DDX_Control(pDX, IDC_TRAINLIST_TAB, m_locationTab);
    DDX_Control(pDX, IDC_TRN_MESSAGE_FILTER_TB, m_messageFilterTb);
    DDX_Control(pDX, IDC_TRN_LIVE_BTN, m_liveBtn);
    DDX_Control(pDX, IDC_TRN_DVA_BTN, m_dvaBtn);
    DDX_Control(pDX, IDC_TRN_CYCLIC_CB, m_cyclicCb);
    //DDX_Control(pDX, IDC_INBOUND_LABEL, m_inboundLabel);
    //DDX_Control(pDX, IDC_OUTBOUND_LABEL, m_outboundLabel);
	DDX_Control(pDX, IDC_COMBO_DVA_PRIORITY, m_DVAPriorityCombo);
	DDX_Control(pDX, IDC_STATUS_CB, m_statusNameList);
	DDX_Control(pDX, IDC_RESTORE_STATUS_BTN, m_restoreStatusButton);
	DDX_Control(pDX, IDC_KILL_STATUS_BTN, m_killStatusButton);
	DDX_Control(pDX, IDC_KILL_ALL_STATUS_BTN, m_killAllStatusButton);
    //}}AFX_DATA_MAP

    //DDX_Control(pDX, IDC_STN_OUTBOUND_SELECTION_LST, getOutboundTrainList());
    //DDX_Control(pDX, IDC_STN_INBOUND_SELECTION_LST, getInboundTrainList());

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(TrainBroadcastPage, ITrainBorne)
    //{{AFX_MSG_MAP(TrainBroadcastPage)
    ON_BN_CLICKED(IDC_TRN_CYCLIC_CB, OnTrnCyclicCb)
    ON_BN_CLICKED(IDC_TRN_DVA_BTN, OnTrnDvaBtn)
    ON_BN_CLICKED(IDC_TRN_LIVE_BTN, OnTrnLiveBtn)
    ON_MESSAGE(WM_PA_TRAIN_INBOUND_LIST_UPDATE, OnInboundTrainListUpdate)
    ON_MESSAGE(WM_PA_TRAIN_OUTBOUND_LIST_UPDATE, OnOutboundTrainListUpdate)
    ON_MESSAGE(WM_PA_TRAIN_DYN_LIST_UPDATE, OnDynamicGroupListUpdate)
    ON_MESSAGE(WM_PA_TRAIN_DVA_MESSAGE_UPDATE, OnTrainDvaMessageUpdate)
    ON_MESSAGE(WM_PA_ERROR_MESSAGE, OnErrorMsg)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_DELETE_GROUP, OnButtonDeleteGroup)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_GROUP_SEL, OnButtonSaveGroupSel)
	ON_CBN_SELCHANGE(IDC_STATUS_CB, OnSelchangeStatusCb)

	ON_MESSAGE(WM_PA_STN_STATUS_NAME_UPDATE, OnStatusNameChange)
    //ON_BN_CLICKED(IDC_BUTTONALLTRAINS, OnButtonalltrains)
    //ON_BN_CLICKED(IDC_BUTTONCLEARALL, OnButtonclearall)
    //ON_NOTIFY(TCN_SELCHANGING, IDC_TRAINLIST_TAB, OnSelchangingTrainlistTab)
    ON_EN_CHANGE(IDC_TRN_MESSAGE_FILTER_TB, OnChangeTrnMessageFilterTb)
    //ON_NOTIFY(NM_CLICK, IDC_STN_INBOUND_SELECTION_LST, OnClickStnInboundSelectionLst)
    //ON_NOTIFY(NM_CLICK, IDC_STN_OUTBOUND_SELECTION_LST, OnClickStnOutboundSelectionLst)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TRN_CYCLICAL_START, OnDurationChange)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TRN_CYCLICAL_STOP, OnPeriodChange)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRN_DVA_MESSAGES_LST, OnItemchangedTrnDvaMessagesLst)
    //ON_BN_CLICKED(IDC_BUTTONALLOUTBOUND, OnButtonalloutbound)
    //ON_BN_CLICKED(IDC_BUTTONALLINBOUND, OnButtonallinbound)
	ON_CBN_SELCHANGE(IDC_COMBO_DVA_PRIORITY, OnSelchangeComboDvaPriority)
	
	ON_BN_CLICKED(IDC_RESTORE_STATUS_BTN, OnRestoreStatusBtn)
    ON_MESSAGE(WM_PA_RIGHTS_CHANGE, OnRightsChanged)
	ON_BN_CLICKED(IDC_KILL_STATUS_BTN, OnKillStatusBtn)
	ON_BN_CLICKED(IDC_KILL_ALL_STATUS_BTN, OnKillAllStatusBtn)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TrainBroadcastPage message handlers

void TrainBroadcastPage::OnTrnLiveBtn()
{
    FUNCTION_ENTRY( "OnTrnLiveBtn" );

    // Force always checked
    setSelectedBroadcastButton(&m_liveBtn);

    // enable GOA group selection
    //m_trainSelectorGUI->enableRadioGroupCombo( true );
	if ( m_isAtLeastTwoTrainsSelected )
	{
		m_staticGroupCombo.EnableWindow(TRUE);
	}
	else
	{
		m_staticGroupCombo.EnableWindow(FALSE);
	}
	m_DVAPriorityCombo.EnableWindow(FALSE);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::setSelectedBroadcastButton(CButton* selection)
{
    FUNCTION_ENTRY( "setSelectedBroadcastButton" );

    if (0 != selection)
    {
        selection->SetCheck(BST_CHECKED);
    }

    if ((&m_liveBtn) != selection)
    {
        m_liveBtn.SetCheck(BST_UNCHECKED);
    }
    if ((&m_dvaBtn) != selection)
    {
        m_dvaBtn.SetCheck(BST_UNCHECKED);
    }

      EnableDvaArea(m_dvaBtn.GetCheck() != 0 ? true: false);

    updateExecuteButtonEnabledState();

    FUNCTION_EXIT;
}


void TrainBroadcastPage::OnTrnDvaBtn()
{
    FUNCTION_ENTRY( "OnTrnDvaBtn" );

    setSelectedBroadcastButton(&m_dvaBtn);

    // disable GOA group selection
    //m_trainSelectorGUI->enableRadioGroupCombo( false );
	m_staticGroupCombo.EnableWindow(FALSE);
	m_DVAPriorityCombo.EnableWindow(TRUE);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::OnTrnCyclicCb()
{
    FUNCTION_ENTRY( "OnTrnCyclicCb" );

    EnableCyclicArea(m_cyclicCb.GetCheck()!=0 ? true: false);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::EnableDvaArea(bool isEnabled)
{
    FUNCTION_ENTRY( "EnableDvaArea" );

    EnableCyclicArea(isEnabled);

    m_dvaMessageList.EnableWindow(isEnabled);
    m_messageFilterTb.EnableWindow(isEnabled);
    m_cyclicCb.EnableWindow(isEnabled);

    m_cyclicCb.EnableWindow(isEnabled);
    if (isEnabled == false)
    {
        EnableCyclicArea(false);
    }
    else
    {
        EnableCyclicArea(m_cyclicCb.GetCheck()!=0 ? true: false);
    }

    FUNCTION_EXIT;
}


void TrainBroadcastPage::EnableCyclicArea(bool isEnabled)
{
    FUNCTION_ENTRY( "EnableCyclicArea" );

    m_duration.EnableWindow(isEnabled);
    m_period.EnableWindow(isEnabled);

    m_trainBroadcastModel->setCyclicalEnabled(isEnabled);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::postDialogCreate()
{
    FUNCTION_ENTRY( "postDialogCreate" );

    initialiseButtonIcons();

    /////////////////////////
    // Setup Prerecorded List
    /////////////////////////
    formatTrainDvaMessageSelectionList(m_dvaMessageList);

    m_dvaFixedHeaderCtrl.subclassHeader(m_dvaMessageList.GetHeaderCtrl());

    // Adjust dimensions of list controls
    static const DWORD LOCN_WIDTH_PIXELS = 60;
    static const DWORD TRAIN_ID_WIDTH_PIXELS = 120;

    // Account for the scroll bar
    // jeffrey++ TD8850
    //int headerTotalWidth = PaLayoutHelper::getListHeaderWidth(getInboundTrainList(), true);
    //int headerTotalWidth = PaLayoutHelper::getListHeaderWidth(getInboundTrainList(), false);
    int headerTotalWidth = 80;
    // ++jeffrey TD8850
    /*getInboundTrainList().setStandardColumnSizes(LOCN_WIDTH_PIXELS,
                                               TRAIN_ID_WIDTH_PIXELS,
                                               headerTotalWidth - LOCN_WIDTH_PIXELS - TRAIN_ID_WIDTH_PIXELS);*/

    // jeffrey++ TD8850
    //headerTotalWidth = PaLayoutHelper::getListHeaderWidth(getOutboundTrainList(), true);
    //headerTotalWidth = PaLayoutHelper::getListHeaderWidth(getOutboundTrainList(), false);
    // ++jeffrey TD8850
    /*getOutboundTrainList().setStandardColumnSizes(LOCN_WIDTH_PIXELS,
                                               TRAIN_ID_WIDTH_PIXELS,
                                               headerTotalWidth - LOCN_WIDTH_PIXELS - TRAIN_ID_WIDTH_PIXELS);*/

    // This will force initial refresh of train messages..
    // Now that list is set up
    OnChangeTrnMessageFilterTb();

    /////////////////////////
    // Setup Train Lists
    /////////////////////////
    // Only initialise the update listener once the window has been initialised
    m_trainBroadcastModel->enableUpdateListener(true);

	refreshISCSVersion();
    // Apply train selector to shared train selection component (should not been done in broadcastpage, but in main page 21/12/2010)
    //m_trainSelectorGUI->setTrainSelector( m_trainBroadcastModel->getTrainSelector() ); 
	//
	m_trainBroadcastModel->attemptTrainSubscriberConnection();

	refreshStatusNameCombo(true);

    FUNCTION_EXIT;
}


BOOL TrainBroadcastPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    //m_rightsChangeHandler =  new RightsChangedWindowNotificationHandler(m_hWnd);

    ITrainBorne::OnInitDialog();

	TB_SetObserver(m_trainBroadcastModel);

    // Create a handler so we can receive zone change notifications from config editor
    m_trainDvaUpdateNotificationHandler =
                    new CachedMapChangedWindowNotificationHandler(
                                        m_hWnd,
                                        WM_PA_TRAIN_DVA_MESSAGE_UPDATE,
                                        TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap());

    // Only want to receive a single notification no matter how many
    // changes are buffered up
    m_trainDvaUpdateNotificationHandler->setMustAcknowledgeBeforeResendFlag(true);

    initialiseTrainListTabSelector();

    /////////////////////////
    // Setup Cyclic Broadcast
    /////////////////////////
    setupCyclicalBroadcastArea();

    OnTrnCyclicCb();

    refreshBroadcastButtonEnabledState();
    // Enter some suitable default broadcast mode
    enterDefaultBroadcastMode();

    // Refresh dynamic group combo contents
    OnDynamicGroupListUpdate(0, 0);

    updateExecuteButtonEnabledState();

    m_messageFilterTb.SetLimitText(MAX_FILTER_TEXT);

	//m_DVAPriorityCombo.mapItemDataToColour(0, COLOR_RED);
	initialiseDVAPriorityComboData();

    PaErrorHandler::setPrimaryWindowHandle(GetSafeHwnd());

	//refreshStatusNameCombo(true);
    // TD 15349
    //std::string selectedTrainMessage = m_paManagerDlg->getSelectedTrainMessage();
    //if (selectedTrainMessage != "" && m_dvaBtn.GetCheck() == BST_CHECKED)
    //{
    //    selectMessageNameInList(selectedTrainMessage);
    //}
    // TD 15349
    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void TrainBroadcastPage::setupCyclicalBroadcastArea()
{
    FUNCTION_ENTRY( "setupCyclicalBroadcastArea" );

    // set the time format
    m_duration.SetFormat("    HH:mm:ss");
    m_period.SetFormat("    HH:mm:ss");

    // set the maximum time - the time can be 0 to 65536 seconds
    CTime lowerTime(1971, 1, 1, 0, 0, 1, 0);
    unsigned short minutes = MAX_TIME_INTERVAL / 60;
    unsigned short hours = minutes / 60;
    minutes -= (hours * 60);
    unsigned short seconds = MAX_TIME_INTERVAL - (hours * 60 * 60) - (minutes * 60);
    CTime upperTime(1971, 1, 1, hours, minutes, seconds, 0);
    m_duration.SetRange(&lowerTime, &upperTime);
    m_period.SetRange(&lowerTime, &upperTime);

    // put a couple of values in for defaults
    CTime defaultPeriod(1971, 1, 1, 0, 1, 0, 0);
    CTime defaultDuration(1971, 1, 1, 0, 5, 0, 0);
    m_period.SetTime(&defaultPeriod);
    m_duration.SetTime(&defaultDuration);

    // Perform initial refresh taking value from controls
    processStartStopTimeChange();

    FUNCTION_EXIT;
}


void TrainBroadcastPage::initialiseButtonIcons()
{
    FUNCTION_ENTRY( "initialiseButtonIcons" );

    HINSTANCE hInstance = AfxGetInstanceHandle();
    CRect tempRect;

    /////////////////////////
    // Load PA Mode icons
    /////////////////////////

    m_liveBtn.GetWindowRect(tempRect);
    m_hLiveIcon = LoadImage( hInstance,
                           MAKEINTRESOURCE( IDI_LIVE_ICON ),
                           IMAGE_ICON,
                           tempRect.Width(),
                           tempRect.Height(),
                           LR_DEFAULTCOLOR );
    m_liveBtn.SetIcon((HICON)m_hLiveIcon);


    m_dvaBtn.GetWindowRect(tempRect);
    m_hDvaIcon = LoadImage( hInstance,
                           MAKEINTRESOURCE( IDI_DVA_ICON ),
                           IMAGE_ICON,
                           tempRect.Width(),
                           tempRect.Height(),
                           LR_DEFAULTCOLOR );
    m_dvaBtn.SetIcon((HICON)m_hDvaIcon );

    FUNCTION_EXIT;
}


afx_msg LRESULT TrainBroadcastPage::OnInboundTrainListUpdate(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnInboundTrainListUpdate" );

    TA_ASSERT(m_trainBroadcastModel != NULL, "m_trainBroadcastModel NULL");

    try
    {
        // Data waiting at the model, collect it for processing
        TrainListUpdateContainer::BufferedItem data =
                            m_trainBroadcastModel->fetchUpdatedInboundTrainData();

        getTrainSelectorGUI().updateInboundList(data.action, data.trainDetails);

        // AZ++ TES 10924
        //if (m_trainDvaVersionsPage != NULL)
        //{
        //    m_trainDvaVersionsPage->updateTrainList(data.action, data.trainDetails);
        //}
        // ++AZ
    }
    STANDARD_PA_CATCH("TrainBroadcastPage::OnInboundTrainListUpdate", PaErrorHandler::UNSPECIFIED_ERROR);

    FUNCTION_EXIT;
    return 0;
}


afx_msg LRESULT TrainBroadcastPage::OnOutboundTrainListUpdate(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnOutboundTrainListUpdate" );

    TA_ASSERT(m_trainBroadcastModel != NULL, "m_trainBroadcastModel NULL");

    try
    {
        // Data waiting at the model, collect it for processing
        TrainListUpdateContainer::BufferedItem data =
                            m_trainBroadcastModel->fetchUpdatedOutboundTrainData();

        getTrainSelectorGUI().updateOutboundList(data.action, data.trainDetails);

        // AZ++ TES 10924
        //if (m_trainDvaVersionsPage != NULL)
        //{
        //    m_trainDvaVersionsPage->updateTrainList(data.action, data.trainDetails);
        //}
        // ++AZ
    }
    STANDARD_PA_CATCH("TrainBroadcastPage::OnOutboundTrainListUpdate", PaErrorHandler::UNSPECIFIED_ERROR);

    FUNCTION_EXIT;
    return 0;
}


afx_msg LRESULT TrainBroadcastPage::OnTrainDvaMessageUpdate(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnTrainDvaMessageUpdate" );

    if (0 == m_trainDvaUpdateNotificationHandler)
    {
        FUNCTION_EXIT;
        return 0;
    }

    try
    {
        // Acknowledge the message prior to update (if a whole bunch of updates
        // are received, we will only get the one notification - to avoid
        // excessive waiting times calling OnChangeTrnMessageFilterTb() many times)
        m_trainDvaUpdateNotificationHandler->acknowledgeMessage();

        DWORD changedItemKey = wParam;

        // Calling this next function will refresh list fully (and deselect
        // any existing selections)
        OnChangeTrnMessageFilterTb();

        // Train messages are not local to a station - indicate to user there
        // has been an update
        PaErrorHandler::displayError(PaErrorHandler::DVA_MESSAGE_UPDATE_RECEIVED_EXTERNALLY);
    }
    STANDARD_PA_CATCH("TrainBroadcastPage::OnTrainDvaMessageUpdate", PaErrorHandler::UNSPECIFIED_ERROR);

    FUNCTION_EXIT;
    return 0;
}


afx_msg LRESULT TrainBroadcastPage::OnDynamicGroupListUpdate(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnDynamicGroupListUpdate" );

    TA_ASSERT(m_trainBroadcastModel != NULL, "m_trainBroadcastModel NULL");

    try
    {
        TA_IRS_Bus::CommonTypes::TrainIdList previousSelection = getTrainSelectorGUI().getSelectedTrains();

        getTrainSelectorGUI().updateSelectionCombo( m_trainBroadcastModel->fetchDynamicGroupList() );

        // TD #3315 - if an external source causes the current selection to change in any way
        // (whether a group was deleted, or selection updated for currently selected group)
        // we must inform the user (not safe to silently update selection without them knowing it)
        TA_IRS_Bus::CommonTypes::TrainIdList newSelection = getTrainSelectorGUI().getSelectedTrains();


        if (newSelection != previousSelection)
        {
            PaErrorHandler::displayError(PaErrorHandler::EXTERNAL_TRAIN_DYNAMIC_GROUP_UPDATE_RECEIVED);
        }
    }
    STANDARD_PA_CATCH("TrainBroadcastPage::OnDynamicGroupListUpdate", PaErrorHandler::UNSPECIFIED_ERROR);

    FUNCTION_EXIT;
    return 0;
}

afx_msg LRESULT TrainBroadcastPage::OnErrorMsg(UINT wParam, LONG lParam)
{
    try
    {
        // This is called by PaErrorHandler class, to allow modal error
        // windows to be displayed from the main loop.
        // Serious potential problems exist if a modal error dialog pops up
        // (with notifications continuing to be processed), since while
        // the modal error message is display, further execution may lead
        // to the same position in code, breaking NonReEntrantThreadlockable locks..
        PaErrorHandler::displayModalError(PaErrorHandler::EErrorId(wParam));
        // Confirm afterwards (to avoid repetitive displaying of errors)
        PaErrorHandler::confirmReceiptOfRequest(wParam);
    }
    catch (...)
    {
    }

    return 0;
}


void TrainBroadcastPage::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    if (m_trainSelectorGUI != NULL)
    {
        //delete m_trainSelectorGUI;
        m_trainSelectorGUI = NULL;
    }

    // Delete the rights change notification object before window handle becomes invalid
    //delete m_rightsChangeHandler;
    //m_rightsChangeHandler = 0;

    // Cannot delete update notification objects after window made
    // invalid, so must do here
    delete m_trainDvaUpdateNotificationHandler;
    m_trainDvaUpdateNotificationHandler = 0;

    TA_ASSERT(m_hWnd != 0, "Internal check failed");

    // Shut down the listener before we destroy this window (otherwise
    // it will attempt to post to non-existent window, and assert)
    m_trainBroadcastModel->enableUpdateListener(false);

    // Delete train broadcast model before window destroyed
    delete m_trainBroadcastModel;
    m_trainBroadcastModel = 0;

    //delete m_outboundTrainList;
    //m_outboundTrainList = 0;

    //delete m_inboundTrainList;
    //m_inboundTrainList = 0;

    ITrainBorne::OnDestroy();

    FUNCTION_EXIT;
}


void TrainBroadcastPage::formatTrainDvaMessageSelectionList(CListCtrl& listToUpdate)
{
    FUNCTION_ENTRY( "formatTrainDvaMessageSelectionList" );

    // ++AZ TES 9613, tooltips set in ListCtrlSelNoFocus
    listToUpdate.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    //listToUpdate.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    // AZ++

    static const DWORD MESSAGE_ID_PIXELS = 50;

    // jeffrey++ TD8850
    //bool accountForScroll = true;
    bool accountForScroll = false;
    // ++jeffrey TD8850

    //int headerTotalWidth = PaLayoutHelper::getListHeaderWidth(listToUpdate, accountForScroll);
    int headerTotalWidth = 80;

    listToUpdate.InsertColumn(0, "DVA Message", LVCFMT_LEFT,
                                    headerTotalWidth);
    /*
    listToUpdate.InsertColumn(1, "ID", LVCFMT_LEFT,
                                    MESSAGE_ID_PIXELS);
    */

    FUNCTION_EXIT;
}


//void TrainBroadcastPage::OnSelchangingTrainlistTab(NMHDR* pNMHDR, LRESULT* pResult)
//{
//    FUNCTION_ENTRY( "OnSelchangingTrainlistTab" );
//
//    if (!refreshTrainListState())
//    {
//        // Failed to switch modes - don't allow tab to change
//        *pResult = TRUE;
//    }
//    else
//    {
//        // Success
//        *pResult = 0;
//
//        LockControlRedraw lock(*this);
//
//        // Remember TAB current selection hasn't changed yet
//        if (m_locationTab.GetCurSel() == 1)       // Switching from Depot (1) mode
//        {
//            // set the GUI to mainline mode
//            getTrainSelectorGUI().setMainlineMode();
//        }
//        else if (m_locationTab.GetCurSel() == 0)      // Switching from Mainline(0) mode
//        {
//            getTrainSelectorGUI().setDepotMode();
//        }
//    }
//
//    FUNCTION_EXIT;
//}


TA_IRS_Bus::LocationCache::EStationType TrainBroadcastPage::getAppropriateInServiceMode() const
{
    FUNCTION_ENTRY( "getAppropriateInServiceMode" );

    if (CachedConfig::getInstance()->getLocationType() ==
                                        CachedConfig::LOCATION_TYPE_OCC)
    {
        // We're at OCC, display all in service trains on circle line
        FUNCTION_EXIT;
        return TA_IRS_Bus::LocationCache::InService;
    }
    else
    {
        // Stations aren't allowed to view all in service, only local trains
        FUNCTION_EXIT;
        return TA_IRS_Bus::LocationCache::Station;
    }
}


bool TrainBroadcastPage::switchToTrainDisplayMode(TA_IRS_Bus::LocationCache::EStationType mode)
{
    FUNCTION_ENTRY( "switchToTrainDisplayMode" );

    try
    {
        CWaitCursor hourGlassCursor;

        //m_trainBroadcastModel->switchTrainListingMode(mode);

        FUNCTION_EXIT;
        return true;
    }
    catch (const TA_Base_Core::PasConnectionException&)
    {
        // Not connected
        PaErrorHandler::displayError(PaErrorHandler::ENTITY_NAME_NOT_FOUND);

        FUNCTION_EXIT;
        return false;
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        // CORBA error
        PaErrorHandler::displayError(PaErrorHandler::ENTITY_NAME_NOT_FOUND);

        FUNCTION_EXIT;
        return false;
    }
    catch (const CORBA::Exception&)
    {
        // Comms error
        TA_THROW(TA_Base_Core::PasConnectionException("COMMS ERROR"));

        FUNCTION_EXIT;
        return false;
    }
}


void TrainBroadcastPage::updateExecuteButtonEnabledState()
{
    FUNCTION_ENTRY( "updateExecuteButtonEnabledState" );

    bool enable = false;

    BOOL anyButtonPressed = m_dvaBtn.GetCheck() ||
                              m_liveBtn.GetCheck();

    // Need to update this in the following cases:
    //- Selection change of trains (selection cleared / modified)
    //- Change of message selection (when a message initially selected)
	int tmp;
    // At least one button must be pressed to enable "Execute" button
    if (anyButtonPressed)
    {
        // If there are no trains selected, cannot enable
        if (m_anyTrainsSelected)
        {
            if (m_dvaBtn.GetCheck())
            {
                // If DVA, must select a message to go with it
                if (TA_Base_Core::DatabaseKey::isValidKey(getSelectedDvaMessageKey()) && (m_DVAPriorityCombo.GetCurSel() >= 0))
                {
                    enable = true;
                    m_trainBroadcastModel->setBroadcastType(TA_Base_Bus::IPAAgentCorbaDef::TrainDva);
                }
            }
            else if (m_liveBtn.GetCheck())
            {
                enable = true;
                m_trainBroadcastModel->setBroadcastType(TA_Base_Bus::IPAAgentCorbaDef::TrainLive);
            }
        }
    }

    if (!enable)
    {
        m_trainBroadcastModel->unsetBroadcastType();
    }

    if (m_pageActive)
    {
        TB_CanApply(enable);
    }

    FUNCTION_EXIT;
}


void TrainBroadcastPage::executeBroadcast()
{
    FUNCTION_ENTRY( "executeBroadcast" );

    CWaitCursor hourGlassCursor;

    TA_ASSERT(m_trainBroadcastModel != NULL, "m_trainBroadcastModel NULL");

    // Collect the train coverage data just before execution
    std::vector<unsigned int> selTrains = getSelectedTrains();
    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage trainCoverage;
    trainCoverage.length(selTrains.size());

    // Transfer selected trains into appropriate structure
    for (unsigned int i = 0; i < selTrains.size(); i ++)
    {
        trainCoverage[i] = selTrains[i];
    }

    m_trainBroadcastModel->setSelectedStaticGroup(m_staticGroupCombo.getCurrentGroupName());

    m_trainBroadcastModel->executeBroadcast(trainCoverage);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::setTimeStartDtp(const CTime& time)
{
    FUNCTION_ENTRY( "setTimeStartDtp" );

    m_duration.SetTime(&time);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::setTimeStopDtp(const CTime& time)
{
    FUNCTION_ENTRY( "setTimeStopDtp" );

    m_period.SetTime(&time);

    FUNCTION_EXIT;
}


CTime TrainBroadcastPage::getTimeStartDtp() const
{
    FUNCTION_ENTRY( "getTimeStartDtp" );

    //time_t seconds = PaLayoutHelper::getSecondsFromControls(0,
    //                                    &m_duration,
    //                                    &m_duration,
    //                                    &m_duration);

    FUNCTION_EXIT;
    return CTime(0);
}


CTime TrainBroadcastPage::getTimeStopDtp() const
{
    FUNCTION_ENTRY( "getTimeStopDtp" );

    //time_t seconds = PaLayoutHelper::getSecondsFromControls(0,
    //                                     &m_period,
    //                                     &m_period,
    //                                     &m_period);

    FUNCTION_EXIT;
    return CTime(0);
}


void TrainBroadcastPage::processStartStopTimeChange()
{
    FUNCTION_ENTRY( "processPeriodDurationChange" );

    CTime startTime;
    CTime stopTime;

    m_duration.GetTime(startTime);
    m_period.GetTime(stopTime);

    int actualStartTime = (startTime.GetHour() * 3600) + (startTime.GetMinute() * 60) + startTime.GetSecond();
    int actualEndTime = (stopTime.GetHour() * 3600) + (stopTime.GetMinute() * 60) + stopTime.GetSecond();

    m_trainBroadcastModel->setDuration(actualStartTime);
    m_trainBroadcastModel->setPeriod(actualEndTime);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::OnDurationChange(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDurationChange" );

    processStartStopTimeChange();

    *pResult = 0;

    FUNCTION_EXIT;
}


void TrainBroadcastPage::OnPeriodChange(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnPeriodChange" );

    processStartStopTimeChange();

    *pResult = 0;

    FUNCTION_EXIT;
}

/*
void TrainBroadcastPage::trainSelectionChanged(bool trainsAreSelected)
{
    FUNCTION_ENTRY( "trainSelectionChanged" );

    // User changed current selection of trains
    if (trainsAreSelected != m_anyTrainsSelected)
    {
        m_anyTrainsSelected = trainsAreSelected;

        // Must update various items whenever there is a state change in current selection
        updateExecuteButtonEnabledState();
    }

    FUNCTION_EXIT;
}
*/

void TrainBroadcastPage::OnButtonDeleteGroup()
{
    FUNCTION_ENTRY( "OnButtonDeleteGroup" );

    getTrainSelectorGUI().deleteTrainSelection();

    FUNCTION_EXIT;
}


void TrainBroadcastPage::OnButtonSaveGroupSel()
{
    FUNCTION_ENTRY( "OnButtonSaveGroupSel" );

    getTrainSelectorGUI().saveTrainSelection();

    FUNCTION_EXIT;
}


std::vector<unsigned int> TrainBroadcastPage::getSelectedTrains()
{
    FUNCTION_ENTRY( "getSelectedTrains" );

    TA_ASSERT(m_trainSelectorGUI != NULL, "Train selection component is NULL");
    std::vector<TA_IRS_Bus::TrainInformationTypes::TrainDetails> selectedTrains = m_trainSelectorGUI->getSelectedTrainData();

    // Also create a vector of strings of the trains that are already in the dymaic group (can't be in the group
    // more than once).
    std::vector<unsigned int> trains;

    for (std::vector<TA_IRS_Bus::TrainInformationTypes::TrainDetails>::iterator iter = selectedTrains.begin();
         iter != selectedTrains.end(); iter++)
    {
        // Work out the ID of this train
        trains.push_back(iter->trainId);
        // TD16865
        unsigned int trainId = iter->trainId;
        std::string serviceNumber = iter->serviceNumber;
        m_trainIdToServiceNumber[trainId] = serviceNumber;
        // TD16865
    }

    FUNCTION_EXIT;
    return trains;
}


//void TrainBroadcastPage::OnButtonalloutbound()
//{
//    FUNCTION_ENTRY( "OnButtonalloutbound" );
//
//    getTrainSelectorGUI().selectAllOutbound();
//
//    FUNCTION_EXIT;
//}


//void TrainBroadcastPage::OnButtonallinbound()
//{
//    FUNCTION_ENTRY( "OnButtonallinbound" );
//
//    getTrainSelectorGUI().selectAllInbound();
//
//    FUNCTION_EXIT;
//}


//void TrainBroadcastPage::OnButtonalltrains()
//{
//    FUNCTION_ENTRY( "OnButtonalltrains" );
//
//    getTrainSelectorGUI().selectAllTrains();
//
//    FUNCTION_EXIT;
//}


//void TrainBroadcastPage::OnButtonclearall()
//{
//    FUNCTION_ENTRY( "OnButtonclearall" );
//
//    getTrainSelectorGUI().clearSelection();
//
//    FUNCTION_EXIT;
//}


void TrainBroadcastPage::OnChangeTrnMessageFilterTb()
{
    FUNCTION_ENTRY( "OnChangeTrnMessageFilterTb" );

    CString rString;
    m_messageFilterTb.GetWindowText(rString);    
    std::string filter = (rString.GetBuffer(0));
    rString.ReleaseBuffer();

    TA_IRS_Bus::TrainDvaMessageRecords trainMessages = m_trainBroadcastModel->getMessages(filter);

    updateDvaMessageList(trainMessages);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::updateDvaMessageList(const TA_IRS_Bus::TrainDvaMessageRecords& messages)
{
    FUNCTION_ENTRY( "updateDvaMessageList" );

    LockControlRedraw redrawLock(m_dvaMessageList);

    // Refresh message list with input items
    m_dvaMessageList.DeleteAllItems();

    for (TA_IRS_Bus::TrainDvaMessageRecords::const_iterator it = messages.begin();
                it != messages.end();
                it ++)
    {
        int nInsertionPoint = m_dvaMessageList.GetItemCount();
        std::string messageLabel = std::string((*it).m_label);

        // Keep adding items to end of CListCtrl (to maintain
        // a natural ordering)
        int nItem = m_dvaMessageList.InsertItem(nInsertionPoint, messageLabel.c_str());

        
        /*CString identifierString;
        identifierString.Format(_T("%d"), (*it).m_pKeyOrId);
        m_dvaMessageList.SetItemText(nItem, TDM_COL, identifierString);*/
        

        m_dvaMessageList.SetItemData(nItem, (*it).m_pKeyOrId);
    }
	m_dvaMessageList.SetColumnWidth(0, LVSCW_AUTOSIZE );

    FUNCTION_EXIT;
}


//void TrainBroadcastPage::OnClickStnInboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult)
//{
//    FUNCTION_ENTRY( "OnClickStnInboundSelectionLst" );
//
//    LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;
//
//    // If user clicks on inbound list without holding ctrl down, clear outbound list selection
//    if (!(lpnmitem->uKeyFlags & LVKF_CONTROL))
//    {
//        getOutboundTrainList().deselectAll();
//    }
//
//    m_dynamicGroupCombo.SetCurSel(-1);                //TD17987
//    m_dynamicGroupCombo.SetWindowText("");            //TD17987
//
//    *pResult = 0;
//
//    FUNCTION_EXIT;
//}


//void TrainBroadcastPage::OnClickStnOutboundSelectionLst(NMHDR* pNMHDR, LRESULT* pResult)
//{
//    FUNCTION_ENTRY( "OnClickStnOutboundSelectionLst" );
//
//    LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;
//
//    // If user clicks on outbound list without holding ctrl down, clear inbound list selection
//    if (!(lpnmitem->uKeyFlags & LVKF_CONTROL))
//    {
//        //getInboundTrainList().deselectAll();
//    }
//
//    m_dynamicGroupCombo.SetCurSel(-1);                //TD17987
//    m_dynamicGroupCombo.SetWindowText("");            //TD17987
//
//    *pResult = 0;
//
//    FUNCTION_EXIT;
//}


BOOL TrainBroadcastPage::OnKillActive()
{
    FUNCTION_ENTRY( "OnKillActive" );

    m_pageActive = false;

    // Disable execute button on the main dialog when lose focus
    //m_paManagerDlg->enableExecuteButton(false);

    FUNCTION_EXIT;
    return TRUE;
}


BOOL TrainBroadcastPage::OnSetActive()
{
    FUNCTION_ENTRY( "OnSetActive" );

    m_pageActive = true;

    CWaitCursor hourGlassCursor;

    // Every time page re-activated, attempt a reconnection if required
	
    if (!m_trainBroadcastModel->isTrainSubscriberConnected())
    {
        m_trainBroadcastModel->attemptTrainSubscriberConnection();
    }
	
    // Should call this after activation (CPropertyPage::OnSetActive)
    // otherwise won't correctly update execute button state on parent page
    updateExecuteButtonEnabledState();

    FUNCTION_EXIT;
    return TRUE;
}


void TrainBroadcastPage::displayDvaStatusDialog(const std::string& broadcastId)
{
    FUNCTION_ENTRY( "displayDvaStatusDialog" );

    TA_ASSERT(m_trainBroadcastModel->testCanObtainLock(), "This thread shouldn't own model lock");

    // jeffrey++ TD7956
    //TrainDvaStatusDlg trainDvaStatusDialog(broadcastId.c_str(), this);
    // TrainDvaStatusDlg trainDvaStatusDialog(broadcastId.c_str(), m_trainBroadcastModel->getHasOverrideOption(), this);
    // ++jeffrey TD7956

    // TD16865
    TrainDvaStatusDlg trainDvaStatusDialog(broadcastId.c_str(), m_trainBroadcastModel->getHasOverrideOption(), m_trainIdToServiceNumber, this);
    // TD16865

    trainDvaStatusDialog.DoModal();

    FUNCTION_EXIT;
}


void TrainBroadcastPage::displayLiveStatusDialog(const std::string& broadcastId)
{
    FUNCTION_ENTRY( "displayLiveStatusDialog" );

    TA_ASSERT(m_trainBroadcastModel->testCanObtainLock(), "This thread shouldn't own model lock");

    // jeffrey++ TD7956
    //TrainLiveStatusDlg trainLiveStatusDialog(broadcastId.c_str(), this);
    // TrainLiveStatusDlg trainLiveStatusDialog(broadcastId.c_str(), m_trainBroadcastModel->getHasOverrideOption(), this);
    // ++jeffrey TD7956

    // TD16865
    TrainLiveStatusDlg trainLiveStatusDialog(broadcastId.c_str(), m_trainBroadcastModel->getHasOverrideOption(), m_trainIdToServiceNumber, this);
    // TD16865

    trainLiveStatusDialog.DoModal();

    FUNCTION_EXIT;
}


void TrainBroadcastPage::OnItemchangedTrnDvaMessagesLst(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemchangedTrnDvaMessagesLst" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // Only interested in state changes
    if (pNMListView->uChanged != LVIF_STATE)
    {
        FUNCTION_EXIT;
        return;
    }

    OnMessageSelectionChanged();

    *pResult = 0;

    FUNCTION_EXIT;
}


int TrainBroadcastPage::getSelectedDvaMessageKey()
{
    FUNCTION_ENTRY( "getSelectedDvaMessageKey" );

    std::vector<unsigned int> selection =
            TA_Base_Bus::SelectionHelper::getSelectedItemIndices(m_dvaMessageList);

    TA_ASSERT(selection.size() <= 1, "Multiple selections not allowed");

    if (0 != selection.size())
    {
        FUNCTION_EXIT;
        return m_dvaMessageList.GetItemData(selection[0]);
    }

    FUNCTION_EXIT;
    return TA_Base_Core::DatabaseKey::getInvalidKey();
}


void TrainBroadcastPage::OnMessageSelectionChanged()
{
    FUNCTION_ENTRY( "OnMessageSelectionChanged" );

    m_trainBroadcastModel->setSelectedMessage(getSelectedDvaMessageKey());

	m_trainBroadcastModel->setSelectedDvaMessageText(getSelectedDvaMessageText());

    updateExecuteButtonEnabledState();

    FUNCTION_EXIT;
}


void TrainBroadcastPage::refreshBroadcastButtonEnabledState()
{
    FUNCTION_ENTRY( "refreshBroadcastButtonEnabledState" );

    //BOOL enableBroadcastLive = PaRightsManager::getInstance().
    //                                    hasRightToBroadcastTrainLive();


    //BOOL enableBroadcastDva = PaRightsManager::getInstance().hasRight(
    //                                    PaRightsManager::TRAIN_PA_PRERECORDED_ANNOUNCEMENT);

    //m_liveBtn.EnableWindow(enableBroadcastLive);
    //m_dvaBtn.EnableWindow(enableBroadcastDva);

    FUNCTION_EXIT;
}


void TrainBroadcastPage::enterDefaultBroadcastMode()
{
    FUNCTION_ENTRY( "enterDefaultBroadcastMode" );

    if (m_dvaBtn.IsWindowEnabled())
    {
        // Attempt to default to DVA Button pressed, by default
        OnTrnDvaBtn();
    }
    else if (m_liveBtn.IsWindowEnabled())
    {
        OnTrnLiveBtn();
    }
    else
    {
        // No selection possible..
        setSelectedBroadcastButton(0);
    }

    FUNCTION_EXIT;
}


//bool TrainBroadcastPage::refreshTrainListState()
//{
//    FUNCTION_ENTRY( "refreshTrainListState" );
//
//    TA_IRS_Bus::LocationCache::EStationType newMode;
//
//    // There should be a better way of identifying which tab we're changing
//    // the selection to - but while we only have two tabs total, it will be
//    // sufficient to use the current tab selection to find this info out
//    if (m_locationTab.GetCurSel() == 1)
//    {
//        // Currently on Depot tab - can only possibly be switching to In Service tab
//        newMode = getAppropriateInServiceMode();
//    }
//    else if (m_locationTab.GetCurSel() == 0)
//    {
//        // Current on In Service tab, can only possible be switching to Depot tab
//        newMode = TA_IRS_Bus::LocationCache::Depot;
//    }
//
//    bool result = switchToTrainDisplayMode(newMode);
//
//    if (result)
//    {
//        // Switch went OK
//        // Clear all current train selections when switching tabs
//        //OnButtonclearall();
//    }
//
//    FUNCTION_EXIT;
//    return result;
//}


void TrainBroadcastPage::initialiseTrainListTabSelector()
{
    FUNCTION_ENTRY( "initialiseTrainListTabSelector" );

    // this does the GUI logic
    //m_trainSelectorGUI = new TA_IRS_Bus::TrainSelectorGUI( getInboundTrainList(),  // pass all controls
    //                                                   &getOutboundTrainList(),
    //                                                   &m_allInboundButton,
    //                                                   &m_allOutboundButton,
    //                                                   &m_selectAllTrainsButton,
    //                                                   &m_clearTrainSelectionButton,
    //                                                   &m_inboundLabel,
    //                                                   &m_outboundLabel,
    //                                                   &m_dynamicGroupCombo,
    //                                                   &m_saveDynamicGroupButton,
    //                                                   &m_deleteDynamicGroupButton,
    //                                                   &m_staticGroupCombo,
    //                                                   NULL );  // this listens for train selection changes
	m_trainSelectorGUI = m_parentGUI;
    // for ease of debugging - have it so you can enable all train selection tabs
    //if (!TA_Base_Core::RunParams::getInstance().isSet("EnableAllTabs"))
    //{
    //    if (m_trainBroadcastModel->getCurrentListingMode() != TA_IRS_Bus::LocationCache::InService)
    //    {
    //        // if this is at a station, disable the depot tab
    //        // if at the depot, disable the mainline tab
    //        // if this is the occ, dont disable any tabs
    //        if (m_trainBroadcastModel->getCurrentListingMode() == TA_IRS_Bus::LocationCache::Depot)
    //        {
    //            m_locationTab.SetActiveTab(1);
    //            getTrainSelectorGUI().setDepotMode();
    //            m_locationTab.EnableTabIndex(0, false);

    //            // the train selection component should already be in depot mode
    //        }
    //        else
    //        {
    //            // already on mainline tab
    //            m_locationTab.EnableTabIndex(1, false);
    //        }
    //    }
    //    else if (CachedConfig::getInstance()->isAtOcc())
    //    {
    //        // TES #628: Depot tab only available at depot (since a change in requirements, there
    //        // is never a situation when both tabs are enabled, making two tabs a bit redundant)
    //        m_locationTab.EnableTabIndex(1, false);
    //    }
    //}

    //// Add the required tab items
    //m_locationTab.InsertItem(0, s_inServiceTabText.c_str());
    //m_locationTab.InsertItem(1, s_depotTabText.c_str());

    //// Synchronize the current train mode with the tab selection
    //// (ie select the tab that reflects current train selection)
    //if (TA_IRS_Bus::LocationCache::Depot ==
    //            m_trainBroadcastModel->getCurrentListingMode())
    //{
    //    // Select the depot (second) tab initially
    //    m_locationTab.SetCurSel(1);
    //}
    //else if (getAppropriateInServiceMode() ==
    //                m_trainBroadcastModel->getCurrentListingMode())
    //{
    //    // Select the in service (first) tab
    //    m_locationTab.SetCurSel(0);
    //}
    //else
    //{
    //    TA_ASSERT(false, "Unexpected initial state");
    //}

    //// Enable the owner draw style for tab control (so can properly
    //// display enabled / disabled tabs)
    //m_locationTab.ModifyStyle(0, TCS_OWNERDRAWFIXED);

    //// Now, depending on which location we're running at, may need
    //// to disable some of the tabs
    //switch (CachedConfig::getInstance()->getLocationType())
    //{
    //case CachedConfig::LOCATION_TYPE_DEPOT:
    //    // The depot can only display depot items
    //    // Disable in service tab at first index
    //    m_locationTab.EnableTabIndex(0, false);
    //    break;
    //case CachedConfig::LOCATION_TYPE_STATION:
    //    // Stations can't view the depot trains (at second index)
    //    m_locationTab.EnableTabIndex(1, false);
    //    break;
    //case CachedConfig::LOCATION_TYPE_OCC:
    //    // Both tabs enabled, nothing to disable
    //    break;
    //default:
    //    TA_ASSERT(false, "Unexpected location");
    //}

    //// Confirm initially selected tab is an enabled tab (this should be
    //// set appropriately in constructor)
    //TA_ASSERT(m_locationTab.IsTabEnabled(m_locationTab.GetCurSel()), "Invalid initial selection");

    // Set up initial rights settings..
    refreshTrainSelectionComponentRights();

    FUNCTION_EXIT;
}


afx_msg LRESULT TrainBroadcastPage::OnRightsChanged(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnRightsChanged" );

    try
    {
        // Refresh the enabled state of the DVA / Live broadcast buttons
        refreshBroadcastButtonEnabledState();

        // If the currently checked button happens to become disabled, we need to
        // have the active selection change (to something..anything..that's valid)
        if (BST_CHECKED == m_dvaBtn.GetCheck() && !m_dvaBtn.IsWindowEnabled() ||
            BST_CHECKED == m_liveBtn.GetCheck() && !m_liveBtn.IsWindowEnabled())
        {
            enterDefaultBroadcastMode();
        }

        refreshTrainSelectionComponentRights();
    }
    STANDARD_PA_CATCH("TrainBroadcastPage::OnRightsChanged", PaErrorHandler::UNSPECIFIED_ERROR);

    FUNCTION_EXIT;
    return 0;
}


void TrainBroadcastPage::refreshTrainSelectionComponentRights()
{
    FUNCTION_ENTRY( "refreshTrainSelectionComponentRights" );

    // Because the train selection component is shared between apps,
    // use the Radio Manager privelege aca_RADIO_MANAGE_TRAIN_SELECTIONS to
    // determine appropriate right settings
 //   bool enableTrainGroups = PaRightsManager::getInstance().hasRight(PaRightsManager::RADIO_MANAGE_TRAIN_SELECTIONS);

 //   // update the rights in the train selection component
 //   //getTrainSelectorGUI().setRights( true,    // Train list always enabled
 //   //getTrainSelectorGUI().setRights( enableTrainGroups,    // TD17648 lichao
	//getTrainSelectorGUI().setRights( true,    // hongzhi, temporarily revert it, since rights have some problem, always disabled
 //                                  enableTrainGroups,           // train group combo
 //                                  enableTrainGroups,           // save train groups
 //                                  enableTrainGroups);          // delete train groups

    FUNCTION_EXIT;
}


TA_IRS_Bus::TrainSelectorGUI& TrainBroadcastPage::getTrainSelectorGUI()
{
    FUNCTION_ENTRY( "getTrainSelectorGUI" );

    TA_ASSERT(m_trainSelectorGUI != NULL, "Train selection component is NULL");

    FUNCTION_EXIT;
    return *m_trainSelectorGUI;
}


bool TrainBroadcastPage::selectMessageNameInList(const std::string messageName)
{
    FUNCTION_ENTRY( "selectMessageNameInList" );
    FUNCTION_EXIT;
    return findAndSelectMessageNameInList(messageName);
}


bool TrainBroadcastPage::findAndSelectMessageNameInList(const std::string messageName)
{
    FUNCTION_ENTRY( "findAndSelectMessageNameInList" );

    bool isSelectMessageNameInList = false;

    // To represent space between words or characters in selected message, use underscore ( _ ).
    // So underscore is reserved for parsing the command-line and substituted with space.
    // It must not be used as a character in the name.
    std::string::size_type index = 0;
    std::string::size_type pindex = 0;
    std::string paramValue("");

    pindex = messageName.find("_", index);
    while (pindex != std::string::npos)
    {
        paramValue.append(messageName.substr(index,pindex-index));
        paramValue.append(" ");
        index = pindex + 1;
        pindex = messageName.find("_", index);
    }
    paramValue.append(messageName.substr(index,messageName.size()-index));

    // compare selected message with the messages in list
    for (int nItem = 0; nItem < m_dvaMessageList.GetItemCount(); nItem++)
    {
        CString nameInList = m_dvaMessageList.GetItemText(nItem, 0);

        if (stricmp(nameInList, paramValue.c_str()) == 0)
        {
            if (clearMessageSelectionInList() && m_dvaMessageList.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED))
            {
                isSelectMessageNameInList = true;
                break;
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot clear the exist selected message and add the pre-select message into message list");
            }
        }
    }

    FUNCTION_EXIT;
    return isSelectMessageNameInList;
}


bool TrainBroadcastPage::clearMessageSelectionInList()
{
    FUNCTION_ENTRY( "clearMessageSelectionInList" );

    for (int nItem = 0; nItem < m_dvaMessageList.GetItemCount(); nItem++)
    {
        if (m_dvaMessageList.SetItemState(nItem, 0, LVIS_SELECTED | LVIS_FOCUSED) == false)
        {
            FUNCTION_EXIT;
            return false;
        }

    }

    FUNCTION_EXIT;
    return true;
}

bool TrainBroadcastPage::TB_DoApply()
{
	FUNCTION_ENTRY( "executeBroadcast" );

	TA_ASSERT(m_trainBroadcastModel != NULL, "m_trainBroadcastModel NULL");

	// Collect the train coverage data just before execution
	std::vector<unsigned int> selTrains = getSelectedTrains();
	TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage trainCoverage;
	trainCoverage.length(selTrains.size());

	// Transfer selected trains into appropriate structure
	for (unsigned int i = 0; i < selTrains.size(); i ++)
	{
		trainCoverage[i] = selTrains[i];
	}

	m_trainBroadcastModel->setSelectedStaticGroup(m_staticGroupCombo.getCurrentGroupName());

	m_trainBroadcastModel->executeBroadcast(trainCoverage);

	FUNCTION_EXIT;
    return true;
}

bool TrainBroadcastPage::TB_CheckEntity( TA_Base_Core::IEntityData* guiEntity )
{
	//if (m_commandLineInvalid)
	//{
	//	return;
	//}
	FUNCTION_ENTRY( "checkEntity" );

	//setGuiEntity(*guiEntity);

	// Note that the following can throw InvalidPAConfigurationException
	// which will be handled by the GenericGUI framework

	CachedConfig::getInstance()->setEntityKey(guiEntity->getKey());
	CachedConfig::getInstance()->setEntityName(guiEntity->getName());
	CachedConfig::getInstance()->setEntitySubsystemKey(guiEntity->getSubsystem());
	CachedConfig::getInstance()->setEntityTypeKey(guiEntity->getTypeKey());
	
	CachedConfig::getInstance()->setPhysicalLocationKey(atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str()));

	CachedConfig::getInstance()->setSessionId(RunParams::getInstance().get(RPARAM_SESSIONID).c_str());

	unsigned long locationKey = CachedConfig::getInstance()->getPhysicalLocationKey();

	ILocation::ELocationType eLocationType = LocationAccessFactory::getInstance().getLocationType(locationKey);

	// Determine special location flags:
	if (ILocation::OCC == eLocationType)
	{
		CachedConfig::getInstance()->setLocationType(CachedConfig::LOCATION_TYPE_OCC);
	}
	else
	{ 
		if (ILocation::DPT == eLocationType)
		{
			CachedConfig::getInstance()->setLocationType(CachedConfig::LOCATION_TYPE_DEPOT);
		}
		else
		{
			// Station
			CachedConfig::getInstance()->setLocationType(CachedConfig::LOCATION_TYPE_STATION);
		}   
	}

	std::string entityName;
	if (locationKey == LocationAccessFactory::getInstance().getOccLocationKey())
	{
		entityName = MASTER_PA_AGENT_ENTITY_TYPE_NAME;
	}
	else
	{
		entityName = STATION_PA_AGENT_ENTITY_TYPE_NAME;
	}

	// Not used currently
	//    CachedConfig::getInstance()->setLocalPAAgentKey(DEMO_PA_AGENT_KEY);
	std::vector<CorbaName> allNames =
		EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(entityName, locationKey,true);   
	TA_ASSERT(allNames.size() == 1, "There can be only one CORBA NAME at specified location");
	CachedConfig::getInstance()->setLocalPAAgentCorbaName(allNames[0]);

	{
		TA_Base_Core::IEntityDataList entityArray = 
			EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( entityName, locationKey );
		TA_ASSERT(entityArray.size() == 1, "There can be only one Entity NAME at specified location");

		CachedConfig::getInstance()->setLocalPAAgentEntityName(entityArray[0]->getName());
		TA_IRS_Bus::deleteAllItemsInArray(entityArray);
	}

	{
		TA_Base_Core::IEntityDataList entityArray = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( 
			TrainAgentEntityData::getStaticType(), locationKey );
		TA_ASSERT(entityArray.size() == 1, "There can be only one Entity NAME at specified location");

		CachedConfig::getInstance()->setLocalTrainAgentEntityName(entityArray[0]->getName());
		TA_IRS_Bus::deleteAllItemsInArray(entityArray);
	}

	try
	{
		// Perform initial refresh of cached maps
		CachedMapLoader::loadAllCachedMaps();
	}
	catch (const TA_Base_Core::TransactiveException& ex)
	{
		// Error reading data from database
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Database initial read error", ex.what());
		throw;
	}
	catch (const std::exception& se)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Internal error reading database", se.what());
		throw;
	}

	postDialogCreate();

	return true;
}

void TrainBroadcastPage::TB_PageActived( bool bActived )
{
	OnSetActive();
}

void TrainBroadcastPage::refreshISCSVersion()
{
	FUNCTION_ENTRY( "refreshISCSVersion" );

	std::ostringstream iscsVersion;

    try
    {
        iscsVersion << m_trainBroadcastModel->getTrainDvalibraryVersion();
    }
    catch(const TA_Base_Core::InvalidPasConfigurationException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, typeid(e).name(), e.what());
    }
	

	CWnd* pVersion  = GetDlgItem( IDC_DVA_VERSION_FIELD );

	if ( 0 != pVersion )
	{
		pVersion->SetWindowText( ( LPCTSTR ) iscsVersion.str().c_str() );
	}

	FUNCTION_EXIT;
}



void TrainBroadcastPage::TB_TrainSelected( TA_IRS_Bus::CommonTypes::TrainIdList selList )
{
	if (0 == selList.size())
	{
		m_anyTrainsSelected = false;
		m_staticGroupCombo.EnableWindow(FALSE);
	}
	else
	{
		if ( selList.size() >= 2 )
		{
			m_isAtLeastTwoTrainsSelected = TRUE;
		}
		else
		{
			m_isAtLeastTwoTrainsSelected = FALSE;
		}
		if ( m_liveBtn.GetCheck() && m_isAtLeastTwoTrainsSelected )
		{
			m_staticGroupCombo.EnableWindow(TRUE);
		}
		else
		{
			m_staticGroupCombo.EnableWindow(FALSE);
		}
		m_anyTrainsSelected = true;
	}
	updateExecuteButtonEnabledState();

}

void TrainBroadcastPage::TB_SetObserver( TA_IRS_Bus::ITrainSelectorGUI* gui/*=NULL*/ )
{
	m_observer = gui;
}

TA_IRS_Bus::TrainSelector& TrainBroadcastPage::getTrainSelector()
{
	
	return *m_parentSelector;
}

void TrainBroadcastPage::TB_SetParentSelector( TA_IRS_Bus::TrainSelector* selector )
{
	ITrainBorne::TB_SetParentSelector(selector);
    m_trainBroadcastModel->TB_SetParentSelector(selector);
}

void TrainBroadcastPage::initialiseDVAPriorityComboData()
{
	FUNCTION_ENTRY( "initialiseDVAPriorityComboData" );

	m_DVAPriorityCombo.saveSelection();

	try
	{
		// Clear everything in combo
		m_DVAPriorityCombo.ResetContent();

		int index = m_DVAPriorityCombo.AddString(_T("Normal"));
		m_DVAPriorityCombo.SetItemData(index, false);
		m_DVAPriorityCombo.SetCurSel(index);

		index = m_DVAPriorityCombo.AddString(_T("Emergency"));
		m_DVAPriorityCombo.SetItemData(index, true);

		m_DVAPriorityCombo.mapItemDataToColour(true, COLOR_RED);
		
	}
	catch (const TA_Base_Core::TransactiveException&)
	{
		// Consequences of this: the broadcast selection zone list will be empty
		PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
	}
	catch (const CORBA::Exception&)
	{
		// Comms error??
		PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);
	}
}

void TrainBroadcastPage::OnSelchangeComboDvaPriority()
{
	FUNCTION_ENTRY( "OnSelchangeComboDvaPriority" );

	m_trainBroadcastModel->setSelectedDvaMessagePriority(getSelectedDvaMessagePriority());

	updateExecuteButtonEnabledState();

	FUNCTION_EXIT;
}

bool TrainBroadcastPage::getSelectedDvaMessagePriority()
{
	FUNCTION_ENTRY( "getSelectedDvaMessagePriority" );

	int curSel = m_DVAPriorityCombo.GetCurSel();
	bool priorityKey = false;

	// Should always have at least one selection..
	if (curSel >= 0)
	{
		priorityKey = static_cast<bool>(m_DVAPriorityCombo.GetItemData(curSel));
	}

	return priorityKey;
}

void TrainBroadcastPage::OnSelchangeStatusCb()
{
	FUNCTION_ENTRY( "OnSelchangeStatusCb" );

	int curSel = m_statusNameList.GetCurSel();

	// User selected a new DVA broadcast group (aka Status Name)
	BOOL validSelection = (curSel >= 0);

	std::vector<std::string> statusNames;
	for (int i = 0; i < m_statusNameList.GetCount(); i ++)
	{
		CString broadcastId;
		m_statusNameList.GetLBText(i, broadcastId);
		statusNames.push_back(broadcastId.GetBuffer(0));
	}

	///////////////////////
	// Check if clear selected broadcast should be enabled
	BOOL allowedToClearSelectedBroadcast = validSelection;
	if (allowedToClearSelectedBroadcast)
	{
		// Confirm if about to enable the kill status button
		// that the user has the required access privileges to do so
		allowedToClearSelectedBroadcast = PaRightsManager::getInstance().hasRightToClearBroadcast(
			statusNames[curSel],
			CachedConfig::getInstance()->getAgentObject());

		// TES #703: Cannot clear if currently in progress
		if (PaRightsManager::getInstance().isBroadcastInProgress(
			statusNames[curSel],
			CachedConfig::getInstance()->getAgentObject()))
		{
			allowedToClearSelectedBroadcast = false;
		}
	}

	//m_killStatusButton.EnableWindow(allowedToClearSelectedBroadcast);
	////////////////////////


	////////////////////////
	// Check if restore button should be enabled
	m_restoreStatusButton.EnableWindow(validSelection);
	////////////////////////

	//////////////////////////////
	// Determine whether clear all button should be enabled
	//bool enableKillAll = (m_statusNameList.GetCount() > 0);

	// May use clear all provided there is one or more item in list
	// NOTE: May not be able to clear the broadcasts if privileges not present (feedback provided to operator)
	//m_killAllStatusButton.EnableWindow(enableKillAll);
	//////////////////////////////

	FUNCTION_EXIT;
}

void TrainBroadcastPage::refreshStatusNameCombo( bool bInitialization )
{
	FUNCTION_ENTRY( "refreshStatusNameCombo" );

	// First off clear the list
	m_statusNameList.ResetContent();
	m_statusNameList.SetCurSel(-1); // Deselect all

	// Perform initial update of buttons linked to selection in this combo
	// The buttons will be disabled initially until valid selection made
	OnSelchangeStatusCb();

	// Before fill out selection, choose type of messages
	// we respond to (when a status name comes through, we need to
	// filter out the ones we don't care about)
	m_currentStatusNameBroadcastType.resetValue();

	//if (getActiveTabPage() == m_stationPage)
	//{
	//	m_currentStatusNameBroadcastType.setValue(TA_Base_Bus::IPAAgentCorbaDef::StationDva);
	//}
	//else if (getActiveTabPage() == m_trainPage)
	//{
		m_currentStatusNameBroadcastType.setValue(TA_Base_Bus::IPAAgentCorbaDef::TrainDva);
	//}
	//else
	//{
	//	// Any unrecognized windows do not respond to any status name changes
	//	// so may exit early
	//	m_statusNameList.EnableWindow(FALSE);

	//	FUNCTION_EXIT;
	//	return;
	//}

	//if (0 == m_managerModel)
	if (0 == m_trainBroadcastModel)
	{
		TA_ASSERT(FALSE, "m_trainBroadcastModel NULL");

		FUNCTION_EXIT;
		return;
	}

	// Establish connection to name change subscriber before filling
	// out the status name combo box (important to minimize risk of out
	// of sync issues)
	if (bInitialization)
	{
		m_trainBroadcastModel->initializeStatusNameSubscriber(m_hWnd);
		//m_managerModel->initializeStatusNameSubscriber(m_hWnd);
	}

	std::vector<std::string> broadcastZones;

	bool success = false;

	try
	{
		/*if (getActiveTabPage() == m_stationPage)
		{
			broadcastZones = m_managerModel->getStationDvaBroadcastZones();
		}
		else if (getActiveTabPage() == m_trainPage)
		{*/
			//broadcastZones = m_managerModel->getTrainDvaBroadcastZones();
			broadcastZones = m_trainBroadcastModel->getTrainDvaBroadcastZones();
		/*}
		else
		{
			TA_ASSERT(FALSE, "Unrecognized page");
		}*/

		success = true;
	}
	catch (TA_Base_Core::ObjectResolutionException&)
	{
	}
	catch (const CORBA::Exception&)
	{
	}

	if (!success)
	{
		// Unable to resolve CORBA object, no update available (combo remains empty)
		PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);

		// The combo can't be used
		m_statusNameList.EnableWindow(false);

		FUNCTION_EXIT;
		return;
	}

	// Enable list iff items present
	m_statusNameList.EnableWindow(broadcastZones.size() > 0);

	// Add all the names to the status name combo box
	for (unsigned int i = 0; i < broadcastZones.size(); i ++)
	{
		m_statusNameList.AddString(broadcastZones[i].c_str());
	}

	// Now all items present in status name list, perform another refresh
	// to keep up to date (typically results in clear all button being enabled
	// if applicable)
	OnSelchangeStatusCb();

	FUNCTION_EXIT;
}

void TrainBroadcastPage::OnRestoreStatusBtn()
{
	FUNCTION_ENTRY( "OnRestoreStatusBtn" );

	CString broadcastId;
	int sel = m_statusNameList.GetCurSel();
	if (sel < 0)
	{
		// Despite all sorts of attempts, this condition can still occur when a combo box
		// changes while it is 'dropped down' (the onSelChanged function is not called when it should be)
		// So silently ignore
		OnSelchangeStatusCb();
		//TA_ASSERT(FALSE, "Invalid current selection");
		FUNCTION_EXIT;
		return;
	}

	m_statusNameList.GetLBText(sel, broadcastId);

	// Check if this broadcast ID still exists before attempting restore
	if (!m_trainBroadcastModel->confirmStatusNameExists(broadcastId.GetBuffer(0)))
	//if (!m_managerModel->confirmStatusNameExists(broadcastId.GetBuffer(0)))
	{
		// No longer exists - we're out of sync, perform full refresh of combo
		refreshStatusNameCombo(false);

		FUNCTION_EXIT;
		return;
	}

	if (!m_currentStatusNameBroadcastType.hasBeenSet())
	{
		TA_ASSERT(FALSE, "Unknown broadcast ID context");

		FUNCTION_EXIT;
		return;
	}

	// If we don't have the right to display this status name (that we're
	// attempting to restore), then don't allow it) - we shouldn't get here
	// normally (because the status name shouldn't exist to be selectable
	// in the first place..)
	if (!PaRightsManager::getInstance().hasRightToDisplayBroadcast(
		broadcastId.GetBuffer(0),
		CachedConfig::getInstance()->getAgentObject()) )
	{
		PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);

		FUNCTION_EXIT;
		return;
	}

	//if (getActiveTabPage() == m_stationPage)
	//{
	//	m_stationPage->displayDvaStatusDialog(broadcastId.GetBuffer(0));
	//}
	//else if (getActiveTabPage() == m_trainPage)
    //{
    std::string strBroadcastId = broadcastId.GetBuffer(0);
    m_trainBroadcastModel->restoreBroadcast(strBroadcastId);
    displayDvaStatusDialog(strBroadcastId.c_str());
		//m_trainPage->displayDvaStatusDialog(broadcastId.GetBuffer(0));
	/*}
	else
	{
		TA_ASSERT(FALSE, "PAManagerDlg::OnRestoreStatusBtn: Invalid mode for restore operation");
	}*/

	FUNCTION_EXIT;
}

void TrainBroadcastPage::OnKillStatusBtn()
{
	FUNCTION_ENTRY( "OnKillStatusBtn" );
	if(0 == m_trainBroadcastModel)
	//if (0 == m_managerModel)
	{
		TA_ASSERT(FALSE, "m_managerModel NULL");

		FUNCTION_EXIT;
		return;
	}

	int sel = m_statusNameList.GetCurSel();

	if (sel < 0)
	{
		// Despite all sorts of attempts, this condition can still occur when a combo box
		// changes while it is 'dropped down' (the onSelChanged function is not called when it should be)
		// So silently ignore
		OnSelchangeStatusCb();

		//TA_ASSERT(FALSE, "Invalid current selection");
		FUNCTION_EXIT;
		return;
	}

	// Kill off a single item
	CString broadcastId;
	m_statusNameList.GetLBText(sel, broadcastId);

	std::vector<std::string> broadcastArray;
	broadcastArray.push_back(broadcastId.GetBuffer(0));

	try
	{
		if (m_trainBroadcastModel->terminateAndRemoveBroadcastIds(broadcastArray)
		//if (m_managerModel->terminateAndRemoveBroadcastIds(broadcastArray)
			!= broadcastArray.size())
		{
			// Not all broadcasts passed in could be removed (maybe some already
			// removed..) - meaning we're not up to date
			// Force a full update, to ensure we're in sync with agent
			refreshStatusNameCombo(false);
		}
	}
	catch (const CORBA::Exception&)
	{
		PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
	}
	catch (const TA_Base_Core::TransactiveException&)
	{
		PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
	}

	FUNCTION_EXIT;
}

void TrainBroadcastPage::OnKillAllStatusBtn()
{
	FUNCTION_ENTRY( "OnKillAllStatusBtn" );
	if( 0 == m_trainBroadcastModel)
	//if (0 == m_managerModel)
	{
		TA_ASSERT(FALSE, "m_managerModel NULL");

		FUNCTION_EXIT;
		return;
	}

	std::vector<std::string> broadcastArray;

	// Array of all broadcasts we don't have a right to terminate
	std::vector<std::string> invalidRights;
	// Array of all broadcasts still in progress
	std::vector<std::string> inProgressBroadcasts;

	// Progressively go through and kill each item currently in listbox..
	for (int i = 0; i < m_statusNameList.GetCount(); i ++)
	{
		CString broadcastId;
		m_statusNameList.GetLBText(i, broadcastId);

		// Only add those items we're allowed to remove to the list
		// (but if rights change before actual call, secondary test performed
		// there also, which will pop up error dialog)
		if (PaRightsManager::getInstance().hasRightToClearBroadcast(
			broadcastId.GetBuffer(0),
			CachedConfig::getInstance()->getAgentObject()))
		{
			if (!PaRightsManager::getInstance().isBroadcastInProgress(
				broadcastId.GetBuffer(0),
				CachedConfig::getInstance()->getAgentObject()))
			{
				// TES #703: Can only clear inactive broadcasts
				broadcastArray.push_back(broadcastId.GetBuffer(0));
			}
			else
			{
				inProgressBroadcasts.push_back(broadcastId.GetBuffer(0));
			}
		}
		else
		{
			invalidRights.push_back(broadcastId.GetBuffer(0));
		}
	}

	try
	{
		// This could feasibly be empty if no right to clear any in the list
		if (broadcastArray.size() > 0)
		{
			if (m_trainBroadcastModel->terminateAndRemoveBroadcastIds(broadcastArray) !=
			//if (m_managerModel->terminateAndRemoveBroadcastIds(broadcastArray) !=
				broadcastArray.size())
			{
				// Not all broadcasts passed in could be removed (maybe some already
				// removed..) - meaning we're not up to date
				// Force a full update, to ensure we're in sync with agent
				refreshStatusNameCombo(false);
			}
		}

		std::ostringstream reportStr;

		// Display a report showing the broadcasts that could not be cleared
		if (invalidRights.size() > 0)
		{
			// UI-610002
			reportStr << "The following broadcasts could not be cleared due to insufficient operator privileges:" << std::endl;

			for (unsigned int i = 0; i < invalidRights.size(); i ++)
			{
				reportStr << "\t" << invalidRights[i] << std::endl;
			}
		}

		// TES #703 indicate which broadcasts couldn't be cleared because they were in progress
		if (inProgressBroadcasts.size() > 0)
		{
			// UI-610002
			reportStr << "The following broadcasts are currently active, and cannot be cleared:" << std::endl;

			for (unsigned int i = 0; i < inProgressBroadcasts.size(); i ++)
			{
				reportStr << "\t" << inProgressBroadcasts[i] << std::endl;
			}
		}

		if (reportStr.str().length() > 0)
		{
			//TD14164 yezh++
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UI_610002);
			//AfxMessageBox(reportStr.str().c_str());
			//TD14164 yezh++
		}
	}
	catch (const CORBA::Exception&)
	{
		PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
	}
	catch (const TA_Base_Core::TransactiveException&)
	{
		PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
	}

	FUNCTION_EXIT;
}

LRESULT TrainBroadcastPage::OnStatusNameChange( UINT wParam, LONG lParam )
{
	FUNCTION_ENTRY( "OnStatusNameChange" );

	try
	{
		if (!updateComboBoxOnNameChange(wParam))
		{
			// Failed to update the combo box, so refresh it entirely
			// - because the name list may now not be up to date, because failed to
			//   process the received update

			// Attempt to update the name change properly
			refreshStatusNameCombo(false);
		}

		// Update enabled state of status name list dependent on how
		// many items are present there
		m_statusNameList.EnableWindow(m_statusNameList.GetCount() > 0);

		// Now that combo box has changed, update selection options
		OnSelchangeStatusCb();
	}
	STANDARD_PA_CATCH("PAManagerDlg::OnStatusNameChange", PaErrorHandler::UNSPECIFIED_ERROR);

	FUNCTION_EXIT;
	return 0;
}

bool TrainBroadcastPage::updateComboBoxOnNameChange( UINT id )
{
	FUNCTION_ENTRY( "updateComboBoxOnNameChange" );

	if (0 == m_trainBroadcastModel)
	{
		TA_ASSERT(FALSE, "m_managerModel NULL");

		FUNCTION_EXIT;
		return false;
	}

	// When a Status Name change occurs, we must intelligently respond so
	// that the current selection (in the Combo Box) remains consistent
	// There are a couple of possibilities:
	//  a) Selected item renamed - the appropriate item in list renamed
	//  b) Selected item deleted - the selection in list gets zeroed out
	// If there are any invalid notifications received (referencing non existent
	// status names etc), they are simply ignored..
	std::string from;
	std::string to;

	try
	{
		TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType =
			//m_managerModel->collectStatusNameChange(id, m_hWnd, from, to);
			m_trainBroadcastModel->collectStatusNameChange(id, m_hWnd, from, to);

		if (!m_currentStatusNameBroadcastType.hasBeenSetToValue(broadcastType))
		{
			// If the current status name type we want doesn't match
			// the broadcast type of the change we're responding to -
			// we may discard the change (return true to indicate
			// processing successful).  It is quite normal to receive
			// changes not relevant to us (ie not an error condition)
			FUNCTION_EXIT;
			return true;
		}
	}
	catch (const TA_Base_Core::TransactiveException&)
	{
		// The status (broadcast) name list may not be up to date, because
		// failed to collect the name change information
		PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);

		FUNCTION_EXIT;
		return false;
	}


	// Check if the target index already exists in combo box
	if (LB_ERR != TA_Base_Bus::SelectionHelper::findStringInComboCaseSensitive(m_statusNameList, to))
	{
		// This happens if the PA agent and local combo box are out of sync
		FUNCTION_EXIT;
		return false;
	}

	if (0 == from.length())
	{
		TA_ASSERT(to.length() > 0, "Internal error");

		// Source string is empty - this means we're adding to the combo box
		// Perform a final check to confirm we're allowed to display this new status name
		// (this should already have been handled at a higher level)
		if (PaRightsManager::getInstance().hasRightToDisplayBroadcast(
			to,
			CachedConfig::getInstance()->getAgentObject()))
		{
			m_statusNameList.AddString(to.c_str());
		}

		FUNCTION_EXIT;
		return true;
	}

	int nIndex = TA_Base_Bus::SelectionHelper::findStringInComboCaseSensitive(m_statusNameList, from);

	if (LB_ERR == nIndex)
	{
		// The 'from' field wasn't found in the list, we must be a little out of sync with
		// the PA Agent - ignore this update, but refresh combo list with latest items
		FUNCTION_EXIT;
		return false;
	}

	// Set a flag to indicate if the current selection corresponds to item being modified
	bool removedCurrentlySelectedItem = (m_statusNameList.GetCurSel() == nIndex);

	if (m_statusNameList.GetDroppedState() &&
		1 == m_statusNameList.GetCount() &&
		0 == to.length())
	{
		// Clear dropdown list just before removing the final string
		// To fix TestDirector Defect #2274
		m_statusNameList.ShowDropDown(FALSE);
	}

	// From item was found, determine whether we need to rename or delete an item from
	// the list - when we delete an item that's currently selected, the selection
	// is automatically cleared
	m_statusNameList.DeleteString(nIndex);

	if (0 == to.length())
	{
		// We removed the item, but nothing to replace it with - ensure the current selection is cleared
		if (removedCurrentlySelectedItem)
		{
			m_statusNameList.SetCurSel(-1);
		}
	}
	else
	{
		// Insert in the new string - it's position will depend on alphabetical ordering...
		nIndex = m_statusNameList.AddString(to.c_str());

		// Ensure newly added item is selected - but only if we just removed an item
		// that was currently selected
		if (removedCurrentlySelectedItem)
		{
			int confirm = m_statusNameList.SetCurSel(nIndex);

			TA_ASSERT(confirm == nIndex, "Internal error");
		}
	}

	FUNCTION_EXIT;
	return true;
}

CString TrainBroadcastPage::getSelectedDvaMessageText()
{
	FUNCTION_ENTRY( "getSelectedDvaMessageText" );

	std::vector<unsigned int> selection =
		TA_Base_Bus::SelectionHelper::getSelectedItemIndices(m_dvaMessageList);

	TA_ASSERT(selection.size() <= 1, "Multiple selections not allowed");

	if (0 != selection.size())
	{
		FUNCTION_EXIT;
		return m_dvaMessageList.GetItemText(selection[0], 0);
	}

	FUNCTION_EXIT;
	return "";
}

void TrainBroadcastPage::updateTrainLiveStaticGroupNames( const std::vector<std::string>& trainListNames )
{
    FUNCTION_ENTRY( "updateTrainLiveStaticGroupNames" );

    m_staticGroupCombo.updateGroupNames( trainListNames );

    FUNCTION_EXIT;
}
// TD 15349

