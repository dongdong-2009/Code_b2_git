/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainListColumns.cpp $
  * @author : Lucky Cuizon
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by: $Author: Lucky Cuizon $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma warning (disable : 4284 4786)

#include "StdAfx.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainListColumns.h"

TrTrainListColumns::TrTrainListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     60,     "Status",       COMPARE_STRING);
	//setColumnParams(COL_KEY,            70,     "Key",          COMPARE_INTEGER);
	
    setColumnParams(COL_TRAIN_ID,       80,     "Train ID",     COMPARE_INTEGER);
    setColumnParams(COL_PRIMARY_TSI,			380,    "RadioID 1", COMPARE_STRING);    
	
    setColumnParams(COL_SECONDARY_TSI,			450,    "RadioID 2",   COMPARE_STRING);
}


const CString TrTrainListColumns::getFieldFromRecord(DWORD column, 
    const TA_IRS_App::TrTrainDatabaseAccessor::CfgTrTrainRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
//         case COL_KEY:           
//             return details.key;     
        case COL_TRAIN_ID:        
            return details.trainId;
        case COL_PRIMARY_TSI:   
            return details.primaryTsi;
        case COL_SECONDARY_TSI:     
            return details.secondaryTsi;		
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    };

    return _T("");
}

