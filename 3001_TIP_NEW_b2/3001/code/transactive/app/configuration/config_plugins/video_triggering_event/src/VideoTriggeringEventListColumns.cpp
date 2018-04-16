/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/video_triggering_event/src/VideoTriggeringEventListColumns.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma warning (disable : 4284)

#include "StdAfx.h"
#include "VideoTriggeringEventListColumns.h"

VideoTriggeringEventListColumns::VideoTriggeringEventListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     90,     "Status",           COMPARE_STRING);
    setColumnParams(COL_STATION,       120,     "Station",          COMPARE_STRING);
    setColumnParams(COL_EVENT,         100,     "Event ID",         COMPARE_INTEGER);
    setColumnParams(COL_LOCATION,      200,     "Location",         COMPARE_STRING);
    setColumnParams(COL_TRIGGERING,    400,     "Triggering",       COMPARE_STRING);
	setColumnParams(COL_ALARMENTITY,   300,     "Alarm Type",       COMPARE_STRING); // TD15848
    setColumnParams(COL_MANUALCLOSING, 135,     "Manually Closed",  COMPARE_STRING);
}


const CString VideoTriggeringEventListColumns::getFieldFromRecord(DWORD column, const TA_IRS_App::VideoTriggeringEventDatabaseAccessor::VideoTriggeringEventRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_STATION:
            return details.stationName;
        case COL_EVENT:
            return details.eventId;
        case COL_LOCATION:
            return details.location;
        case COL_TRIGGERING:
            return details.cctvTriggering;
		case COL_ALARMENTITY:
			return details.alarmType;
        case COL_MANUALCLOSING:
            return details.manualClosing;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    }

    return _T("");
}

