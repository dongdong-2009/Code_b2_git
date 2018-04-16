/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_train_dva_message/src/PATrainDvaMessageListColumns.cpp $
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
#include "PaTrainDvaMessageListColumns.h"

PaTrainDvaMessageListColumns::PaTrainDvaMessageListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     60,     "Status",       COMPARE_STRING);
    setColumnParams(COL_KEY,            70,     "ID",          COMPARE_INTEGER);
    setColumnParams(COL_LABEL,          350,    "Label",        COMPARE_STRING);
}


const CString PaTrainDvaMessageListColumns::getFieldFromRecord(DWORD column, const TA_IRS_App::PaTrainDvaMessageDatabaseAccessor::PaTrainDvaMessageRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_KEY:           
            return details.key;
        case COL_LABEL:         
            return details.label;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    }
}

