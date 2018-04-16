/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/pa_zone/src/PAZoneListColumns.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma warning (disable : 4284)

#include "StdAfx.h"
#include "PaZoneListColumns.h"

PaZoneListColumns::PaZoneListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,   60, "Status",   COMPARE_STRING);
    setColumnParams(COL_KEY,          70, "Key",      COMPARE_INTEGER);
    setColumnParams(COL_LOCATION,     80, "Location", COMPARE_STRING);
    setColumnParams(COL_ID,           80, "ID",       COMPARE_INTEGER);
    setColumnParams(COL_LABEL,        250, "Label",   COMPARE_STRING);
    setColumnParams(COL_STN_EQUIPMENT_ENTITY_NAME, 250, "STN Equip. Entity Name", COMPARE_STRING);
    setColumnParams(COL_OCC_EQUIPMENT_ENTITY_NAME, 250, "OCC Equip. Entity Name", COMPARE_STRING);
}



const CString PaZoneListColumns::getFieldFromRecord(DWORD column, const TA_IRS_App::PaZoneDatabaseAccessor::PaZoneRecord& details)
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
        case COL_STN_EQUIPMENT_ENTITY_NAME:  
            return details.stnEquipmentEntityName;
        case COL_OCC_EQUIPMENT_ENTITY_NAME:
            return details.occEquipmentEntityName;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    }

    return _T("");
}
