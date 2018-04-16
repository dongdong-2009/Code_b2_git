/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_station_dva_message/src/PAStationDvaMessageListColumns.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma warning (disable : 4284)

#include "StdAfx.h"
#include "PaStationDvaMessageListColumns.h"

PaStationDvaMessageListColumns::PaStationDvaMessageListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     60,     "Status",       COMPARE_STRING);
    setColumnParams(COL_KEY,            70,     "Key",          COMPARE_INTEGER);

    setColumnParams(COL_LOCATION,       80,     "Location",     COMPARE_STRING);
    setColumnParams(COL_ID,             50,     "ID",           COMPARE_INTEGER);    
    setColumnParams(COL_LABEL,          250,    "Label",        COMPARE_STRING);

    setColumnParams(COL_TYPE,           100,    "Type",         COMPARE_STRING);

}


const CString PaStationDvaMessageListColumns::getFieldFromRecord(DWORD column, const TA_IRS_App::PaStationDvaMessageDatabaseAccessor::PaStationDvaMessageRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_KEY:           
            return details.key;
        case COL_LOCATION:      
            return details.location;
        case COL_ID:         
            return details.id;
        case COL_LABEL:         
            return details.label;
        case COL_TYPE:  
            return details.type;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    };

    return _T("");
}

