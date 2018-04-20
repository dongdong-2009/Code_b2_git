/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainDisplayPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The train display page.
  *
  */

#include "stdafx.h"

#include "DisplayAttributeManager.h"
#include "DisplayRequestManager.h"
#include "PidSelectionManager.h"
#include "TTISManager.h"
#include "TTISMessageManager.h"
#include "TTISPredefinedMessages.h"
#include "TrainDisplayPage.h"
#include "TrainSelectFreeTextPage.h"
#include "TrainSelectPreDefinedPage.h"

#include "app/signs/ttis_manager/src/RightsManager.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include <iomanip>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_IRS_Bus::TrainSelector;


namespace TA_IRS_App
{

    using TA_IRS_Bus::TrainSelectorGUI;
    using TA_IRS_Bus::MFCTrainSelectorGUI;
    using TA_IRS_Bus::LocationCache;
    using TA_Base_Bus::TISAgentAccessFactory;

	CTrainDisplayPage::CTrainDisplayPage(CWnd* pParent /*=NULL*/)
		: CDialog(CTrainDisplayPage::IDD, pParent),
		  m_trainSelectorGUI(NULL),
		  m_mfcTrainSelector(NULL),
		  m_trainSelector(NULL),
		  m_pidSelectionManager(NULL),
		  m_displayAttributeManager(NULL),
		  m_displayRequestManager(NULL),
		  m_trainSelected(false),
		  m_messageSelected(false),
          m_pidSelected(false),
          m_canSendFreeTextDisplayRequest(false),
          m_canSendPredefinedDisplayRequest(false),
          m_canSendClearRequest(false)
    {
        FUNCTION_ENTRY( "CTrainDisplayPage" );

        //{{AFX_DATA_INIT(CTrainDisplayPage)
        //}}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CTrainDisplayPage)
        DDX_Control(pDX, IDC_TRAIN_SELECTION_TAB, m_trainSelectionTab);
        DDX_Control(pDX, IDC_INBOUND_LABEL, m_inboundLabel);
        DDX_Control(pDX, IDC_OUTBOUND_LABEL, m_outboundLabel);
        DDX_Control(pDX, IDC_OUTBOUND_LIST, m_outboundTrainList);
        DDX_Control(pDX, IDC_INBOUND_LIST, m_inboundTrainList);
        DDX_Control(pDX, IDC_CLEAR_ALL_TRAINS, m_clearAllTrains);
        DDX_Control(pDX, IDC_ALL_TRAINS, m_selectAllTrains);
        DDX_Control(pDX, IDC_ALL_INBOUND, m_allInboundTrains);
        DDX_Control(pDX, IDC_ALL_OUTBOUND, m_allOutboundTrains);
        DDX_Control(pDX, IDC_GROUP_SELECTION, m_trainGroupCombo);
        DDX_Control(pDX, IDC_SAVE_TRAIN_GROUP, m_saveTrainGroup);
        DDX_Control(pDX, IDC_DELETE_TRAIN_GROUP, m_deleteTrainGroup);
        DDX_Control(pDX, IDC_CLEAR_ALL_MESSAGES, m_clearAllMessages);
        DDX_Control(pDX, IDC_CLEAR_EMERGENCY_MESSAGES, m_clearEmergencyMessages);
        DDX_Control(pDX, IDC_CLEAR_NORMAL_MESSAGES, m_clearNormalMessages);
        DDX_Control(pDX, IDC_CLEAR_PRIORITY6_MESSAGES, m_clearPriority6);
        DDX_Control(pDX, IDDISPLAY, m_display);
        DDX_Control(pDX, IDC_ENTIRE_TRAIN, m_selectEntireTrain);
        DDX_Control(pDX, IDC_CLEAR_PID_SELECTION, m_clearPids);
        DDX_Control(pDX, IDC_ALL_CAR3, m_allCar3);
        DDX_Control(pDX, IDC_ALL_CAR2, m_allCar2);
        DDX_Control(pDX, IDC_ALL_CAR1, m_allCar1);
        DDX_Control(pDX, IDC_CAR3PID4, m_car3PID4);
        DDX_Control(pDX, IDC_CAR3PID3, m_car3PID3);
        DDX_Control(pDX, IDC_CAR3PID2, m_car3PID2);
        DDX_Control(pDX, IDC_CAR3PID1, m_car3PID1);
        DDX_Control(pDX, IDC_CAR2PID4, m_car2PID4);
        DDX_Control(pDX, IDC_CAR2PID3, m_car2PID3);
        DDX_Control(pDX, IDC_CAR2PID2, m_car2PID2);
        DDX_Control(pDX, IDC_CAR2PID1, m_car2PID1);
        DDX_Control(pDX, IDC_CAR1PID4, m_car1PID4);
        DDX_Control(pDX, IDC_CAR1PID3, m_car1PID3);
        DDX_Control(pDX, IDC_CAR1PID2, m_car1PID2);
        DDX_Control(pDX, IDC_CAR1PID1, m_car1PID1);
        DDX_Control(pDX, IDC_PRIORITY, m_priority);
        DDX_Control(pDX, IDC_TIME_TYPE_CONTINUOUS, m_timeTypeContinuous);
        DDX_Control(pDX, IDC_TIME_TYPE_CYCLIC, m_timeTypeCyclic);
        DDX_Control(pDX, IDC_END_TIME, m_endTime);
        DDX_Control(pDX, IDC_REPEAT_INTERVAL, m_repeatInterval);
        DDX_Control(pDX, IDC_START_TIME, m_startTime);
        DDX_Control(pDX, IDC_LABEL_START, m_endTimeLabel);
        DDX_Control(pDX, IDC_LABEL_END, m_startTimeLabel);
        DDX_Control(pDX, IDC_LABEL_REPEAT, m_repeatIntervalLabel);
        DDX_Control(pDX, IDC_TRAIN_DISPLAY_TAB, m_trainDisplayTab);
        //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CTrainDisplayPage, CDialog)
        //{{AFX_MSG_MAP(CTrainDisplayPage)
        ON_BN_CLICKED(IDC_ALL_CAR1, OnAllCar1)
        ON_BN_CLICKED(IDC_ALL_CAR2, OnAllCar2)
        ON_BN_CLICKED(IDC_ALL_CAR3, OnAllCar3)
        ON_BN_CLICKED(IDC_CAR1PID1, OnCar1PID1Clicked)
        ON_BN_CLICKED(IDC_ENTIRE_TRAIN, OnEntireTrain)
        ON_BN_CLICKED(IDC_CLEAR_PID_SELECTION, OnClearPidSelection)
        ON_BN_CLICKED(IDDISPLAY, OnDisplay)
        ON_BN_CLICKED(IDC_CLEAR_ALL_MESSAGES, OnClearAll)
        ON_BN_CLICKED(IDC_CLEAR_PRIORITY6_MESSAGES, OnClearPriority6)
        ON_BN_CLICKED(IDC_CLEAR_NORMAL_MESSAGES, OnClearNormal)
        ON_BN_CLICKED(IDC_CLEAR_EMERGENCY_MESSAGES, OnClearEmergency)
        ON_BN_CLICKED(IDC_TIME_TYPE_CONTINUOUS, onTimeTypeContinuous)
        ON_BN_CLICKED(IDC_TIME_TYPE_CYCLIC, onTimeTypeCyclic)
        ON_WM_DESTROY()
        ON_MESSAGE(WM_UPDATEINBOUND, onUpdateInboundList)
        ON_MESSAGE(WM_UPDATEOUTBOUND, onUpdateOutboundList)
        ON_MESSAGE(WM_UPDATESELECTIONCOMBO, onUpdateSelectionCombo)
        ON_BN_CLICKED(IDC_ALL_INBOUND, onAllInbound)
        ON_BN_CLICKED(IDC_ALL_OUTBOUND, onAllOutbound)
        ON_BN_CLICKED(IDC_ALL_TRAINS, onAllTrains)
        ON_BN_CLICKED(IDC_CLEAR_ALL_TRAINS, onClearAllTrains)
        ON_BN_CLICKED(IDC_DELETE_TRAIN_GROUP, onDeleteTrainGroup)
        ON_BN_CLICKED(IDC_SAVE_TRAIN_GROUP, onSaveTrainGroup)
        ON_NOTIFY(TCN_SELCHANGE, IDC_TRAIN_SELECTION_TAB, onSelchangeTrainSelectionTab)
        ON_BN_CLICKED(IDC_CAR1PID2, OnCar1PID2Clicked)
        ON_BN_CLICKED(IDC_CAR1PID3, OnCar1PID3Clicked)
        ON_BN_CLICKED(IDC_CAR1PID4, OnCar1PID4Clicked)
        ON_BN_CLICKED(IDC_CAR2PID1, OnCar2PID1Clicked)
        ON_BN_CLICKED(IDC_CAR2PID2, OnCar2PID2Clicked)
        ON_BN_CLICKED(IDC_CAR2PID3, OnCar2PID3Clicked)
        ON_BN_CLICKED(IDC_CAR2PID4, OnCar2PID4Clicked)
        ON_BN_CLICKED(IDC_CAR3PID1, OnCar3PID1Clicked)
        ON_BN_CLICKED(IDC_CAR3PID2, OnCar3PID2Clicked)
        ON_BN_CLICKED(IDC_CAR3PID3, OnCar3PID3Clicked)
        ON_BN_CLICKED(IDC_CAR3PID4, OnCar3PID4Clicked)
        ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CTrainDisplayPage message handlers

    BOOL CTrainDisplayPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        //
        // Create the object that handles the display buttons
        //

        m_displayRequestManager = new DisplayRequestManager( m_clearAllMessages,
                                                             m_clearEmergencyMessages,
                                                             m_clearNormalMessages,
                                                             m_clearPriority6,
                                                             m_display);


        this->m_car1PID1.SetCheckedColour( 0x00FFFFFF );
        this->m_car1PID2.SetCheckedColour( 0x00FFFFFF );
        this->m_car1PID3.SetCheckedColour( 0x00FFFFFF );
        this->m_car1PID4.SetCheckedColour( 0x00FFFFFF );
        this->m_car2PID1.SetCheckedColour( 0x00FFFFFF );
        this->m_car2PID2.SetCheckedColour( 0x00FFFFFF );
        this->m_car2PID3.SetCheckedColour( 0x00FFFFFF );
        this->m_car2PID4.SetCheckedColour( 0x00FFFFFF );
        this->m_car3PID1.SetCheckedColour( 0x00FFFFFF );
        this->m_car3PID2.SetCheckedColour( 0x00FFFFFF );
        this->m_car3PID3.SetCheckedColour( 0x00FFFFFF );
        this->m_car3PID4.SetCheckedColour( 0x00FFFFFF );
        // TES799 ~ added (start)
        this->m_allCar1.SetCheckedColour( 0x00FFFFFF );
        this->m_allCar2.SetCheckedColour( 0x00FFFFFF );
        this->m_allCar3.SetCheckedColour( 0x00FFFFFF );
        // TES799 ~ added (end)

        //
        // Create the object that manages PID selection
        //

        // build the PID button list
        std::vector<ToggleButton*> pidButtons;
        pidButtons.push_back(&m_car1PID1);
        pidButtons.push_back(&m_car1PID2);
        pidButtons.push_back(&m_car1PID3);
        pidButtons.push_back(&m_car1PID4);
        pidButtons.push_back(&m_car2PID1);
        pidButtons.push_back(&m_car2PID2);
        pidButtons.push_back(&m_car2PID3);
        pidButtons.push_back(&m_car2PID4);
        pidButtons.push_back(&m_car3PID1);
        pidButtons.push_back(&m_car3PID2);
        pidButtons.push_back(&m_car3PID3);
        pidButtons.push_back(&m_car3PID4);

        std::vector<ToggleButton*> allCarButtons;   // TES799 ~ added
        allCarButtons.push_back(&m_allCar1);
        allCarButtons.push_back(&m_allCar2);
        allCarButtons.push_back(&m_allCar3);

        m_pidSelectionManager = new PidSelectionManager( *this,
                                                         m_selectEntireTrain,
                                                         m_clearPids,
                                                         allCarButtons,
                                                         pidButtons );

        //
        // Create the object that manages Display attributes
        //
        m_displayAttributeManager = new DisplayAttributeManager( m_priority,
                                                                  m_timeTypeContinuous,
                                                                 m_timeTypeCyclic,
                                                                 m_startTime,
                                                                 m_endTime,
                                                                 m_repeatInterval,
                                                                 m_startTimeLabel,
                                                                 m_endTimeLabel,
                                                                 m_repeatIntervalLabel);

        FUNCTION_EXIT;
        return TRUE;
    }


    void CTrainDisplayPage::Init(REBProgressManager & mgr)
    {
        FUNCTION_ENTRY( "Init" );

        // check rights
        mgr.SetStaticText(0, "Initializing the Train Display Page: get rights");
        m_canSendFreeTextDisplayRequest = RightsManager::getInstance().canDisplayFreeText();
        m_canSendPredefinedDisplayRequest = RightsManager::getInstance().canDisplayPredefined();
        m_canSendClearRequest = RightsManager::getInstance().canClear();
        mgr.SetProgress(10);

        // Set up the train selection componenet
        mgr.SetStaticText(0, "Initializing the Train Display Page: set up the Train Selection Component");

        setupTrainSelectionComponent();

        mgr.SetProgress(20);

        // register for rights changes
        RightsManager::getInstance().registerForRightsChanges(this);

        m_trainDisplayTab.setMessageSelectionListener(this);

        m_trainDisplayTab.Init();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        CDialog::OnDestroy();

        // deregister for rights changes
        RightsManager::getInstance().deregisterForRightsChanges(this);

        //
        // clean up all objects on the heap
        //

        cleanpTrainSelectionComponent();

        // PID selection
        if (m_pidSelectionManager != NULL)
        {
            delete m_pidSelectionManager;
            m_pidSelectionManager = NULL;
        }

        // display attributes
        if (m_displayAttributeManager != NULL)
        {
            delete m_displayAttributeManager;
            m_displayAttributeManager = NULL;
        }


        // display request
        if (m_displayRequestManager != NULL)
        {
            delete m_displayRequestManager;
            m_displayRequestManager = NULL;
        }

        FUNCTION_EXIT;
    }


    /* Train selection stuff */


    void CTrainDisplayPage::setupTrainSelectionComponent()
    {
        FUNCTION_ENTRY( "setupTrainSelectionComponent" );

        m_inboundTrainList.setStandardColumnSizes(60, 125);
        m_outboundTrainList.setStandardColumnSizes(60, 125);


        // this does the GUI logic
        m_trainSelectorGUI = new TrainSelectorGUI( m_inboundTrainList,
                                                   &m_outboundTrainList,
                                                   &m_allInboundTrains,
                                                   &m_allOutboundTrains,
                                                   &m_selectAllTrains,
                                                   &m_clearAllTrains,
                                                   &m_inboundLabel,
                                                   &m_outboundLabel,
                                                   &m_trainGroupCombo,
                                                   &m_saveTrainGroup,
                                                   &m_deleteTrainGroup,
                                                   NULL,
                                                   this );

        // this receives the updates and posts them - to resolve threading issues
        m_mfcTrainSelector = new MFCTrainSelectorGUI( this,
                                                      WM_UPDATEINBOUND,
                                                      WM_UPDATEOUTBOUND,
                                                      WM_UPDATESELECTIONCOMBO );

        // this sets the rights
        bool canModifyGroups = RightsManager::getInstance().canModifyTrainGroups();
        m_trainSelectorGUI->setRights( true, // train lists always enabled
                                       true,
                                       canModifyGroups,
                                       canModifyGroups);


        // firstly - what sort of location is this
        TA_IRS_Bus::LocationCache::EStationType stationType =
            TA_IRS_Bus::LocationCache::getInstance().getAppropriateStationMode();

        // This actually gets the updates
        try
        {
            // the train selection component
            m_trainSelector = new TrainSelector( *m_mfcTrainSelector,
                                                 TA_IRS_Bus::LocationCache::getInstance().getLocationKey(),
                                                 stationType );

            m_trainSelectorGUI->setTrainSelector( m_trainSelector );
        }
        catch (const TA_Base_Core::ObjectResolutionException& ore)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what());

            TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::NO_TRAIN_AGENT_NO_TRAIN_LISTS,
                                                               TTISMessageManager::NO_TRAIN_AGENT_TRAIN_LISTS_UNAVAILABLE);
        }
        catch( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception caught while intialising train updates: " );
                exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::NO_TRAIN_AGENT_NO_TRAIN_LISTS,
                                                               TTISMessageManager::NO_TRAIN_AGENT_TRAIN_LISTS_UNAVAILABLE);
        }
        catch (const TA_Base_Core::TransactiveException& te)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what());

            TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::NO_TRAIN_AGENT_NO_TRAIN_LISTS,
                                                               TTISMessageManager::NO_TRAIN_AGENT_TRAIN_LISTS_UNAVAILABLE);
        }


        // add tabs
        m_trainSelectionTab.InsertItem(0, "Mainline");
        m_trainSelectionTab.InsertItem(1, "Depot");

        // set it so that disabled tabs can be greyed
        m_trainSelectionTab.ModifyStyle(0, TCS_OWNERDRAWFIXED);

        // if this is at a station, disable the depot tab
        // if at the depot, disable the mainline tab
        // if this is the occ, dont disable any tabs
        if (stationType == TA_IRS_Bus::LocationCache::Depot)
        {
            m_trainSelectionTab.SetActiveTab(1);
            m_trainSelectorGUI->setDepotMode();
            m_trainSelectionTab.EnableTabIndex(0, false);
            m_trainSelectionTab.EnableTabIndex(1, true);
        }
        else
        {
            // already on mainline tab
            m_trainSelectionTab.EnableTabIndex(0, true);
            m_trainSelectionTab.EnableTabIndex(1, false);
        }

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::cleanpTrainSelectionComponent()
    {
        FUNCTION_ENTRY( "cleanpTrainSelectionComponent" );

        if (m_trainSelectorGUI != NULL)
        {
            m_trainSelectorGUI->setTrainSelector( NULL );
        }

        // train updates
        if (m_trainSelector != NULL)
        {
            delete m_trainSelector;
            m_trainSelector = NULL;
        }

        // train update posting
        if (m_mfcTrainSelector != NULL)
        {
            delete m_mfcTrainSelector;
            m_mfcTrainSelector = NULL;
        }

        // GUI logic
        if (m_trainSelectorGUI != NULL)
        {
            delete m_trainSelectorGUI;
            m_trainSelectorGUI = NULL;
        }

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::trainSelectionChanged(bool trainsAreSelected)
    {
        FUNCTION_ENTRY( "trainSelectionChanged" );

        m_trainSelected = trainsAreSelected;
        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");

        bool canDisplay = false;

        // get the active message page
        CTrainSelectPredefinedPage* predefinedPage =
            dynamic_cast<CTrainSelectPredefinedPage*>( m_trainDisplayTab.getActiveDialog() );
        if (predefinedPage != NULL)
        {
            // PD page active
            canDisplay = m_canSendPredefinedDisplayRequest;
        }
        else
        {
            // free text active
            canDisplay = m_canSendFreeTextDisplayRequest;
        }

        m_displayRequestManager->enableButtons( m_trainSelected,
                                                m_pidSelected,
                                                m_messageSelected,
                                                canDisplay,
                                                m_canSendClearRequest);

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onSelchangeTrainSelectionTab(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onSelchangeTrainSelectionTab" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");

        if (m_trainSelectionTab.GetCurSel()==0)
        {
            // set the GUI to mainline mode
            m_trainSelectorGUI->setMainlineMode();

            // set the train selector to mainline mode
            if (m_trainSelector != NULL)
            {
                TA_IRS_Bus::LocationCache::EStationType stationType =
                    TA_IRS_Bus::LocationCache::getInstance().getAppropriateStationMode();

                // if this is the depot, mainline tab should show in service
                if (stationType == TA_IRS_Bus::LocationCache::Depot)
                {
                    stationType = TA_IRS_Bus::LocationCache::InService;
                }

                m_trainSelector->setStationMode(stationType,
                                                TA_IRS_Bus::LocationCache::getInstance().getLocationKey());
            }
        }
        else if (m_trainSelectionTab.GetCurSel()==1)
        {
            // set the GUI to Depot mode
            m_trainSelectorGUI->setDepotMode();

            // set the train selector to depot mode
            if (m_trainSelector != NULL)
            {
                m_trainSelector->setStationMode(LocationCache::Depot,
                                                TA_IRS_Bus::LocationCache::getInstance().getLocationKey());
            }
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onAllInbound()
    {
        FUNCTION_ENTRY( "onAllInbound" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->selectAllInbound();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onAllOutbound()
    {
        FUNCTION_ENTRY( "onAllOutbound" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->selectAllOutbound();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onAllTrains()
    {
        FUNCTION_ENTRY( "onAllTrains" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->selectAllTrains();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onClearAllTrains()
    {
        FUNCTION_ENTRY( "onClearAllTrains" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->clearSelection();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onSaveTrainGroup()
    {
        FUNCTION_ENTRY( "onSaveTrainGroup" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->saveTrainSelection();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onDeleteTrainGroup()
    {
        FUNCTION_ENTRY( "onDeleteTrainGroup" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->deleteTrainSelection();

        FUNCTION_EXIT;
    }


    LRESULT CTrainDisplayPage::onUpdateInboundList(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onUpdateInboundList" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->updateInboundList(update->action, update->trainDetails);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CTrainDisplayPage::onUpdateOutboundList(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onUpdateOutboundList" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->updateOutboundList(update->action, update->trainDetails);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CTrainDisplayPage::onUpdateSelectionCombo(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onUpdateSelectionCombo" );

        TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate*>(wParam);

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI is NULL");
        m_trainSelectorGUI->updateSelectionCombo(update->trainListNames);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    /* PID selection stuff */

    void CTrainDisplayPage::pidSelectionChanged(bool pidSelectionExists)
    {
        FUNCTION_ENTRY( "pidSelectionChanged" );

        m_pidSelected = pidSelectionExists;
        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");

        bool canDisplay = false;

        // get the active message page
        CTrainSelectPredefinedPage* predefinedPage =
            dynamic_cast<CTrainSelectPredefinedPage*>( m_trainDisplayTab.getActiveDialog() );
        if (predefinedPage != NULL)
        {
            // PD page active
            canDisplay = m_canSendPredefinedDisplayRequest;
        }
        else
        {
            // free text active
            canDisplay = m_canSendFreeTextDisplayRequest;
        }

        m_displayRequestManager->enableButtons( m_trainSelected,
                                                m_pidSelected,
                                                m_messageSelected,
                                                canDisplay,
                                                m_canSendClearRequest);

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnAllCar1()
    {
        FUNCTION_ENTRY( "OnAllCar1" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID Selection Manager is NULL");

        const bool isCheckedOnToggle = !this->m_allCar1.isChecked();

        this->m_allCar1.setCheck(isCheckedOnToggle);

        if (isCheckedOnToggle)
        {
        m_pidSelectionManager->selectAllCar(1);
    }
        else
        {
            m_pidSelectionManager->deselectAllCar(1);
        }

        FUNCTION_EXIT;
    }


    // TES799 ~ modified method
    void CTrainDisplayPage::OnAllCar2()
    {
        FUNCTION_ENTRY( "OnAllCar2" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID Selection Manager is NULL");

        const bool isCheckedOnToggle = !this->m_allCar2.isChecked();

        this->m_allCar2.setCheck(isCheckedOnToggle);

        if (isCheckedOnToggle)
        {
            m_pidSelectionManager->selectAllCar(2);
        }
        else
        {
            m_pidSelectionManager->deselectAllCar(2);
        }

        FUNCTION_EXIT;
    }


    // TES799 ~ modified method
    void CTrainDisplayPage::OnAllCar3()
    {
        FUNCTION_ENTRY( "OnAllCar3" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID Selection Manager is NULL");

        const bool isCheckedOnToggle = !this->m_allCar3.isChecked();

        this->m_allCar3.setCheck(isCheckedOnToggle);

        if (isCheckedOnToggle)
        {
            m_pidSelectionManager->selectAllCar(3);
        }
        else
        {
            m_pidSelectionManager->deselectAllCar(3);
        }

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnEntireTrain()
    {
        FUNCTION_ENTRY( "OnEntireTrain" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID Selection Manager is NULL");
        m_pidSelectionManager->selectAllPids();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnPIDButtonClicked()
    {
        FUNCTION_ENTRY( "OnPIDButtonClicked" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID Selection Manager is NULL");
        m_pidSelectionManager->pidButtonPressed();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar1PID1Clicked()
    {
        FUNCTION_ENTRY( "OnCar1PID1Clicked" );

        this->m_car1PID1.setCheck( !this->m_car1PID1.isChecked() );
        this->updateAllCar1State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar1PID2Clicked()
    {
        FUNCTION_ENTRY( "OnCar1PID2Clicked" );

        this->m_car1PID2.setCheck( !this->m_car1PID2.isChecked() );
        this->updateAllCar1State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar1PID3Clicked()
    {
        FUNCTION_ENTRY( "OnCar1PID3Clicked" );

        this->m_car1PID3.setCheck( !this->m_car1PID3.isChecked() );
        this->updateAllCar1State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar1PID4Clicked()
    {
        FUNCTION_ENTRY( "OnCar1PID4Clicked" );

        this->m_car1PID4.setCheck( !this->m_car1PID4.isChecked() );
        this->updateAllCar1State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar2PID1Clicked()
    {
        FUNCTION_ENTRY( "OnCar2PID1Clicked" );

        this->m_car2PID1.setCheck( !this->m_car2PID1.isChecked() );
        this->updateAllCar2State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar2PID2Clicked()
    {
        FUNCTION_ENTRY( "OnCar2PID2Clicked" );

        this->m_car2PID2.setCheck( !this->m_car2PID2.isChecked() );
        this->updateAllCar2State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar2PID3Clicked()
    {
        FUNCTION_ENTRY( "OnCar2PID3Clicked" );

        this->m_car2PID3.setCheck( !this->m_car2PID3.isChecked() );
        this->updateAllCar2State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar2PID4Clicked()
    {
        FUNCTION_ENTRY( "OnCar2PID4Clicked" );

        this->m_car2PID4.setCheck( !this->m_car2PID4.isChecked() );
        this->updateAllCar2State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar3PID1Clicked()
    {
        FUNCTION_ENTRY( "OnCar3PID1Clicked" );

        this->m_car3PID1.setCheck( !this->m_car3PID1.isChecked() );
        this->updateAllCar3State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar3PID2Clicked()
    {
        FUNCTION_ENTRY( "OnCar3PID2Clicked" );

        this->m_car3PID2.setCheck( !this->m_car3PID2.isChecked() );
        this->updateAllCar3State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar3PID3Clicked()
    {
        FUNCTION_ENTRY( "OnCar3PID3Clicked" );

        this->m_car3PID3.setCheck( !this->m_car3PID3.isChecked() );
        this->updateAllCar3State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCar3PID4Clicked()
    {
        FUNCTION_ENTRY( "OnCar3PID4Clicked" );

        this->m_car3PID4.setCheck( !this->m_car3PID4.isChecked() );
        this->updateAllCar3State(); // TES799 ~ added
        this->OnPIDButtonClicked();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnClearPidSelection()
    {
        FUNCTION_ENTRY( "OnClearPidSelection" );

        TA_ASSERT (m_pidSelectionManager != NULL, "PID Selection Manager is NULL");
        m_pidSelectionManager->clearSelection();

        FUNCTION_EXIT;
    }


    // TES799 ~ added method
    void CTrainDisplayPage::updateAllCar1State()
    {
        FUNCTION_ENTRY( "updateAllCar1State" );

        const bool allCar1PidChecked = m_car1PID1.isChecked() && m_car1PID2.isChecked() &&
                                       m_car1PID3.isChecked() && m_car1PID4.isChecked();

        m_allCar1.setCheck(allCar1PidChecked);

        FUNCTION_EXIT;
    }


    // TES799 ~ added method
    void CTrainDisplayPage::updateAllCar2State()
    {
        FUNCTION_ENTRY( "updateAllCar2State" );

        const bool allCar2PidChecked = m_car2PID1.isChecked() && m_car2PID2.isChecked() &&
                                       m_car2PID3.isChecked() && m_car2PID4.isChecked();

        m_allCar2.setCheck(allCar2PidChecked);

        FUNCTION_EXIT;
    }


    // TES799 ~ added method
    void CTrainDisplayPage::updateAllCar3State()
    {
        FUNCTION_ENTRY( "updateAllCar3State" );

        const bool allCar3PidChecked = m_car3PID1.isChecked() && m_car3PID2.isChecked() &&
                                       m_car3PID3.isChecked() && m_car3PID4.isChecked();

        m_allCar3.setCheck(allCar3PidChecked);

        FUNCTION_EXIT;
    }


    /* Time and priority control related stuff */


    DisplayAttributeManager* CTrainDisplayPage::getDisplayAttributeManager()
    {
        FUNCTION_ENTRY( "getDisplayAttributeManager" );
        FUNCTION_EXIT;
        return m_displayAttributeManager;
    }


    void CTrainDisplayPage::onTimeTypeContinuous()
    {
        FUNCTION_ENTRY( "onTimeTypeContinuous" );

        TA_ASSERT (m_displayAttributeManager != NULL, "Display attribute Manager is NULL");
        m_displayAttributeManager->onTimeTypeContinuous();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::onTimeTypeCyclic()
    {
        FUNCTION_ENTRY( "onTimeTypeCyclic" );

        TA_ASSERT (m_displayAttributeManager != NULL, "Display attribute Manager is NULL");
        m_displayAttributeManager->onTimeTypeCyclic();

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::messageSelectionChanged(bool messageIsSelected)
    {
        FUNCTION_ENTRY( "messageSelectionChanged" );

        m_messageSelected = messageIsSelected;
        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");

        bool canDisplay = false;

        // get the active message page
        CTrainSelectPredefinedPage* predefinedPage =
            dynamic_cast<CTrainSelectPredefinedPage*>( m_trainDisplayTab.getActiveDialog() );
        if (predefinedPage != NULL)
        {
            // PD page active
            canDisplay = m_canSendPredefinedDisplayRequest;
        }
        else
        {
            // free text active
            canDisplay = m_canSendFreeTextDisplayRequest;
        }

        m_displayRequestManager->enableButtons( m_trainSelected,
                                                m_pidSelected,
                                                m_messageSelected,
                                                canDisplay,
                                                m_canSendClearRequest);

        FUNCTION_EXIT;
    }


    /* Display related stuff */

    void CTrainDisplayPage::OnDisplay()
    {
        FUNCTION_ENTRY( "OnDisplay" );

        TA_ASSERT (m_displayAttributeManager != NULL, "Display attribute Manager is NULL");
        TA_ASSERT (m_pidSelectionManager != NULL, "PID Selection Manager is NULL");
        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");
        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI Manager is NULL");

        // get message information (pre-defined or ad-hoc)
        // get the active page on the tab
        CDialog* activeMessageDialog = m_trainDisplayTab.getActiveDialog();

        // find out which page it is that's active
        // (pre-defined or ad-hoc)
        // try downcasting
        CTrainSelectFreeTextPage* freeTextPage = dynamic_cast<CTrainSelectFreeTextPage*>(activeMessageDialog);
        CTrainSelectPredefinedPage* predefinedPage = dynamic_cast<CTrainSelectPredefinedPage*>(activeMessageDialog);

        // if the pre-defined tab is active, and a valid message is selected
        if ( ( predefinedPage != NULL ) &&
              predefinedPage->hasValidSelection() )
        {
            // get the message
            TA_Base_Core::PredefinedMessage message = predefinedPage->getSelectedMessage();
            std::string prompt = TTISMessageManager::ASK_DISPLAY_PREDEFINED + std::string(" ") + message.message;
            int ret = TTISMessageManager::getInstance().askQuestion(prompt.c_str());
            if (ret == IDYES)
            {
                m_displayRequestManager->onDisplay( *m_pidSelectionManager,
                                                    *m_trainSelectorGUI,
                                                    *m_displayAttributeManager,
                                                    message );
            }
        }
        // otherwise if the free text page is selected and has a message entered
        else if ( ( freeTextPage != NULL ) &&
                  freeTextPage->hasValidMessage() )
        {
            // get the message
            CTrainSelectFreeTextPage::AdHocMessage message = freeTextPage->getMessage();
            std::string prompt = TTISMessageManager::ASK_DISPLAY_ADHOC + std::string(" ") + message.messageContent;
            int ret = TTISMessageManager::getInstance().askQuestion(prompt.c_str());
            if (ret == IDYES)
            {
                m_displayRequestManager->onDisplay( *m_pidSelectionManager,
                                                    *m_trainSelectorGUI,
                                                    *m_displayAttributeManager,
                                                    message );
            }
        }
        else
        {
            TA_ASSERT(true, "The display button was active without a message selected");
        }

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnClearAll()
    {
        FUNCTION_ENTRY( "OnClearAll" );

        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_SUBMIT_CLEAR_ALL);
        if (ret == IDYES)
        {
            m_displayRequestManager->onClear( TA_Base_Core::CLEAR_ALL,
                                              *m_pidSelectionManager,
                                              *m_trainSelectorGUI );
        }

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnClearPriority6()
    {
        FUNCTION_ENTRY( "OnClearPriority6" );

        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_SUBMIT_CLEAR_6);
        if (ret == IDYES)
        {
            m_displayRequestManager->onClear( TA_Base_Core::CLEAR_PRIORITY6,
                                              *m_pidSelectionManager,
                                              *m_trainSelectorGUI );
        }

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnClearNormal()
    {
        FUNCTION_ENTRY( "OnClearNormal" );

        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_SUBMIT_CLEAR_NORMAL);
        if (ret == IDYES)
        {
            m_displayRequestManager->onClear( TA_Base_Core::CLEAR_NORMAL,
                                              *m_pidSelectionManager,
                                              *m_trainSelectorGUI );
        }

        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnClearEmergency()
    {
        FUNCTION_ENTRY( "OnClearEmergency" );

        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_SUBMIT_CLEAR_EMERGENCY);
        if (ret == IDYES)
        {
            m_displayRequestManager->onClear( TA_Base_Core::CLEAR_EMERGENCY,
                                              *m_pidSelectionManager,
                                              *m_trainSelectorGUI );
        }

        FUNCTION_EXIT;
    }


    /* Other callbacks */

    LRESULT CTrainDisplayPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onRightsChanged" );

        TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selector GUI Manager is NULL");
        TA_ASSERT (m_displayRequestManager != NULL, "Display Request Manager is NULL");

        // the rights have changed - re check them
        m_canSendFreeTextDisplayRequest = RightsManager::getInstance().canDisplayFreeText();
        m_canSendPredefinedDisplayRequest = RightsManager::getInstance().canDisplayPredefined();
        m_canSendClearRequest = RightsManager::getInstance().canClear();
        bool canModifyGroups = RightsManager::getInstance().canModifyTrainGroups();

        m_trainSelectorGUI->setRights( true, // train lists always enabled
                                       true,
                                       canModifyGroups,
                                       canModifyGroups);


        bool canDisplay = false;
        // get the active message page
        CTrainSelectPredefinedPage* predefinedPage =
            dynamic_cast<CTrainSelectPredefinedPage*>( m_trainDisplayTab.getActiveDialog() );
        if (predefinedPage != NULL)
        {
            // PD page active
            canDisplay = m_canSendPredefinedDisplayRequest;
        }
        else
        {
            // free text active
            canDisplay = m_canSendFreeTextDisplayRequest;
        }

        m_displayRequestManager->enableButtons( m_trainSelected,
                                                m_pidSelected,
                                                m_messageSelected,
                                                canDisplay,
                                                m_canSendClearRequest);

        FUNCTION_EXIT;
        return 0;
    }


    void CTrainDisplayPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void CTrainDisplayPage::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );
        FUNCTION_EXIT;
    }


    BOOL CTrainDisplayPage::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        switch ( pMsg->message )
        {
        case WM_KEYDOWN:
            {
                switch ( pMsg->wParam )
                {
                case VK_ESCAPE:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                case VK_RETURN:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                break;
                default:
                    {
                        //do nothing
                    }
                    break;
                }
            }
            break;
        default:
            {
                //do nothing
            }
        }

        FUNCTION_EXIT;
        return CDialog::PreTranslateMessage(pMsg);
    }


    //TD 15349
    //zhou yuan++
    bool CTrainDisplayPage::findAndSelectStationMessage(const std::string& messageName)
    {
        FUNCTION_ENTRY( "findAndSelectStationMessage" );
        FUNCTION_EXIT;
        return m_trainDisplayTab.findAndSelectStationMessage(messageName);
    }


    //++zhou yuan

} // TA_IRS_App
