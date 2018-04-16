/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/ScheduleDownloadPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the train time schedule download page.
  * It is onthe train download tab.
  *
  */

#include "stdafx.h"

#include "RightsManager.h"
#include "ScheduleDownloadPage.h"
#include "TTISManager.h"
#include "TTISMessageManager.h"
#include "TrainTimeScheduleManager.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include <iomanip>
#include <sstream>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Bus::TISAgentAccessFactory;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{


    /////////////////////////////////////////////////////////////////////////////
    // CScheduleDownloadPage dialog


    CScheduleDownloadPage::CScheduleDownloadPage(CWnd* pParent /*=NULL*/)
        : CDialog(CScheduleDownloadPage::IDD, pParent),
          m_downloadManager(NULL),
          m_versionManager(NULL),
          m_currentScheduleVersion(0),
          m_currentScheduleKey(0),
          m_canSetCurrentSchedule(false),
          m_canDownload(false),
          m_canUpgrade(false),
          m_trainTimeScheduleManager( NULL )
    {
        FUNCTION_ENTRY( "CScheduleDownloadPage" );

        //{{AFX_DATA_INIT(CScheduleDownloadPage)
        //}}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::setDownloadManager(TrainDownloadManager& downloadManager)
    {
        FUNCTION_ENTRY( "setDownloadManager" );

        m_downloadManager = &downloadManager;

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::setVersionManager(TrainVersionManager& versionManager)
    {
        FUNCTION_ENTRY( "setVersionManager" );

        m_versionManager = &versionManager;

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CScheduleDownloadPage)
        DDX_Control(pDX, IDC_RETRY, m_retryCheck);
        DDX_Control(pDX, IDC_VERSION_PROGRESS, m_versionProgress);
        DDX_Control(pDX, IDC_ISCS_CURRENT_VERSION, m_currentISCSVersion);
        DDX_Control(pDX, IDC_SCHEDULE_LIST, m_scheduleList);
        DDX_Control(pDX, IDC_DOWNLOAD_STATUS_LIST, m_downloadStatusList);
        DDX_Control(pDX, IDC_TRAIN_VERSION_LIST, m_trainVersionList);
        //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CScheduleDownloadPage, CDialog)
        //{{AFX_MSG_MAP(CScheduleDownloadPage)
        ON_BN_CLICKED(IDC_ALL_TRAINS, OnAllTrains)
        ON_BN_CLICKED(IDC_CLEAR_SELECTION, OnClearSelection)
        ON_BN_CLICKED(IDC_CLEAR_COMPLETED, OnClearCompleted)
        ON_BN_CLICKED(IDC_DOWNLOAD_CURRENT, OnDownloadCurrent)
        ON_BN_CLICKED(IDC_UPGRADE, OnUpgradeCurrent)
        ON_BN_CLICKED(IDC_SET_CURRENT_SCHEDULE, OnSetCurrentSchedule)
        ON_MESSAGE(WM_UPDATE_CURRENT_TIME_SCHEDULE, onUpdateCurrentTimeSchedule)
        ON_MESSAGE(WM_UPDATE_TIMESCHEDULE, onTimeScheduleChanged)
        ON_WM_DESTROY()
        ON_BN_CLICKED(IDC_SEL_ALL_DOWNLOAD, onSelectAllDownloads)
        ON_BN_CLICKED(IDC_CLEAR_SEL_DOWNLOAD, onDeselectAlllDownloads)
        ON_BN_CLICKED(IDC_CANCEL_RETRY, onStopAutoRetry)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_DOWNLOAD_STATUS_LIST, onItemchangedDownloadStatusList)
        ON_NOTIFY(NM_CLICK, IDC_DOWNLOAD_STATUS_LIST, onClickDownloadStatusList)
        ON_NOTIFY(NM_CLICK, IDC_TRAIN_VERSION_LIST, onClickTrainList)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRAIN_VERSION_LIST, onItemchangedTrainList)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_SCHEDULE_LIST, onSelchangeScheduleList)
        ON_NOTIFY(NM_CLICK, IDC_SCHEDULE_LIST, onClickScheduleList)
        ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CScheduleDownloadPage message handlers


    BOOL CScheduleDownloadPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void CScheduleDownloadPage::Init()
    {
        FUNCTION_ENTRY( "Init" );

        m_scheduleList.SetExtendedStyle(m_scheduleList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_scheduleList.InsertColumn(0,"Time Schedule Name");
        m_scheduleList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

        // default to showing vertical scroll and hiding horizontal
        m_scheduleList.setScrollBarVisibility();

        // register for rights changes
        RightsManager::getInstance().registerForRightsChanges(this);
        m_canSetCurrentSchedule = RightsManager::getInstance().canSetCurrentSchedule();
        m_canDownload = RightsManager::getInstance().canDownloadUpgradeSchedule();
        m_canUpgrade = m_canDownload; // same action

        // need to be told when the current time schedule changes
        if ( NULL == m_trainTimeScheduleManager )
        {
            m_trainTimeScheduleManager = &( TrainTimeScheduleManager::getInstance() );
        }

        m_trainTimeScheduleManager->registerCurrentTimeScheduleUser(this);
        m_trainTimeScheduleManager->registerForTimeScheduleChanges(this);

        // populate the time schedule list
        loadTimeSchedules();

        // get the current version
        updateCurrentVersion();

        // disable the stop retrying button
        GetDlgItem(IDC_CANCEL_RETRY)->EnableWindow(FALSE);

        // disable download/upgrade buttons
        GetDlgItem(IDC_DOWNLOAD_CURRENT)->EnableWindow(FALSE);
        GetDlgItem(IDC_UPGRADE)->EnableWindow(FALSE);

        // disable select/deselect all buttons
        GetDlgItem(IDC_ALL_TRAINS)->EnableWindow(FALSE);
        GetDlgItem(IDC_CLEAR_SELECTION)->EnableWindow(FALSE);

        // disable the other select/deselect all buttons
        GetDlgItem(IDC_SEL_ALL_DOWNLOAD)->EnableWindow(FALSE);
        GetDlgItem(IDC_CLEAR_SEL_DOWNLOAD)->EnableWindow(FALSE);

        // disable the clear completed button
        GetDlgItem(IDC_CLEAR_COMPLETED)->EnableWindow(FALSE);

        // initialise the download list
        m_trainVersionList.setupList(this);

        //xufeng++ TD12956
        m_downloadStatusList.Init();
        //++xufeng TD12956

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        CDialog::OnDestroy();

        if ( NULL != m_trainTimeScheduleManager )
        {
            m_trainTimeScheduleManager->deregisterCurrentTimeScheduleUser(this);
            m_trainTimeScheduleManager->deregisterForTimeScheduleChanges(this);
            m_trainTimeScheduleManager = NULL;
        
            TrainTimeScheduleManager::removeInstance();
        }

        // deregister for rights changes
        RightsManager::getInstance().deregisterForRightsChanges(this);

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::updateTrainList(TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                         const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
    {
        FUNCTION_ENTRY( "updateTrainList" );

        m_trainVersionList.updateTrainList(action, trainDetails);
        // all this just to show the service number
        m_downloadStatusList.updateTrainDetails(trainDetails);

        // update % complete
        updatePercentComplete();

        // enable/disable the select/deselect buttons depending on whether there are items
        bool enable = (m_trainVersionList.GetItemCount() > 0);
        GetDlgItem(IDC_ALL_TRAINS)->EnableWindow(enable);
        GetDlgItem(IDC_CLEAR_SELECTION)->EnableWindow(enable);

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::updateTrainVersion(unsigned int trainId)
    {
        FUNCTION_ENTRY( "updateTrainVersion" );

        m_trainVersionList.updateTrainVersion(trainId);

        // update % complete
        updatePercentComplete();

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::updateDownloadList(const DownloadStatusDisplay& trainDownload)
    {
        FUNCTION_ENTRY( "updateDownloadList" );

        // update if its the right type
        if ( (trainDownload.downloadType == ScheduleDownload) ||
            (trainDownload.downloadType == ScheduleUpgrade) )
        {
            // get the service number for the train
            // and update the list
            m_downloadStatusList.updateDownloadList( trainDownload,
                m_trainVersionList.getServiceNumberForTrain(trainDownload.trainId));

            // enable/disable the select/deselect buttons depending on whether there are items
            bool enable = (m_downloadStatusList.GetItemCount() > 0);
            GetDlgItem(IDC_SEL_ALL_DOWNLOAD)->EnableWindow(enable);
            GetDlgItem(IDC_CLEAR_SEL_DOWNLOAD)->EnableWindow(enable);

            GetDlgItem(IDC_CLEAR_COMPLETED)->EnableWindow(m_downloadStatusList.completedDownloadsInList());
        }

        FUNCTION_EXIT;
    }


    std::string CScheduleDownloadPage::getCustomData( unsigned int trainId,
                                                      const std::string& columnName)
    {
        FUNCTION_ENTRY( "getCustomData" );

        // if the version manager hasnt been set, return an empty string
        if (m_versionManager == NULL)
        {
            FUNCTION_EXIT;
            return "";
        }

        // otherwise, get the version for this train
        TrainVersionManager::TrainLibraryAndTimeScheduleVersion version =
            m_versionManager->getVersionForTrain(trainId);

        std::stringstream value;

        if (columnName == DownloadTrainListCtrl::VERSION_COLUMN)
        {
            value << std::setw(3) << std::setfill('0') << version.trainTimeScheduleVersion;
        }
        else if (columnName == DownloadTrainListCtrl::UPGRADED_COLUMN)
        {
            value << std::setw(3) << std::setfill('0') << version.nextTrainTimeScheduleVersion;
        }

        FUNCTION_EXIT;
        return value.str();
    }


    LRESULT CScheduleDownloadPage::onUpdateCurrentTimeSchedule(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onUpdateCurrentTimeSchedule" );

        updateCurrentVersion();

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CScheduleDownloadPage::onTimeScheduleChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onTimeScheduleChanged" );

        // get the change details
        TA_Base_Core::TimeScheduleChange* change =
            reinterpret_cast<TA_Base_Core::TimeScheduleChange*>(wParam);

        bool scheduleNameListChanged = (lParam != 0);

        // this only affects this object if the list has changed
        if (scheduleNameListChanged)
        {
            // reload schedules
            loadTimeSchedules();

            // redisplay current schedule if the schedule changed is for the current
            // time schedule
            if ( change->timeSchedulePkey == m_currentScheduleKey )
            {
                updateCurrentVersion();
            }
        }

        delete change;
        change = NULL;

        FUNCTION_EXIT;
        return 0;
    }


    void CScheduleDownloadPage::updatePercentComplete()
    {
        FUNCTION_ENTRY( "updatePercentComplete" );

        // get the % complete from the list
        //xufeng++ TD17545
        int percent = m_trainVersionList.getPercentComplete(m_currentScheduleVersion, 0);
        //int percent = m_trainVersionList.getPercentComplete(m_currentScheduleVersion);
        //++xufeng TD17545

        // convert it to a string
        CString completeString;
        completeString.Format("%01d", percent);

        // set the text
        m_versionProgress.SetWindowText(completeString);

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::loadTimeSchedules()
    {
        FUNCTION_ENTRY( "loadTimeSchedules" );

        // clear the map and the lists
        m_timeScheduleInformation.clear();
        m_scheduleList.DeleteAllItems();

        // get the current list from the schedule manager
        m_timeScheduleInformation = m_trainTimeScheduleManager->getCurrentTimeScheduleNames();

        // populate the list of time schedules
        int i = 0;
        for(TrainTimeScheduleManager::TimeScheduleMap::iterator iter = m_timeScheduleInformation.begin();
        iter != m_timeScheduleInformation.end(); iter++, i++)
        {
            m_scheduleList.InsertItem( i, iter->first.c_str() );
        }

        // blank out the set current button until something is selected
        scheduleSelectionChanged();

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::updateCurrentVersion()
    {
        FUNCTION_ENTRY( "updateCurrentVersion" );

        try
        {
            // get the current version
            m_currentScheduleVersion = m_trainTimeScheduleManager->getCurrentTimeScheduleVersion();
            m_currentScheduleKey = m_trainTimeScheduleManager->getCurrentTimeScheduleKey();

            // look up the pkey in the map and get the name for it
            TrainTimeScheduleManager::TimeScheduleMap::iterator iter;
            for (iter = m_timeScheduleInformation.begin();
            iter != m_timeScheduleInformation.end(); iter++)
            {
                // if the pkeys match
                if (iter->second == m_currentScheduleKey)
                {
                    break;
                }
            }

            CString displayText;

            // if it was found
            if ( iter != m_timeScheduleInformation.end() )
            {
                displayText.Format("%03d (%s)", m_currentScheduleVersion, iter->first.c_str());
            }
            else
            {
                // just use the version
                displayText.Format("%03d (ID: %d)", m_currentScheduleVersion, m_currentScheduleKey);
            }

            // set the text
            m_currentISCSVersion.SetWindowText(displayText);

            // update % complete
            updatePercentComplete();
        }
        catch ( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch ( const TA_Base_Core::DataException& de )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );
        }
        catch( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitClearRequest: " );
                exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnSetCurrentSchedule()
    {
        FUNCTION_ENTRY( "OnSetCurrentSchedule" );
        CWaitCursor waitCursor;

        // get selected schedule
        int item = -1;
        POSITION currSel = m_scheduleList.GetFirstSelectedItemPosition();

        // if something is selected
        if(currSel != NULL)
        {
            item = m_scheduleList.GetNextSelectedItem(currSel);
        }

        // if nothing was selected
        if (item < 0)
        {
            // do nothing
            FUNCTION_EXIT;
            return;
        }

        // get the name
        CString itemText = m_scheduleList.GetItemText(item, 0);

        // get its pkey
        TrainTimeScheduleManager::TimeScheduleMap::iterator iter = m_timeScheduleInformation.find(std::string(itemText));
        TA_ASSERT( iter != m_timeScheduleInformation.end(), "Selected item doesnt have an associated map entry" );

        TA_Base_Core::TimeScheduleVersion newVersion;
        // new pkey
        newVersion.timeScheduleKey = iter->second;
        // the version in the GUI, for an up to date check
        newVersion.version = m_currentScheduleVersion;

        // ask the question
        std::stringstream promptStream;
        promptStream << TTISMessageManager::ASK_SET_TIME_CURRENT_TIME_SCHEDULE << " " << newVersion.version+1;
        int ret = TTISMessageManager::getInstance().askQuestion(promptStream.str().c_str());
        if (ret != IDYES) // client said no
        {
            FUNCTION_EXIT;
            return;
        }

        // tell the agent to set it as the current schedule
        try
        {
            // get the session Id
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

            //TISAgentAccessFactory::getInstance().getOccSTISAgent()->setCurrentTrainTimeScheduleVersion(newVersion,sessionId.c_str() );
						//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getOccSTISAgent(),setCurrentTrainTimeScheduleVersion,(newVersion,sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getOccSTISAgent(),setCurrentTrainTimeScheduleVersion,(newVersion,sessionId.c_str()));
			}
			//++libo

            TTISMessageManager::getInstance().displayMessage( "Successfully set current Train Time Schedule.", MB_ICONINFORMATION );

            //hongran++ TD17554
            std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
            TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );

            std::string scheduleName(itemText);
            std::string scheduleVersion;
            std::stringstream toStr;
            toStr << m_currentScheduleVersion;
            toStr >> scheduleVersion;
            TA_Base_Core::DescriptionParameters desc;
            TA_Base_Core::NameValuePair vpScheduleName("Schedule Name", scheduleName );
            TA_Base_Core::NameValuePair vpScheduleVersion("version", scheduleVersion );

            desc.push_back(&vpScheduleName);
            desc.push_back(&vpScheduleVersion);
            TA_Base_Core::AuditMessageSender* auditMessageSender =
                TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );
            auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISSetCurrentTimeSchedule, guiEntity->getKey(),
                desc,
                "", // Further description text
                TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                "", // alarm ID - not required
                "", // incident key - not required
                ""); // event key - not required
            delete auditMessageSender;
            //++hongran TD17554

        }
        catch ( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            TTISMessageManager::getInstance().displayError(TTISMessageManager::CANT_SET_CURRENT_SCHEDULE);
        }
        catch ( const TA_Base_Core::DataException& de )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );

            TTISMessageManager::getInstance().displayError(TTISMessageManager::CANT_SET_CURRENT_SCHEDULE);
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            TTISMessageManager::getInstance().displayError(TTISMessageManager::CANT_SET_CURRENT_SCHEDULE);
        }
        catch( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitClearRequest: " );
                exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TTISMessageManager::getInstance().displayError(TTISMessageManager::CANT_SET_CURRENT_SCHEDULE);
        }

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnDownloadCurrent()
    {
        FUNCTION_ENTRY( "OnDownloadCurrent" );

        // get selected trains
        TA_IRS_Bus::CommonTypes::TrainIdList selectedTrains = m_trainVersionList.getSelectedTrainIds();
        TA_ASSERT(selectedTrains.size() != 0, "Download button should not be enabled when no trains are selected");
        TA_ASSERT(m_downloadManager != NULL, "Download and Upgrade thread is null");
        TA_ASSERT(m_versionManager != NULL, "version manager is null");

        // ask the question
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_DOWNLOAD_TIME_SCHEDULE);
        if (ret != IDYES) // client said no
        {
            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::CommonTypes::TrainIdList trainList;
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::iterator iter = selectedTrains.begin();
              iter != selectedTrains.end(); iter++)
        {
            TrainVersionManager::TrainLibraryAndTimeScheduleVersion version =
                m_versionManager->getVersionForTrain(*iter);

            // check whether it's eligible for a download
            if ( (version.trainTimeScheduleVersion != m_currentScheduleVersion) &&
                 (version.nextTrainTimeScheduleVersion != m_currentScheduleVersion) )
            {
                trainList.insert( *iter );
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train already has the current version. Not initiating download.");

                // add it to the status list so the operator knows whats going on
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = ScheduleDownload;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::NOT_STARTING_ALREADY_CURRENT;

                m_downloadStatusList.updateDownloadList( update,
                    m_trainVersionList.getServiceNumberForTrain(*iter));
            }
        }

        if ( false == trainList.empty() )
        {
            m_downloadManager->startScheduleDownload(trainList, m_retryCheck.GetCheck() != 0);
        }

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnUpgradeCurrent()
    {
        FUNCTION_ENTRY( "OnUpgradeCurrent" );

        // get selected trains
        TA_IRS_Bus::CommonTypes::TrainIdList selectedTrains = m_trainVersionList.getSelectedTrainIds();
        TA_ASSERT(selectedTrains.size() != 0, "Upgrade button should not be enabled when no trains are selected");
        TA_ASSERT(m_downloadManager != NULL, "Download and Upgrade thread is null");
        TA_ASSERT(m_versionManager != NULL, "version manager is null");

        // ask the question
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_UPGRADE_TIME_SCHEDULE);
        if (ret != IDYES) // client said no
        {
            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::CommonTypes::TrainIdList trainList;
        // for each selected train
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::iterator iter = selectedTrains.begin();
              iter != selectedTrains.end(); iter++)
        {
            TrainVersionManager::TrainLibraryAndTimeScheduleVersion version =
                m_versionManager->getVersionForTrain(*iter);

            // check whether it's eligible for an upgrade
            // is the next version the same as the ISCS version?
            if ( version.nextTrainTimeScheduleVersion != m_currentScheduleVersion )
            {
                // not the current version. Cant upgrade
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train must have the current schedule version to upgrade. Not initiating upgrade.");

                // add it to the status list so the operator knows whats going on
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = ScheduleUpgrade;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::TRAIN_CANT_UPGRADE_NOT_CURRENT;

                m_downloadStatusList.updateDownloadList( update,
                    m_trainVersionList.getServiceNumberForTrain(*iter));
            }
            else if ( version.trainTimeScheduleVersion == version.nextTrainTimeScheduleVersion )
            {
                // current version, already upgraded
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train already has the current version upgraded. Not initiating upgrade.");

                // add it to the status list so the operator knows whats going on
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = ScheduleUpgrade;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::TRAIN_CANT_UPGRADE_ALREADY_UPGRADED;

                m_downloadStatusList.updateDownloadList( update,
                    m_trainVersionList.getServiceNumberForTrain(*iter));
            }
            else
            {
                // current version, not upgraded
                trainList.insert( *iter );
            }

        }

        if ( false == trainList.empty() )
        {
            m_downloadManager->startScheduleUpgrade(trainList, m_retryCheck.GetCheck() != 0);
        }


        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnAllTrains()
    {
        FUNCTION_ENTRY( "OnAllTrains" );

        m_trainVersionList.selectAll();
        m_trainVersionList.SetFocus();

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnClearSelection()
    {
        FUNCTION_ENTRY( "OnClearSelection" );

        m_trainVersionList.deselectAll();

        GetDlgItem(IDC_DOWNLOAD_CURRENT)->EnableWindow(FALSE);
        GetDlgItem(IDC_UPGRADE)->EnableWindow(FALSE);

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnClearCompleted()
    {
        FUNCTION_ENTRY( "OnClearCompleted" );

        m_downloadStatusList.clearCompleted();

        // enable/disable the select/deselect buttons depending on whether there are items
        bool enable = (m_downloadStatusList.GetItemCount() > 0);
        GetDlgItem(IDC_SEL_ALL_DOWNLOAD)->EnableWindow(enable);
        GetDlgItem(IDC_CLEAR_SEL_DOWNLOAD)->EnableWindow(enable);

        GetDlgItem(IDC_CLEAR_COMPLETED)->EnableWindow(m_downloadStatusList.completedDownloadsInList());

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onSelectAllDownloads()
    {
        FUNCTION_ENTRY( "onSelectAllDownloads" );

        m_downloadStatusList.selectAll();

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onDeselectAlllDownloads()
    {
        FUNCTION_ENTRY( "onDeselectAlllDownloads" );

        m_downloadStatusList.deselectAll();
        // disable the stop retrying button
        GetDlgItem(IDC_CANCEL_RETRY)->EnableWindow(FALSE);

        FUNCTION_EXIT;
    }


    // these two functions enable/disable the stop retrying button
    void CScheduleDownloadPage::onItemchangedDownloadStatusList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onItemchangedDownloadStatusList" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
                 ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
            {
                // enable the stop retrying button
                GetDlgItem(IDC_CANCEL_RETRY)->EnableWindow(TRUE);
            }
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onClickDownloadStatusList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onClickDownloadStatusList" );

        // if nothing is selected then this was effectively a deselection click
        POSITION currSel = m_downloadStatusList.GetFirstSelectedItemPosition();

        if(currSel == NULL)
        {
            // disable the stop retrying button
            GetDlgItem(IDC_CANCEL_RETRY)->EnableWindow(FALSE);
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onClickTrainList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onClickTrainList" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // if nothing is selected then this was effectively a deselection click
        POSITION currSel = m_trainVersionList.GetFirstSelectedItemPosition();

        if(currSel == NULL)
        {
            // disable download/upgrade buttons
            GetDlgItem(IDC_DOWNLOAD_CURRENT)->EnableWindow(FALSE);
            GetDlgItem(IDC_UPGRADE)->EnableWindow(FALSE);
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onItemchangedTrainList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onItemchangedTrainList" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
                 ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
            {
                // enable download/upgrade buttons
                GetDlgItem(IDC_DOWNLOAD_CURRENT)->EnableWindow(m_canDownload);
                GetDlgItem(IDC_UPGRADE)->EnableWindow(m_canUpgrade);
            }
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onStopAutoRetry()
    {
        FUNCTION_ENTRY( "onStopAutoRetry" );

        // get all selected downloads
        std::vector<DownloadStatusDisplay> selectedDownloads =
            m_downloadStatusList.getSelectedDownloads();

        TA_IRS_Bus::CommonTypes::TrainIdList downloads;
        TA_IRS_Bus::CommonTypes::TrainIdList upgrades;

        for ( std::vector<DownloadStatusDisplay>::iterator iter = selectedDownloads.begin();
              iter != selectedDownloads.end(); ++iter)
        {
            if (iter->downloadType == ScheduleDownload)
            {
                downloads.insert( iter->trainId );
            }
            else if (iter->downloadType == ScheduleUpgrade)
            {
                upgrades.insert( iter->trainId );
            }
        }

        // stop the right type
        if ( false == downloads.empty() )
        {
            m_downloadManager->stopScheduleDownload(downloads);
        }
        
        if ( false == upgrades.empty() )
        {
            m_downloadManager->stopScheduleUpgrade(upgrades);
        }

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onSelchangeScheduleList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onSelchangeScheduleList" );

        NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

        *pResult = 0;

        // Only interested in state changes
        if (pNMListView->uChanged != LVIF_STATE)
        {
            FUNCTION_EXIT;
            return;
        }

        // update the button status
        scheduleSelectionChanged();

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::onClickScheduleList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onClickScheduleList" );

        *pResult = 0;

        // update the button status
        scheduleSelectionChanged();

        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::scheduleSelectionChanged()
    {
        FUNCTION_ENTRY( "scheduleSelectionChanged" );

        POSITION currSel = m_scheduleList.GetFirstSelectedItemPosition();

        // if something is selected
        if(currSel != NULL)
        {
            // enable the button if the operator has permissions
            GetDlgItem(IDC_SET_CURRENT_SCHEDULE)->EnableWindow( m_canSetCurrentSchedule );
        }
        else
        {
            // nothing is selected
            // disable the button
            GetDlgItem(IDC_SET_CURRENT_SCHEDULE)->EnableWindow(FALSE);
        }

        FUNCTION_EXIT;
    }


    LRESULT CScheduleDownloadPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onRightsChanged" );

        m_canSetCurrentSchedule = RightsManager::getInstance().canSetCurrentSchedule();
        m_canDownload = RightsManager::getInstance().canDownloadUpgradeSchedule();
        m_canUpgrade = m_canDownload; // same action

        // enable/disable buttons
        scheduleSelectionChanged();

        bool trainsSelected = (m_trainVersionList.GetSelectedCount() > 0);
        GetDlgItem(IDC_DOWNLOAD_CURRENT)->EnableWindow(m_canDownload && trainsSelected);
        GetDlgItem(IDC_UPGRADE)->EnableWindow(m_canUpgrade && trainsSelected);

        FUNCTION_EXIT;
        return 0;
    }


    void CScheduleDownloadPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void CScheduleDownloadPage::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );
        FUNCTION_EXIT;
    }


    BOOL CScheduleDownloadPage::PreTranslateMessage(MSG* pMsg)
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


} // TA_IRS_App


