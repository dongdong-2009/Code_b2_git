/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/LibraryVersionPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #11 $
  *
  * Last modification: $DateTime: 2014/09/26 17:39:18 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This page shows the library versions at each location.
  */

#include "stdafx.h"
#include "app/signs/stis_manager/src/LibraryVersionPage.h"
#include "app/signs/stis_manager/src/RightsManager.h"
#include "app/signs/stis_manager/src/STISPredefinedMessages.h"
#include "app/signs/stis_manager/src/UserMessages.h"
#include "app/signs/stis_manager/src/MainTab.h"
#include "app/signs/stis_manager/src/RunThreadedFunc.h"
#include "bus/signs/tis_agent_access/src/STISLibraryVersionMonitor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/ILocation.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "LocationsManager.h"
#include <iomanip>
#include "boost/bind.hpp"
#include <cstdlib>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MainTab.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Bus::ListCtrlSelNoFocus;

namespace
{
    const std::string CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion");
    const std::string NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-NextSTISLibraryVersion");
    const std::string CURRENT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME(".TIS.STIS.SEV.aiiTISC-CurrentSTISLibraryVersion");
    const std::string NEXT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME(".TIS.STIS.SEV.aiiTISC-NextSTISLibraryVersion");
}


namespace TA_IRS_App
{

    LibraryVersionPage::LibraryVersionPage(CWnd* pParent /*=NULL*/)
        : CDialog(LibraryVersionPage::IDD, pParent),
          m_canUpgradeISCS(false),
          m_currentLibraryVersion(0),
          m_nextLibraryVersion(0),
          m_librariesAreSynchronised(false),
          m_locationName(""),
		  m_stisPredefinedMessages(NULL)
    {
        FUNCTION_ENTRY( "LibraryVersionPage" );

        // {{AFX_DATA_INIT(LibraryVersionPage)
            // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT

		unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
		TA_Base_Core::ILocation* location = LocationsManager::instance().getLocationByKey(locationKey);
		if(location != NULL)
		{
            m_locationName = location->getName();
		}
        FUNCTION_EXIT;
    }

	LibraryVersionPage::~LibraryVersionPage()
	{
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
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(LibraryVersionPage, CDialog)
        // {{AFX_MSG_MAP(LibraryVersionPage)
        ON_WM_DESTROY()
        ON_BN_CLICKED(IDC_UPGRADE_ISCS, OnUpgradeIscs)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_VERSION, OnCurrentLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_NEXT_STIS_VERSION, OnNextLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_LIBRARY_SYNCHRONISED, OnOCCLibrariesSynchronisedChanged)
		ON_MESSAGE(WM_LIBRARYVERSIONUPDATED_MSG, OnLibraryVersionUpdate)
		ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL LibraryVersionPage::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );
        CDialog::OnInitDialog();
        // Setup the version lists - create the column headings etc
        setupLists();
        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


	void LibraryVersionPage::initRightsCheck()
	{
        m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS();
		this->PostMessage(WM_UPDATE_RIGHTS);
        RightsManager::getInstance().registerForRightsChanges(this);
	}

    void LibraryVersionPage::Init()
    {
        FUNCTION_ENTRY( "Init" );

		runThreadedFunc(boost::bind(&LibraryVersionPage::initRightsCheck, this));
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();
        m_currentLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();
        m_nextLibraryVersion = m_stisPredefinedMessages->getNextMessageLibraryVersion();
        m_librariesAreSynchronised = m_stisPredefinedMessages->getMessageLibrarySynchronised();

        m_stisPredefinedMessages->registerCurrentVersionUser( this );
        m_stisPredefinedMessages->registerNextVersionUser( this );
        m_stisPredefinedMessages->registerLibrarySynchronisedUser( this );

		// The function running in another thread
		runThreadedFunc(boost::bind(&STISPredefinedMessages::init, m_stisPredefinedMessages));
		// Station only observer its location datapoints
		if(m_locationName != "OCC")
		{
			std::map<std::string, TA_IRS_Bus::ELibraryVersionPointType> dpPostNameToType;
			dpPostNameToType[m_locationName+CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME] = TA_IRS_Bus::CURRENT_ISCS_VERSION;
			dpPostNameToType[m_locationName+NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME] = TA_IRS_Bus::NEXT_ISCS_VERSION;
			dpPostNameToType[m_locationName+CURRENT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME] = TA_IRS_Bus::CURRENT_STIS_VERSION;
			dpPostNameToType[m_locationName+NEXT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME] = TA_IRS_Bus::NEXT_STIS_VERSION;
			TA_IRS_Bus::STISLibraryVersionMonitor::setDPPostfixNameToType(dpPostNameToType);
		}
		TA_IRS_Bus::STISLibraryVersionMonitor::instance().registerForChanges(this);
		runThreadedFunc(boost::bind(&TA_IRS_Bus::STISLibraryVersionMonitor::setupVersionInfo, &TA_IRS_Bus::STISLibraryVersionMonitor::instance()));
        FUNCTION_EXIT;
    }


	void LibraryVersionPage::libraryVersionUpdate( TA_IRS_Bus::LibraryVersionUpdateInfo libraryVersion )
	{
        FUNCTION_ENTRY( "libraryVersionUpdate" );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Library version update:%d,%d,%d", libraryVersion.locationKey, libraryVersion.entityKey, libraryVersion.newValue);
		WPARAM wparam = (WPARAM)(new TA_IRS_Bus::LibraryVersionUpdateInfo(libraryVersion));
		PostMessage(WM_LIBRARYVERSIONUPDATED_MSG, wparam, 0);
        FUNCTION_EXIT;
	}

	LRESULT LibraryVersionPage::OnLibraryVersionUpdate(WPARAM wparam, LPARAM nouse)
	{
		TA_IRS_Bus::LibraryVersionUpdateInfo* updateInfo = (TA_IRS_Bus::LibraryVersionUpdateInfo*)wparam;
		if( NULL != updateInfo)
		{
			for(int i = 0, size = m_stationVersionList.GetItemCount(); i < size; ++i)
			{
				if(m_stationVersionList.GetItemData(i) == updateInfo->locationKey)
				{
					int subItem = -1;
					if(TA_IRS_Bus::CURRENT_ISCS_VERSION == updateInfo->type )
					{
						subItem = CurrentISCS;
					}
					else if(TA_IRS_Bus::NEXT_ISCS_VERSION == updateInfo->type)
					{
						subItem = NextISCS;
					}
					else if(TA_IRS_Bus::CURRENT_STIS_VERSION == updateInfo->type)
					{
						subItem = CurrentSTIS;
					}
					else
					{
						subItem = NextSTIS;
					}
					char strValue[10];
					m_stationVersionList.SetItemText(i, subItem, itoa(updateInfo->newValue, strValue, 10));
					break;
				}
			}
			delete updateInfo;
		}
		return 0;
	}

    void LibraryVersionPage::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

		if(NULL != m_stisPredefinedMessages)
		{
	        m_stisPredefinedMessages->deregisterCurrentVersionUser( this );
	        m_stisPredefinedMessages->deregisterNextVersionUser( this );
	        m_stisPredefinedMessages->deregisterLibrarySynchronisedUser( this );
			m_stisPredefinedMessages->removeInstance();
		}
		TA_IRS_Bus::STISLibraryVersionMonitor::instance().deregisterForChanges(this);
		TA_IRS_Bus::STISLibraryVersionMonitor::instance().removeInstance();
        RightsManager::getInstance().deregisterForRightsChanges(this);
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

	int LibraryVersionPage::initLocationItems(TA_Base_Core::ILocation* location, int insertIndex)
	{
		TA_ASSERT(location != NULL,"");
		const char* NA = "NA";
		insertIndex = m_stationVersionList.InsertItem(insertIndex, location->getDisplayName().c_str());
		if(-1 != insertIndex)
		{
	        m_stationVersionList.SetItemText(insertIndex, CurrentISCS, NA);
	        m_stationVersionList.SetItemText(insertIndex, CurrentSTIS, NA);
	        m_stationVersionList.SetItemText(insertIndex, NextISCS, NA);
	        m_stationVersionList.SetItemText(insertIndex, NextSTIS, NA);
	        m_stationVersionList.SetItemData(insertIndex, location->getKey());
		}
		return insertIndex;
	}

    void LibraryVersionPage::setupLists()
    {
        FUNCTION_ENTRY( "setupLists" );

        // set the column headers
        RECT rect;
        m_mainHeader.GetClientRect(&rect);

        int stationHeaderSize = 120;
        int otherHeaderSize = (float)(rect.right - stationHeaderSize) / 5.0;
        int currentIscsHeaderSize = otherHeaderSize;
        int currentStisHeaderSize = otherHeaderSize;
        int nextIscsHeaderSize = otherHeaderSize;
        int nextStisHeaderSize = otherHeaderSize;
        int spareHeaderSize = rect.right - otherHeaderSize*4 - stationHeaderSize;

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


        m_stationVersionList.setScrollBarVisibility();
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

		SetRedraw(FALSE);

		// Fill up init data
		if("OCC" == m_locationName)
		{
			int index = -1;
			std::vector<TA_Base_Core::ILocation*> occLocations, dptLocations;
			std::vector<TA_Base_Core::ILocation*> locations = LocationsManager::instance().getAllLocations();
			for(std::vector<TA_Base_Core::ILocation*>::iterator it = locations.begin(); it != locations.end(); ++it)
			{
				if((*it)->getKey() == 0) // All location
				{
					continue;
				}
				if((*it)->getLocationType() == TA_Base_Core::ILocation::OCC)
				{
					occLocations.push_back(*it);
				}
				else if((*it)->getLocationType() == TA_Base_Core::ILocation::DPT)
				{
					dptLocations.push_back(*it);
				}
				else if((*it)->getLocationType() == TA_Base_Core::ILocation::STATION)
				{
					index = initLocationItems(*it, ++index);
				}
			}
			// Occ & dpt should be insert as first and second
			for(std::vector<TA_Base_Core::ILocation*>::iterator it = dptLocations.begin(); it != dptLocations.end(); ++it)
			{
				initLocationItems(*it, 0);
			}
			for(std::vector<TA_Base_Core::ILocation*>::iterator it = occLocations.begin(); it != occLocations.end(); ++it)
			{
				initLocationItems(*it, 0);
                m_stationVersionList.setItemColour((*it)->getKey(),
                                                   COLORREF(RGB( 255, 0, 0 )),
                                                   ListCtrlSelNoFocus::I_ITEMDATA);
			}
		}
		else
		{
			TA_Base_Core::ILocation* location = LocationsManager::instance().getLocationByName(m_locationName);
			if(location != NULL)
			{
				initLocationItems(location, 0);
			}
		}
		SetRedraw(TRUE);
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

        // ask the question
        int ret = UserMessages::getInstance().askQuestion(UserMessages::QUESTION_UPGRADE_LIBRARY);
        if( ret != IDYES )
        {
            FUNCTION_EXIT;
            return;
        }
        unsigned long version = static_cast<unsigned long>( m_nextLibraryVersion );
		try
		{
			CORBA_CALL(TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(),upgradePredefinedStationMessageLibrary,
				(version, sessionId.c_str()));

		}
		CATCH_ALL_EXCEPTIONS( "While attempting to upgrade the STIS predefined message library" );
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
        switch ( pMsg->message )
        {
        case WM_KEYDOWN:
            switch ( pMsg->wParam )
            {
            case VK_ESCAPE:
                return TRUE;
            case VK_RETURN:
                return TRUE;
            default:
                break;
            }
        default:
			break;
        }
        return CDialog::PreTranslateMessage(pMsg);
    }

    // Only the OCC should have registered for these
    LRESULT LibraryVersionPage::OnCurrentLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnCurrentLibraryVersionChanged" );

        TA_Base_Core::ThreadGuard guard(m_versionInfoLock);

        m_currentLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();

        enableUpgradeIscsButton();

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT LibraryVersionPage::OnNextLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnNextLibraryVersionChanged" );

        TA_Base_Core::ThreadGuard guard(m_versionInfoLock);

        m_nextLibraryVersion = m_stisPredefinedMessages->getNextMessageLibraryVersion();


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


