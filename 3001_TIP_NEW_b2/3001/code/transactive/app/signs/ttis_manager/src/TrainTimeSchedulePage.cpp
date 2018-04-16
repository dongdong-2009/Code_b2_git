/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainTimeSchedulePage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is the train time schedule page. It allows editing of train time schedules
  * in the database.
  *
  */

#include "stdafx.h"
#include "resource.h"
#include "TrainTimeSchedulePage.h"
#include "TTISPredefinedMessages.h"
#include "TTISMessageManager.h"
#include "TrainTimeScheduleManager.h"
#include "RightsManager.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/CorbaException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TTISManagerEntityData.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::DataConfigurationException;
using TA_IRS_Core::ITrainTimeSchedule;
using TA_IRS_Core::ITrainTimeScheduleEntry;
using TA_Base_Core::PredefinedMessage;


/////////////////////////////////////////////////////////////////////////////
// CTrainTimeSchedulePage dialog

namespace TA_IRS_App
{
// This is the biggest repeat interval to fit into
// the 1 byte that the protocol allows.
static const unsigned short MAX_REPEAT_INTERVAL = 255;
    
CTrainTimeSchedulePage::CTrainTimeSchedulePage(CWnd* pParent /*=NULL*/)
    : CDialog(CTrainTimeSchedulePage::IDD, pParent),
      m_currentSchedule(NULL),
      m_editMode(false),
      m_canEditSchedule(false),
      m_ttisPredefinedMessages( NULL ),
      m_trainTimeScheduleManager( NULL )
{
    //{{AFX_DATA_INIT(CTrainTimeSchedulePage)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

	//xufeng++ 2006/11/06 TD14367
    m_startTime.setCallback(this);
	m_endTime.setCallback(this);
	//++xufeng 2006/11/06 TD14367
}

void CTrainTimeSchedulePage::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTrainTimeSchedulePage)
	DDX_Control(pDX, IDC_END_LABEL, m_endLabel);
	DDX_Control(pDX, IDC_PRIORITY_LABEL, m_priorityLabel);
	DDX_Control(pDX, IDC_REPEAT_LABEL, m_repeatLabel);
	DDX_Control(pDX, IDC_START_LABEL, m_startLabel);
    DDX_Control(pDX, IDC_SCHEDULE_LIBRARY, m_scheduleLibraryList);
    DDX_Control(pDX, IDC_SCHEDULE_NAME, m_scheduleName);
    DDX_Control(pDX, IDC_END_TIME, m_endTime);
    DDX_Control(pDX, IDC_START_TIME, m_startTime);
    DDX_Control(pDX, IDC_REPEAT_INTERVAL, m_repeatInterval);
    DDX_Control(pDX, IDC_SCH_PRIORITY, m_priority);
    DDX_Control(pDX, IDC_SCH_PREDEFINED_LIST, m_predefinedList);
    DDX_Control(pDX, IDC_SEARCH_NORMAL, m_searchNormal);
    DDX_Control(pDX, IDC_SCHEDULE, m_scheduleEntryList);
    DDX_Control(pDX, IDC_MESSAGE_CONTENT, m_messageContent);
    DDX_Control(pDX, IDC_FREE_TEXT, m_freeTextList);
	DDX_Control(pDX, IDC_SCH_REMOVE_ENTRY, m_removeEntry);
	DDX_Control(pDX, IDC_SCH_REMOVE_ALL_ENTRIES, m_removeAllEntries);
	DDX_Control(pDX, IDC_NEW_SCHEDULE, m_newSchedule);
	DDX_Control(pDX, IDC_EDIT_SCHEDULE, m_editSchedule);
	DDX_Control(pDX, IDC_EDIT_FREE_TEXT, m_editFreeText);
	DDX_Control(pDX, IDC_DELETE_SCHEDULE, m_deleteSchedule);
	DDX_Control(pDX, IDC_COPY_SCHEDULE, m_copySchedule);
    DDX_Control(pDX, IDC_ADD_ENTRY, m_addEntry);
    DDX_Control(pDX, IDC_SCH_SAVE, m_save);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrainTimeSchedulePage, CDialog)
//{{AFX_MSG_MAP(CTrainTimeSchedulePage)
ON_BN_CLICKED(IDC_EDIT_FREE_TEXT, OnEditFreeText)
ON_BN_CLICKED(IDC_ADD_ENTRY, OnAddEntry)
ON_BN_CLICKED(IDC_SCH_REMOVE_ENTRY, OnRemoveEntry)
ON_BN_CLICKED(IDC_SCH_REMOVE_ALL_ENTRIES, OnRemoveAllEntries)
ON_BN_CLICKED(IDC_SCH_SAVE, OnSaveSchedule)
ON_BN_CLICKED(IDC_NEW_SCHEDULE, OnNewSchedule)
ON_BN_CLICKED(IDC_DELETE_SCHEDULE, OnDeleteSchedule)
ON_BN_CLICKED(IDC_EDIT_SCHEDULE, OnEditSchedule)
ON_EN_UPDATE(IDC_SEARCH_NORMAL, OnUpdateSearchNormal)
ON_EN_CHANGE(IDC_SCHEDULE_NAME, OnScheduleNameChange)
ON_NOTIFY(NM_CLICK, IDC_FREE_TEXT, onClickFreeTextList)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_FREE_TEXT, onItemchangedFreeText)
ON_NOTIFY(NM_CLICK, IDC_SCH_PREDEFINED_LIST, onClickPredefinedList)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_SCH_PREDEFINED_LIST, onItemchangedPredefinedList)
ON_NOTIFY(NM_CLICK, IDC_SCHEDULE_LIBRARY, onClickScheduleList)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_SCHEDULE_LIBRARY, onItemchangedScheduleList)
ON_NOTIFY(NM_CLICK, IDC_SCHEDULE, onClickScheduleEntryList)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_SCHEDULE, onItemchangedScheduleEntryList)
ON_MESSAGE(WM_UPDATE_CURRENT_TTIS_VERSION, onUpdateCurrentTTISVersion)
ON_MESSAGE(WM_UPDATE_TIMESCHEDULE, onTimeScheduleChanged)
ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_COPY_SCHEDULE, onCopySchedule)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CTrainTimeSchedulePage::OnInitDialog() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnInitDialog" );
    CDialog::OnInitDialog();
       
    // DB limits this to 85 characters
    m_scheduleName.LimitText(85);

    // set up the message priorities
    m_priority.AddString("Normal - 4");
    m_priority.AddString("Normal - 5");
    //m_priority.AddString("6"); Reserved for ratis.
    m_priority.AddString("Normal - 7");
    m_priority.AddString("Normal - 8");
    
    CTime lowerTime(1971, 1, 1, 0, 0, 0, 0);
    CTime upperTime(1971, 1, 1, 4, 15, 0, 0);
    m_repeatInterval.SetRange(&lowerTime, &upperTime);

    m_predefinedList.SetExtendedStyle(m_predefinedList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    m_freeTextList.SetExtendedStyle(m_predefinedList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    m_scheduleLibraryList.SetExtendedStyle(m_predefinedList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
    m_scheduleLibraryList.setItemChangeRequestForwarding(TRUE);

    RECT rect;
    m_predefinedList.GetClientRect(&rect);
    m_predefinedList.InsertColumn(0, "Message Name");
    m_predefinedList.SetColumnWidth(0, rect.right);
    m_freeTextList.GetClientRect(&rect);
    m_freeTextList.InsertColumn(0, "Message Name");
    m_freeTextList.SetColumnWidth(0, rect.right);
    m_scheduleLibraryList.GetClientRect(&rect);
    m_scheduleLibraryList.InsertColumn(0, "Message Name");
    m_scheduleLibraryList.SetColumnWidth(0, rect.right);
    
    // set the time field format
    m_startTime.SetFormat(_T("    HH:mm"));
    m_endTime.SetFormat(_T("    HH:mm"));
    m_repeatInterval.SetFormat(_T("    HH:mm"));
    
    // set the icon on the add entry button
    HICON buttonIcon;
    buttonIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
    m_addEntry.SetIcon(buttonIcon);
    
 
    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnInitDialog" );
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CTrainTimeSchedulePage::Init(REBProgressManager & mgr)
{
    if ( NULL == m_ttisPredefinedMessages )
    {
        m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
    }
	
	//hongran++ TD17548
    // register with the pre-defined message singleton to be notified of a change
    m_ttisPredefinedMessages->registerCurrentVersionUser(this);

    if ( NULL == m_trainTimeScheduleManager )
    {
        m_trainTimeScheduleManager = &( TrainTimeScheduleManager::getInstance() );
    }

    m_trainTimeScheduleManager->registerForTimeScheduleChanges(this);

    // register for rights changes
    RightsManager::getInstance().registerForRightsChanges(this);
    m_canEditSchedule = RightsManager::getInstance().canEditSchedule();
	//++hongran TD17548

    // load the pre-defined messages into the list
	mgr.SetStaticText(0, "Initializing the Train Time Schedule Page: loading all predefined messages");

    loadPreDefined();
	
	mgr.SetProgress(40);
    
    // load the time schedules into the list
	mgr.SetStaticText(0, "Initializing the Train Time Schedule Page: loading all predefined messages");

    loadTimeSchedules();
    
	mgr.SetProgress(50);
	try
	{
		std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
		TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
		TA_Base_Core::TTISManagerEntityData* entityData = dynamic_cast<TA_Base_Core::TTISManagerEntityData*>(guiEntity);
		m_nMaxScheduleMessage = entityData->getMaxScheduleMessage();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[XF]entityName = %s, m_nMaxScheduleMessage = %d", entityName.c_str(), m_nMaxScheduleMessage);
	}
	catch ( const TA_Base_Core::DataException& de )
	{
		LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
	}
	catch( ... )
	{
		std::string exceptionMsg( "unknow exception " );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "unknow exception");
	}


	mgr.SetProgress(60);

    // setup the state of the page
    resetToBlankSchedule();

}


void CTrainTimeSchedulePage::OnDestroy() 
{
	CDialog::OnDestroy();
	
    resetToBlankSchedule();

    // deregister for rights changes
    RightsManager::getInstance().deregisterForRightsChanges(this);

	// no longer accepting updates
    if ( NULL != m_ttisPredefinedMessages )
    {
        // deregister for pre-defined message library version update
        m_ttisPredefinedMessages->deregisterCurrentVersionUser(this);
        m_ttisPredefinedMessages = NULL;
        
        TTISPredefinedMessages::removeInstance();
    }
    
    if ( NULL != m_trainTimeScheduleManager )
    {
        // deregister for pre-defined message library version update
        m_trainTimeScheduleManager->deregisterForTimeScheduleChanges(this);
        m_trainTimeScheduleManager = NULL;
        
        TrainTimeScheduleManager::removeInstance();
    }

    // clean up lists
    m_scheduleLibraryList.DeleteAllItems();
    m_predefinedList.DeleteAllItems();
}


void CTrainTimeSchedulePage::loadTimeSchedules()
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::loadTimeSchedules" );
    
    // clear the list
    m_scheduleLibraryList.DeleteAllItems();
	m_scheduleKeyMap.clear();

    // get the current list from the schedule manager
    TrainTimeScheduleManager::TimeScheduleMap timeScheduleInformation =
        m_trainTimeScheduleManager->getCurrentTimeScheduleNames();
    
    // populate the list of time schedules
    int count = 0;
    for(TrainTimeScheduleManager::TimeScheduleMap::iterator iter = timeScheduleInformation.begin();
    iter != timeScheduleInformation.end(); iter++, count++)
    {
        m_scheduleLibraryList.InsertItem( count, iter->first.c_str() );
//        m_scheduleLibraryList.SetItemData( count, iter->second );
		//modified by hongzhi, SetItemData will get errors sometime
		m_scheduleKeyMap[count] = iter->second;
    }
    
    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::loadTimeSchedules" );
}


void CTrainTimeSchedulePage::loadPreDefined()
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::loadPreDefined" );
    
    // make sure the search text is empty
    m_searchNormal.SetWindowText("");

    // build the list
    OnUpdateSearchNormal();

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::loadPreDefined" );
}


void CTrainTimeSchedulePage::OnUpdateSearchNormal() 
{
    // get the search string
    CString searchText;
    m_searchNormal.GetWindowText(searchText);
    searchText.MakeLower();

    m_predefinedList.DeleteAllItems();

    // re-build the list based on the search criteria
    std::vector<TA_Base_Core::PredefinedMessage> normalMessages =
            m_ttisPredefinedMessages->getNormalMessages();

    int count = 0;
    for(std::vector<TA_Base_Core::PredefinedMessage>::iterator messageIter = normalMessages.begin();
    messageIter != normalMessages.end(); messageIter++)
    {
        CString messageString(messageIter->description.c_str());
        messageString.MakeLower();

        // if the string matches
        if ( messageString.Find(searchText) != -1 )
        {
            // add the string to the list
            int index = m_predefinedList.InsertItem(count, messageIter->description.c_str());
            m_predefinedList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag) );
            count++;
        }
    }

    // update the message preview
    messageSelectionChanged();
}


/** 
  * OnUpdateCurrentTTISVersion
  *
  * When it is discovered that the pre-defined message library has been upgraded,
  * we get this message. This rebuilds the list of pre-defined messages from the
  * list held in the singleton.
  */
LRESULT CTrainTimeSchedulePage::onUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam)
{
    // rebuild the list
    loadPreDefined();
    
    // update the current schedule
    m_scheduleEntryList.predefinedMessagesChanged();

    return 0;
}


/** 
  * onTimeScheduleChanged
  *
  * If a time schedule is changed, the time schedule list may need to be reloaded.
  * Also if the currently open schedule was changed elsewhere, a prompt is needed
  * to reload the schedule.
  */
LRESULT CTrainTimeSchedulePage::onTimeScheduleChanged(WPARAM wParam, LPARAM lParam)
{
    // get the change details
    TA_Base_Core::TimeScheduleChange* change =
        reinterpret_cast<TA_Base_Core::TimeScheduleChange*>(wParam);

    bool scheduleNameListChanged = (lParam != 0);

    // only reload if the list has changed
    if (scheduleNameListChanged)
    {
        // reload list
        loadTimeSchedules();
    }

    // if the changed schedule is being edited
    if ( (m_currentSchedule != NULL) &&
         (!m_currentSchedule->isNew()) &&
         (m_currentSchedule->getKey() == change->timeSchedulePkey) )
    {
        // the schedule that changed is currently loaded
        // obviously it cant be an add operation

        if (change->changeType == TA_Base_Core::Deleted)
        {
            // if it was deleted, pop up a message
            TTISMessageManager::getInstance().displayWarning(TTISMessageManager::CURRENT_SCHEDULE_DELETED);

            // create a new schedule - and delete this one
            resetToBlankSchedule();
        }
        else if (change->changeType == TA_Base_Core::Modified)
        {
            // if it was changed, and the local schedule is being edited
            if (m_editMode)
            {
                // prompt for a reload
                int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::CURRENT_SCHEDULE_MODIFIED);
                if (ret == IDYES)
                {
                    m_currentSchedule->invalidate();
                    populateTimeScheduleData(m_currentSchedule);
                }
            }
            else
            {
                // if it hasnt been modified, then just reload it
                m_currentSchedule->invalidate();
                populateTimeScheduleData(m_currentSchedule);
            }
        }
    }

    // make sure the list selection is as it was
    selectCurrentSchedule();

    delete change;
    change = NULL;

    return 0;
}


void CTrainTimeSchedulePage::setEditMode(bool editMode)
{
    // cannot edit a null schedule
    if (m_currentSchedule == NULL)
    {
        editMode = false;
    }

    m_editMode = editMode;
    m_scheduleEntryList.setEditMode(editMode);
    m_scheduleName.EnableWindow(editMode);

    // time and priority controls
    // are only enabled in edit mode
    m_startLabel.EnableWindow(m_editMode);
	m_endLabel.EnableWindow(m_editMode);
    m_startTime.EnableWindow(m_editMode);
	m_endTime.EnableWindow(m_editMode);
    m_repeatLabel.EnableWindow(m_editMode);
	m_repeatInterval.EnableWindow(m_editMode);
    m_priorityLabel.EnableWindow(m_editMode);
    m_priority.EnableWindow(m_editMode);
    if (m_editMode)
    {
		//xufeng++ 2006/11/08 TD17547
		m_priority.SetCurSel(0);
		//m_priority.SetCurSel(3);
		//++xufeng 2006/11/08 TD17547

        // show the time again
        m_startTime.SetFormat(_T("   HH:mm"));
        m_endTime.SetFormat(_T("   HH:mm"));
        m_repeatInterval.SetFormat(_T("   HH:mm"));
    }
    else
    {
        m_priority.SetCurSel(-1);

        // hide the time
        m_startTime.SetFormat(_T(" "));
        m_endTime.SetFormat(_T(" "));
        m_repeatInterval.SetFormat(_T(" "));
    }

    enableScheduleManagementButtons();
    enableScheduleEditingButtons();
}


void CTrainTimeSchedulePage::enableScheduleManagementButtons()
{
    // enable
    // if schedule editing is allowed
    bool enable = m_canEditSchedule;
    m_newSchedule.EnableWindow(enable);

    // also if a valid (saved) schedule is selected
    enable = enable &&
             (m_currentSchedule != NULL) &&
             !(m_currentSchedule->isNew()) &&
             !m_editMode;
    m_deleteSchedule.EnableWindow(enable);
    m_copySchedule.EnableWindow(enable);

    // a valid schedule is selected, and not already editing it
    enable = m_canEditSchedule &&
             (m_currentSchedule != NULL) &&
             !m_editMode;
    m_editSchedule.EnableWindow(enable);
}


void CTrainTimeSchedulePage::enableScheduleEditingButtons()
{
    // remove all entries if there are entries
    // and edit mode is enabled
    bool enable = m_editMode && m_scheduleEntryList.GetItemCount() > 0;
    m_removeAllEntries.EnableWindow(enable);
    
    // if there is an entry selected
    enable = enable && scheduleEntrySelected();
    m_removeEntry.EnableWindow(enable);

    // enable save if in edit mode
    // and there are entries
    // and the schedule has changed
    // and it has a title
    enable = m_editMode &&
             (m_scheduleEntryList.GetItemCount() > 0) &&
             m_currentSchedule->hasChanged() &&
             (m_currentSchedule->getName() != "");

    m_save.EnableWindow(enable);

    // enable the edit free text if in edit mode
    // and a free text message is selected
    enable = m_editMode && freeTextSelected();
    m_editFreeText.EnableWindow(enable);

    // enable add entry if in edit mode and there is either a
    // free text or predefined message selected
    enable = m_editMode &&
             (freeTextSelected() || predefinedSelected());
    m_addEntry.EnableWindow(enable);
}


void CTrainTimeSchedulePage::enableTimeScheduleFields()
{
    // only enable the schedule entry list
    // and free text list if a schedule is loaded
    bool enable = (m_currentSchedule != NULL);
    m_scheduleEntryList.EnableWindow(enable);
    m_freeTextList.EnableWindow(enable);
    m_predefinedList.EnableWindow(enable);
    m_searchNormal.EnableWindow(enable);
}


bool CTrainTimeSchedulePage::freeTextSelected()
{
    return (m_freeTextList.GetSelectedCount() > 0);
}


bool CTrainTimeSchedulePage::predefinedSelected()
{
    return (m_predefinedList.GetSelectedCount() > 0);
}


bool CTrainTimeSchedulePage::scheduleEntrySelected()
{
    return (m_scheduleEntryList.GetSelectedCount() > 0);
}


void CTrainTimeSchedulePage::resetToBlankSchedule()
{
    // clean up current schedule
    if (m_currentSchedule != NULL)
    {
        m_currentSchedule->invalidate();
        delete m_currentSchedule;
        m_currentSchedule = NULL;
    }

    // clear selected schedule
    deselectAll(&m_scheduleLibraryList);

    // clean up lists
    m_freeTextList.DeleteAllItems();
    m_scheduleEntryList.removeAllEntries();

    // reset name
    m_scheduleName.SetWindowText("");

    // get repeat interval from default adhoc attributes
    int repeatInterval = m_ttisPredefinedMessages->getDefaultDisplayAttributes().repeatInterval;
    if (repeatInterval > 255)
    {
        repeatInterval = 255;
    }
    unsigned short hours = repeatInterval / 60;
    unsigned short minutes = repeatInterval % 60;
    m_repeatInterval.SetTime(&CTime(1971, 1, 1, hours, minutes, 0, 0));
   
    enableTimeScheduleFields();

    // reset the editing mode
    setEditMode(false);
}


void CTrainTimeSchedulePage::populateTimeScheduleData(TA_IRS_Core::ITrainTimeSchedule* newSchedule)
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::populateTimeScheduleData" );
    
    // this should never be given as null
    TA_ASSERT( newSchedule != NULL,
        "Given a null schedule to load" );
    
    //
    // switch the current schedule over to the new schedule
    // the current (old) schedule must be deleted
    // and the new loaded.
    // If the new schedule is the current schedule just being reloaded
    // or there was no current schedule then dont delete the current schedule.
    //

    // save the old schedule for deleting
    TA_IRS_Core::ITrainTimeSchedule* oldSchedule = m_currentSchedule;

    // set the new schedule as the current time schedule (being edited)
    m_currentSchedule = newSchedule;

    // delete the old m_currentSchedule,
    // dont delete it if the same schedule is being reloaded
    if ( (oldSchedule != NULL) &&
         (newSchedule != oldSchedule) )
    {
        delete oldSchedule;
        oldSchedule = NULL;
    }
    
    // clear the schedule entry list
    m_scheduleEntryList.removeAllEntries();
    // clear the free text list
    m_freeTextList.DeleteAllItems();

    // clear the message text
    m_messageContent.SetWindowText("");
    
    try
    {
        // set the name
        m_scheduleName.SetWindowText(m_currentSchedule->getName().c_str());
        
        // populate the free text messages

        m_freeTextList.InsertItem(0, 
            m_currentSchedule->getAdHocMessageTitle(TA_IRS_Core::ITrainTimeSchedule::Adhoc1).c_str());

        m_freeTextList.InsertItem(1, 
            m_currentSchedule->getAdHocMessageTitle(TA_IRS_Core::ITrainTimeSchedule::Adhoc2).c_str());

        m_freeTextList.InsertItem(2, 
            m_currentSchedule->getAdHocMessageTitle(TA_IRS_Core::ITrainTimeSchedule::Adhoc3).c_str());

        m_freeTextList.InsertItem(3, 
            m_currentSchedule->getAdHocMessageTitle(TA_IRS_Core::ITrainTimeSchedule::Adhoc4).c_str());

        m_freeTextList.InsertItem(4, 
            m_currentSchedule->getAdHocMessageTitle(TA_IRS_Core::ITrainTimeSchedule::Adhoc5).c_str());

        m_freeTextList.InsertItem(5, 
            m_currentSchedule->getAdHocMessageTitle(TA_IRS_Core::ITrainTimeSchedule::Adhoc6).c_str());

        
        
        // populate the schedule entries
        std::vector<ITrainTimeScheduleEntry*> tempTimeScheduleEntries = m_currentSchedule->getEntries();
        
        // for each time schedule entry
        for (std::vector<ITrainTimeScheduleEntry*>::iterator entryIter = tempTimeScheduleEntries.begin();
        entryIter != tempTimeScheduleEntries.end(); entryIter++)
        {
            try
            {
                m_scheduleEntryList.addTimeScheduleEntry( *entryIter, m_currentSchedule );
            }
            // if it's a dataexception, theres something wrong with
            // the individual schedule entry
            // mark it for deletion because it's broken
            catch (DataException& dbe)
            {
                LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DataException", dbe.what() );

                std::stringstream msg;
                msg << TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_ENTRY
                    << (*entryIter)->getKey()
                    << TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_ENTRY_END;
                
                TTISMessageManager::getInstance().displayError(msg.str().c_str());
                
                // mark the entry for deletion
                // they can always back out by not saving
                m_currentSchedule->deleteEntry( *entryIter );
            }
        }
        
    }
    catch (DatabaseException& de)
    {
        LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", de.what() );
            
        std::stringstream msg;
        msg << TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_PKEY
            << m_currentSchedule->getKey();

        TTISMessageManager::getInstance().displayError(msg.str().c_str());
    }
    catch (DataException& de)
    {
        LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DataException", de.what() );
            
        std::stringstream msg;
        msg << TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_PKEY
            << m_currentSchedule->getKey();

        TTISMessageManager::getInstance().displayError(msg.str().c_str());
    }

    enableTimeScheduleFields();

    // reset the editing mode
    setEditMode(false);

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::populateTimeScheduleData" );
}


/////////////////////////////////////////////////////////////////////////////
// Schedule editing functions



void CTrainTimeSchedulePage::OnEditSchedule() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnEditSchedule" );
  
    TA_ASSERT(m_currentSchedule != NULL, "Cannot edit a null schedule");

    setEditMode(true);
    
    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnEditSchedule" );
}


void CTrainTimeSchedulePage::OnNewSchedule() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnNewSchedule" );
    
    // if the schedule has been modified in any way
    if( ( m_currentSchedule != NULL ) &&
        ( m_currentSchedule->hasChanged() ) )
    {
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_LOSE_UNSAVED_SCHEDULE);
                
        // if they dont want to continue
        if( ret != IDYES)
        {
            // exit here
            LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnNewSchedule" );
            return;
        }
    }
    
    // deselect fields and set defaults
    resetToBlankSchedule();
    
    // create and load the new schedule
    TA_IRS_Core::ITrainTimeSchedule* newSchedule = 
        m_trainTimeScheduleManager->createNewTrainTimeSchedule();

    // will set the new schedule as m_currentSchedule
    populateTimeScheduleData(newSchedule);

    // theres no point in creating a new schedule if you cant edit it
    setEditMode(true);

	//  [7/16/2009 xinyang++]:CL-17923
	/*
	
	//hongran++ TD17554
	CString scheduleName;
	m_scheduleName.GetWindowText(scheduleName);
	std::string scheduleNameStr(scheduleName);

	std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
	TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );

	TA_Base_Core::DescriptionParameters desc;
	TA_Base_Core::NameValuePair vpScheduleName("name", scheduleNameStr );

	desc.push_back(&vpScheduleName);
	TA_Base_Core::AuditMessageSender* auditMessageSender = 
		TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );
    auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISTimeScheduleCreated, guiEntity->getKey(),
		desc, 
		"", // Further description text
		TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
		"", // alarm ID - not required
		"", // incident key - not required
		""); // event key - not required 
	//++hongran TD17554
	*/
    
	//  [7/16/2009 xinyang--]:CL-17923
    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnNewSchedule" );
}


void CTrainTimeSchedulePage::onCopySchedule() 
{
    // TODO
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::onCopySchedule" );
    
    TA_ASSERT( m_currentSchedule != NULL, "Cannot copy a null schedule" );

    // if the schedule has been modified in any way
    if( m_currentSchedule->hasChanged() )
    {
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_LOSE_UNSAVED_SCHEDULE);
                
        // if they dont want to continue
        if( ret != IDYES)
        {
            // exit here
            LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::onCopySchedule" );
            return;
        }
    }

    // create and load the new schedule
    TA_IRS_Core::ITrainTimeSchedule* newSchedule = 
        m_trainTimeScheduleManager->copyTrainTimeSchedule(m_currentSchedule);

    // deselect fields and set defaults
    resetToBlankSchedule();

    // will set the new schedule as m_currentSchedule
    populateTimeScheduleData(newSchedule);

    // theres no point in creating a new schedule if you cant edit it
    setEditMode(true);

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::onCopySchedule" );
}


void CTrainTimeSchedulePage::OnSaveSchedule() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnSaveSchedule" );

    // the current schedule should not be null
    TA_ASSERT( m_currentSchedule != NULL, "The current schedule is null. Cannot save or edit a null schedule.");

    // edit mode should be active
    TA_ASSERT( m_editMode, "Not in edit mode. Button should be disabled.");

    // prompt first
    int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_SAVE_SCHEDULE);
        
    // if they dont want to continue
    if( ret != IDYES)
    {
        // exit here
        LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnSaveSchedule" );
        return;
    }

    try
    {
        // save the changes
        // any invalid data, like a non-unique name will cause an exception
        // this also notifies everyone else of the change - so other GUIs
        // reflect the change
        m_trainTimeScheduleManager->saveTimeSchedule(m_currentSchedule);

        // takes us out of edit mode
        setEditMode(false);
    }
    catch(DataConfigurationException& dce)
    {
        // something wasnt specified fully
        std::stringstream message;
        message << TTISMessageManager::ERROR_SAVING_SCHEDULE;
    
        std::vector<std::string> errors = dce.getMissingFields();
        for (std::vector<std::string>::const_iterator errorIter = errors.begin();
        errorIter != errors.end(); errorIter++)
        {
            message << (*errorIter) << std::endl;
        }
    
        TTISMessageManager::getInstance().displayError( message.str().c_str() );
    }
    catch(DataException& de)
    {
        // something breaks a database constraint, like name not unique
        std::stringstream message;
        message << TTISMessageManager::ERROR_SAVING_SCHEDULE_SHORT << std::endl
            << de.what() <<std::endl;
    
        TTISMessageManager::getInstance().displayError( message.str().c_str() );
    }
    catch(DatabaseException& dbe)
    {
        // error executing sql
        std::stringstream message;
        message << TTISMessageManager::ERROR_SAVING_SCHEDULE_SHORT << std::endl
            << dbe.what() <<std::endl;
    
        TTISMessageManager::getInstance().displayError( message.str().c_str() );
    }

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnSaveSchedule" );
}


void CTrainTimeSchedulePage::OnDeleteSchedule() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnDeleteSchedule" );

    // the current schedule should not be null
    TA_ASSERT( m_currentSchedule != NULL, "The current schedule is null. Cannot delete a null schedule.");

    int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_DELETE_SCHEDULE);

    if(ret == IDYES)
    {
        try
        {
            // delete it from the database
            // this will also let other GUIs know
            m_trainTimeScheduleManager->deleteTimeSchedule(m_currentSchedule);

			//hongran++ TD17554
			std::string scheduleName = m_currentSchedule->getName();
			//++hongran TD17554

            // now delete the object
            delete m_currentSchedule;
            m_currentSchedule = NULL;

            resetToBlankSchedule();
			
			//hongran++ TD17554
			std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );

			TA_Base_Core::DescriptionParameters desc;
			TA_Base_Core::NameValuePair vpScheduleName("name", scheduleName );

			desc.push_back(&vpScheduleName);
			TA_Base_Core::AuditMessageSender* auditMessageSender = 
				TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );
            auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISTimeScheduleDeleted, guiEntity->getKey(),
			    desc, 
			    "", // Further description text
			    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
			    "", // alarm ID - not required
			    "", // incident key - not required
			    ""); // event key - not required 
			//++hongran TD17554

			delete auditMessageSender;
        }
        catch(TransactiveException& te)
        {
            std::stringstream message;
            message << TTISMessageManager::ERROR_DELETING_SCHEDULE << std::endl
                << te.what() <<std::endl;

            TTISMessageManager::getInstance().displayError( message.str().c_str() );
        }
    }

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnDeleteSchedule" );
}


void CTrainTimeSchedulePage::OnAddEntry() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnAddEntry" );

    // make sure the schedule we are editing exists
    TA_ASSERT( m_currentSchedule != NULL, "Currently loaded schedule is null" );

	//CL20137, hongzhi, Schedule message limit on ISCS-MFT
	//re-locate the verifing process
	if(m_scheduleEntryList.GetItemCount() == m_nMaxScheduleMessage)
	{
        // schedule messages have exceeded the max count.
        std::stringstream message;
        message << TTISMessageManager::MAX_SCHEDULE_MESSAGE_REACHED << std::endl;
    
        TTISMessageManager::getInstance().displayWarning( message.str().c_str() );
		return;
	}

    // get selected message
    POSITION freeSel = m_freeTextList.GetFirstSelectedItemPosition();
    int freeTextSelected = -1;

    POSITION pdSel = m_predefinedList.GetFirstSelectedItemPosition();
    int predefinedSelected = -1;

    if (freeSel != NULL)
    {
        freeTextSelected = m_freeTextList.GetNextSelectedItem(freeSel);
    }

    if (pdSel != NULL)
    {
        predefinedSelected = m_predefinedList.GetNextSelectedItem(pdSel);
    }

    if ( (freeTextSelected > -1) && (predefinedSelected > -1) )
    {
        TA_ASSERT(false, "Both predefined and free text messages are selected");
    }


    //
    // read the data from the controls
    //


    // get the times
    CTime tempStartTime;
	CTime tempEndTime;
    CTime tempRepeatInterval;

    CString startTimeString;
    m_startTime.GetTime(tempStartTime);
    startTimeString = tempStartTime.Format("%I:%M %p");

	CString endTimeString;
    m_endTime.GetTime(tempEndTime);
    endTimeString = tempEndTime.Format("%I:%M %p");

	//xufeng++ 2006/10/14 TD14367
	// CTime tempCurrentTime = CTime::GetCurrentTime(); //zhiqiang-- TD20713
 	//++xufeng 2006/10/14 TD14367

    m_repeatInterval.GetTime(tempRepeatInterval);
    unsigned short repeatInterval = (tempRepeatInterval.GetHour() * 60) + tempRepeatInterval.GetMinute();


	/*zhiqiang-- TD20713
	// Check if they are valid
	if( tempStartTime == tempEndTime )
	{
        TTISMessageManager::getInstance().displayError(TTISMessageManager::START_TIME_SAME_AS_END_TIME);
		return;
	}
	//xufeng++ 2006/10/14 TD14367
	else if (tempStartTime > tempEndTime) {
        TTISMessageManager::getInstance().displayError(TTISMessageManager::START_TIME_AFTER_END_TIME);
		return;
	}
	else if (tempStartTime < tempCurrentTime) {
        TTISMessageManager::getInstance().displayError(TTISMessageManager::START_TIME_BEFORE_CURRENT_TIME);
		return;
	}	
	//++xufeng 2006/10/14 TD14367
	//*/ 
	//--zhiqiang TD20713

    /*
    // The PV App A specifies that end time may be early the next morning
    else if( tempStartTime > tempEndTime )
	{
		AfxMessageBox("Start time cannot be after End time");
		return;
	}
    */

    // read the priority
    int sel = m_priority.GetCurSel();
    // convert from the index to the number
    unsigned short priority = sel + 4;
    // allow for the missing 6
    if (priority > 5)
    {
        priority++;
    }


    // get the message information
    ITrainTimeScheduleEntry::MessageTableType messageTable;
    unsigned short messageId;

    if (freeTextSelected > -1)
    {
        TA_ASSERT(m_currentSchedule != NULL, "There was a free text message selected without a schedule being loaded");
        
        // selected is a 0 based index, Adhoc1 to 6 are 1 based
        TA_ASSERT( freeTextSelected < ITrainTimeSchedule::Adhoc6,
            "Selected index is greater than the last Free Text index" );

        messageTable = ITrainTimeScheduleEntry::FreeText;

        messageId = static_cast<ITrainTimeSchedule::AdhocMessageNumber>(freeTextSelected+1);

    }
    else if (predefinedSelected > -1)
    {
        // get the data for the selected item
        unsigned short itemData = static_cast<unsigned short> (m_predefinedList.GetItemData(predefinedSelected));
    
        // find the appropriate message
        const PredefinedMessage* message = 
            m_ttisPredefinedMessages->getNormalMessageById( itemData );

        TA_ASSERT( message != NULL,
            "The selected message is not a valid message");

        messageTable = ITrainTimeScheduleEntry::NormalPreDefined;

        messageId = message->messageTag;
    }
    else
    {
        TA_ASSERT(false, "No predefined or free text messages are selected");
    }


    //
    // if everything went OK
    // store it in a database object
    //

    // create a new schedule entry
    ITrainTimeScheduleEntry* newEntry = m_currentSchedule->createNewEntry();

    // set times in the database object
    newEntry->setStartTime(tempStartTime.GetTime());
    newEntry->setEndTime(tempEndTime.GetTime());
    newEntry->setRepeatInterval(repeatInterval);

    // set the message table and id
    newEntry->setMessageTable(messageTable);
    newEntry->setMessageId(messageId);
    
    // set it in the database object
    newEntry->setPriority(priority);

    // add it to the list
    m_scheduleEntryList.addTimeScheduleEntry(newEntry, m_currentSchedule);

    // re-enable buttons
    enableScheduleEditingButtons();

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnAddEntry" );
}


void CTrainTimeSchedulePage::OnRemoveEntry() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnRemoveEntry" );

    // make sure the schedule we are editing exists
    TA_ASSERT( m_currentSchedule != NULL, "Currently loaded schedule is null" );

    int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_REMOVE_ENTRY);
        
    if(ret == IDYES)
    {
        // remove the selected item from the list
        ITrainTimeScheduleEntry* removed = 
            m_scheduleEntryList.removeSelectedTimeScheduleEntry();

        // delete the entry from the database object
        m_currentSchedule->deleteEntry(removed);

        enableScheduleEditingButtons();
    }

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnRemoveEntry" );
}


void CTrainTimeSchedulePage::OnRemoveAllEntries() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnRemoveAllEntries" );

    TA_ASSERT(m_scheduleEntryList.GetItemCount() > 0, "No schedule entries, button shouldnt be enabled")

    int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_REMOVE_ALL_ENTRIES);
        
    if(ret == IDYES)
    {
        // delete all entries
    
        // make sure the schedule we are editing exists
        TA_ASSERT( m_currentSchedule != NULL, "Currently loaded schedule is null" );
    
        // get the entries
        std::vector<ITrainTimeScheduleEntry*> tempTimeScheduleEntries = 
            m_currentSchedule->getEntries();
    
        // check that the number of entries matches the list we have
        TA_ASSERT( (unsigned int) m_scheduleEntryList.GetItemCount() == tempTimeScheduleEntries.size(),
            "The time schedule list is out of sync with the time schedule entries");
    
        // delete all items from the list
        m_scheduleEntryList.removeAllEntries();
    
        // for each entry
        for(std::vector<ITrainTimeScheduleEntry*>::iterator iter = tempTimeScheduleEntries.begin();
        iter != tempTimeScheduleEntries.end(); iter++)
        {
            // check for null
            TA_ASSERT( (*iter) != NULL, "A time schedule entry is null");
        
            // delete the entry from the database object
            m_currentSchedule->deleteEntry(*iter);
        }
    }

    enableScheduleEditingButtons();

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnRemoveAllEntries" );
}


void CTrainTimeSchedulePage::OnScheduleNameChange()
{
    // the user has changed the name of this schedule
    // get the new name and set in in the database object
    CString newName;
    m_scheduleName.GetWindowText(newName);

    if (m_currentSchedule != NULL)
    {
        m_currentSchedule->setName( std::string(newName) );
    
        // enable the save button if necessary
        enableScheduleEditingButtons();
    }
}


void CTrainTimeSchedulePage::OnEditFreeText() 
{
    LOG_FUNCTION_ENTRY( SourceInfo, "CTrainTimeSchedulePage::OnEditFreeText" );

    // get the free text message that's selected
    POSITION freeSel = m_freeTextList.GetFirstSelectedItemPosition();
    int selected = -1;

    if (freeSel != NULL)
    {
        selected = m_freeTextList.GetNextSelectedItem(freeSel);
    }

    TA_ASSERT(selected > -1, "No message selected. Button should not be enabled");
    TA_ASSERT(m_currentSchedule != NULL, "No schedule loaded. Button should not be enabled");

    // make sure it is in range
    TA_ASSERT( selected < ITrainTimeSchedule::Adhoc6,
        "Selected index is greater than the last Free Text index" );

    ITrainTimeSchedule::AdhocMessageNumber messageNum =
        static_cast<ITrainTimeSchedule::AdhocMessageNumber>(selected+1);

    // pop up the edit free text dialog with the selected message
    m_freeTextDlg.displayDialog(m_currentSchedule->getAdHocMessageTitle(messageNum).c_str(),
                                m_currentSchedule->getAdHocMessage(messageNum).c_str() );

    // if the message was changed
    if ( m_freeTextDlg.messageWasChanged() )
    {
        // update the appropriate free text message and
        // set the modified flag
    
        // make sure the schedule we are editing exists
        TA_ASSERT( m_currentSchedule != NULL,
            "Currently loaded schedule is null" );
    
        std::string newMessage(m_freeTextDlg.getMessageContent());
        std::string newMessageTitle(m_freeTextDlg.getMessageName());
    
        m_currentSchedule->setAdHocMessage(messageNum, newMessage);
        m_currentSchedule->setAdHocMessageTitle(messageNum, newMessageTitle);

        // update the list
        m_freeTextList.SetItemText(selected, 0, newMessageTitle.c_str());

        // update the entry display
        m_scheduleEntryList.freeTextDescriptionUpdated(messageNum, newMessageTitle);
    
        // update the preview window
        messageSelectionChanged();
    }

    LOG_FUNCTION_EXIT( SourceInfo, "CTrainTimeSchedulePage::OnEditFreeText" );
}




/////////////////////////////////////////////////////////////////////////////
// Miscellaneous callback functions


void CTrainTimeSchedulePage::onClickFreeTextList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE lpnmitem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;

    // If user clicks on free text list without holding ctrl down,
    // clear the predefined list selection
    if ( !(lpnmitem->uKeyFlags & LVKF_CONTROL) )
    {
        // deselect pre defined list
        deselectAll(&m_predefinedList);
    }

    messageSelectionChanged();
}


void CTrainTimeSchedulePage::onItemchangedFreeText(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	
    *pResult = 0;

    // Only interested in state changes
	if (pNMListView->uChanged != LVIF_STATE) 
	{
        return;
	}

    // if something is being selected
    if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
         ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
    {
        // deselect the other list
        deselectAll(&m_predefinedList);
    }

    messageSelectionChanged();
}


void CTrainTimeSchedulePage::onClickPredefinedList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE lpnmitem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;


    // If user clicks on predefined list without holding ctrl down,
    // clear the free text list selection
    if ( !(lpnmitem->uKeyFlags & LVKF_CONTROL) )
    {
        // deselect free text list
        deselectAll(&m_freeTextList);
    }

    messageSelectionChanged();
}


void CTrainTimeSchedulePage::onItemchangedPredefinedList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	
    *pResult = 0;

    // Only interested in state changes
	if (pNMListView->uChanged != LVIF_STATE) 
	{
        return;
	}

    // if something is being selected
    if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
         ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
    {
        // deselect the other list
        deselectAll(&m_freeTextList);
    }

    messageSelectionChanged();
}


void CTrainTimeSchedulePage::messageSelectionChanged()
{
    POSITION freeSel = m_freeTextList.GetFirstSelectedItemPosition();
    int freeTextSelected = -1;

    POSITION pdSel = m_predefinedList.GetFirstSelectedItemPosition();
    int predefinedSelected = -1;

    if (freeSel != NULL)
    {
        freeTextSelected = m_freeTextList.GetNextSelectedItem(freeSel);
    }

    if (pdSel != NULL)
    {
        predefinedSelected = m_predefinedList.GetNextSelectedItem(pdSel);
    }

    if ( (freeTextSelected > -1) && (predefinedSelected > -1) )
    {
        TA_ASSERT(false, "Both predefined and free text messages are selected");
    }
    else if ( (freeTextSelected > -1) /*&& (m_editMode==true) TD17921*/ )
    {
        TA_ASSERT(m_currentSchedule != NULL, "There was a free text message selected without a schedule being loaded");
        
        // selected is a 0 based index, Adhoc1 to 6 are 1 based
        TA_ASSERT( freeTextSelected < ITrainTimeSchedule::Adhoc6,
            "Selected index is greater than the last Free Text index" );

        std::string message =
            m_currentSchedule->getAdHocMessage( static_cast<ITrainTimeSchedule::AdhocMessageNumber>(freeTextSelected+1) );

		unsigned short repeatInterval= m_ttisPredefinedMessages->getDefaultDisplayAttributes().repeatInterval;
        m_messageContent.SetWindowText(message.c_str());
		if (m_editMode == true)
		{
			setPriority(4, true);
			setRepeatInterval(repeatInterval, true);
		}
    }
    else if ((predefinedSelected > -1) /*&& (m_editMode==true) Same as TD17921*/)
    {
        // get the data for the selected item
        unsigned short itemData = static_cast<unsigned short> (m_predefinedList.GetItemData(predefinedSelected));
    
        // find the appropriate message
        const PredefinedMessage* message = 
            m_ttisPredefinedMessages->getNormalMessageById( itemData );

        TA_ASSERT( message != NULL,
            "The selected message is not a valid message");

        m_messageContent.SetWindowText(message->message.c_str());
		if (m_editMode == true)
		{
			setPriority(message->priority , true);
			setRepeatInterval(message->repeatInterval, true);
		}
    }
    else
    {
        // clear the text in the preview window
        m_messageContent.SetWindowText("");
    }

    enableScheduleEditingButtons();
}


void CTrainTimeSchedulePage::onClickScheduleList(NMHDR* pNMHDR, LRESULT* pResult)
{
    // if nothing is selected, reset to the blank schedule
    POSITION sel = m_scheduleLibraryList.GetFirstSelectedItemPosition();
    int selected = -1;

    if (sel == NULL)
    {
        // if the current schedule has been modified or is new - prompt before clearing the fields
        if( ( m_currentSchedule != NULL ) &&
            ( m_currentSchedule->hasChanged() || m_currentSchedule->isNew() ) )
        {
            // prompt for whether to lose those changes
            int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_LOSE_UNSAVED_SCHEDULE);

            // if they dont want to continue
            if( ret != IDYES)
            {
                // select the loaded schedule
                selectCurrentSchedule();

                // exit here
                return;
            }
        }

        resetToBlankSchedule();
    }
}


void CTrainTimeSchedulePage::onItemchangedScheduleList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	
    *pResult = 0;

    // Only interested in state changes
	if (pNMListView->uChanged != LVIF_STATE) 
	{
        return;
	}

    // if something is being selected
    if ( ((pNMListView->uOldState & LVIS_SELECTED) == 0) &&
         ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) )
    {
        scheduleListSelectionChanged();
    }
}


void CTrainTimeSchedulePage::scheduleListSelectionChanged() 
{
    // this is used to ignore extra messages that MFC sends
    // when the selection is reset after answering no to a lose changes call
    // This is a last resort, the same thing happens if you prevent the state 
    // change in the first place using LVM_ITEMCHANGING
    static int callsToIgnore = 0;

    POSITION sel = m_scheduleLibraryList.GetFirstSelectedItemPosition();
    int selected = -1;

    if (sel != NULL)
    {
        selected = m_scheduleLibraryList.GetNextSelectedItem(sel);
    }

    if (selected > -1)
    {
        // get the pkey of the schedule that was just selected
        //unsigned long pkey = m_scheduleLibraryList.GetItemData(selected);
		unsigned long pkey = m_scheduleKeyMap[selected];
        // if the currently loaded schedule is the same as this one
        // then do nothing
        if ( (m_currentSchedule != NULL) &&
            !(m_currentSchedule->isNew()) &&
             (m_currentSchedule->getKey() == pkey) )
        {
            // exit here
            return;
        }

        if (callsToIgnore > 0)
        {
            callsToIgnore--;
            // select the loaded schedule
            selectCurrentSchedule();
            return;
        }

        // if the current schedule has been modified (or is new) - prompt before loading this one
        if( ( m_currentSchedule != NULL ) &&
            ( m_currentSchedule->hasChanged() || m_currentSchedule->isNew() ) )
        {
            // prompt for whether to lose those changes
            int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_LOSE_UNSAVED_SCHEDULE);

            // if they dont want to continue
            if( ret != IDYES)
            {
                // skip the next message
                callsToIgnore++;
                
                // select the loaded schedule
                selectCurrentSchedule();

                // exit here
                return;
            }
        }

        // if we are here - the selected schedule is different from the one that is currently loaded
        // and the operator has agreed to lose any changes
        // load the schedule
        try
        {
            ITrainTimeSchedule* selectedSchedule =
                m_trainTimeScheduleManager->getTrainTimeSchedule(pkey);

            if (selectedSchedule != NULL)
            {   
                // load the new schedule
                // sets it as m_currentSchedule
                // cleans up the old m_currentSchedule
                populateTimeScheduleData(selectedSchedule);
            }
            else
            {
                // selected schedule doesnt exist - or is broken
                LOG_GENERIC ( SourceInfo, DebugUtil::DebugError,
                    "Time schedule with key %d not found. Did this gui miss a schedule update message?",
                    pkey);

                CString scheduleName = m_scheduleLibraryList.GetItemText(selected, 0);

                std::stringstream msg;
                msg << TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_PKEY
                    << scheduleName.GetBuffer(scheduleName.GetLength());
        
                TTISMessageManager::getInstance().displayError(msg.str().c_str());

                resetToBlankSchedule();
                deselectAll(&m_scheduleLibraryList);
            }
        }
        catch(...)
        {
            LOG ( SourceInfo, DebugUtil::ExceptionCatch, "...", "DatabaseException or DataException while loading time schedule" );

            CString scheduleName = m_scheduleLibraryList.GetItemText(selected, 0);

            std::stringstream msg;
            msg << TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_PKEY
                << scheduleName.GetBuffer(scheduleName.GetLength());
    
            TTISMessageManager::getInstance().displayError(msg.str().c_str());
            
            resetToBlankSchedule();
            deselectAll(&m_scheduleLibraryList);
        }
    }
}


void CTrainTimeSchedulePage::onClickScheduleEntryList(NMHDR* pNMHDR, LRESULT* pResult)
{
    *pResult = 0;

    scheduleEntryListSelectionChanged();
}


void CTrainTimeSchedulePage::onItemchangedScheduleEntryList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	
    *pResult = 0;

    // Only interested in state changes
	if (pNMListView->uChanged != LVIF_STATE) 
	{
        return;
	}

    scheduleEntryListSelectionChanged();
}


void CTrainTimeSchedulePage::scheduleEntryListSelectionChanged()
{
    enableScheduleEditingButtons();
}


void CTrainTimeSchedulePage::deselectAll(CListCtrl* theList)
{
    UINT uSelectedCount = theList->GetSelectedCount();
    UINT i;
    int  nItem = -1;

    if (uSelectedCount > 0)
    {
        // Remove all selection masks
        for (i = 0; i < uSelectedCount;i++)
        {
            nItem = theList->GetNextItem(nItem, LVNI_SELECTED);
        
            TA_ASSERT(nItem != -1, "Operation failed");
                    
            theList->SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
        }
    }
}


void CTrainTimeSchedulePage::selectCurrentSchedule()
{
    if ( (m_currentSchedule != NULL) && !(m_currentSchedule->isNew()) )
    {
        LVFINDINFO findInfo;
        findInfo.flags = LVFI_PARAM;
        findInfo.lParam  = m_currentSchedule->getKey();
        int item = m_scheduleLibraryList.FindItem(&findInfo);

        if (item > -1)
        {
            if (m_scheduleLibraryList.GetItemState(item, LVIS_SELECTED) != LVIS_SELECTED)
            {
                m_scheduleLibraryList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
        else
        {
            deselectAll(&m_scheduleLibraryList);
        }
    }
    else
    {
        deselectAll(&m_scheduleLibraryList);
    }
}


LRESULT CTrainTimeSchedulePage::onRightsChanged(WPARAM wParam, LPARAM lParam)
{
    // the rights have changed - re check them
    m_canEditSchedule = RightsManager::getInstance().canEditSchedule();

    // if the operator is editing a schedule, but can no longer edit a schedule
    // then just disable edit mode - dont lose their changes
    // they may need to yell at someone to give them back access
    if (m_editMode && !m_canEditSchedule)
    {
        setEditMode(false);

        TA_ASSERT(m_currentSchedule != NULL, "Cannot be in edit mode with no current schedule.");

        // if they have unsaved changes let them know
        if (m_currentSchedule->hasChanged())
        {
            TTISMessageManager::getInstance().displayWarning(TTISMessageManager::SCHEDULE_EDIT_PERMISSIONS_LOST);
        }
    }

    // reset button states
    enableScheduleManagementButtons();
    enableScheduleEditingButtons();

    return 0;
}


void CTrainTimeSchedulePage::OnOK()
{
}


void CTrainTimeSchedulePage::OnCancel()
{
}


BOOL CTrainTimeSchedulePage::PreTranslateMessage(MSG* pMsg) 
{
    switch ( pMsg->message )
    {
    case WM_KEYDOWN:
        {
            switch ( pMsg->wParam )
            {
            case VK_ESCAPE:
                {
                    return TRUE;
                }
                break;
            case VK_RETURN:
                {
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

    return CDialog::PreTranslateMessage(pMsg);
}


void CTrainTimeSchedulePage::setPriority( unsigned short newPriority,
                                          bool canChange /* = true */)
{
	// enable the control only if the priority can be changed
    m_priority.EnableWindow(canChange);

	int selectionIndex = 0;
    // 4 to 6 normal . cyclic
	if ( (newPriority > 3) && (newPriority < 6) )
    {
        selectionIndex = newPriority - 4;
    }
    // 7 to 8 normal . cyclic
    else if ( (newPriority > 6) && (newPriority < 9) )
    {
        selectionIndex = newPriority - 5;
    }
    // invalid priority
    else
    {
        // log the error
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                     "Invalid priority given: %d. Check the pre-defined message library, and default TTIS attributes.",
                     newPriority);

        // leave it alone
        return;
    }

    // set the priority
    m_priority.SetCurSel(selectionIndex);
}


void CTrainTimeSchedulePage::setRepeatInterval( unsigned short repeatInterval,
                                                 bool canChange /* = true */)
{
    if (repeatInterval > MAX_REPEAT_INTERVAL)
    {
        repeatInterval = MAX_REPEAT_INTERVAL;
    }

    // set the repeat interval
    unsigned short hours = repeatInterval / 60;
    unsigned short minutes = repeatInterval % 60;

    CTime temp(1971, 1, 1, hours, minutes, 0, 0);
    m_repeatInterval.SetTime(&temp);

    // the repeat interval is only enabled if this is true, and the current
    // time type is cyclic
    
    m_repeatInterval.EnableWindow( canChange );
    m_repeatLabel.EnableWindow( canChange );
}

//xufeng++ 2006/11/06 TD14367

CTrainTimeSchedulePage::~CTrainTimeSchedulePage()
{
    m_startTime.setCallback(NULL);
	m_endTime.setCallback(NULL);
}

void CTrainTimeSchedulePage::dateTimeChanged(CallbackDateTimeCtrl* control)
{
    // this is used to make sure the end time is never before the start time
    // and the end time hasnt passed

    // firstly, get the start, end, and current times
    CTime startTime = getStartTime();
    CTime endTime = getEndTime();

	//zhiqiang TD20713
    // CTime currentTime = CTime::GetCurrentTime();
	/*
    // if the start time is before the current time, set it to the current time
    if (startTime < currentTime)
    {
		startTime = currentTime;
        m_startTime.SetTime(&startTime);
    }
    // if the end time is before the start time, set it to the start time
    if (endTime < startTime)
    {
        endTime = startTime;
        m_endTime.SetTime(&endTime);
    }
	//*/

	m_startTime.SetTime(&startTime);
	m_endTime.SetTime(&endTime);
}

CTime CTrainTimeSchedulePage::getStartTime() const
{
    CTime startTime;

    m_startTime.GetTime(startTime);

    return startTime;
}


CTime CTrainTimeSchedulePage::getEndTime() const
{
    CTime endTime;

    m_endTime.GetTime(endTime);

    return endTime;
}
//++xufeng 2006/11/06 TD14367

} // end TA_IRS_App
