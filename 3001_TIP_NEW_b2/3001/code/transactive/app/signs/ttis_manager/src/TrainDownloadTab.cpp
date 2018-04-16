/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainDownloadTab.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is the train download tab. It just holds the
  * schedule and library download tabs.
  *
  */


#include "stdafx.h"

#include "TTISManager.h"
#include "TTISMessageManager.h"
#include "TrainDownloadTab.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainDownloadTab

namespace TA_IRS_App
{


        using TA_IRS_Bus::MFCTrainSelectorGUI;
        using TA_IRS_Bus::TrainSelector;
        using TA_Base_Bus::TISAgentAccessFactory;


    CTrainDownloadTab::CTrainDownloadTab()
     : CTabDialogCtrl(),
       m_trainVersionManager(NULL),
       m_trainDownloadManager(NULL),
       m_mfcTrainSelector(NULL),
       m_trainSelector(NULL)
    {
        FUNCTION_ENTRY( "CTrainDownloadTab" );

        AddPage(m_libraryDownloadPage,IDD_LIBRARY_DOWNLOAD,CString("Message Library"));
        AddPage(m_scheduleDownloadPage,IDD_SCHEDULE_DOWNLOAD,CString("Time Schedule"));

        FUNCTION_EXIT;
    }


    CTrainDownloadTab::~CTrainDownloadTab()
    {
        FUNCTION_ENTRY( "~CTrainDownloadTab" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CTrainDownloadTab, CTabDialogCtrl)
        //{{AFX_MSG_MAP(CTrainDownloadTab)
        ON_MESSAGE(WM_UPDATEINBOUND, OnUpdateInboundList)
        ON_MESSAGE(WM_UPDATEOUTBOUND, OnUpdateOutboundList)
        ON_MESSAGE(WM_UPDATESELECTIONCOMBO, OnUpdateSelectionCombo)
        ON_MESSAGE(WM_UPDATE_TRAINVERSION, OnUpdateTrainVersion)
        ON_MESSAGE(WM_UPDATE_DOWNLOADLIST, OnUpdateDownloadList)
        ON_WM_DESTROY()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CTrainDownloadTab message handlers

    void CTrainDownloadTab::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        CTabDialogCtrl::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    void CTrainDownloadTab::Init(REBProgressManager & mgr)
    {
        FUNCTION_ENTRY( "Init" );

        // receives train version change messages
        m_trainVersionManager = new TrainVersionManager(this);

        // the thread that downloads/upgrades to trains. Handles periodic retrying,
        // receives download/upgrade messages and gives them to this window.
        m_trainDownloadManager = new TrainDownloadManager(this);


           mgr.SetStaticText(0, "Initializing the Train Download Page: set download manager");
        m_libraryDownloadPage.setDownloadManager(*m_trainDownloadManager);
        m_libraryDownloadPage.Init();

        m_scheduleDownloadPage.setDownloadManager(*m_trainDownloadManager);
        m_scheduleDownloadPage.Init();
        mgr.SetProgress(80);

           mgr.SetStaticText(0, "Initializing the Train Download Page: set version manager");
        m_libraryDownloadPage.setVersionManager(*m_trainVersionManager);
        m_scheduleDownloadPage.setVersionManager(*m_trainVersionManager);
        mgr.SetProgress(90);

        // start the thread for upgrade/downloading
        m_trainDownloadManager->start();

        //
        // Set up the train selection componenet
        //

        // this receives the updates and posts them - to resolve threading issues
        m_mfcTrainSelector = new MFCTrainSelectorGUI( this,
                                                      WM_UPDATEINBOUND,
                                                      WM_UPDATEOUTBOUND,
                                                      WM_UPDATESELECTIONCOMBO );

        // This actually gets the updates
        try
        {
            // the train selection component
            // subscribe for all trains
            m_trainSelector = new TrainSelector( *m_mfcTrainSelector,
                                                 TA_IRS_Bus::LocationCache::getInstance().getLocationKey(),
                                                 TA_IRS_Bus::LocationCache::All );
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

        FUNCTION_EXIT;
    }


    void CTrainDownloadTab::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        CTabDialogCtrl::OnDestroy();

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

        if (m_trainDownloadManager != NULL)
        {
            m_trainDownloadManager->terminateAndWait();
            delete m_trainDownloadManager;
            m_trainDownloadManager = NULL;
        }

        if ( m_trainVersionManager != NULL)
        {
            delete m_trainVersionManager;
            m_trainVersionManager = NULL;
        }

        FUNCTION_EXIT;
    }


    LRESULT CTrainDownloadTab::OnUpdateInboundList(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateInboundList" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        updateTrainLists(update->action, update->trainDetails);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CTrainDownloadTab::OnUpdateOutboundList(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateOutboundList" );

        TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate* update =
            reinterpret_cast<TA_IRS_Bus::MFCTrainSelectorGUI::TrainListUpdate*>(wParam);

        updateTrainLists(update->action, update->trainDetails);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CTrainDownloadTab::OnUpdateSelectionCombo(WPARAM wParam, LPARAM lParam)
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


    void CTrainDownloadTab::updateTrainLists( TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                            const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
    {
        FUNCTION_ENTRY( "updateTrainLists" );

        // this should never be null
        TA_ASSERT(m_trainVersionManager != NULL, "Update listener is null");

        // pass the update on
        m_libraryDownloadPage.updateTrainList(action, trainDetails);
        m_scheduleDownloadPage.updateTrainList(action, trainDetails);

        FUNCTION_EXIT;
    }


    LRESULT CTrainDownloadTab::OnUpdateTrainVersion(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateTrainVersion" );

        // cast the train version structure
        TrainVersionManager::TrainLibraryAndTimeScheduleVersion* update =
            reinterpret_cast<TrainVersionManager::TrainLibraryAndTimeScheduleVersion*>(wParam);

        // send the version to the train lists
        m_libraryDownloadPage.updateTrainVersion(update->trainNumber);
        m_scheduleDownloadPage.updateTrainVersion(update->trainNumber);

        delete update;

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT CTrainDownloadTab::OnUpdateDownloadList(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnUpdateDownloadList" );

        DownloadStatusDisplay* update =
            reinterpret_cast<DownloadStatusDisplay*>(wParam);

        // send the download list update to the appropriate page
        switch(update->downloadType)
        {
        case LibraryDownload:
        case LibraryUpgrade:
            m_libraryDownloadPage.updateDownloadList(*update);
            break;
        case ScheduleDownload:
        case ScheduleUpgrade:
            m_scheduleDownloadPage.updateDownloadList(*update);
            break;
        }

        delete update;

        FUNCTION_EXIT;
        return 0;
    }

} // TA_IRS_App

