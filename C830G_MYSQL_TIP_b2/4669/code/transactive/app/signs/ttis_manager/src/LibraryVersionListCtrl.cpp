/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/LibraryVersionListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * This list is formatted to display library versions.
  */

#include "stdafx.h"

#include "LibraryVersionListCtrl.h"

#include "app/signs/ttis_manager/src/TTISCommonConfiguration.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Bus::ListCtrlSelNoFocus;


namespace TA_IRS_App
{


    LibraryVersionListCtrl::LibraryVersionListCtrl()
        : TA_IRS_Bus::ILibraryVersionListener()
        , m_libraryVersionInfo()
        , m_locationInfo()
    {
        FUNCTION_ENTRY( "LibraryVersionListCtrl" );
        FUNCTION_EXIT;
    }


    LibraryVersionListCtrl::~LibraryVersionListCtrl()
    {
        FUNCTION_ENTRY( "~LibraryVersionListCtrl" );

        // deregister itself with the TTISLibraryVersionMonitor
        TA_IRS_Bus::TTISLibraryVersionMonitor::getTTISLibraryVersionMonitor().deregisterForChanges( this );
        TA_IRS_Bus::LibraryVersionMonitor::removeFromAllMonitors( this );
        
        TA_IRS_Bus::TTISLibraryVersionMonitor::removeTTISLibraryVersionMonitor();

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(LibraryVersionListCtrl, TA_Base_Bus::ListCtrlSelNoFocus)
        //{{AFX_MSG_MAP(LibraryVersionListCtrl)
        ON_MESSAGE(WM_USER, OnLibraryVersionUpdate)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void LibraryVersionListCtrl::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        CListCtrl::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    void LibraryVersionListCtrl::Init()
    {
        FUNCTION_ENTRY( "Init" );

        // This is messy and should be shifted to the LibraryVersionMonitor somehow
        std::vector<TA_Base_Core::ILocation*> locations =
        TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

        std::vector<TA_Base_Core::ILocation*>::iterator iter = locations.begin();
        for ( ; iter != locations.end(); iter++)
        {
            TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo locationInfo;

            int locationKey = (*iter)->getKey();

            if ( locationKey != 0)
            {
                std::string locationName = (*iter)->getName();

                locationInfo.name = locationName;
                locationInfo.locationKey = locationKey;

                locationInfo.currentTTISLibraryVersionKey =
                    TA_IRS_Bus::LibraryVersionMonitor::getEntityKeyFromName( std::string( locationName + TA_IRS_Bus::TTISLibraryVersionMonitor::CURRENT_TTIS_STATION_LIBRARY_VERSION_DP_NAME ) );
                locationInfo.nextTTISLibraryVersionKey =
                    TA_IRS_Bus::LibraryVersionMonitor::getEntityKeyFromName( std::string( locationName + TA_IRS_Bus::TTISLibraryVersionMonitor::NEXT_TTIS_STATION_LIBRARY_VERSION_DP_NAME ) );

                m_locationInfo.insert( std::pair<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo>( locationKey, locationInfo ) );
            }
        }

        // set the style
        SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
        m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());

        setNonSelectableGray(false);
        setScrollBarVisibility(true, false);

        // Register itself with the TTISLibraryVersionMonitor
        TA_IRS_Bus::TTISLibraryVersionMonitor::getTTISLibraryVersionMonitor().registerForChanges( this );

        //TD 14395
        //zhou yuan
        TA_IRS_Bus::TTISLibraryVersionMonitor::getTTISLibraryVersionMonitor().setupVersionInfo();


        // set up the column headers
        RECT rect;
        GetClientRect(&rect);

        int stationHeaderSize = 60;
        int otherColumnSize = (rect.right - stationHeaderSize) / 2;

        InsertColumn(Station, "LOC", LVCFMT_CENTER);
        InsertColumn(CurrentISCS, "Current ISCS", LVCFMT_CENTER);
        InsertColumn(NextISCS, "Next ISCS", LVCFMT_CENTER);

        SetColumnWidth(Station, stationHeaderSize);
        SetColumnWidth(CurrentISCS, otherColumnSize);
        SetColumnWidth(NextISCS, otherColumnSize);

        this->setupList();
        this->populateData();

        FUNCTION_EXIT;
    }


    void LibraryVersionListCtrl::setupList()
    {
        FUNCTION_ENTRY( "setupList" );

        // map by pkey so they come out in the right order
        std::map<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo> locationMap;

        for (std::map<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo>::iterator iter = m_locationInfo.begin();
             iter != m_locationInfo.end(); iter++)
        {
            if ((*iter).second.locationKey != 0)
            {
                // insert OCC
                if ((*iter).second.name == "OCC")
                {
                    //hongran++ TD17500
                    // Location name is changed into OCCA for occ loation in the list
                    int i = InsertItem(0, "OCCA");
                    //++hongran TD17500
                    CString currentISCSVersionString = getVersionString( (*iter).second.currentTTISLibraryVersionKey );
                    CString nextISCSVersionString = getVersionString( (*iter).second.nextTTISLibraryVersionKey );
                    SetItemText(i, CurrentISCS, currentISCSVersionString);
                    SetItemText(i, NextISCS, nextISCSVersionString);
                    SetItemData(i, (*iter).second.locationKey);
                    setItemColour( (*iter).second.locationKey,
                                    COLORREF( RGB( 255, 0, 0 ) ),
                                    ListCtrlSelNoFocus::I_ITEMDATA );
                }
                // depot
                else if ((*iter).second.name == "KCD")
                {
                    //hongran++ TD17500
                    // Location name is changed into KCDA for kcd loation in the list
                    int i = InsertItem(1, "KCDA");
                    //++hongran TD17500

                    CString currentISCSVersionString = getVersionString( (*iter).second.currentTTISLibraryVersionKey );
                    CString nextISCSVersionString = getVersionString( (*iter).second.nextTTISLibraryVersionKey );
                    SetItemText(i, CurrentISCS, currentISCSVersionString);
                    SetItemText(i, NextISCS, nextISCSVersionString);
                    SetItemData(i, (*iter).second.locationKey);
                    setItemColour( (*iter).second.locationKey,
                                   COLORREF( RGB( 255, 0, 0 ) ),
                                   ListCtrlSelNoFocus::I_ITEMDATA );
                }
                // other stations ordered by location key
                else
                {
                    // station
                    locationMap.insert( std::pair<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo>( iter->second.locationKey, iter->second ) );
                }
            }
        }

        int count = 2;
        for( std::map<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo>::iterator mapIter = locationMap.begin();
             mapIter != locationMap.end();
             mapIter++)
        {
            CString currentISCSVersionString = getVersionString( (*mapIter).second.currentTTISLibraryVersionKey );
            CString nextISCSVersionString = getVersionString( (*mapIter).second.nextTTISLibraryVersionKey );
            count = InsertItem(count, mapIter->second.name.c_str());
            SetItemText(count, CurrentISCS, currentISCSVersionString);
            SetItemText(count, NextISCS, nextISCSVersionString);
            count++;
        }

        locationMap.clear();

        FUNCTION_EXIT;
    }


    void LibraryVersionListCtrl::populateData()
    {
        FUNCTION_ENTRY( "populateData" );

        // map by pkey so they come out in the right order
        std::map<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo> locationMap;

        for (std::map<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo>::iterator iter = m_locationInfo.begin();
             iter != m_locationInfo.end(); iter++)
        {
            if ((*iter).second.locationKey != 0)
            {
                // insert OCC
                if ((*iter).second.name == "OCC")
                {
                    CString currentISCSVersionString = getVersionString( (*iter).second.currentTTISLibraryVersionKey );
                    CString nextISCSVersionString = getVersionString( (*iter).second.nextTTISLibraryVersionKey );
                    SetItemText(0, CurrentISCS, currentISCSVersionString);
                    SetItemText(0, NextISCS, nextISCSVersionString);
                    SetItemData(0, (*iter).second.locationKey);
                    setItemColour( (*iter).second.locationKey,
                                    COLORREF( RGB( 255, 0, 0 ) ),
                                    ListCtrlSelNoFocus::I_ITEMDATA );
                }
                // depot
                else if ((*iter).second.name == "KCD")
                {
                    CString currentISCSVersionString = getVersionString( (*iter).second.currentTTISLibraryVersionKey );
                    CString nextISCSVersionString = getVersionString( (*iter).second.nextTTISLibraryVersionKey );
                    SetItemText(1, CurrentISCS, currentISCSVersionString);
                    SetItemText(1, NextISCS, nextISCSVersionString);
                    SetItemData(1, (*iter).second.locationKey);
                    setItemColour( (*iter).second.locationKey,
                                   COLORREF( RGB( 255, 0, 0 ) ),
                                   ListCtrlSelNoFocus::I_ITEMDATA );
                }
                // other stations ordered by location key
                else
                {
                    // station
                    locationMap.insert( std::pair<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo>( iter->second.locationKey, iter->second ) );
                }
            }
        }

        int count = 2;
        for( std::map<unsigned long, TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo>::iterator mapIter = locationMap.begin();
             mapIter != locationMap.end();
             mapIter++)
        {
            CString currentISCSVersionString = getVersionString( (*mapIter).second.currentTTISLibraryVersionKey );
            CString nextISCSVersionString = getVersionString( (*mapIter).second.nextTTISLibraryVersionKey );
            SetItemText(count, CurrentISCS, currentISCSVersionString);
            SetItemText(count, NextISCS, nextISCSVersionString);
            count++;
        }

        locationMap.clear();

        FUNCTION_EXIT;
    }


    CString LibraryVersionListCtrl::getVersionString( unsigned long entityKey )
    {
        FUNCTION_ENTRY( "getVersionString" );

        if( entityKey == 0 )
        {
            FUNCTION_EXIT;
            return CString( "N/A" );
        }


        CString version;
        version.Format( "%03d", this->m_libraryVersionInfo[ entityKey ] );

        FUNCTION_EXIT;
        return version;
    }


    void LibraryVersionListCtrl::libraryVersionUpdate( TA_IRS_Bus::LibraryVersionInfo libraryVersion )
    {
        FUNCTION_ENTRY( "libraryVersionUpdate" );

        this->m_libraryVersionInfo[ libraryVersion.entityKey ] = libraryVersion.value;

        ::PostMessage( this->m_hWnd, WM_USER, 0, 0 );

        FUNCTION_EXIT;
    }


    LRESULT LibraryVersionListCtrl::OnLibraryVersionUpdate(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnLibraryVersionUpdate" );

        this->populateData();

        ::PostMessage( this->m_hWnd, WM_PAINT, 0, 0 );

        FUNCTION_EXIT;
        return 0;
    }


    bool LibraryVersionListCtrl::isSelectable(int rowNumber)
    {
        FUNCTION_ENTRY( "isSelectable" );
        FUNCTION_EXIT;
        return false;
    }


    //hongran++ TD14352

    void LibraryVersionListCtrl::setLocalCurrentLibVerstion(unsigned long version)
    {
        FUNCTION_ENTRY( "setLocalCurrentLibVerstion" );

        unsigned int locationKey = TTISCommonConfiguration::getInstance().getLocationKey();
        TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo localInfo = m_locationInfo[locationKey];
        m_libraryVersionInfo[localInfo.currentTTISLibraryVersionKey] = version;
        OnLibraryVersionUpdate(0, 0);

        FUNCTION_EXIT;
    }


    void LibraryVersionListCtrl::setLocalNextLibVerstion(unsigned long version)
    {
        FUNCTION_ENTRY( "setLocalNextLibVerstion" );

        unsigned int locationKey = TTISCommonConfiguration::getInstance().getLocationKey();
        TA_IRS_Bus::TTISLibraryVersionMonitor::LocationInfo localInfo = m_locationInfo[locationKey];
        m_libraryVersionInfo[localInfo.nextTTISLibraryVersionKey] = version;
        OnLibraryVersionUpdate(0, 0);

        FUNCTION_EXIT;
    }


    //++ hongran TD14352

} // TA_IRS_App
