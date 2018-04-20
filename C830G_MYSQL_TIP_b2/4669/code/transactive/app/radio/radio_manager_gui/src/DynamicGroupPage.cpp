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
// DynamicGroupPage.cpp : implementation file
//

#pragma warning ( disable : 4786 )

#include "app/radio/radio_manager_gui/src/stdafx.h"

#include "app/radio/radio_manager_gui/src/Resource.h"

#include "app/radio/radio_manager_gui/src/DynamicGroupPage.h"
#include "app/radio/radio_manager_gui/src/RadioManagerDlg.h"
#include "app/radio/radio_manager_gui/src/RmLayoutHelper.h"
#include "app/radio/radio_manager_gui/src/SearchPageHelper.h"
#include "app/radio/radio_manager_gui/src/radiomanager.h"

#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/RunParams.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Forward declaration
void selectTrains(CListCtrl& listCtrl, bool isSelected);

/////////////////////////////////////////////////////////////////////////////
// CDynamicGroupPage dialog

static const DWORD COL_ID   = 0;
static const DWORD COL_LOC  = 1;
static const DWORD COL_TSI  = 2;

CDynamicGroupPage::CDynamicGroupPage(CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicGroupPage::IDD, pParent),
      m_dynamicGroupPageEnabled(false),
      m_makeTrainCallEnabled(false),
      m_trainsAreSelected(false),
      m_trainSelectorGUI(NULL),
      m_trainSelectorSet(false)
{
    FUNCTION_ENTRY( "CDynamicGroupPage" );

    //{{AFX_DATA_INIT(CDynamicGroupPage)
    //}}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CDynamicGroupPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDynamicGroupPage)
    DDX_Control(pDX, IDC_LOCATION_TAB, m_locationTab);
    DDX_Control(pDX, IDC_REMOVE, m_removeButton);
    DDX_Control(pDX, IDC_RESET, m_resetButton);
    DDX_Control(pDX, IDC_ADD, m_addButton);
    DDX_Control(pDX, IDC_TRAINLIST, m_inboundTrainList);
    DDX_Control(pDX, IDC_GROUPLIST, m_groupList);
    DDX_Control(pDX, IDC_COMBO1, m_selectionName);
    DDX_Control(pDX, IDC_TRAINLIST2, m_outboundTrainList);
    DDX_Control(pDX, IDC_INBOUND_LABEL, m_inboundLabel);
    DDX_Control(pDX, IDC_OUTBOUND_LABEL, m_outboundLabel);
    DDX_Control(pDX, IDC_CLEAR_ALL, m_clearAllTrains);
    DDX_Control(pDX, IDC_ALL_TRAINS, m_selectAllTrains);
    DDX_Control(pDX, IDC_ALL_INBOUND, m_allInboundTrains);
    DDX_Control(pDX, IDC_ALL_OUTBOUND, m_allOutboundTrains);
    DDX_Control(pDX, IDC_SAVETRAINSELECTION, m_saveTrainGroup);
    DDX_Control(pDX, IDC_DELETETRAINSELECTION2, m_deleteTrainGroup);
    //}}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CDynamicGroupPage, CDialog)
	//{{AFX_MSG_MAP(CDynamicGroupPage)
    ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SAVETRAINSELECTION, OnSavetrainselection)
	ON_BN_CLICKED(IDC_DELETETRAINSELECTION2, OnDeletetrainselection)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_TRAINLIST, OnDblclkTrainList)
    ON_NOTIFY(NM_DBLCLK, IDC_TRAINLIST2, OnDblclkTrainList)
    ON_NOTIFY(NM_CLICK, IDC_GROUPLIST, onClickGroupList)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_GROUPLIST, onItemChangedGroupList)
	ON_BN_CLICKED(IDC_ALL_TRAINS, onAllTrains)
	ON_BN_CLICKED(IDC_ALL_INBOUND, onAllInboundTrains)
	ON_BN_CLICKED(IDC_ALL_OUTBOUND, onAllOutboundTrains)
	ON_BN_CLICKED(IDC_CLEAR_ALL, onClearAllTrains)
	ON_NOTIFY(TCN_SELCHANGE, IDC_LOCATION_TAB, onSelChangeLocationTab)
	//}}AFX_MSG_MAP
    ON_MESSAGE(TRAIN_INBOUND_MSG, OnInboundTrain)
    ON_MESSAGE(TRAIN_OUTBOUND_MSG, OnOutboundTrain)
    ON_MESSAGE(TRAIN_UPDATE_SELECTION_MSG, OnComboChanged)
    ON_MESSAGE(ADD_DYNAMIC_GROUP_MEMBERS, OnAddGroupMember)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicGroupPage message handlers

BOOL CDynamicGroupPage::OnInitDialog() 
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // Setup headings
    m_groupList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
    m_groupList.InsertColumn(0,"ID",LVCFMT_LEFT,250);
    m_groupList.InsertColumn(1,"Loc",LVCFMT_LEFT,40);
    m_groupList.InsertColumn(2,"TSI",LVCFMT_LEFT);

    // TES #422 - Provide default formatting
    TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(m_groupList);


    //addTestData();
    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDynamicGroupPage::Init()
{
    FUNCTION_ENTRY( "Init" );
    //
    // train selection component
    //
    try
    {
        setupTrainSelectionComponent();
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::OnInitDialog");

	// Enable buttons
	onUpdateRights();
    FUNCTION_EXIT;
}


void CDynamicGroupPage::setupTrainSelectionComponent()
{
    FUNCTION_ENTRY( "setupTrainSelectionComponent" );

    //TD 4614
    //zhou yuan ++
    // column sizes
    m_inboundTrainList.setStandardColumnSizes(60, 115, 89);
    m_outboundTrainList.setStandardColumnSizes(60, 115, 89);
    //++zhou yuan

    // this does the GUI logic
    m_trainSelectorGUI = new TA_IRS_Bus::TrainSelectorGUI( m_inboundTrainList,  // pass all controls
                                                       &m_outboundTrainList,
                                                       &m_allInboundTrains,
                                                       &m_allOutboundTrains,
                                                       &m_selectAllTrains,
                                                       &m_clearAllTrains,
                                                       &m_inboundLabel,
                                                       &m_outboundLabel,
                                                       &m_selectionName,
                                                       &m_saveTrainGroup,
                                                       &m_deleteTrainGroup,
                                                       NULL,
                                                       this );  // this listens for train selection changes
    //TD 4614
    //zhou yuan ++
    //remove it because we have already set the column size
    // ++AZ TD 4896
//    m_inboundTrainList.setAutoSizeColumn(2);
//    m_outboundTrainList.setAutoSizeColumn(2);
    // AZ++
    //++zhou yuan

    // set up the tabs
    m_locationTab.InsertItem(0,"Mainline");
    m_locationTab.InsertItem(1,"Depot");

    // set it so that disabled tabs can be greyed
    m_locationTab.ModifyStyle(0, TCS_OWNERDRAWFIXED);

    // based on the current location, set the active tab, and possibly disable the other
    // firstly - what sort of location is this
    TA_IRS_Bus::LocationCache::EStationType stationMode =
        TA_IRS_Bus::LocationCache::getInstance().getAppropriateStationMode();

    // for ease of debugging - have it so you can enable all train selection tabs
    if ( !TA_Base_Core::RunParams::getInstance().isSet("EnableAllTabs") &&
         (stationMode != TA_IRS_Bus::LocationCache::InService))
    {
        // if this is at a station, disable the depot tab
        // if at the depot, disable the mainline tab
        // if this is the occ, dont disable any tabs
        if (stationMode == TA_IRS_Bus::LocationCache::Depot)
        {
            m_locationTab.SetActiveTab(1);
            m_trainSelectorGUI->setDepotMode();
            m_locationTab.EnableTabIndex(0, false);

            // the train selection component should already be in depot mode
        }
        else
        {
            // already on mainline tab
            m_locationTab.EnableTabIndex(1, false);
        }
    }
    else if (stationMode == TA_IRS_Bus::LocationCache::InService) //TD19302 ++
    {
        m_locationTab.SetActiveTab(0);
        m_trainSelectorGUI->setMainlineMode();
        m_locationTab.EnableTabIndex(1, false);
    }//++ TD19302

    // Get the parent class and retrieve the reference to the radio manager
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

    // Register to retrieve train information
    manager->getTrainSelectorGUI().addCallbackWindow(this);

    // this will allow the train selection component to do its own dynamic group handling
    // this cannot be set here - the main dialog hasnt created it yet!
    // this will be set the first time a selection update is received
    //m_trainSelectorGUI->setTrainSelector( &(manager->getTrainSelector()) );
    m_trainSelectorSet = false;

    FUNCTION_EXIT;
}


void CDynamicGroupPage::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Removing call back window from Train Selector GUI");
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
	manager->getTrainSelectorGUI().removeCallbackWindow(this);

    CDialog::OnDestroy();

    //
    // clean up all objects on the heap
    //
    if (m_trainSelectorGUI != NULL)
    {
        delete m_trainSelectorGUI;
        m_trainSelectorGUI = NULL;
    }

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onUpdateRights()
{
    FUNCTION_ENTRY( "onUpdateRights" );

    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

    // check rights - store those needed when train selection is changed
    m_dynamicGroupPageEnabled = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_DYNAMIC_GROUP);
    m_makeTrainCallEnabled = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_MAKE_TRAIN_CALL);

    // this is not needed anywhere else in this object
    bool enableTrainGroups = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_MANAGE_TRAIN_SELECTIONS);

    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
            "Dynamic Group Page UpdateRight: \t aca_RADIO_DYNAMIC_GROUP = %d, \t aca_RADIO_MAKE_TRAIN_CALL = %d, \t aca_RADIO_MANAGE_TRAIN_SELECTIONS = %d",
            m_dynamicGroupPageEnabled, m_makeTrainCallEnabled, enableTrainGroups);

    TA_ASSERT(m_trainSelectorGUI != NULL, "Train selection component is NULL");

    try
    {
        // update the rights in the train selection component
        m_trainSelectorGUI->setRights( m_dynamicGroupPageEnabled,   // train lists
                                    enableTrainGroups,           // train group combo
                                    enableTrainGroups,           // save train groups
                                    enableTrainGroups);          // delete train groups
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::onUpdateRights");

    // update the status of the buttons that the train selection component doesnt control
    enableButtons();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::trainSelectionChanged(bool trainsAreSelected)
{
    FUNCTION_ENTRY( "trainSelectionChanged" );

    m_trainsAreSelected = trainsAreSelected;

    enableButtons();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::enableButtons()
{
    FUNCTION_ENTRY( "enableButtons" );

    // if the page is enabled and there are selected items in the group list
    BOOL enable = m_dynamicGroupPageEnabled && (m_groupList.GetItemCount() > 0);
    m_resetButton.EnableWindow( enable );

    enable = enable && (m_groupList.GetSelectedCount() > 0);
    m_removeButton.EnableWindow( enable );

    // Enable train selection buttons
    enable = m_dynamicGroupPageEnabled && m_trainsAreSelected && m_makeTrainCallEnabled;
    m_addButton.EnableWindow( enable );

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onSelChangeLocationTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onSelChangeLocationTab" );

    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");

    try
    {
        if (m_locationTab.GetCurSel()==0)
        {
            // set the GUI to mainline mode
            m_trainSelectorGUI->setMainlineMode();

            manager->resetStationMode();
        }
        else if (m_locationTab.GetCurSel()==1)
        {
            // set the GUI to Depot mode
            m_trainSelectorGUI->setDepotMode();

            manager->setDepotMode();
        }
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::onSelChangeLocationTab");

    *pResult = 0;

    FUNCTION_EXIT;
}


LRESULT CDynamicGroupPage::OnInboundTrain(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnInboundTrain" );

    try
    {
        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* trainListUpdate =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        if (NULL != trainListUpdate)
        {
            TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");

            m_trainSelectorGUI->updateInboundList(trainListUpdate->action,
                                                  trainListUpdate->trainDetails);

            // prevent memory leaks - every window gets its own copy
            delete trainListUpdate;
        }
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::OnInboundTrain");

    FUNCTION_EXIT;
    return 0;
}


LRESULT CDynamicGroupPage::OnOutboundTrain(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnOutboundTrain" );

    try
    {
        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* trainListUpdate =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        if (NULL != trainListUpdate)
        {
            TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");

            m_trainSelectorGUI->updateOutboundList(trainListUpdate->action,
                                                   trainListUpdate->trainDetails);

            // prevent memory leaks - every window gets its own copy
            delete trainListUpdate;
        }
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::OnOutboundTrain");

    FUNCTION_EXIT;
    return 0;
}


LRESULT CDynamicGroupPage::OnComboChanged(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnComboChanged" );

    try
    {
        // if this is the first update - set the train selector
        if (!m_trainSelectorSet)
        {
            try
            {
                CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
                m_trainSelectorGUI->setTrainSelector( &(manager->getTrainSelector()) );

                m_trainSelectorSet = true;
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "...", "Train selector not yet created" );
            }
        }

        TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate*>(wParam);

        if (NULL != update)
        {
            TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");

            m_trainSelectorGUI->updateSelectionCombo(update->trainListNames);

            // prevent memory leaks - every window gets its own copy
            delete update;
        }
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::OnComboChanged");

    FUNCTION_EXIT;
    return 0;
}


void CDynamicGroupPage::OnSavetrainselection()
{
    FUNCTION_ENTRY( "OnSavetrainselection" );

    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");

    try
    {
        m_trainSelectorGUI->saveTrainSelection();
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::OnSavetrainselection");

    FUNCTION_EXIT;
}


void CDynamicGroupPage::OnDeletetrainselection()
{
    FUNCTION_ENTRY( "OnDeletetrainselection" );

    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");

    try
    {
        m_trainSelectorGUI->deleteTrainSelection();
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::OnDeletetrainselection");

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onAllTrains()
{
    FUNCTION_ENTRY( "onAllTrains" );

    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");
    m_trainSelectorGUI->selectAllTrains();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onAllInboundTrains()
{
    FUNCTION_ENTRY( "onAllInboundTrains" );

    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");
    m_trainSelectorGUI->selectAllInbound();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onAllOutboundTrains()
{
    FUNCTION_ENTRY( "onAllOutboundTrains" );

    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");
    m_trainSelectorGUI->selectAllOutbound();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onClearAllTrains()
{
    FUNCTION_ENTRY( "onClearAllTrains" );

    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");
    m_trainSelectorGUI->clearSelection();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::OnAdd()
{
    FUNCTION_ENTRY( "OnAdd" );

    using TA_IRS_Bus::TrainInformationTypes::TrainDetailsList;
    // Find all the selected trains, and add them to the dynamic group page

    // get the selected trains first (vector of train ids)
    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");
    TrainDetailsList selectedTrains = m_trainSelectorGUI->getSelectedTrainData();

    // Also create a vector of strings of the trains that are already in the dymaic group (can't be in the group
    // more than once).
    std::vector<CString> existingTrains;

    for (TrainDetailsList::iterator iter = selectedTrains.begin();
         iter != selectedTrains.end(); iter++)
    {
        // Work out the ID of this train
        CString selectedTrainNum = iter->serviceNumber.c_str();

        CString selectedTsi = iter->primaryTsi.c_str();

        // Ensure the current train is not in the dynamic list box already
        CString foundTrain = isIDInGroupList(selectedTsi);

        if (foundTrain.IsEmpty())
        {
            // Add it to the list
            CString location(
                TA_IRS_Bus::LocationCache::getInstance().getLocationByKey(iter->currentLocation).c_str());
            int itemPos = m_groupList.InsertItem(COL_ID, selectedTrainNum);
            m_groupList.SetItemText(itemPos, COL_LOC, location);
            m_groupList.SetItemText(itemPos, COL_TSI, selectedTsi);
        }
        else
        {
            existingTrains.push_back(foundTrain);
        }

        TA_IRS_App::SearchPageHelper::getInstance().addDynamicGroupSubscriber(selectedTrainNum, selectedTsi);
    }

    if (existingTrains.size() > 0)
    {
        // Let the user know which trains were already on the list
        // TD14164 ++
        /*CString alreadyExistingMessage("The following trains were already in the dynamic group, and were not added a second time:");*/
        CString alreadyExistingMessage;
        // ++ TD14164

        int i = 0;
        for (i = 0;i < existingTrains.size() && i < 10 /*only list 10 trains*/;i++)
        {
            alreadyExistingMessage += "\n" + existingTrains[i];
        }

        if (i < existingTrains.size())
        {
            // At least one train wasn't displayed, so display a "..."
            alreadyExistingMessage += "\n...";
        }

        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        CString fieldName = "trains";
        userMsg << fieldName;
        userMsg << alreadyExistingMessage;
        CString errMsg = userMsg.constructMessage(IDS_UW_632008);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, alreadyExistingMessage,"Radio Manager",MB_OK|MB_ICONINFORMATION);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager",MB_OK|MB_ICONINFORMATION);
        // ++ TD14164
    }

    AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,IDM_UPDATEBUTTONS);

    // Re-enable buttons
    enableButtons();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::OnReset()
{
    FUNCTION_ENTRY( "OnReset" );

    // TD14416
    for (int i=0; i < m_groupList.GetItemCount(); i++)
    {
        CString tsi = m_groupList.GetItemText(i, COL_TSI);
        TA_IRS_App::SearchPageHelper::getInstance().removeDynamicGroupSubscriber(tsi);
    }
    // TD14416

    // This one's easy - just delete everything from the list
    if(m_groupList.m_hWnd!=NULL)
    {
        m_groupList.DeleteAllItems();
    }
    enableButtons();

    AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,IDM_UPDATEBUTTONS);

    FUNCTION_EXIT;
}


void CDynamicGroupPage::OnRemove()
{
    FUNCTION_ENTRY( "OnRemove" );

    // This is multiple selection dialog as of TES #440
    if (m_groupList.GetSelectedCount() >= 0)
    {
        while (m_groupList.GetSelectedCount() > 0)
        {
            // Iteratively remove the selected items, till all gone
            int iItem = m_groupList.GetNextItem(-1, LVNI_SELECTED);

            TA_ASSERT(iItem >= 0, "Invalid item index returned");

            // Remove the item from the search page component list
            CString tsi = m_groupList.GetItemText(iItem, COL_TSI);
            TA_IRS_App::SearchPageHelper::getInstance().removeDynamicGroupSubscriber(tsi);

            // Remove from our local dynamic group list
            m_groupList.DeleteItem(iItem);
        }
        enableButtons();

        AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,IDM_UPDATEBUTTONS);
    }

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onClickGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onClickGroupList" );

    enableButtons();

    FUNCTION_EXIT;
}


void CDynamicGroupPage::onItemChangedGroupList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onItemChangedGroupList" );

    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

    // Only interested in state changes (ie selection)
    if (pNMListView->uChanged != LVIF_STATE)
    {
        FUNCTION_EXIT;
        return;
    }

    enableButtons();

    FUNCTION_EXIT;
}


LRESULT CDynamicGroupPage::OnAddGroupMember(WPARAM members, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnAddGroupMember" );

    try
    {
        std::vector<std::vector<CString> >* membersVector = reinterpret_cast<std::vector<std::vector<CString> >*>(members);

        if (NULL != membersVector)
        {
            std::vector<CString> existingIDs;
            for (unsigned int i = 0;i < membersVector->size();i++)
            {
                if ( (*membersVector)[i].size() == 3)
                {
                    // First determine if this item is currently in the list

                    CString foundUser = isIDInGroupList((*membersVector)[i][2]);
                    if (foundUser.IsEmpty())
                    {
                        int index = m_groupList.InsertItem(COL_ID, (*membersVector)[i][0]);
                        m_groupList.SetItemText(index, COL_LOC, (*membersVector)[i][1]);
                        m_groupList.SetItemText(index, COL_TSI, (*membersVector)[i][2]);
                    }
                    else
                    {
                        existingIDs.push_back(foundUser);
                    }
                }
            }

            if (existingIDs.size() > 0)
            {
                // TD14164 ++
                /*CString alreadyExistingMessage("The following subscribers were already in the dynamic group, and were not added a second time:\n");*/
                CString alreadyExistingMessage;
                // ++ TD14164

                int i = 0;
                for (i = 0;i < existingIDs.size() && i < 10 /*only list 10 trains*/;i++)
                {
                    alreadyExistingMessage += "\n\t\t" + existingIDs[i];
                }

                if (i < existingIDs.size())
                {
                    // At least one train wasn't displayed, so display a "..."
                    alreadyExistingMessage += "\n...";
                }

                // TD14164 ++
                TA_Base_Bus::TransActiveMessage userMsg;
                CString fieldName = "subscribers";
                userMsg << fieldName;
                userMsg << alreadyExistingMessage;
                CString errMsg = userMsg.constructMessage(IDS_UW_632008);
                /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, alreadyExistingMessage,"Radio Manager",MB_OK|MB_ICONINFORMATION);*/
                RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager",MB_OK|MB_ICONINFORMATION);
                // ++ TD14164
            }

            enableButtons();

            AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,IDM_UPDATEBUTTONS);
        }
    }
    SILENT_RM_CATCH_HANDLER("CDynamicGroupPage::OnAddGroupMember");

    FUNCTION_EXIT;
    return 0;
}


CString CDynamicGroupPage::isIDInGroupList(CString& tsi)
{
    FUNCTION_ENTRY( "isIDInGroupList" );

    CString foundID;
    foundID.Empty();
    for (int i = 0;i < m_groupList.GetItemCount();i++)
    {
        CString subscriberName = m_groupList.GetItemText(i, COL_ID);
        CString subscriberTsi = m_groupList.GetItemText(i, COL_TSI);

        if (subscriberTsi == tsi)
        {
            // This train is already in the list
            foundID = subscriberName;

            break;
        }
    }

    FUNCTION_EXIT;
    return foundID;
}


void CDynamicGroupPage::updateExistingDynamicGroupMembers( CString& oldTsiNumber,
                                                           CString& oldServiceNumber,
                                                           CString& oldLocation,
                                                           CString& newTsiNumber,
                                                           CString& newServiceNumber,
                                                           CString& newLocation )
{
    FUNCTION_ENTRY( "updateExistingDynamicGroupMembers" );

    for (int i = 0;i < m_groupList.GetItemCount();i++)
    {
        CString subscriberName = m_groupList.GetItemText(i, COL_ID);
        CString subscriberTsi = m_groupList.GetItemText(i, COL_TSI);

        if (subscriberTsi == oldTsiNumber)
        {
            // This train is in the list update it
            m_groupList.SetItemText(i, COL_ID, newServiceNumber);
            m_groupList.SetItemText(i, COL_LOC, newLocation);
            m_groupList.SetItemText(i, COL_TSI, newTsiNumber);
            break;
        }
    }

    FUNCTION_EXIT;
}


void CDynamicGroupPage::addTestData()
{
    FUNCTION_ENTRY( "addTestData" );

    /* this will break the train selection component
    replace it with add messages and traindetails structures
    int position;

    position = m_groupList.InsertItem(0,"Train2",0);
               m_groupList.SetItemText(position,1,"212/0033");
    position = m_groupList.InsertItem(0,"Train3",0);
               m_groupList.SetItemText(position,1,"211/0022");
    position = m_groupList.InsertItem(0,"SO",0);
               m_groupList.SetItemText(position,1,"DBG");

    position = m_inboundTrainList.InsertItem(0,"DBG",0);
               m_inboundTrainList.SetItemText(position,1,"212/0033");
               m_inboundTrainList.SetItemText(position,2,"206-987-234");
    position = m_inboundTrainList.InsertItem(1,"DBG",0);
               m_inboundTrainList.SetItemText(position,1,"123/0001");
               m_inboundTrainList.SetItemText(position,2,"100-654-876");
    position = m_inboundTrainList.InsertItem(0,"MSM",0);
               m_inboundTrainList.SetItemText(position,1,"034/0011");
               m_inboundTrainList.SetItemText(position,2,"990-887-654");

    position = m_outboundTrainList.InsertItem(0,"DBG",0);
               m_outboundTrainList.SetItemText(position,1,"211/0022");
               m_outboundTrainList.SetItemText(position,2,"640-123-456");

    m_selectionName.AddString("MyList1");
    m_selectionName.AddString("MyList2");
    m_selectionName.AddString("MyList3");
    m_selectionName.SetCurSel(1);
    */

    FUNCTION_EXIT;
}


void CDynamicGroupPage::OnDblclkTrainList(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnDblclkTrainList" );

    using TA_IRS_Bus::TrainInformationTypes::TrainDetailsList;

    // get the selected trains first (vector of train ids)
    TA_ASSERT (m_trainSelectorGUI != NULL, "Train Selection Component is NULL");
    TrainDetailsList selectedTrains = m_trainSelectorGUI->getSelectedTrainData();

    // Work out the ID of this train
    CString selectedTrainNum = selectedTrains[0].serviceNumber.c_str();
    CString selectedTsi = selectedTrains[0].primaryTsi.c_str();
    unsigned long trainLocation = selectedTrains[0].currentLocation;
    unsigned long trainCommunicationLocation = selectedTrains[0].communicatingLocation;//limin++, ECP-338 CL-20150

    // check the train duty for this location.
    // Get the parent class and retrieve the reference to the radio manager
    bool isLocInCtrl = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd())->getTrainSelectorGUI().isLocationInControl( trainLocation, trainCommunicationLocation );//limin++, ECP-338 CL-20150

    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
            "Double click on train list:  m_makeTrainCallEnabled = %s, trainLocation = %d, isLocInCtrl = %s, trainCommunicationLocation=%d",
            m_dynamicGroupPageEnabled ?  "TRUE" : "FALSE", trainLocation, isLocInCtrl ?  "TRUE" : "FALSE", trainCommunicationLocation );

    if( m_makeTrainCallEnabled && isLocInCtrl )
    {
        TA_IRS_App::SearchPageHelper::getInstance().SetCall(selectedTsi, selectedTrainNum, false, TA_IRS_Bus::ISearchPageHelper::ESelectionType::TRAIN);

        AfxGetApp()->m_pMainWnd->PostMessage(WM_COMMAND,IDC_CALL);
    }
    *pResult = 0;

    FUNCTION_EXIT;
}


afx_msg void CDynamicGroupPage::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );

    // Do nothing

    FUNCTION_EXIT;
}


afx_msg void CDynamicGroupPage::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // Do nothing

    FUNCTION_EXIT;
}


