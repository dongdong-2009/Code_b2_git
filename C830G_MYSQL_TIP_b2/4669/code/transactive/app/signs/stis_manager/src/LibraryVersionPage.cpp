/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/LibraryVersionPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This page shows the library versions at each location.
  */

#include "stdafx.h"
#include "LibraryVersionPage.h"
#include "RightsManager.h"
#include "STISPredefinedMessages.h"
#include "LibraryVersionMonitor.h"
#include "UserMessages.h"
#include "REBProgressManager.h"
#include "MainTab.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"


#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MainTab.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Bus::ListCtrlSelNoFocus;

namespace TA_IRS_App
{

    LibraryVersionPage::LibraryVersionPage(CWnd* pParent /*=NULL*/)
        : CDialog(LibraryVersionPage::IDD, pParent),
          m_canUpgradeISCS(false),
          m_currentLibraryVersion(0),
          m_nextLibraryVersion(0),
          m_librariesAreSynchronised(false),
          m_locationName("")
    {
        FUNCTION_ENTRY( "LibraryVersionPage" );

        // {{AFX_DATA_INIT(LibraryVersionPage)
            // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }

	 LibraryVersionPage::~LibraryVersionPage()
	 {
		 TA_Base_Core::ThreadGuard guard2( MainTab::s_downLoadPageLock );

		 m_stisPredefinedMessages->deregisterCurrentVersionUser( this );
		 m_stisPredefinedMessages->deregisterNextVersionUser( this );
		 m_stisPredefinedMessages->deregisterLibrarySynchronisedUser( this );

		 m_libraryVersionMonitor->deregisterForChanges( this );

		 RightsManager::getInstance().deregisterForRightsChanges(this);

		 // TD11310 ~ added
		 m_libraryVersionMonitor->removeInstance();
		 m_stisPredefinedMessages->removeInstance();
	 }

    void LibraryVersionPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(LibraryVersionPage)
        DDX_Control(pDX, IDC_VERSION_LIST_SUBHEADER, m_subHeader);
        DDX_Control(pDX, IDC_VERSION_LIST_MAIN_HEADER, m_mainHeader);
        DDX_Control(pDX, IDC_UPGRADE_ISCS, m_upgradeISCS);
        DDX_Control(pDX, IDC_STATION_VERSION_LIST, m_stationVersionList);
        DDX_Control(pDX, IDC_ISCS_NEXT_VERSION, m_nextISCSVersion);
        DDX_Control(pDX, IDC_ISCS_CURRENT_VERSION, m_currentISCSVersion);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(LibraryVersionPage, CDialog)
        // {{AFX_MSG_MAP(LibraryVersionPage)
        ON_WM_DESTROY()
        ON_BN_CLICKED(IDC_UPGRADE_ISCS, OnUpgradeIscs)
        ON_MESSAGE(WM_UPDATE_LIBRARY_VERSION, OnLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_VERSION, OnCurrentLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_NEXT_STIS_VERSION, OnNextLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_LIBRARY_SYNCHRONISED, OnOCCLibrariesSynchronisedChanged)
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL LibraryVersionPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void LibraryVersionPage::Init()
    {
        FUNCTION_ENTRY( "Init" );

        // TD1131 ~ added
        m_libraryVersionMonitor = LibraryVersionMonitor::getInstance();
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();

        //mgr.SetProgress(60);
        //mgr.SetStaticText(0, "Initializing the Library Version Page: get rights");
        // access control
        m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS();
        RightsManager::getInstance().registerForRightsChanges(this);

        // Establish the OCC message library version info for the main display
        // (the table information is established via the LibraryVersionMonitor

        //mgr.SetProgress(70);
        //mgr.SetStaticText(0, "Initializing the Library Version Page: get location key");
        // get the location name
        std::stringstream locationKeyStream;
        locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
        unsigned long locationKey = 0;
        locationKeyStream >> locationKey;

        TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);

        // insert it into the map
        m_locationName = location->getName();

        // clean up
        delete location;
        //mgr.SetProgress(80);
        m_currentLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();
        m_nextLibraryVersion = m_stisPredefinedMessages->getNextMessageLibraryVersion();
        m_librariesAreSynchronised = m_stisPredefinedMessages->getMessageLibrarySynchronised();

        m_stisPredefinedMessages->registerCurrentVersionUser( this );
        m_stisPredefinedMessages->registerNextVersionUser( this );
        m_stisPredefinedMessages->registerLibrarySynchronisedUser( this );

        //mgr.SetStaticText(0, "Registering to receive library version datapoint updates");
        // Register to receive library version datapoint updates
        m_libraryVersionMonitor->registerForChanges( this );

        // Retrieve current library version info for all locations
        // the info is stored as a map of entity keys to values
        m_libraryVersionInfo = m_libraryVersionMonitor->getAllLibraryVersions();

        //mgr.SetProgress(85);
        //mgr.SetStaticText(0, "Setup the version lists");
        // Setup the version lists - create the column headings etc
        setupLists();

        // populate the initial data into the lists
        populatePageData();

        //mgr.SetProgress(93);
        //mgr.SetStaticText(0, "Set the initial state of the upgrade button");
        // Set the initial state of the upgrade button
        enableUpgradeIscsButton();
        //mgr.SetProgress(97);

        FUNCTION_EXIT;
    }


    void LibraryVersionPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    LRESULT LibraryVersionPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onRightsChanged" );

        // the rights have changed - re check them
        m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS();

        // re-enable button
        enableUpgradeIscsButton();

        FUNCTION_EXIT;
        return 0;
    }


    void LibraryVersionPage::setupLists()
    {
        FUNCTION_ENTRY( "setupLists" );

        // set the column headers
        RECT rect;
        m_mainHeader.GetClientRect(&rect);

        int stationHeaderSize = 80;
        int currentIscsHeaderSize = 150;
        int currentStisHeaderSize = 150;
        int nextIscsHeaderSize = 150;
        int nextStisHeaderSize = 150;
        int spareHeaderSize = rect.right - (stationHeaderSize +
                                            currentIscsHeaderSize +
                                            currentStisHeaderSize +
                                            nextIscsHeaderSize +
                                            nextStisHeaderSize);

        // Fancy pants headers
        m_mainHeader.setScrollBarVisibility(false, false);
        m_mainHeader.InsertColumn(0, "", LVCFMT_CENTER);
        m_mainHeader.InsertColumn(1, "", LVCFMT_CENTER);
        m_mainHeader.InsertColumn(2, "", LVCFMT_CENTER);
        m_mainHeader.InsertColumn(3, "", LVCFMT_CENTER);
        m_mainHeader.SetColumnWidth(0, stationHeaderSize);
        m_mainHeader.SetColumnWidth(1, currentIscsHeaderSize + currentStisHeaderSize);
        m_mainHeader.SetColumnWidth(2, nextIscsHeaderSize + nextStisHeaderSize);
        m_mainHeader.SetColumnWidth(3, spareHeaderSize);
        m_mainHeader.InsertItem(0, "");
        m_mainHeader.SetItemText(0, 1, "Current Version");
        m_mainHeader.SetItemText(0, 2, "Next Version");
        m_mainHeader.setItemColour( 0, COLORREF( RGB( 255, 255, 255 ) ),
                                    ListCtrlSelNoFocus::I_INDEX );
        m_mainHeader.setItemColour( 0, COLORREF( RGB( 0, 0, 0 ) ),
                                    ListCtrlSelNoFocus::I_INDEX,
                                    ListCtrlSelNoFocus::CT_BACKGROUND );

        m_subHeader.setScrollBarVisibility(false, false);
        m_subHeader.InsertColumn(Station, "", LVCFMT_CENTER);
        m_subHeader.InsertColumn(CurrentISCS, "", LVCFMT_CENTER);
        m_subHeader.InsertColumn(CurrentSTIS, "", LVCFMT_CENTER);
        m_subHeader.InsertColumn(NextISCS, "", LVCFMT_CENTER);
        m_subHeader.InsertColumn(NextSTIS, "", LVCFMT_CENTER);
        m_subHeader.InsertColumn(Spare, "", LVCFMT_CENTER);
        m_subHeader.InsertItem(0, "LOC");
        m_subHeader.SetItemText(0, CurrentISCS, "ISCS");
        m_subHeader.SetItemText(0, CurrentSTIS, "STIS");
        m_subHeader.SetItemText(0, NextISCS, "ISCS");
        m_subHeader.SetItemText(0, NextSTIS, "STIS");
        m_subHeader.setItemColour( 0, COLORREF( RGB( 255, 255, 255 ) ),
                                   ListCtrlSelNoFocus::I_INDEX );
        m_subHeader.setItemColour( 0, COLORREF( RGB( 0, 0, 0 ) ),
                                   ListCtrlSelNoFocus::I_INDEX,
                                   ListCtrlSelNoFocus::CT_BACKGROUND );

        m_subHeader.SetColumnWidth(Station, stationHeaderSize);
        m_subHeader.SetColumnWidth(CurrentISCS, currentIscsHeaderSize);
        m_subHeader.SetColumnWidth(CurrentSTIS, currentStisHeaderSize);
        m_subHeader.SetColumnWidth(NextISCS, nextIscsHeaderSize);
        m_subHeader.SetColumnWidth(NextSTIS, nextStisHeaderSize);
        m_subHeader.SetColumnWidth(Spare, spareHeaderSize);


        m_stationVersionList.InsertColumn(Station, "");
        m_stationVersionList.InsertColumn(CurrentISCS, "", LVCFMT_RIGHT);
        m_stationVersionList.InsertColumn(CurrentSTIS, "", LVCFMT_RIGHT);
        m_stationVersionList.InsertColumn(NextISCS, "", LVCFMT_RIGHT);
        m_stationVersionList.InsertColumn(NextSTIS, "", LVCFMT_RIGHT);
        m_stationVersionList.InsertColumn(Spare, "");

        m_stationVersionList.SetColumnWidth(Station, stationHeaderSize);
        m_stationVersionList.SetColumnWidth(CurrentISCS, currentIscsHeaderSize);
        m_stationVersionList.SetColumnWidth(CurrentSTIS, currentStisHeaderSize);
        m_stationVersionList.SetColumnWidth(NextISCS, nextIscsHeaderSize);
        m_stationVersionList.SetColumnWidth(NextSTIS, nextStisHeaderSize);
        m_stationVersionList.SetColumnWidth(Spare, spareHeaderSize);

        /*try
        {
            std::vector<TA_Base_Core::ILocation*> locations =
            TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

            std::vector<TA_Base_Core::ILocation*>::iterator iter = locations.begin();

            for ( ; iter != locations.end(); iter++)
            {
                LocationInfo locationInfo;

                int locationKey = (*iter)->getKey();

                if ( locationKey != 0)
                {
                    std::string locationName = (*iter)->getName();

                    locationInfo.name = locationName;
                    locationInfo.locationKey = locationKey;

                    locationInfo.currentISCSLibraryVersionKey = getEntityKeyFromName( std::string( locationName + CURRENT_ISCS_STATION_LIBRARY_VERSION_DP_NAME ) );
                    locationInfo.nextISCSLibraryVersionKey    = getEntityKeyFromName( std::string( locationName + NEXT_ISCS_STATION_LIBRARY_VERSION_DP_NAME ) );
                    locationInfo.currentSTISLibraryVersionKey = getEntityKeyFromName( std::string( locationName + CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME ) );
                    locationInfo.nextSTISLibraryVersionKey    = getEntityKeyFromName( std::string( locationName + NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME ) );

                    m_locationInfo.insert( std::pair<unsigned long, LocationInfo>( locationKey, locationInfo ) );

                }

            }


            for ( iter = locations.begin() ; iter != locations.end(); iter++)
            {
                delete (*iter);
            }
        }
        catch(TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch(TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }*/

        stationLibraryVersionList currentISCSLibraryVersionList;
        stationLibraryVersionList nextISCSLibraryVersionList;
        stationLibraryVersionList currentSTISLibraryVersionList;
        stationLibraryVersionList nextSTISLibraryVersionList;

        getLibraryVersion(CURRENT_ISCS_STATION_LIBRARY_VERSION_DP_NAME,currentISCSLibraryVersionList);
        getLibraryVersion(NEXT_ISCS_STATION_LIBRARY_VERSION_DP_NAME,nextISCSLibraryVersionList);
        getLibraryVersion(CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME,currentSTISLibraryVersionList);
        getLibraryVersion(NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME,nextSTISLibraryVersionList);

        stationLibraryVersionList::iterator currentISCSLibraryiter     = currentISCSLibraryVersionList.begin();



        for ( ; currentISCSLibraryiter != currentISCSLibraryVersionList.end(); ++currentISCSLibraryiter )
        {
            LocationInfo locationInfo;
            locationInfo.locationKey                  = currentISCSLibraryiter->locationkey;
            locationInfo.name                         = currentISCSLibraryiter->locationName;
            locationInfo.currentISCSLibraryVersionKey = currentISCSLibraryiter->currentVersionKey;

			stationLibraryVersionList::iterator nextISCSlibraryiter        = nextISCSLibraryVersionList.begin();
			stationLibraryVersionList::iterator currentSTISlibraryiter     = currentSTISLibraryVersionList.begin();
			stationLibraryVersionList::iterator nextSTISlibraryiter        = nextSTISLibraryVersionList.begin();

            for ( nextISCSlibraryiter; nextISCSlibraryiter != nextISCSLibraryVersionList.end(); ++nextISCSlibraryiter )
            {
                if ( currentISCSLibraryiter->locationkey == nextISCSlibraryiter->locationkey )
                {
                    locationInfo.nextISCSLibraryVersionKey = nextISCSlibraryiter->currentVersionKey;
                    break;
                 }
            }

            for (currentSTISlibraryiter; currentSTISlibraryiter != currentSTISLibraryVersionList.end(); ++currentSTISlibraryiter)
            {
                if ( currentISCSLibraryiter->locationkey == currentSTISlibraryiter->locationkey)
                {
                    locationInfo.currentSTISLibraryVersionKey = currentSTISlibraryiter->currentVersionKey;
                    break;
                }
            }

            for (nextSTISlibraryiter; nextSTISlibraryiter != nextSTISLibraryVersionList.end(); ++ nextSTISlibraryiter )
            {
                if ( currentISCSLibraryiter->locationkey == nextSTISlibraryiter->locationkey )
                {
                    locationInfo.nextSTISLibraryVersionKey = nextSTISlibraryiter->currentVersionKey;
                }

            }

            TA_Base_Core::ThreadGuard guard( m_versionInfoLock );
            m_locationInfo.insert( std::pair< unsigned long, LocationInfo >( currentISCSLibraryiter->locationkey, locationInfo ));
        }

        FUNCTION_EXIT;
    }


    void LibraryVersionPage::populatePageData()
    {
        FUNCTION_ENTRY( "populatePageData" );

        TA_Base_Core::ThreadGuard guard(m_versionInfoLock);

        CString str;
        str.Format("%03d", m_currentLibraryVersion);
        m_currentISCSVersion.SetWindowText(str);

        str.Format("%03d", m_nextLibraryVersion);
        m_nextISCSVersion.SetWindowText(str);

        // Get all locations from the database
        // start with OCC
        // then add each station

        CString currentISCSVersionString;
        CString nextISCSVersionString;
        CString currentSTISVersionString;
        CString nextSTISVersionString;

        m_stationVersionList.DeleteAllItems();

        try
        {
            std::map<unsigned long, LocationInfo>::iterator iter1 = m_locationInfo.begin();
            std::map<unsigned long, LocationInfo> locationMap;

            for (; iter1 != m_locationInfo.end(); iter1++)
            {
                locationMap.insert(std::pair<unsigned long, LocationInfo>(iter1->second.locationKey, iter1->second));
            }

            int count = 0;
            std::map<unsigned long, LocationInfo>::iterator iter = locationMap.begin();
            for ( ; iter != locationMap.end(); iter++, count++)
            {
                // hongran++ TD17500
                int i;

                // If the location name is occ, then show the location name as OCCA in list.
                if ((*iter).second.name == "OCC")
                {
                    i = m_stationVersionList.InsertItem(count, "OCCA");
                }
                else
                {
                    i = m_stationVersionList.InsertItem(count, (*iter).second.name.c_str());
                }

                std::map<unsigned long, unsigned short>::iterator pointInfo;

                if( (*iter).second.currentISCSLibraryVersionKey != 0 )
                {
                    std::stringstream versionStream;
                    pointInfo = m_libraryVersionInfo.find( (*iter).second.currentISCSLibraryVersionKey );
                    versionStream << std::setw(3) << std::setfill('0') << pointInfo->second;
                    currentISCSVersionString = versionStream.str().c_str();
                }
                else
                {
                    currentISCSVersionString = "N/A";
                }

                if( (*iter).second.currentSTISLibraryVersionKey != 0 )
                {
                    std::stringstream versionStream;
                    pointInfo = m_libraryVersionInfo.find( (*iter).second.currentSTISLibraryVersionKey );
                    versionStream << std::setw(3) << std::setfill('0')  << pointInfo->second;
                    currentSTISVersionString = versionStream.str().c_str();
                }
                else
                {
                    currentSTISVersionString = "N/A";
                }

                if( (*iter).second.nextISCSLibraryVersionKey != 0 )
                {
                    std::stringstream versionStream;
                    pointInfo = m_libraryVersionInfo.find( (*iter).second.nextISCSLibraryVersionKey );
                    versionStream << std::setw(3) << std::setfill('0')  << pointInfo->second;
                    nextISCSVersionString = versionStream.str().c_str();
                }
                else
                {
                    nextISCSVersionString = "N/A";
                }

                if( (*iter).second.nextSTISLibraryVersionKey != 0 )
                {
                    std::stringstream versionStream;
                    pointInfo = m_libraryVersionInfo.find( (*iter).second.nextSTISLibraryVersionKey );
                    versionStream << std::setw(3) << std::setfill('0')  << pointInfo->second;
                    nextSTISVersionString = versionStream.str().c_str();
                }
                else
                {
                    nextSTISVersionString = "N/A";
                }

                m_stationVersionList.SetItemText(i, CurrentISCS, currentISCSVersionString);
                m_stationVersionList.SetItemText(i, CurrentSTIS, currentSTISVersionString);
                m_stationVersionList.SetItemText(i, NextISCS, nextISCSVersionString);
                m_stationVersionList.SetItemText(i, NextSTIS, nextSTISVersionString);
                m_stationVersionList.SetItemData(i, (*iter).second.locationKey);


                // Display the OCC points in red
                if( (*iter).second.name == "OCC" )
                {
                    m_stationVersionList.setItemColour( (*iter).second.locationKey,
                                                        COLORREF( RGB( 255, 0, 0 ) ),
                                                        ListCtrlSelNoFocus::I_ITEMDATA );
                }
                else // otherwise display in black
                {
                    m_stationVersionList.setItemColour( (*iter).second.locationKey,
                                                        COLORREF( RGB( 0, 0, 0 ) ),
                                                        ListCtrlSelNoFocus::I_ITEMDATA );
                }
            }

        }
        catch (...)
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "LibraryVersionPage::populatePageData() Unknown Exception");
        }

        FUNCTION_EXIT;
    }


    void LibraryVersionPage::enableUpgradeIscsButton()
    {
        FUNCTION_ENTRY( "enableUpgradeIscsButton" );

        // enable the upgrade ISCS button if
        // - the libraries are synchronised across the stations
        // - The library hasnt been upgraded yet

        // TD 10971
        bool enableButton = ((m_locationName.compare("OCC") == 0 ) &&
                m_canUpgradeISCS == true &&
                m_librariesAreSynchronised == true &&
                (m_currentLibraryVersion != m_nextLibraryVersion));

        GetDlgItem(IDC_UPGRADE_ISCS)->EnableWindow(enableButton);

        FUNCTION_EXIT;
    }


    void LibraryVersionPage::OnUpgradeIscs()
    {
        FUNCTION_ENTRY( "OnUpgradeIscs" );

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        // xufeng++ 2006/11/14 TD17539
        // ask the question
        int ret = UserMessages::getInstance().askQuestion(UserMessages::QUESTION_UPGRADE_LIBRARY);
        if( ret != IDYES )
        {
            FUNCTION_EXIT;
            return;
        }
        // ++xufeng 2006/11/14 TD17539

      // try
       // {
            unsigned short version = static_cast<unsigned short>( m_nextLibraryVersion );

            /*TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent()->
                upgradePredefinedStationMessageLibrary( version, sessionId.c_str() );*/
		//libo++
		try
		{
			CORBA_CALL(TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(),upgradePredefinedStationMessageLibrary,
				(version, sessionId.c_str()));

		}
		catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
			CORBA_CALL(TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(),upgradePredefinedStationMessageLibrary,
				(version, sessionId.c_str()));
		}
		//++libo
      //  }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % "Upgrade STIS predefined message library" % "Could not resolve TIS Agent" ).c_str());
        }
        catch ( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % "Upgrade STIS predefined message library" % "Could not resolve TIS Agent" ).c_str());
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "While attempting to upgrade the STIS predefined message library" );

            UserMessages::getInstance().displayError(
                str( format(UserMessages::ERROR_REQUEST_FAILED)
                % "Upgrade STIS predefined message library" % "Could not resolve TIS Agent" ).c_str());
        }

        FUNCTION_EXIT;
    }


    void LibraryVersionPage::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void LibraryVersionPage::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );
        FUNCTION_EXIT;
    }


    BOOL LibraryVersionPage::PreTranslateMessage(MSG* pMsg)
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
                        // do nothing
                    }
                    break;
                }
            }
            break;
        default:
            {
                // do nothing
            }
        }

        FUNCTION_EXIT;
        return CDialog::PreTranslateMessage(pMsg);
    }


    unsigned long LibraryVersionPage::getEntityKeyFromName( const std::string& entityName )
    {
        FUNCTION_ENTRY( "getEntityKeyFromName" );

        CString versionInfoString = "N/A";

        TA_Base_Core::IEntityData* entityData = NULL;
        unsigned long entityKey = 0;

        try
        {
            entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
        }
        catch (const TA_Base_Core::DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch (const TA_Base_Core::DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch (...)
        {
            // data point proxy creation error
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxy" );
        }


        if( entityData )
        {
            entityKey = entityData->getKey();
            delete entityData;
        }

        FUNCTION_EXIT;
        return entityKey;
    }


    void LibraryVersionPage::getLibraryVersion(const std::string&  TTISStationLibraryVersionName, stationLibraryVersionList& libraryVersionList)
    {
        FUNCTION_ENTRY( "getLibraryVersion" );

        TA_Base_Core::IEntityDataList entiyDataList;
        try
        {
            entiyDataList = TA_Base_Core::EntityAccessFactory::getInstance().getTTISLibraryVersionEntityFromNamelike( TTISStationLibraryVersionName );
        }
        catch (const TA_Base_Core::DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch (const TA_Base_Core::DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxy" );
        }
        LocationVerionInfo locationInfo;
        for ( TA_Base_Core::IEntityDataList::iterator iter = entiyDataList.begin(); iter != entiyDataList.end(); ++iter )
        {
            locationInfo.currentVersionKey = (*iter)->getKey();
            locationInfo.locationkey       = (*iter)->getLocation();
            locationInfo.locationName      = (*iter)->getLocationName();
            locationInfo.nextVersionkey    = (*iter)->getKey();
            libraryVersionList.push_back( locationInfo );
            delete(*iter);
            *iter = NULL;
        }
        entiyDataList.clear();

        FUNCTION_EXIT;
    }


    LRESULT LibraryVersionPage::OnLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnLibraryVersionChanged" );

        // return 0;
        TA_Base_Core::ThreadGuard guard(m_versionInfoLock);

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                 "Received library version datapoint change");

        LibraryVersionInfo *versionInfo = NULL;

        // reinterpret the parameter
        versionInfo = reinterpret_cast<LibraryVersionInfo*>(wParam);

        if( versionInfo != NULL )
        {
            // Use it
            std::map<unsigned long, unsigned short>::iterator pointInfo =
                m_libraryVersionInfo.find( versionInfo->entityKey );

            if( pointInfo != m_libraryVersionInfo.end() )
            {
                pointInfo->second = versionInfo->value;
            }

            // Get rid of it
            delete versionInfo;
        }

        populatePageData();

        FUNCTION_EXIT;
        return 0;
    }


    // Only the OCC should have registered for these
    LRESULT LibraryVersionPage::OnCurrentLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnCurrentLibraryVersionChanged" );

        TA_Base_Core::ThreadGuard guard(m_versionInfoLock);

        m_currentLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();

        CString str;
        str.Format("%03d", m_currentLibraryVersion);
        m_currentISCSVersion.SetWindowText(str);

        enableUpgradeIscsButton();

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT LibraryVersionPage::OnNextLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnNextLibraryVersionChanged" );

        TA_Base_Core::ThreadGuard guard(m_versionInfoLock);

        m_nextLibraryVersion = m_stisPredefinedMessages->getNextMessageLibraryVersion();

        CString str;
        str.Format("%03d", m_nextLibraryVersion);
        m_nextISCSVersion.SetWindowText(str);

        enableUpgradeIscsButton();

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT LibraryVersionPage::OnOCCLibrariesSynchronisedChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnOCCLibrariesSynchronisedChanged" );

        m_librariesAreSynchronised = m_stisPredefinedMessages->getMessageLibrarySynchronised();

        // If rights and synchronisation permit...
        enableUpgradeIscsButton();

        FUNCTION_EXIT;
        return 0;
    }


} // TA_IRS_App


