/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/LibraryDownloadPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the message library download page.
  * It is located on the train download page
  *
  */

#include "stdafx.h"

#include "LibraryDownloadPage.h"
#include "RightsManager.h"
#include "TTISManager.h"
#include "TTISMessageManager.h"
#include "TTISPredefinedMessages.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <iomanip>
#include <sstream>


using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Bus::TISAgentAccessFactory;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{

/////////////////////////////////////////////////////////////////////////////
// CLibraryDownloadPage dialog


CLibraryDownloadPage::CLibraryDownloadPage( CWnd* pParent /*=NULL*/)
	: CDialog(CLibraryDownloadPage::IDD, pParent),
      m_currentLibraryVersion(0),
      m_nextLibraryVersion(0),
      m_librariesAreSynchronised(false),
      m_downloadManager(NULL),
      m_versionManager(NULL),
      m_canUpgradeISCS(false),
      m_canDownload(false),
          m_canUpgrade(false),
          m_ttisPredefinedMessages( NULL )
    {
        FUNCTION_ENTRY( "CLibraryDownloadPage" );

        //{{AFX_DATA_INIT(CLibraryDownloadPage)
        //}}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::setDownloadManager(TrainDownloadManager& downloadManager)
    {
        FUNCTION_ENTRY( "setDownloadManager" );

        m_downloadManager = &downloadManager;

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::setVersionManager(TrainVersionManager& versionManager)
    {
        FUNCTION_ENTRY( "setVersionManager" );

        m_versionManager = &versionManager;

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CLibraryDownloadPage)
        DDX_Control(pDX, IDC_STATION_TRAIN_TAB, m_stationTrainTab);
        DDX_Control(pDX, IDC_STATION_VERSION_LIST, m_stationVersionList);
        DDX_Control(pDX, IDC_RETRY, m_retryCheck);
        DDX_Control(pDX, IDC_ISCS_NEXT_VERSION, m_nextISCSVersion);
        DDX_Control(pDX, IDC_VERSION_PROGRESS, m_versionProgress);
        DDX_Control(pDX, IDC_ISCS_CURRENT_VERSION, m_currentISCSVersion);
        DDX_Control(pDX, IDC_DOWNLOAD_STATUS, m_downloadStatusList);
        DDX_Control(pDX, IDC_LIBRARY_VERSION_LIST, m_trainVersionList);
        //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CLibraryDownloadPage, CDialog)
        //{{AFX_MSG_MAP(CLibraryDownloadPage)
        ON_BN_CLICKED(IDC_ALL_TRAINS, OnAllTrains)
        ON_BN_CLICKED(IDC_CLEAR_SELECTION, OnClearSelection)
        ON_BN_CLICKED(IDC_CLEAR_COMPLETED, OnClearCompleted)
        ON_WM_DESTROY()
        ON_MESSAGE(WM_UPDATE_CURRENT_TTIS_VERSION, OnUpdateCurrentTTISVersion)
        ON_MESSAGE(WM_UPDATE_NEXT_TTIS_VERSION, OnUpdateNextTTISVersion)
        ON_MESSAGE(WM_UPDATE_LIBRARY_SYNCHRONISED, OnUpdateLibrariesSynchronised)
        ON_BN_CLICKED(IDC_UPGRADE_ISCS, OnUpgradeIscs)
        ON_BN_CLICKED(IDC_DOWNLOAD_NEXT, OnDownloadNext)
        ON_BN_CLICKED(IDC_UPGRADE_NEXT, onUpgradeNext)
        ON_BN_CLICKED(IDC_SEL_ALL_DOWNLOAD, onSelectAllDownloads)
        ON_BN_CLICKED(IDC_CLEAR_SEL_DOWNLOAD, onDeselectAlllDownloads)
        ON_BN_CLICKED(IDC_CANCEL_RETRY, onStopAutoRetry)
        ON_NOTIFY(NM_CLICK, IDC_DOWNLOAD_STATUS, onClickDownloadStatus)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_DOWNLOAD_STATUS, onItemchangedDownloadStatus)
        ON_NOTIFY(NM_CLICK, IDC_LIBRARY_VERSION_LIST, onClickTrainList)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIBRARY_VERSION_LIST, onItemchangedTrainList)
        ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
        ON_NOTIFY(TCN_SELCHANGE, IDC_STATION_TRAIN_TAB, onSelchangeStationTrainTab)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CLibraryDownloadPage message handlers

    BOOL CLibraryDownloadPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void CLibraryDownloadPage::Init()
    {
        FUNCTION_ENTRY( "Init" );

        if ( NULL == m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
        }

        // register for pre-defined message library version updates
        m_ttisPredefinedMessages->registerCurrentVersionUser(this);
        m_ttisPredefinedMessages->registerNextVersionUser(this);
        m_ttisPredefinedMessages->registerLibrarySynchronisedUser(this);

        // register for rights changes
        RightsManager::getInstance().registerForRightsChanges(this);
        m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS();
        m_canDownload = RightsManager::getInstance().canDownloadUpgradeLibrary();
        m_canUpgrade = m_canDownload; // same action

    //    // get the initial values
    //    OnUpdateCurrentTTISVersion(0, 0);
    //    OnUpdateNextTTISVersion(0, 0);
    //    OnUpdateLibrariesSynchronised(0, 0);

        // station/train version selector
        m_stationTrainTab.InsertItem(0, "Train Versions");
        m_stationTrainTab.InsertItem(1, "Station Versions");
        m_stationTrainTab.BringWindowToTop();
        setTrainMode();

        // disable the stop retrying button
        GetDlgItem(IDC_CANCEL_RETRY)->EnableWindow(FALSE);

        // disable download/upgrade buttons
        GetDlgItem(IDC_DOWNLOAD_NEXT)->EnableWindow(FALSE);
        GetDlgItem(IDC_UPGRADE_NEXT)->EnableWindow(FALSE);

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

        m_stationVersionList.Init();

        //hongran++ TD14352
        // get the initial values
        OnUpdateCurrentTTISVersion(0, 0);
        OnUpdateNextTTISVersion(0, 0);
        OnUpdateLibrariesSynchronised(0, 0);
        // ++hongran TD14352

        m_downloadStatusList.Init();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        CDialog::OnDestroy();

        if ( NULL != m_ttisPredefinedMessages )
        {
            // deregister for pre-defined message library version update
            m_ttisPredefinedMessages->deregisterCurrentVersionUser(this);
            m_ttisPredefinedMessages->deregisterNextVersionUser(this);
            m_ttisPredefinedMessages->deregisterLibrarySynchronisedUser(this);

            m_ttisPredefinedMessages = NULL;
            
            TTISPredefinedMessages::removeInstance();
        }

        // deregister for rights changes
        RightsManager::getInstance().deregisterForRightsChanges(this);

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::updateTrainList(TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                         const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
    {
        FUNCTION_ENTRY( "updateTrainList" );

        m_trainVersionList.updateTrainList(action, trainDetails);

        // all this just to show the service number
        m_downloadStatusList.updateTrainDetails(trainDetails);

        // update % complete
        updatePercentComplete();

        // enable buttons
        enableTrainListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::updateTrainVersion(unsigned int trainId)
    {
        FUNCTION_ENTRY( "updateTrainVersion" );

        m_trainVersionList.updateTrainVersion(trainId);

        // update % complete
        updatePercentComplete();

        FUNCTION_EXIT;
    }


    std::string CLibraryDownloadPage::getCustomData( unsigned int trainId,
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
            value << std::setw(3) << std::setfill('0') << version.predefinedLibraryVersion;
        }
        else if (columnName == DownloadTrainListCtrl::UPGRADED_COLUMN)
        {
            value << std::setw(3) << std::setfill('0') << version.nextPredefinedLibraryVersion;
        }

        FUNCTION_EXIT;
        return value.str();
    }


    void CLibraryDownloadPage::updateDownloadList(const DownloadStatusDisplay& trainDownload)
    {
        FUNCTION_ENTRY( "updateDownloadList" );

        // if it's the wrong type, return
        if ( (trainDownload.downloadType == LibraryDownload) ||
             (trainDownload.downloadType == LibraryUpgrade) )
        {
            // get the service number for the train
            // and update the list
            m_downloadStatusList.updateDownloadList( trainDownload,
                m_trainVersionList.getServiceNumberForTrain(trainDownload.trainId));

            // enable buttons
            enableDownloadListButtons();
        }

        FUNCTION_EXIT;
    }


    LRESULT CLibraryDownloadPage::OnUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateCurrentTTISVersion" );

        // get the current pre-defined message library version

        m_currentLibraryVersion = m_ttisPredefinedMessages->getCurrentMessageLibraryVersion();
        //hongran++ TD17500
        // Following is removed by hongran for design change
        //hongran++ TD14352
        //m_stationVersionList.setLocalCurrentLibVerstion(m_currentLibraryVersion);
        //++hongran TD14352
        //++hongran TD17500

        // enable the button if it should be enabled
        enableUpgradeIscsButton();

         // display the current version
        CString str;
        str.Format("%03d", m_currentLibraryVersion);
        m_currentISCSVersion.SetWindowText(str);

        // get the % complete
        updatePercentComplete();

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CLibraryDownloadPage::OnUpdateNextTTISVersion(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateNextTTISVersion" );

        // get the next pre-defined message library version

        m_nextLibraryVersion = m_ttisPredefinedMessages->getNextMessageLibraryVersion();
        //hongran++ TD17500
        // Following is removed by hongran for design change
        //hongran++ TD14352
    //    m_stationVersionList.setLocalNextLibVerstion(m_nextLibraryVersion);
        //++hongran TD14352
        //++hongran TD17500

        // enable the button if it should be enabled
        enableUpgradeIscsButton();

        // display the next version
        CString str;
        str.Format("%03d", m_nextLibraryVersion);
        m_nextISCSVersion.SetWindowText(str);

        // get the % complete
        //updatePercentComplete();
        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CLibraryDownloadPage::OnUpdateLibrariesSynchronised(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateLibrariesSynchronised" );

        // get the new value
        m_librariesAreSynchronised = m_ttisPredefinedMessages->getMessageLibrarySynchronised();

        // enable the button if it should be enabled
        enableUpgradeIscsButton();

        FUNCTION_EXIT;
        return 0;
    }


    void CLibraryDownloadPage::updatePercentComplete()
    {
        FUNCTION_ENTRY( "updatePercentComplete" );

        // get the % complete from the list
        //xufeng++ TD17545
        int percent = m_trainVersionList.getPercentComplete(m_nextLibraryVersion, 1);
        //int percent = m_trainVersionList.getPercentComplete(m_nextLibraryVersion);
        //xufeng++ TD17545

        // convert it to a string
        CString completeString;
        completeString.Format("%01d", percent);

        // set the text
        m_versionProgress.SetWindowText(completeString);

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::enableUpgradeIscsButton()
    {
        FUNCTION_ENTRY( "enableUpgradeIscsButton" );

        unsigned long currentLocationKey = TA_IRS_Bus::LocationCache::getInstance().getLocationKey();

        // enable the upgrade ISCS button if
        // - the libraries are synchronised across the stations
        // - The library hasnt been upgraded yet
        GetDlgItem(IDC_UPGRADE_ISCS)->EnableWindow(
            m_canUpgradeISCS &&
            m_librariesAreSynchronised &&
            (m_currentLibraryVersion != m_nextLibraryVersion) &&
            (currentLocationKey == TA_IRS_Bus::LocationCache::getInstance().getOccLocationKey())
            );

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnDownloadNext()
    {
        FUNCTION_ENTRY( "OnDownloadNext" );

        CWaitCursor waitCursor;

        // get selected trains
        TA_IRS_Bus::CommonTypes::TrainIdList selectedTrains = m_trainVersionList.getSelectedTrainIds();
        TA_ASSERT(selectedTrains.size() != 0, "Download button should not be enabled when no trains are selected");
        TA_ASSERT(m_downloadManager != NULL, "Download and Upgrade thread is null");
        TA_ASSERT(m_versionManager != NULL, "version manager is null");

        // ask the question
        int ret = TTISMessageManager::getInstance().askQuestion( TTISMessageManager::ASK_DOWNLOAD_MESSAGE_LIBRARY );
        if (ret != IDYES) // client said no
        {
            FUNCTION_EXIT;
            return;
        }


        for ( TA_IRS_Bus::CommonTypes::TrainIdList::iterator iter = selectedTrains.begin();
              iter != selectedTrains.end(); /*increment in loop*/ )
        {
            TrainVersionManager::TrainLibraryAndTimeScheduleVersion version = m_versionManager->getVersionForTrain( *iter );

            // check whether it's eligible for a download
            if ( version.nextPredefinedLibraryVersion == m_nextLibraryVersion )
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train already has the next version. Not initiating download.");

                // add it to the status list so the operator knows whats going on
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = LibraryDownload;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::NOT_STARTING_ALREADY_NEXT;

                m_downloadStatusList.updateDownloadList( update, m_trainVersionList.getServiceNumberForTrain( *iter ) );

                // remove this fromt the list to download
                selectedTrains.erase( iter++ );
            }
            else
            {
                ++iter;
            }
        }

        m_downloadManager->startLibraryDownload( selectedTrains, m_retryCheck.GetCheck() != 0 );

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::onUpgradeNext()
    {
        FUNCTION_ENTRY( "onUpgradeNext" );

        CWaitCursor waitCursor;

        // get selected trains
        TA_IRS_Bus::CommonTypes::TrainIdList selectedTrains = m_trainVersionList.getSelectedTrainIds();
        TA_ASSERT(selectedTrains.size() != 0, "Upgrade button should not be enabled when no trains are selected");
        TA_ASSERT(m_downloadManager != NULL, "Download and Upgrade thread is null");
        TA_ASSERT(m_versionManager != NULL, "version manager is null");

        // ask the question
        int ret = TTISMessageManager::getInstance().askQuestion( TTISMessageManager::ASK_UPGRADE_MESSAGE_LIBRARY );
        if (ret != IDYES) // client said no
        {
            FUNCTION_EXIT;
            return;
        }

        // for each selected train
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::iterator iter = selectedTrains.begin();
              iter != selectedTrains.end(); /*increment in loop*/ )
        {
            TrainVersionManager::TrainLibraryAndTimeScheduleVersion version = m_versionManager->getVersionForTrain( *iter );

            // check whether it's eligible for an upgrade
            if ( version.nextPredefinedLibraryVersion != m_nextLibraryVersion )
            {
                // not the current version. Cant upgrade
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train must have the next message version to upgrade. Not initiating upgrade.");

                // add it to the status list so the operator knows whats going on
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = LibraryUpgrade;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::TRAIN_CANT_UPGRADE_NOT_NEXT;

                m_downloadStatusList.updateDownloadList( update, m_trainVersionList.getServiceNumberForTrain( *iter ) );

                // remove this fromt the list to upgrade
                selectedTrains.erase( iter++ );
            }
            else if ( version.predefinedLibraryVersion == version.nextPredefinedLibraryVersion )
            {
                // current version, already upgraded
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train already upgraded. Not initiating upgrade.");

                // add it to the status list so the operator knows whats going on
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = LibraryUpgrade;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::TRAIN_CANT_UPGRADE_ALREADY_UPGRADED;

                m_downloadStatusList.updateDownloadList( update, m_trainVersionList.getServiceNumberForTrain( *iter ) );

                // remove this fromt the list to upgrade
                selectedTrains.erase( iter++ );
            }
            else
            {
                ++iter;
            }
        }

        m_downloadManager->startLibraryUpgrade(selectedTrains, m_retryCheck.GetCheck() != 0);

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnUpgradeIscs()
    {
        FUNCTION_ENTRY( "OnUpgradeIscs" );

        CWaitCursor waitCursor;

        // ask the question
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_UPGRADE_MESSAGE_LIBRARY);
        if (ret != IDYES) // client said no
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // get the session Id
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

            // upgrade the version
            //TISAgentAccessFactory::getInstance().getOccSTISAgent()->upgradePredefinedTrainMessageLibrary(m_nextLibraryVersion, sessionId.c_str());
			//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getOccSTISAgent(),upgradePredefinedTrainMessageLibrary,(m_nextLibraryVersion, sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getOccSTISAgent(),upgradePredefinedTrainMessageLibrary,(m_nextLibraryVersion, sessionId.c_str()));
			}
			//++libo

        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException& gfe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "STISGeneralFailureException", gfe.details );

            std::string message(TTISMessageManager::CANT_UPGRADE_VERSION);
            message += "\n";
            message += gfe.details;

            TTISMessageManager::getInstance().displayError(message.c_str());
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            TTISMessageManager::getInstance().displayError(TTISMessageManager::CANT_UPGRADE_VERSION);
        }
        catch ( const TransactiveException& te)
        {
            // DataException or DatabaseException
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what() );

            // this means there is an error with the data or the database
            TTISMessageManager::getInstance().displayError(TTISMessageManager::CANT_UPGRADE_VERSION);
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling getCurrentTTISMessageLibraryVersion or getNextTTISMessageLibraryVersion: " );
                exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TTISMessageManager::getInstance().displayError(TTISMessageManager::CANT_UPGRADE_VERSION);
        }

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnAllTrains()
    {
        FUNCTION_ENTRY( "OnAllTrains" );

        m_trainVersionList.selectAll();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnClearSelection()
    {
        FUNCTION_ENTRY( "OnClearSelection" );

        m_trainVersionList.deselectAll();

        enableTrainListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnClearCompleted()
    {
        FUNCTION_ENTRY( "OnClearCompleted" );

        m_downloadStatusList.clearCompleted();

        enableDownloadListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::onSelectAllDownloads()
    {
        FUNCTION_ENTRY( "onSelectAllDownloads" );

        m_downloadStatusList.selectAll();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::onDeselectAlllDownloads()
    {
        FUNCTION_ENTRY( "onDeselectAlllDownloads" );

        m_downloadStatusList.deselectAll();

        enableDownloadListButtons();

        FUNCTION_EXIT;
    }


    // these two functions enable/disable the stop retrying button
    void CLibraryDownloadPage::onClickDownloadStatus(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onClickDownloadStatus" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // if nothing is selected then this was effectively a deselection click
        POSITION currSel = m_downloadStatusList.GetFirstSelectedItemPosition();

        enableDownloadListButtons();

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::onItemchangedDownloadStatus(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onItemchangedDownloadStatus" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
                 ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
            {
                enableDownloadListButtons();
            }
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::onClickTrainList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onClickTrainList" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // if nothing is selected then this was effectively a deselection click
        POSITION currSel = m_trainVersionList.GetFirstSelectedItemPosition();

        if(currSel == NULL)
        {
            enableTrainListButtons();
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::onItemchangedTrainList(NMHDR* pNMHDR, LRESULT* pResult)
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
                enableTrainListButtons();
            }
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::onStopAutoRetry()
    {
        FUNCTION_ENTRY( "onStopAutoRetry" );

        // get all selected downloads
        std::vector<DownloadStatusDisplay> selectedDownloads = m_downloadStatusList.getSelectedDownloads();

        TA_IRS_Bus::CommonTypes::TrainIdList libraryDownloads;
        TA_IRS_Bus::CommonTypes::TrainIdList libraryupgrades;

        // stop the auto retry for each.
        for ( std::vector<DownloadStatusDisplay>::iterator iter = selectedDownloads.begin();
              iter != selectedDownloads.end(); iter++ )
        {
            // stop the right type
            if ( iter->downloadType == LibraryDownload )
            {
                libraryDownloads.insert( iter->trainId );
            }
            else if ( iter->downloadType == LibraryUpgrade )
            {
                libraryupgrades.insert( iter->trainId );
            }
        }

        if ( false == libraryDownloads.empty() )
        {
            m_downloadManager->stopLibraryDownload( libraryDownloads );
        }

        if ( false == libraryupgrades.empty() )
        {
            m_downloadManager->stopLibraryUpgrade( libraryupgrades );
        }

        FUNCTION_EXIT;
    }


    LRESULT CLibraryDownloadPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onRightsChanged" );

        m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS();
        m_canDownload = RightsManager::getInstance().canDownloadUpgradeLibrary();
        m_canUpgrade = m_canDownload; // same action

        // enable/disable buttons
        enableUpgradeIscsButton();

        enableTrainListButtons();

        FUNCTION_EXIT;
        return 0;
    }


    void CLibraryDownloadPage::onSelchangeStationTrainTab(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onSelchangeStationTrainTab" );

        if (m_stationTrainTab.GetCurSel() == 0)
        {
            // train mode
            setTrainMode();
        }
        else if (m_stationTrainTab.GetCurSel() == 1)
        {
            // station mode
            setStationMode();
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::setTrainMode()
    {
        FUNCTION_ENTRY( "setTrainMode" );

        m_stationVersionList.ShowWindow(SW_HIDE);
        m_trainVersionList.ShowWindow(SW_SHOW);
        m_trainVersionList.BringWindowToTop();
        enableTrainListButtons();
        enableDownloadListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::setStationMode()
    {
        FUNCTION_ENTRY( "setStationMode" );

        m_trainVersionList.ShowWindow(SW_HIDE);
        m_stationVersionList.ShowWindow(SW_SHOW);
        m_stationVersionList.BringWindowToTop();
        enableTrainListButtons();
        enableDownloadListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::enableTrainListButtons()
    {
        FUNCTION_ENTRY( "enableTrainListButtons" );

        // enable/disable the select/deselect buttons depending on whether there are items
        bool enable = m_trainVersionList.IsWindowVisible() &&
                      (m_trainVersionList.GetItemCount() > 0);

        GetDlgItem(IDC_ALL_TRAINS)->EnableWindow(enable);
        GetDlgItem(IDC_CLEAR_SELECTION)->EnableWindow(enable);

        // enable the download/upgrade buttons depending on whether
        // there are items selected
        enable = m_trainVersionList.IsWindowVisible() &&
                 (m_trainVersionList.GetSelectedCount() > 0);

        GetDlgItem(IDC_DOWNLOAD_NEXT)->EnableWindow(m_canDownload && enable);
        GetDlgItem(IDC_UPGRADE_NEXT)->EnableWindow(m_canUpgrade && enable);

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::enableDownloadListButtons()
    {
        FUNCTION_ENTRY( "enableDownloadListButtons" );

        // enable/disable the select/deselect buttons depending on whether there are items
        bool enable = (m_downloadStatusList.GetItemCount() > 0);

        GetDlgItem(IDC_SEL_ALL_DOWNLOAD)->EnableWindow(enable);
        GetDlgItem(IDC_CLEAR_SEL_DOWNLOAD)->EnableWindow(enable);

        GetDlgItem(IDC_CLEAR_COMPLETED)->EnableWindow( m_downloadStatusList.completedDownloadsInList() );

        // stop retrying button
        enable = (m_downloadStatusList.GetSelectedCount() > 0);
        GetDlgItem(IDC_CANCEL_RETRY)->EnableWindow(enable);

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );
        FUNCTION_EXIT;
    }


    BOOL CLibraryDownloadPage::PreTranslateMessage(MSG* pMsg)
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


