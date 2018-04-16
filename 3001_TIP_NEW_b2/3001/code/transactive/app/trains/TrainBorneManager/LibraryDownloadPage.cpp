/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/LibraryDownloadPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is the message library download page.
  * It is located on the train download page
  *
  */

#include "stdafx.h"

#include "LibraryDownloadPage.h"
#include "../TrainBorne_PIDS/src/RightsManager.h"
#include "../TrainBorne_PIDS/src/TTISMessageManager.h"
#include "../TrainBorne_PIDS/src/TTISPredefinedMessages.h"
#include "../TrainBorne_PIDS/src/TisUserMessageMacroDef.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/generic_gui/src/HelpLauncher.h"

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
      m_currentISCSLibraryVersion(0),
      m_nextISCSLibraryVersion(0),
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
		m_locationKey = TA_IRS_Bus::LocationCache::getInstance().getLocationKey();
		m_locationType = TA_IRS_Bus::LocationCache::getInstance().getLocationTypeByKey(m_locationKey);
		// Init rights
		m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS() && isAtOccOrDpt();
        m_canDownload = RightsManager::getInstance().canDownloadUpgradeLibrary() && isAtOccOrDpt();
        m_canUpgrade = m_canDownload; // same action

        FUNCTION_EXIT;
    }

    void CLibraryDownloadPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CLibraryDownloadPage)
        DDX_Control(pDX, IDC_STATION_TRAIN_TAB, m_stationTrainTab);
        DDX_Control(pDX, IDC_STATION_LIBRARY_VERSION_LIST, m_stationVersionList);
        //DDX_Control(pDX, IDC_RETRY, m_retryCheck);
        DDX_Control(pDX, IDC_ISCS_NEXT_VERSION, m_nextISCSVersion);
        //DDX_Control(pDX, IDC_VERSION_PROGRESS, m_versionProgress);
        DDX_Control(pDX, IDC_ISCS_CURRENT_VERSION, m_currentISCSVersion);
        DDX_Control(pDX, IDC_DOWNLOAD_STATUS, m_downloadStatusList);
        DDX_Control(pDX, IDC_TRAIN_LIBRARY_VERSION_LIST, m_trainVersionList);
        //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CLibraryDownloadPage, CDialog)
        //{{AFX_MSG_MAP(CLibraryDownloadPage)
        ON_BN_CLICKED(IDC_SELECT_ALL_TRAINS, OnSelectAllTrains)
        ON_BN_CLICKED(IDC_DESELECT_ALL_TRAINS, OnDeselectAllTrains)
        ON_BN_CLICKED(IDC_CLEAR_COMPLETED, OnClearCompleted)
        ON_MESSAGE(WM_UPDATE_CURRENT_TTIS_VERSION, OnUpdateCurrentTTISVersion)
        ON_MESSAGE(WM_UPDATE_NEXT_TTIS_VERSION, OnUpdateNextTTISVersion)
        ON_MESSAGE(WM_UPDATE_LIBRARY_SYNCHRONISED, OnUpdateLibrariesSynchronised)
        ON_BN_CLICKED(IDC_UPGRADE_ISCS, OnUpgradeIscs)
        ON_BN_CLICKED(IDC_DOWNLOAD_NEXT, OnDownloadNext)
        ON_BN_CLICKED(IDC_UPGRADE_NEXT, onUpgradeNext)
        ON_BN_CLICKED(IDC_CANCEL_DOWNLOAD, onCancelDownload)
        ON_NOTIFY(NM_CLICK, IDC_DOWNLOAD_STATUS, onClickDownloadStatus)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_DOWNLOAD_STATUS, onItemchangedDownloadStatus)
        ON_NOTIFY(NM_CLICK, IDC_TRAIN_LIBRARY_VERSION_LIST, onClickTrainList)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRAIN_LIBRARY_VERSION_LIST, onItemchangedTrainList)
        ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
        ON_NOTIFY(TCN_SELCHANGE, IDC_STATION_TRAIN_TAB, onSelchangeStationTrainTab)

		ON_BN_CLICKED(IDC_TRAIN_LIBRARY_DOWNLOAD_HELP, OnBnClickedHelp)
		ON_BN_CLICKED(IDC_TRAIN_LIBRARY_DOWNLOAD_CLOSE, OnBnClickedClose)
		//////////////////////////////////////////////////////////////////////////

        ON_MESSAGE(WM_UPDATEINBOUND, OnUpdateInboundList)
        ON_MESSAGE(WM_UPDATEOUTBOUND, OnUpdateOutboundList)
        ON_MESSAGE(WM_UPDATESELECTIONCOMBO, OnUpdateSelectionCombo)
        ON_MESSAGE(WM_UPDATE_TRAINVERSION, OnUpdateTrainVersion)
        ON_MESSAGE(WM_UPDATE_DOWNLOADLIST, OnUpdateDownloadList)
        ON_WM_DESTROY()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CLibraryDownloadPage message handlers

    BOOL CLibraryDownloadPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();
		Init();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void CLibraryDownloadPage::Init()
    {
        FUNCTION_ENTRY( "Init" );

        // initialise the download list
        m_trainVersionList.setupList(this);
        m_stationVersionList.Init();
        m_downloadStatusList.Init();

        // station/train version selector
        m_stationTrainTab.InsertItem(0, "Train Versions");
        m_stationTrainTab.InsertItem(1, "Station Versions");
        m_stationTrainTab.BringWindowToTop();

		enableUpgradeIscsButton();
        setTrainMode();

        // register for rights changes
        RightsManager::getInstance().registerForRightsChanges(this);

        // receives train version change messages
        m_versionManager = new TrainVersionManager(this);
        // the thread that downloads/upgrades to trains. Handles periodic retrying,
        // receives download/upgrade messages and gives them to this window.
        m_downloadManager = new TrainDownloadManager(this);
        // start the thread for upgrade/downloading
        m_downloadManager->start();
        // Set up the train selection component
        // this receives the updates and posts them - to resolve threading issues
		m_mfcTrainSelector = new TA_IRS_Bus::MFCTrainSelectorGUI( this,
                                                      WM_UPDATEINBOUND,
                                                      WM_UPDATEOUTBOUND,
                                                      WM_UPDATESELECTIONCOMBO );

        // This actually gets the updates
        try
        {
            // the train selection component
            // subscribe for all trains
			m_trainSelector = new TA_IRS_Bus::TrainSelector( *m_mfcTrainSelector,
                                                 TA_IRS_Bus::LocationCache::getInstance().getLocationKey(),
                                                 TA_IRS_Bus::LocationCache::All);
        }
        catch (const TA_Base_Core::ObjectResolutionException& ore)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what());
        }
        catch( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception caught while intialising train updates: " );
                exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }
        catch (const TA_Base_Core::TransactiveException& te)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what());
        } 
		// Init TTISPredefinedMessages
        if ( NULL == m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
        }
        // register for pre-defined message library version updates
        m_ttisPredefinedMessages->registerCurrentVersionUser(this);
        m_ttisPredefinedMessages->registerNextVersionUser(this);
        m_ttisPredefinedMessages->registerLibrarySynchronisedUser(this);

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

        if ( m_trainSelector != NULL)
        {
            delete m_trainSelector;
            m_trainSelector = NULL;
        }

        if ( m_mfcTrainSelector != NULL)
        {
            delete m_mfcTrainSelector;
            m_mfcTrainSelector = NULL;
        }

        if (m_downloadManager != NULL)
        {
            m_downloadManager->terminateAndWait();
            delete m_downloadManager;
            m_downloadManager = NULL;
        }

        if ( m_versionManager != NULL)
        {
            delete m_versionManager;
            m_versionManager = NULL;
        }

        FUNCTION_EXIT;
    }


	

    void CLibraryDownloadPage::updateTrainList(TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                         const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
    {
        FUNCTION_ENTRY( "updateTrainList" );

        m_trainVersionList.updateTrainList(action, trainDetails);

        // all this just to show the service number
        m_downloadStatusList.updateTrainDetails(trainDetails);
        enableTrainListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::updateTrainVersion(unsigned int trainId)
    {
        FUNCTION_ENTRY( "updateTrainVersion" );

        m_trainVersionList.updateTrainVersion(trainId);
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

		std::string value;

        if (columnName == DownloadTrainListCtrl::VERSION_COLUMN)
        {
			value = TrainVersionManager::convertTrainVersionToStr(version.predefinedLibraryVersion);
        }
        else if (columnName == DownloadTrainListCtrl::UPGRADED_COLUMN)
        {
			value = TrainVersionManager::convertTrainVersionToStr(version.nextPredefinedLibraryVersion);
        }

        FUNCTION_EXIT;
        return value;
    }

    LRESULT CLibraryDownloadPage::OnUpdateDownloadList(WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY( "OnUpdateDownloadList" );

		DownloadStatusDisplay* update =
			reinterpret_cast<DownloadStatusDisplay*>(wParam);

		// send the download list update to the appropriate page
		switch(update->downloadType)
		{
		case LibraryDownload:
		case LibraryUpgrade:
			updateDownloadList(*update);
			break;
		}

		delete update;

		FUNCTION_EXIT;
		return 0;
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
            enableCancelDownloadButton();
        }

        FUNCTION_EXIT;
    }


    LRESULT CLibraryDownloadPage::OnUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateCurrentTTISVersion" );

        // get the current pre-defined message library version
        m_currentISCSLibraryVersion = wParam;

        // enable the button if it should be enabled
        enableUpgradeIscsButton();

         // display the current version       
		m_currentISCSVersion.SetWindowText(TrainVersionManager::convertTrainVersionToStr(m_currentISCSLibraryVersion).c_str());

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CLibraryDownloadPage::OnUpdateNextTTISVersion(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateNextTTISVersion" );

        // get the next pre-defined message library version
        m_nextISCSLibraryVersion = wParam;

        // enable the button if it should be enabled
        enableUpgradeIscsButton();

        // display the next version
		m_nextISCSVersion.SetWindowText(TrainVersionManager::convertTrainVersionToStr(m_nextISCSLibraryVersion).c_str());

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CLibraryDownloadPage::OnUpdateLibrariesSynchronised(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateLibrariesSynchronised" );

        m_librariesAreSynchronised = wParam;
        enableUpgradeIscsButton();

        FUNCTION_EXIT;
        return 0;
    }


    void CLibraryDownloadPage::enableUpgradeIscsButton()
    {
        FUNCTION_ENTRY( "enableUpgradeIscsButton" );

        GetDlgItem(IDC_UPGRADE_ISCS)->EnableWindow(
            m_canUpgradeISCS &&
            m_librariesAreSynchronised &&
            (m_currentISCSLibraryVersion != m_nextISCSLibraryVersion) &&
			isAtOccOrDpt());

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
            if ( version.nextPredefinedLibraryVersion == m_nextISCSLibraryVersion )
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train already has the next version. Not initiating download.");
				/*

                // add it to the status list so the operator knows whats going on
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = LibraryDownload;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::NOT_STARTING_ALREADY_NEXT;

                m_downloadStatusList.updateDownloadList( update, m_trainVersionList.getServiceNumberForTrain( *iter ) );
				*/

                // remove this fromt the list to download
                iter = selectedTrains.erase( iter );
            }
            else
            {
                ++iter;
            }
        }

        m_downloadManager->startLibraryDownload( selectedTrains, false); //false mean not auto retry

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
            if ( version.nextPredefinedLibraryVersion != m_nextISCSLibraryVersion )
            {
                // not the current version. Cant upgrade
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train must have the next message version to upgrade. Not initiating upgrade.");

                // add it to the status list so the operator knows whats going on
				/*
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = LibraryUpgrade;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::TRAIN_CANT_UPGRADE_NOT_NEXT;

                m_downloadStatusList.updateDownloadList( update, m_trainVersionList.getServiceNumberForTrain( *iter ) );
				*/

                // remove this fromt the list to upgrade
                iter = selectedTrains.erase( iter );
            }
            else if ( version.predefinedLibraryVersion == version.nextPredefinedLibraryVersion )
            {
                // current version, already upgraded
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                    "Train already upgraded. Not initiating upgrade.");

                // add it to the status list so the operator knows whats going on
				/*
                DownloadStatusDisplay update;
                update.trainId = *iter;
                update.downloadType = LibraryUpgrade;
                update.autoRetry = false;
                update.currentState = DownloadStatusDisplay::Finished;
                update.displayMessage = TTISMessageManager::TRAIN_CANT_UPGRADE_ALREADY_UPGRADED;

                m_downloadStatusList.updateDownloadList( update, m_trainVersionList.getServiceNumberForTrain( *iter ) );
				*/

                // remove this fromt the list to upgrade
                iter = selectedTrains.erase( iter );
            }
            else
            {
                ++iter;
            }
        }

        m_downloadManager->startLibraryUpgrade(selectedTrains, false); //false mean not auto retry

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnUpgradeIscs()
    {
        FUNCTION_ENTRY( "OnUpgradeIscs" );

        CWaitCursor waitCursor;

        // ask the question
        int ret = TTISMessageManager::getInstance().askQuestion(TTISMessageManager::ASK_UPGRADE_ISCS_MESSAGE_LIBRARY);
        if (ret != IDYES) // client said no
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // get the session Id
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getOccSTISAgent(),upgradePredefinedTrainMessageLibrary,(m_nextISCSLibraryVersion, sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getOccSTISAgent(),upgradePredefinedTrainMessageLibrary,(m_nextISCSLibraryVersion, sessionId.c_str()));
			}
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException& gfe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "STISGeneralFailureException", gfe.details );
			TTISMessageManager::getInstance().displayError(TTISMessageManager::ERROR_CONTACTED_AGENT);
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );
			TTISMessageManager::getInstance().displayError(TTISMessageManager::ERROR_CONTACTED_AGENT);
        }
        catch ( const TransactiveException& te)
        {
            // DataException or DatabaseException
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what() );
			TTISMessageManager::getInstance().displayError(TTISMessageManager::ERROR_CONTACTED_AGENT);
        }
        catch ( const CORBA::Exception& cex )
        {
			TTISMessageManager::getInstance().displayError(TTISMessageManager::ERROR_CONTACTED_AGENT);
        }

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnSelectAllTrains()
    {
        FUNCTION_ENTRY( "OnSelectAllTrains" );

        m_trainVersionList.selectAll();

		enableTrainListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnDeselectAllTrains()
    {
        FUNCTION_ENTRY( "OnDeselectAllTrains" );

        m_trainVersionList.deselectAll();

        enableTrainListButtons();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::OnClearCompleted()
    {
        FUNCTION_ENTRY( "OnClearCompleted" );

        m_downloadStatusList.clearCompleted();

        enableCancelDownloadButton();

        FUNCTION_EXIT;
    }

    // these two functions enable/disable the stop retrying button
    void CLibraryDownloadPage::onClickDownloadStatus(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onClickDownloadStatus" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // if nothing is selected then this was effectively a deselection click
        POSITION currSel = m_downloadStatusList.GetFirstSelectedItemPosition();

        enableCancelDownloadButton();

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
                enableCancelDownloadButton();
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
        POSITION currSel = m_downloadStatusList.GetFirstSelectedItemPosition();

        enableTrainListButtons();

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


    void CLibraryDownloadPage::onCancelDownload()
    {
        FUNCTION_ENTRY( "onCancelDownload" );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "User init a cancel library download operation");
        // get all selected downloads
        std::vector<DownloadStatusDisplay> notfinishedItems = m_downloadStatusList.getInDownloads();

        TA_IRS_Bus::CommonTypes::TrainIdList libraryCancelDownloads;

        // stop the auto retry for each.
        for ( std::vector<DownloadStatusDisplay>::iterator iter = notfinishedItems.begin();
              iter != notfinishedItems.end(); iter++ )
        {
            // stop the right type
            if ( iter->downloadType == LibraryDownload )
            {
                libraryCancelDownloads.insert( iter->trainId );
            }
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In downloading train size is: %d", libraryCancelDownloads.size());
		if( false == libraryCancelDownloads.empty() )
		{
			m_downloadManager->cancelDownload(libraryCancelDownloads);		
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
        enableCancelDownloadButton();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::setStationMode()
    {
        FUNCTION_ENTRY( "setStationMode" );

        m_trainVersionList.ShowWindow(SW_HIDE);
        m_stationVersionList.ShowWindow(SW_SHOW);
        m_stationVersionList.BringWindowToTop();
        enableTrainListButtons();
        enableCancelDownloadButton();

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::enableTrainListButtons()
    {
        FUNCTION_ENTRY( "enableTrainListButtons" );

        // enable the download/upgrade buttons depending on whether
        // there are items selected
        bool enable = m_trainVersionList.IsWindowVisible() && (m_trainVersionList.GetSelectedCount() > 0);
        TA_IRS_Bus::CommonTypes::TrainIdList selectedTrains = m_trainVersionList.getSelectedTrainIds();
		bool downloadTrainVersionConditon = false;
		bool upgradeTrainVersionConditon = false;
		if(selectedTrains.size() > 0)
		{
			// The list only single selection
            TrainVersionManager::TrainLibraryAndTimeScheduleVersion version = m_versionManager->getVersionForTrain(*(selectedTrains.begin()));
			downloadTrainVersionConditon = (version.nextPredefinedLibraryVersion != m_nextISCSLibraryVersion);
			upgradeTrainVersionConditon = ((version.nextPredefinedLibraryVersion == m_nextISCSLibraryVersion)
					                            && (version.predefinedLibraryVersion != version.nextPredefinedLibraryVersion));
		}
        GetDlgItem(IDC_DOWNLOAD_NEXT)->EnableWindow(m_canDownload && enable && isAtOccOrDpt() && downloadTrainVersionConditon);
        GetDlgItem(IDC_UPGRADE_NEXT)->EnableWindow(m_canUpgrade && enable && isAtOccOrDpt() && upgradeTrainVersionConditon);

        FUNCTION_EXIT;
    }


    void CLibraryDownloadPage::enableCancelDownloadButton()
    {
        FUNCTION_ENTRY( "enableCancelDownloadButton" );
        
		// enable cancel download button when there are trains performing download
       
        GetDlgItem(IDC_CANCEL_DOWNLOAD)->EnableWindow(m_downloadStatusList.uncompletedDownloadsInList());

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
        switch ( pMsg->message )
        {
        case WM_KEYDOWN:
            {
                switch ( pMsg->wParam )
                {
                case VK_ESCAPE:
                    return TRUE;
                case VK_RETURN:
                    return TRUE;
                default:
                    break;
                }
            }
            break;
        default:
			break;
        }
        return CDialog::PreTranslateMessage(pMsg);
    }

	void CLibraryDownloadPage::OnBnClickedHelp()
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
	}

	void CLibraryDownloadPage::OnBnClickedClose()
	{
		CWnd* pParent = GetParent();
		if ( NULL != pParent )
		{
			pParent->PostMessage( WM_CLOSE );
		}
		else
		{
			PostMessage( WM_CLOSE );
		}
	}

    LRESULT CLibraryDownloadPage::OnUpdateInboundList(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateInboundList" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        updateTrainList(update->action, update->trainDetails);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CLibraryDownloadPage::OnUpdateOutboundList(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateOutboundList" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        updateTrainList(update->action, update->trainDetails);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CLibraryDownloadPage::OnUpdateSelectionCombo(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateSelectionCombo" );

        TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::SelectionListUpdate*>(wParam);
        // not interested in this update
        // but it must be handled so it can be deleted
        delete update;
        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CLibraryDownloadPage::OnUpdateTrainVersion(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateTrainVersion" );

        // cast the train version structure
        TrainVersionManager::TrainLibraryAndTimeScheduleVersion* update =
            reinterpret_cast<TrainVersionManager::TrainLibraryAndTimeScheduleVersion*>(wParam);

        // send the version to the train lists
		updateTrainVersion(update->trainNumber);
        delete update;

        FUNCTION_EXIT;
        return 0;
    }	

} // TA_IRS_App


