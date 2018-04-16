/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_zone_group/src/PAZoneGroupListColumns.cpp $
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
#include "PaZoneGroupListColumns.h"

PaZoneGroupListColumns::PaZoneGroupListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     90,     "编辑状态",       COMPARE_STRING);
    setColumnParams(COL_KEY,            70,     "编号",          COMPARE_INTEGER);
    setColumnParams(COL_LOCATION,       80,     "车站",     COMPARE_STRING);
    setColumnParams(COL_LABEL,          250,    "广播区域组名称",        COMPARE_STRING);
    setColumnParams(COL_IS_EVENT_GROUP, 100,    "事件组",  COMPARE_STRING);
}


const CString PaZoneGroupListColumns::getFieldFromRecord(DWORD column, const TA_IRS_App::PaZoneGroupDatabaseAccessor::PaZoneGroupRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_KEY:           
            return details.key;
        case COL_LOCATION:      
            return details.location;
        case COL_LABEL:         
            return details.label;
        case COL_IS_EVENT_GROUP:  
            return details.isEventGroup;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    };

    return _T("");
}

