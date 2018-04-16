/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tis_ratis_tag/src/TisRatisTagListColumns.cpp $
  * @author James
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
#include "TisRatisTagListColumns.h"

TisRatisTagListColumns::TisRatisTagListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,   100, "Status",   COMPARE_STRING);
    setColumnParams(COL_TAG_GROUP,    150,"Tag Group",COMPARE_STRING);
    setColumnParams(COL_DETAIL,       300, "Detail",   COMPARE_STRING);
    setColumnParams(COL_RATIS_TAG,    300, "RATIS Tag",COMPARE_STRING);
}



const CString TisRatisTagListColumns::getFieldFromRecord(DWORD column, const TA_IRS_App::TisRatisTagDatabaseAccessor::TisRatisTagRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_TAG_GROUP:           
            return details.tagGroup;
        case COL_DETAIL:      
            return details.detail;
        case COL_RATIS_TAG:
            return details.ratisTag;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    }

    return _T("");
}
