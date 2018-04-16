/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastListColumns.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma warning (disable : 4284 4786)

#include "StdAfx.h"
#include "PaAtsTriggeredBroadcastListColumns.h"

PaAtsTriggeredBroadcastListColumns::PaAtsTriggeredBroadcastListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     60,     "Status",       COMPARE_STRING);
    setColumnParams(COL_KEY,            70,     "Key",          COMPARE_INTEGER);

    setColumnParams(COL_LOCATION,       80,     "Location",     COMPARE_STRING);
    setColumnParams(COL_DEST_LOCATION,  90,     "Dest Location", COMPARE_STRING);    

    setColumnParams(COL_ZONE_GROUP,     120,    "Zone Group",   COMPARE_STRING);
    setColumnParams(COL_PLATFORM_ID,    80,     "Platform",     COMPARE_STRING);

    setColumnParams(COL_DVA_MESSAGE,    200,    "DVA Message",  COMPARE_STRING);
    setColumnParams(COL_DVA_MESSAGE_VERSION,    90, "DVA Version",  COMPARE_STRING);
    
    setColumnParams(COL_ATS_EVENT_TYPE, 100,     "Event Type",   COMPARE_STRING);
    setColumnParams(COL_IS_LAST_TRAIN,  80,     "Last Train",   COMPARE_STRING);
    setColumnParams(COL_IS_ENABLED,     60,     "Enabled",      COMPARE_STRING);
}


const CString PaAtsTriggeredBroadcastListColumns::getFieldFromRecord(DWORD column, 
    const TA_IRS_App::PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_KEY:           
            return details.key;     
        case COL_ATS_EVENT_TYPE:     
            return details.eventType;
        case COL_LOCATION:        
            return details.location;
        case COL_DEST_LOCATION:   
            return details.destLocation;
        case COL_PLATFORM_ID:     
            return details.platformId;
        case COL_IS_LAST_TRAIN:    
            return details.isLastTrain;
        case COL_ZONE_GROUP:   
            return details.zoneGroupName;
        case COL_DVA_MESSAGE:   
            return details.dvaMessage;
        case COL_IS_ENABLED:   
            return details.isEnabled;
        case COL_DVA_MESSAGE_VERSION:
            return details.dvaMessageVersion;                
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    };

    return _T("");
}

