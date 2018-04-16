/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/video_station/src/VideoStationListColumns.cpp $
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

#pragma warning (disable : 4284)

#include "StdAfx.h"
#include "VideoStationListColumns.h"

VideoStationListColumns::VideoStationListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     70,     "Status",       COMPARE_STRING);
    setColumnParams(COL_KEY,            100,    "Station ID",   COMPARE_INTEGER);
    setColumnParams(COL_LOCATION,       120,    "Location",     COMPARE_STRING);
}


const CString VideoStationListColumns::getFieldFromRecord(DWORD column, const TA_IRS_App::VideoStationDatabaseAccessor::VideoStationRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_KEY:           
            return details.key;
        case COL_LOCATION:         
            return details.location;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    }

    return _T("");
}

