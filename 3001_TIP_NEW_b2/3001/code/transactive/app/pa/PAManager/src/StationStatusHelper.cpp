/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/StationStatusHelper.cpp $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/03/26 10:45:17 $
  * Last modified by: $Author: qi.huang $
  * 
  * StationStatusHelper contains helper functions for all functionality
  * common between station status dialogs (because the hierarchy isn't
  * split up to allow sharing of station specific functionality)
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/StationStatusHelper.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAManager/src/PaLayoutHelper.h"
#include "app/pa/PAManager/src/PaRightsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace TA_IRS_Bus;


static const int STATUS_WIDTH_PIXELS = 110;

static const int STATION_LOC_WIDTH_PIXELS = 50;
static const CString STATION_LOC_HEADER    = "Loc";

static const CString STATION_ZONE_HEADER    = "Zone";
static const CString STATUS_HEADER          = "Status";
static const CString PRIORITY_HEADER        = "Priority";


void StationStatusHelper::retryStationBroadcast(PAAgentNamedObject& agent,
                                                  const std::string& broadcastId,
                                                  const LockedPacketProgressReport& lockedProgressReport,
                                                  const GenericStatusModel::ERetryMode& mode)
{
    if (!PaRightsManager::getInstance().hasRightToRetryBroadcast(broadcastId, agent))
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
        return;
    }

    CWaitCursor hourGlassCursor;

    bool collectFailedOnly = (GenericStatusModel::FAILED == mode);

    // Collect set of trains we want to retry
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones =  collectStationZones(
                                                           lockedProgressReport,
                                                           collectFailedOnly);

    if (0 == zones.length())
    {
        // Silently ignore - this means someone at another
        // terminal has retried the request (and there are no items
        // with a failed state anymore - since they're retrying)
        // *Effectively someone has already retried for us*
        return;
    }

    // Allow all exceptions to propogate
    CORBA_CALL( agent, retryStationBroadcast, ( broadcastId.c_str(), zones, CachedConfig::getInstance()->getSessionId().c_str() ) );//limin
}



TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage 
    StationStatusHelper::collectStationZones(const LockedPacketProgressReport& lockedProgressReport,                                  
                                             bool includeFailedOnly)
{
    // Go through all zone objects, and return those zones that
    // have a status not matching any of the states listed in excludedStates parameter
    
    // Obtain direct reference to data
    const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& lockedData = lockedProgressReport.getLockedData();
    
    // Obtain reference to specialized data (for stations)
    const TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq lockedZoneData = 
                                            lockedData.localProgresses;

    unsigned int index = 0;
    unsigned int i;
    std::vector<unsigned long> zoneVector;

    for (i = 0; i < lockedZoneData.length(); i ++)
    {
        const TA_Base_Bus::IPAAgentCorbaDef::LocalProgress& zp = lockedZoneData[i];

        if (includeFailedOnly)
        {
            if (!zp.errorOccurred)
            {
                // No failures for this train, don't include for result
                continue;
            }
        }

        // This zone state didn't match any excluded state, can include in result
        zoneVector.push_back(zp.zoneKeyOrTrainId);
    }
 
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
    zones.length(zoneVector.size());
    for (i = 0; i < zoneVector.size(); i ++)
    {
        zones[i] = zoneVector[i];
    }

    return zones;
}


bool StationStatusHelper::addToStationReportLists(
                                    bool isOCC,
                                    CListCtrlSort& failuresList, 
                                    CListCtrlSort& progressList,
                                    const TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& zoneProgresses
                                )
{
    bool result = true;

    std::map<std::string, TA_Base_Bus::IPAAgentCorbaDef::LocalProgress> mapLastZoneProgresses;
    std::map<std::string, TA_Base_Bus::IPAAgentCorbaDef::LocalProgress>::const_iterator mapLastZoneProgressesIt;

    // Go through each zone and update the list
    for ( unsigned int nZone = 0; nZone < zoneProgresses.length(); ++nZone )
    {
		// Get the zones starting from the end of the list.
		// Since each one is inserted at index 0, they will display in the lists
		// in reverse order unless we do this.
        const TA_Base_Bus::IPAAgentCorbaDef::LocalProgress& zp = zoneProgresses[zoneProgresses.length() - nZone - 1];
        unsigned long zoneKeyOrTrainId = zp.zoneKeyOrTrainId;
        try
        {
            TA_IRS_Bus::PaZoneRecord record = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey(zoneKeyOrTrainId);
            std::string paZoneRecordId = record.m_strZoneStnLocName + record.m_area + record.m_stnZoneShortName;
            if ( zp.errorOccurred )
            {
                // Error status should not replace successful status, only put item with error status into map once
                if ( mapLastZoneProgresses.end() != mapLastZoneProgresses.find( paZoneRecordId ))
                {
                    continue;
                }
            }
            mapLastZoneProgresses[paZoneRecordId] = zp;
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException&)
        {
            
        }
    }

    for (mapLastZoneProgressesIt = mapLastZoneProgresses.begin(); mapLastZoneProgressesIt != mapLastZoneProgresses.end(); ++mapLastZoneProgressesIt)
    {
        result &= addStationReportEntry(
                                        isOCC,
                                        mapLastZoneProgressesIt->second,
                                        failuresList,
                                        progressList);
    }
    return result;
}

/*
Station DVA and Station Live
[Station + Zone] [Status]
One list shows where broadcast in initialisation phase
	- Connecting
	- In Progress
	- Complete

[Station + Zone] [Status] [Priority]
The second list shows unsucessful ones
	- Iff an override, simply shows priority (priority value required)
	- Any other reason, it is left BLANK
*/
bool StationStatusHelper::addStationReportEntry(
                                    bool isOCC,
                                    const TA_Base_Bus::IPAAgentCorbaDef::LocalProgress& zoneProgress,
                                    CListCtrlSort& failuresList,
                                    CListCtrlSort& progressList
                                    )
{
    // determine column indexes
    int nColIndex = 0;

    unsigned long zoneKeyOrTrainId = zoneProgress.zoneKeyOrTrainId;

    bool result = true;

    std::string stationZoneLabel = "";
    std::string stationLocLabel = "";

    TA_IRS_Bus::PaZoneRecord* pZoneRecord = NULL;
    try
    {
        TA_IRS_Bus::PaZoneRecord record = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey(zoneKeyOrTrainId);

        stationZoneLabel = record.m_label;
        stationLocLabel = record.m_strDispLocName;

        pZoneRecord = new TA_IRS_Bus::PaZoneRecord(record);
    }
    catch (const TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Couldn't find this zone in the database
        result = false;

        stationZoneLabel = "UNKNOWN ZONE";
        stationLocLabel = "???";

        pZoneRecord = new TA_IRS_Bus::PaZoneRecord();
    }

    std::string statusString;            
    try
    {
        statusString = getStationStatusString(zoneProgress.localState);
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        statusString = "UNKNOWN";
        result = false;
    }

    try
    {
        // which list to fill (Progress or Failure)?
        CListCtrlSort& theList = zoneProgress.errorOccurred ? failuresList : progressList;
        int iItem = theList.InsertItem( 0,"");
        theList.SetItemData( 0, (DWORD) pZoneRecord );

        if ( isOCC )
        {
            // Location column is at OCC only
            //theList.SetItemText( iItem, nColIndex++, stationLocLabel.c_str() );
        }
        // 
        stationLocLabel += ("   " + stationZoneLabel);
        theList.SetItemText( iItem, nColIndex++, stationLocLabel.c_str() );
        theList.SetItemText( iItem, nColIndex++, statusString.c_str() );
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        // General failure (couldn't classify zone)
        // - nothing will have been added to the list
        result = false;
    }

    return result;
}


const std::string StationStatusHelper::getStationStatusString(const TA_Base_Bus::IPAAgentCorbaDef::ELocalState& status)
{
    return GenericStatusModel::getStatusString(status);
}


void StationStatusHelper::setupReportWindowList(
                                                bool isOCC,
                                                CListCtrlSort& targetList, 
                                                bool isFailureList )
{      
    // Determine total width, accounting for scroll bar
    int headerWidth = PaLayoutHelper::getListHeaderWidth(targetList, false);

// First column does not display combined station / zone string anymore.
// Below is a summary of possible columns of both Progress/Failure list controls
// at different locations
// 
//        \  List|                      |
// Location\ con-|       Progress       |            Failure
//          \trol|                      |
// ----------\---+----------------------+-------------------------------
// OCC           | Zone,Status | Zone,Status
// --------------+----------------------+-------------------------------
// Others        |  Zone,Status | Zone,Status

    int remainWidth = headerWidth - STATUS_WIDTH_PIXELS;

    if (isOCC)
    {
        // Location column is available at OCC
        //remainWidth -= STATION_LOC_WIDTH_PIXELS;
    }

    int stationZoneWidthPixels = remainWidth;

    int nColIndex = 0;

    if ( isOCC )
    {
        // Location is shown at first column of both lists
        //targetList.InsertColumn( nColIndex++,
        //                        STATION_LOC_HEADER, 
        //                        LVCFMT_LEFT, 
        //                        STATION_LOC_WIDTH_PIXELS );
    }

	targetList.InsertColumn( nColIndex++, 
                            STATION_ZONE_HEADER, 
                            LVCFMT_LEFT, 
                            stationZoneWidthPixels);

    // Status header
    targetList.InsertColumn(nColIndex++, 
                            STATUS_HEADER, 
                            LVCFMT_LEFT, 
                            STATUS_WIDTH_PIXELS);

    targetList.SetExtendedStyle( targetList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
}
