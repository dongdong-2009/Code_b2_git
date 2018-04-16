/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/PAManagerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// PAManagerDlg.cpp : implementation file
//


#include "app/pa/PAManager/src/stdafx.h"

#include "app/pa/PAManager/src/DvaVersionsPage.h"
#include "app/pa/PAManager/src/GraphworxComms.h"
#include "app/pa/PAManager/src/PAManagerDlg.h"
#include "app/pa/PAManager/src/PaErrorHandler.h"
#include "app/pa/PAManager/src/PaManager.h"
#include "app/pa/PAManager/src/PaManagerModel.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/RightsChangedWindowNotificationHandler.h"
#include "app/pa/PAManager/src/RunParamListener.h"
#include "app/pa/PAManager/src/StationBroadcastPage.h"

#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessageVersionAccessFactory.h"





#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_Base_Core;

TA_Base_Core::PrimitiveWrapper<int> PAManagerDlg::s_restartTargetPosition;

/////////////////////////////////////////////////////////////////////////////
// PAManagerDlg dialog

PAManagerDlg::PAManagerDlg(TA_Base_Bus::IGUIAccess& controlClass) : 
TransActiveDialog(controlClass, PAManagerDlg::IDD, NULL),
m_mainTab(),
m_stationPage( NULL ),
m_dvaVersionsPage( NULL ),
m_managerModel ( NULL ),
m_bShouldRespone( false ),
m_rightsChangeHandler(0)
{      
    FUNCTION_ENTRY( "PAManagerDlg" );

    FUNCTION_EXIT;
}


PAManagerDlg::~PAManagerDlg()
{
    FUNCTION_ENTRY( "~PAManagerDlg" );

    try
    {
        // Avoid any destruction here - core framework is
        // mostly destroyed
    }
    catch (...)
    {
         LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    TransActiveDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(PAManagerDlg)
    DDX_Control(pDX, IDC_ABOUTBUTTON, m_aboutButton);
    DDX_Control(pDX, IDC_HELP_BTN, m_helpButton);
    DDX_Control(pDX, IDC_EXECUTE_BTN, m_executeButton);
    DDX_Control(pDX, IDC_CLOSE_BTN, m_closeButton);
    DDX_Control(pDX, IDC_KILL_ALL_STATUS_BTN, m_killAllStatusButton);
    DDX_Control(pDX, IDC_KILL_STATUS_BTN, m_killStatusButton);
    DDX_Control(pDX, IDC_RESTORE_STATUS_BTN, m_restoreStatusButton);
    DDX_Control(pDX, IDC_STATUS_CB, m_statusNameList);
	DDX_Control(pDX, IDC_KILL_ALL_GOING_DVA, m_killAllGoingDVA);
    DDX_Control(pDX,IDC_TAB_PA_FUN,m_mainTab);
    //}}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(PAManagerDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(PAManagerDlg)
	ON_BN_CLICKED(IDC_CLOSE_BTN, OnCloseBtn)
	ON_BN_CLICKED(IDC_EXECUTE_BTN, OnExecuteBtn)
    ON_BN_CLICKED(IDC_HELP_BTN, OnHelpBtn)
    ON_BN_CLICKED(IDC_RESTORE_STATUS_BTN, OnRestoreStatusBtn)
    ON_BN_CLICKED(IDC_KILL_STATUS_BTN, OnKillStatusBtn)
    ON_BN_CLICKED(IDC_ABOUTBUTTON, OnAboutbutton)
    ON_BN_CLICKED(IDC_KILL_ALL_STATUS_BTN, OnKillAllStatusBtn)
    ON_BN_CLICKED(IDC_KILL_ALL_GOING_DVA, OnKillAllGoingDVA)
	ON_CBN_SELCHANGE(IDC_STATUS_CB, OnSelchangeStatusCb)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PA_FUN, OnTabSelChanged)
	ON_WM_DESTROY()
    ON_MESSAGE(WM_PA_STN_STATUS_NAME_UPDATE, OnStatusNameChange)
    ON_MESSAGE(WM_PA_HMI_DISPLAYSEL_CHANGED, OnDisplayModeChange)
    ON_MESSAGE(WM_PA_ATTEMPT_APPLICATION_RESTART, OnAttemptRestart)
    ON_MESSAGE(WM_PA_MANAGER_ACTIVE, OnActivateSchematic)
    ON_MESSAGE(WM_PA_ERROR_MESSAGE, OnErrorMessage)
    ON_MESSAGE(WM_PA_RIGHTS_CHANGE, OnRightsChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL PAManagerDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    RunParamListener::getInstance().setRestartApplicationCallback(this);

    m_rightsChangeHandler =  new RightsChangedWindowNotificationHandler(m_hWnd);

    TA_Base_Bus::ResizingProperties rp;
    rp.canMaximise = false;
    rp.maxWidth = 1680;
    rp.maxHeight = 1050;
    CRect rtClient;
    GetClientRect(rtClient);
    rp.minWidth = rtClient.Width();
    rp.minHeight = rtClient.Height();
    setResizingProperties(rp);

    // Go and create dialog
    TransActiveDialog::OnInitDialog();

    m_bShouldRespone = false;

    PaErrorHandler::setPrimaryWindowHandle(m_hWnd);

    initWindowPos();

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void PAManagerDlg::init()
{
    FUNCTION_ENTRY( "init" );

    if(m_managerModel == NULL)
    {
        m_managerModel = new PaManagerModel();
    }

    if(m_stationPage == NULL)
    {
        m_stationPage = new StationBroadcastPage(m_managerModel->getDvaMessageChangeSubscriber(), this);
    }

    if(m_dvaVersionsPage == NULL)
    {
        m_dvaVersionsPage = new DvaVersionsPage(this);
    }

	CachedConfig::ELocationType locationType = CachedConfig::getInstance()->getAgentLocType();
    std::string locationTypeString;
    std::string locationBroadcastString;

	switch(locationType)
	{
	case CachedConfig::LOCATION_TYPE_OCC:
		locationTypeString = "OCC DVA Set Versions";
        locationBroadcastString = "Station Broadcast";
		break;
	case CachedConfig::LOCATION_TYPE_STATION:
        locationTypeString = "Station DVA Set Versions";
        locationBroadcastString = "Station Broadcast";
		break;
	case CachedConfig::LOCATION_TYPE_DEPOT:
        locationTypeString = "Depot DVA Set Versions";
        locationBroadcastString = "Depot Broadcast";
		break;
	default:
		locationTypeString = "";
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invaild location");
	}

    if ( NULL != m_stationPage )
    {
		m_mainTab.addPage( m_stationPage, m_stationPage->getTemplateID(), locationBroadcastString );
    }

    if ( NULL != m_dvaVersionsPage )
    {
		m_mainTab.addPage( m_dvaVersionsPage, m_dvaVersionsPage->getTemplateID(), locationTypeString );
    }

    m_mainTab.showPage( 0 );

    refreshStatusNameCombo(true);

    // Perform final stage initialization for the tabs (MUST call from here)
    if (m_stationPage)
    {
        m_stationPage->postDialogCreate();    
    }

    if (m_dvaVersionsPage)
    {
        m_dvaVersionsPage->postDialogCreate();
    }

    // Listen out to the Display tag (special system level command)
    // Should listen out for this AFTER posting startup hide request (otherwise
    // the request to display may be overriden by startup hide request)
    RunParamListener::getInstance().registerListener(RunParamListener::DISPLAY_TAG, *this);
    RunParamListener::getInstance().registerListener(RunParamListener::SELECTED_STATION_MSG_TAG, *this);
    RunParamListener::getInstance().registerListener(RunParamListener::SELECTED_TRAIN_MSG_TAG, *this);
	
    // Set window handle AFTER responding to any display tags - if
    // an pop up messagebox is present while we areminimized (result from
    // Display=HIDE message), the message box will (has in past) disappear
    // off the screen (and shift into non-screen area at top left).
    // Probably as a result of the minimize call being dispatched to
    // the currently active message box
    CORBA::Boolean ret = false;
    CORBA_CALL_RETURN( ret, CachedConfig::getInstance()->getAgentObject(), hardwareLinkTest, ( "", CachedConfig::getInstance()->getSessionId().c_str() ) );//limin
    if ( !ret )
    {
        PaErrorHandler::displayError(PaErrorHandler::FEPC_HARDWARE_FAILURE);
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::activateSchematic()
{
    FUNCTION_ENTRY( "activateSchematic" );

    bool schematicUpdated = false;

    // Now we've finished loading, we may activate the graphworx display we're operating for
    try
    {
        std::string schematicId = RunParamListener::getInstance().getSchematicIdentifier();
        schematicUpdated = TA_App::GraphworxComms::getInstance().activateGraphworxDisplay(schematicId);
    }
    catch (const TA_Base_Core::ValueNotSetException& e)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ValueNotSetException", e.what());
    }
    catch (...)
    {
        // Silently log any error communicating with the actual schematic, may still run
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "While activating schematic");
    }

    if (!schematicUpdated)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to update schematic" );
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::refreshStatusNameCombo(bool bInitialization)
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

    if (m_mainTab.GetCurSel() == 0)
    {
        m_currentStatusNameBroadcastType.setValue(TA_Base_Bus::IPAAgentCorbaDef::StationDva);
    }
    else
    {
        // Any unrecognized windows do not respond to any status name changes
        // so may exit early
        m_statusNameList.EnableWindow(FALSE);

        FUNCTION_EXIT;
        return;
    }

    if (NULL == m_managerModel)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_managerModel NULL" );

        FUNCTION_EXIT;
        return;
    }

    // Establish connection to name change subscriber before filling
    // out the status name combo box (important to minimize risk of out
    // of sync issues)
    if (bInitialization)
    {
        m_managerModel->initializeStatusNameSubscriber(m_hWnd);
    }

    std::vector<std::string> broadcastZones;

    bool success = false;

    try
    {
        if (m_mainTab.GetCurSel() == 0)
        {
            broadcastZones = m_managerModel->getStationDvaBroadcastZones();
        }
        else
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognized page" );

			FUNCTION_EXIT;
			return;
        }

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


afx_msg LRESULT PAManagerDlg::OnErrorMessage(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnErrorMessage" );

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

    FUNCTION_EXIT;
    return 0;
}


afx_msg LRESULT PAManagerDlg::OnStatusNameChange(UINT wParam, LONG lParam)
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
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }

    FUNCTION_EXIT;
    return 0;
}


bool PAManagerDlg::updateComboBoxOnNameChange(UINT id)
{
    FUNCTION_ENTRY( "updateComboBoxOnNameChange" );

    if (NULL == m_managerModel)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_managerModel NULL" );

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
                        m_managerModel->collectStatusNameChange(id, m_hWnd, from, to);

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
		if (to.length() == 0)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error" );

			FUNCTION_EXIT;
			return false;
		}

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

			if (confirm != nIndex)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error" );

				FUNCTION_EXIT;
				return false;
			}
        }
    }

    FUNCTION_EXIT;
    return true;
}


BOOL PAManagerDlg::DestroyWindow()
{
    FUNCTION_ENTRY( "DestroyWindow" );

	if (m_stationPage)
	{
        m_stationPage->DestroyWindow();
    }

	if (m_dvaVersionsPage)
	{
		m_dvaVersionsPage->DestroyWindow();
    }

    FUNCTION_EXIT;
    return TransActiveDialog::DestroyWindow();
}


void PAManagerDlg::OnCloseBtn()
{
    FUNCTION_ENTRY( "OnCloseBtn" );

    TransActiveDialog::OnClose();

    FUNCTION_EXIT;
}


void PAManagerDlg::enableExecuteButton(bool isEnabled)
{
    FUNCTION_ENTRY( "enableExecuteButton" );

    CWnd* executeButtonWnd = this->GetDlgItem(IDC_EXECUTE_BTN);
    executeButtonWnd->EnableWindow(isEnabled);

    FUNCTION_EXIT;
}


void PAManagerDlg::OnHelpBtn()
{
    FUNCTION_ENTRY( "OnHelpBtn" );

    TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

    FUNCTION_EXIT;
}


void PAManagerDlg::OnExecuteBtn()
{
    FUNCTION_ENTRY( "OnExecuteBtn" );

    int index = m_mainTab.GetCurSel();

    switch (index)
    {
        case 0 :
        {
            // Station Broadcast
            m_stationPage->executeBroadcast();
            break;
        }
        case 2 :
        // DVA Versions
        default :
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute button pressed in invalid context." );
            break;
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::OnSelchangeStatusCb()
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

    m_killStatusButton.EnableWindow(allowedToClearSelectedBroadcast);
    ////////////////////////


    ////////////////////////
    // Check if restore button should be enabled
    m_restoreStatusButton.EnableWindow(validSelection);
    ////////////////////////

    //////////////////////////////
    // Determine whether clear all button should be enabled
    bool enableKillAll = (m_statusNameList.GetCount() > 0);

    // May use clear all provided there is one or more item in list
    // NOTE: May not be able to clear the broadcasts if privileges not present (feedback provided to operator)
    m_killAllStatusButton.EnableWindow(enableKillAll);
    //////////////////////////////

    FUNCTION_EXIT;
}


void PAManagerDlg::OnTabSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onTabChanged" );

    *pResult = 0;

    // Must refresh the combo each time change tabs (because
    // the status name combo box is context sensitive)
    refreshStatusNameCombo(false);

    FUNCTION_EXIT;
}


void PAManagerDlg::OnRestoreStatusBtn()
{
    FUNCTION_ENTRY( "OnRestoreStatusBtn" );

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

    CString broadcastId;
    m_statusNameList.GetLBText(sel, broadcastId);

    // Check if this broadcast ID still exists before attempting restore
    if (!m_managerModel->confirmStatusNameExists(broadcastId.GetBuffer(0)))
    {
        // No longer exists - we're out of sync, perform full refresh of combo
        refreshStatusNameCombo(false);

        FUNCTION_EXIT;
        return;
    }

    if (!m_currentStatusNameBroadcastType.hasBeenSet())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown broadcast ID context" );

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

    if (m_mainTab.GetCurSel() == 0)
    {
        std::string strBroadcastId = broadcastId.GetBuffer(0);
        m_managerModel->restoreBroadcast(strBroadcastId);
        m_stationPage->displayDvaStatusDialog(strBroadcastId);
    }
    else
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PAManagerDlg::OnRestoreStatusBtn: Invalid mode for restore operation" );
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::OnKillStatusBtn()
{
    FUNCTION_ENTRY( "OnKillStatusBtn" );

    if (NULL == m_managerModel)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_managerModel NULL" );

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
        if (m_managerModel->terminateAndRemoveBroadcastIds(broadcastArray)
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


void PAManagerDlg::OnKillAllStatusBtn()
{
    FUNCTION_ENTRY( "OnKillAllStatusBtn" );

    if (NULL == m_managerModel)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_managerModel NULL" );

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
        if (PaRightsManager::getInstance().hasRightToClearBroadcast( broadcastId.GetBuffer(0), CachedConfig::getInstance()->getAgentObject()))
        {
            if (!PaRightsManager::getInstance().isBroadcastInProgress( broadcastId.GetBuffer(0), CachedConfig::getInstance()->getAgentObject()))
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
            if (m_managerModel->terminateAndRemoveBroadcastIds(broadcastArray) != broadcastArray.size())
            {
                // Not all broadcasts passed in could be removed (maybe some already
                // removed..) - meaning we're not up to date
                // Force a full update, to ensure we're in sync with agent
                refreshStatusNameCombo(false);
            }
        }

        TA_Base_Bus::TransActiveMessage userMsg;
        // Display a report showing the broadcasts that could not be cleared
        if (!invalidRights.empty())
        {
            std::ostringstream ostInvalidRights;
            for ( std::vector<std::string>::iterator itLoop = invalidRights.begin(); itLoop != invalidRights.end(); ++itLoop )
            {
                ostInvalidRights << "\t" << *itLoop << std::endl;
            }
            userMsg << ostInvalidRights.str();
        }

        // indicate which broadcasts couldn't be cleared because they were in progress
        if (!inProgressBroadcasts.empty())
        {
            std::ostringstream ostInprogress;
            for ( std::vector<std::string>::iterator itLoop = inProgressBroadcasts.begin(); itLoop != inProgressBroadcasts.end(); ++itLoop )
            {
                ostInprogress << "\t" << *itLoop << std::endl;
            }
            userMsg << ostInprogress.str();
        }

        if ( !invalidRights.empty() && !inProgressBroadcasts.empty() )
        {
            userMsg.showMsgBox( IDS_UI_670002 );
        }
        else if ( !invalidRights.empty() )
        {
            userMsg.showMsgBox( IDS_UI_670001 );
        }
        else if ( !inProgressBroadcasts.empty() )
        {
            userMsg.showMsgBox( IDS_UI_670000 );
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


LRESULT PAManagerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "WindowProc" );

    LRESULT result = 0;

    // We shouldn't get any CORBA exceptions at this level, not necessarily
    // true for TransactiveExceptions though..
    try
    {
        result = TransActiveDialog::WindowProc(message, wParam, lParam);
    }
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }

    FUNCTION_EXIT;
    return result;
}


afx_msg LRESULT PAManagerDlg::OnDisplayModeChange(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnDisplayModeChange" );

    if (1 == wParam)
    {
        ShowWindow( SW_RESTORE );
        RunParamListener::getInstance().setIsCurrentlyVisible(true);
    }
    else
    {
        // This causes the dialog to minimize(??) - on a normal dev system
        // it's handled OK, but on the MFTs the modal dialog disappears
        // (off the top left of the screen - a SW_MINIMIZE message posts out
        // a OnMove notification, to -32000, -32000)
        if (!PaErrorHandler::isDisplayingModalError())
        {
            ShowWindow(SW_MINIMIZE);
            RunParamListener::getInstance().setIsCurrentlyVisible(false);
        }
    }

    FUNCTION_EXIT;
    return 0;
}


void PAManagerDlg::processRunParamChange(const std::string& strName, const std::string& strValue)
{
    FUNCTION_ENTRY( "processRunParamChange" );

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
         "PamManageDlg  RunParamChange name = '%s',ParamValue ='%s'", strName.c_str(), strValue.c_str());

    if (strName.compare( RunParamListener::DISPLAY_TAG ) == 0)
    {
        if(0 == strValue.compare( "SHOW" ))
        {
            m_bShouldRespone = true;
            PostMessage( WM_PA_HMI_DISPLAYSEL_CHANGED, 1, 0 );
            PostMessage( WM_PA_MANAGER_ACTIVE );
        }
        else
        {
        }
    }
    else if ( 0 == strName.compare( RunParamListener::SELECTED_STATION_MSG_TAG ))
    {
        m_stationPage->selectMessageNameInLists(strValue);
    }
    else
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognized run param change" );
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    RunParamListener::getInstance().setRestartApplicationCallback(0);

    // Delete the rights change notification object before window handle becomes invalid
    delete m_rightsChangeHandler;
    m_rightsChangeHandler = 0;

    if ( ::IsWindow( m_hWnd ) && ( NULL != m_managerModel ))
    {
        m_managerModel->removeStatusNameSubscriber(m_hWnd);
    }

    TransActiveDialog::OnDestroy();

    // Deregister all listeners attached to window on cleanup
    RunParamListener::getInstance().deRegisterListener(*this);

    // Reset error handler window reference before we're destroyed
    PaErrorHandler::setPrimaryWindowHandle(0);

    try
    {
        if (NULL != m_stationPage)
        {
            delete m_stationPage;
            m_stationPage = NULL;
        }

        if (NULL != m_dvaVersionsPage)
        {
            delete m_dvaVersionsPage;
            m_dvaVersionsPage = NULL;
        }

        if (NULL != m_managerModel)
        {
            delete m_managerModel;
            m_managerModel = 0;
        }

        // Must attempt cleanup of singletons here before
        // core servants are de-activated
        PAManagerApp::cleanupSingletons();

        // When we're shutting down, should also de-activate all the graphworx displays
        TA_App::GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();
    }
    catch (...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in OnDestroy");
    }

    FUNCTION_EXIT;
}


void PAManagerDlg::OnAboutbutton()
{
    FUNCTION_ENTRY( "OnAboutbutton" );

    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

    FUNCTION_EXIT;
}


afx_msg LRESULT PAManagerDlg::OnRightsChanged(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnRightsChanged" );

    try
    {
        // Refresh contents of the status name combo fully (may be changes in those we can / can not view)
        refreshStatusNameCombo(false);
    }
    catch (const CORBA::Exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", expWhat._name() );
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }

    FUNCTION_EXIT;
    return 0;
}


void PAManagerDlg::restartApplication(int leftPos)
{
    FUNCTION_ENTRY( "restartApplication" );

    ::PostMessage(m_hWnd, WM_PA_ATTEMPT_APPLICATION_RESTART, (DWORD)leftPos, 0);

    FUNCTION_EXIT;
}


afx_msg LRESULT PAManagerDlg::OnAttemptRestart(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnAttemptRestart" );

    // Before we can restart - we must confirm there are no modal dialogs
    // active (otherwise users changes will be lost), if there are any open
    // we display an appropriate error message
    // We can tell if there are modal dialog boxes if the main dialog window
    // is disabled

    if (!IsWindowEnabled())
    {
        // This means a modal dialog is active..
        PaErrorHandler::displayModalError(PaErrorHandler::UNABLE_TO_PERFORM_REQUEST);
    }
    else
    {
        s_restartTargetPosition.setValue((int)(wParam));

        // Otherwise it shouldn't be a problem restarting, runparams should transfer to
        // new instance no problems, except for SynchroniseSelected (which spans several runparams)
        CloseWindow();
    }

    FUNCTION_EXIT;
    return 0;
}


bool PAManagerDlg::isApplicationRestartInProgress()
{
    FUNCTION_ENTRY( "isApplicationRestartInProgress" );
    FUNCTION_EXIT;
    return s_restartTargetPosition.hasBeenSet();
}


int PAManagerDlg::getAppplicationRestartTargetPosition()
{
    FUNCTION_ENTRY( "getAppplicationRestartTargetPosition" );

    ASSERT(isApplicationRestartInProgress());

    FUNCTION_EXIT;
    return s_restartTargetPosition.getValue();
}


afx_msg LRESULT PAManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnSetWindowPosition" );

    if ( !m_bShouldRespone )
    {
        return 0;
    }

    m_bShouldRespone = false;

    FUNCTION_EXIT;
    return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
}


afx_msg LRESULT PAManagerDlg::OnActivateSchematic(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnActivateSchematic" );

    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "PamManager  OnActivateSchematic");

    activateSchematic();

    FUNCTION_EXIT;
    return 0;
}


void PAManagerDlg::OnKillAllGoingDVA()
{
	FUNCTION_ENTRY("OnKillAllGoingDVA");

    TA_Base_Bus::TransActiveMessage userMsg;
	if( IDYES == userMsg.showMsgBox( IDS_UW_610016 ))
	{
		try
		{
			CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), forceTerminateAllExistPA, ());
		}
		catch(TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unknown exception.");
		}	
	}
	else
	{
		//do nothing
	}
	
	FUNCTION_EXIT;
}

void PAManagerDlg::initWindowPos()
{
    CPoint ptCursor;
    GetCursorPos(&ptCursor);

    // Get schematic rect
    RECT boundary;
    try
    {
        boundary = TA_Base_Bus::AppLauncher::getInstance().getRect( TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
            TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, ptCursor.x );
    }
    catch( const std::exception& ex )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
        return;
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Cautht unknown exception when get Schematic rect" );
        return;
    }

    // Get dialog size
    RECT rtWnd;
    GetWindowRect(&rtWnd);

    int nLeft = boundary.left + ( boundary.right - rtWnd.right + rtWnd.left - boundary.left ) / 2;
    int nTop = boundary.top;

    SetWindowPos( &wndTop, nLeft, nTop, 0, 0, SWP_NOSIZE );

    ShowWindow( SW_MINIMIZE );
}

