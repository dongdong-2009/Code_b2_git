/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/LibraryVersionListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * This list is formatted to display library versions.
  */

#include "stdafx.h"

#include "LibraryVersionListCtrl.h"
#include "TrainVersionManager.h"
#include "TTISCommonConfiguration.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "LocationsManager.h"
#include "app/trains/TrainBorne_PIDS/src/RunThreadedFunc.h"
#include "boost/bind.hpp"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Bus::ListCtrlSelNoFocus;


#define WM_LIBRARYVERSIONUPDATED_MSG WM_USER

namespace
{
	const std::string CURRENT_TTIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentTTISLibraryVersion");
	const std::string NEXT_TTIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-NextTTISLibraryVersion");
}

namespace TA_IRS_App
{


    LibraryVersionListCtrl::LibraryVersionListCtrl()
        : TA_IRS_Bus::ILibraryVersionListener(),
		m_locationName("")
    {
        FUNCTION_ENTRY( "LibraryVersionListCtrl" );
		unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
		TA_Base_Core::ILocation* location = LocationsManager::instance().getLocationByKey(locationKey);
		if(location != NULL)
		{
			m_locationName = location->getName();
		}
        FUNCTION_EXIT;
    }


    LibraryVersionListCtrl::~LibraryVersionListCtrl()
    {
        FUNCTION_ENTRY( "~LibraryVersionListCtrl" );

        TA_IRS_Bus::TTISLibraryVersionMonitor::instance().deregisterForChanges( this );
        TA_IRS_Bus::TTISLibraryVersionMonitor::removeInstance();

        FUNCTION_EXIT;
    }

	int LibraryVersionListCtrl::initLocationItems(TA_Base_Core::ILocation* location, int insertIndex)
	{
		TA_ASSERT(location != NULL,"");
		const char* NA = "NA";
		insertIndex = InsertItem(insertIndex, location->getDisplayName().c_str());
		if(-1 != insertIndex)
		{
			SetItemText(insertIndex, CurrentISCS, NA);
			SetItemText(insertIndex, NextISCS, NA);
			SetItemData(insertIndex, location->getKey());
		}
		return insertIndex;
	}

    void LibraryVersionListCtrl::Init()
    {
        FUNCTION_ENTRY( "Init" );

        // set the style
        SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
        m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());

        setNonSelectableGray(false);
        setScrollBarVisibility(true, false);

        // set up the column headers
        RECT rect;
        GetClientRect(&rect);

        int stationHeaderSize = 120;
        int otherColumnSize = (rect.right - stationHeaderSize) / 2;

        InsertColumn(Station, "Location", LVCFMT_CENTER);
        InsertColumn(CurrentISCS, "Current ISCS Version", LVCFMT_CENTER);
        InsertColumn(NextISCS, "Next ISCS Version", LVCFMT_CENTER);

        SetColumnWidth(Station, stationHeaderSize);
        SetColumnWidth(CurrentISCS, otherColumnSize);
        SetColumnWidth(NextISCS, otherColumnSize);

		// Init data, set all version value to "N/A"
		// First & second list item should be occ & depot, station version info insert from 2
		SetRedraw(FALSE);

		// Fill up init data
		if("OCC" == m_locationName)
		{
			std::vector<TA_Base_Core::ILocation*> locations = LocationsManager::instance().getAllLocations();
			const char* NA = "N/A";
			std::vector<TA_Base_Core::ILocation*> occLocations, dptLocations;
			int insertIndex = -1;
			for(std::vector<TA_Base_Core::ILocation*>::iterator it = locations.begin(); it != locations.end(); ++it)
			{
				unsigned long key = (*it)->getKey();
				TA_Base_Core::ILocation::ELocationType locationType = (*it)->getLocationType();
				std::string displayName = (*it)->getDisplayName();
				if(key > 0)
				{
					if(locationType == TA_Base_Core::ILocation::OCC)
					{
						occLocations.push_back(*it);
					}
					else if(locationType == TA_Base_Core::ILocation::DPT)
					{
						dptLocations.push_back(*it);
					}
					else
					{
						insertIndex = initLocationItems(*it, ++insertIndex);
					}
				}
			}
			// Ensure occ & dpt be inserted as first and second
			for(std::vector<TA_Base_Core::ILocation*>::iterator it = dptLocations.begin(); it != dptLocations.end(); ++it)
			{
				initLocationItems(*it, 0);
			}
			for(std::vector<TA_Base_Core::ILocation*>::iterator it = occLocations.begin(); it != occLocations.end(); ++it)
			{
				initLocationItems(*it, 0);
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
		if(m_locationName != "OCC")
		{
			std::map<std::string, TA_IRS_Bus::ELibraryVersionPointType> dpPostNameToType;
			dpPostNameToType[m_locationName+CURRENT_TTIS_STATION_LIBRARY_VERSION_DP_NAME] = TA_IRS_Bus::CURRENT_VERSION;
			dpPostNameToType[m_locationName+NEXT_TTIS_STATION_LIBRARY_VERSION_DP_NAME] = TA_IRS_Bus::NEXT_VERSION;
			TA_IRS_Bus::TTISLibraryVersionMonitor::setDPPostfixNameToType(dpPostNameToType);
		}
		TA_IRS_Bus::TTISLibraryVersionMonitor::instance().registerForChanges( this );
		runThreadedFunc(boost::bind(&TA_IRS_Bus::TTISLibraryVersionMonitor::setupVersionInfo, &TA_IRS_Bus::TTISLibraryVersionMonitor::instance()));
        FUNCTION_EXIT;
    }

    void LibraryVersionListCtrl::libraryVersionUpdate( TA_IRS_Bus::LibraryVersionUpdateInfo libraryVersion )
    {
        FUNCTION_ENTRY( "libraryVersionUpdate" );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Library version update:%d,%d,%d", libraryVersion.locationKey, libraryVersion.entityKey, libraryVersion.newValue);
		WPARAM wparam = (WPARAM)(new TA_IRS_Bus::LibraryVersionUpdateInfo(libraryVersion));
		PostMessage(WM_LIBRARYVERSIONUPDATED_MSG, wparam, 0);
        FUNCTION_EXIT;
    }


	LRESULT LibraryVersionListCtrl::OnLibraryVersionUpdate(WPARAM wparam, LPARAM nouse)
	{
		TA_IRS_Bus::LibraryVersionUpdateInfo* updateInfo = (TA_IRS_Bus::LibraryVersionUpdateInfo*)wparam;
		if( NULL != updateInfo)
		{
			for(int i = 0, size = GetItemCount(); i < size; ++i)
			{
				if(GetItemData(i) == updateInfo->locationKey)
				{
					int subItem = (updateInfo->type == TA_IRS_Bus::CURRENT_VERSION ? CurrentISCS : NextISCS);
					SetItemText(i, subItem, TrainVersionManager::convertTrainVersionToStr(updateInfo->newValue).c_str());
					break;
				}
			}
			delete updateInfo;
		}
		return 0;
	}

    bool LibraryVersionListCtrl::isSelectable(int rowNumber)
    {
        FUNCTION_ENTRY( "isSelectable" );
        FUNCTION_EXIT;
        return false;
    }


	BEGIN_MESSAGE_MAP(LibraryVersionListCtrl, TA_Base_Bus::ListCtrlSelNoFocus)
		ON_MESSAGE(WM_LIBRARYVERSIONUPDATED_MSG, OnLibraryVersionUpdate)
	END_MESSAGE_MAP()
} // TA_IRS_App
