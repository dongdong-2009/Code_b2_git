/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainListColumns.h $
  * @author Lucky Cuizon
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

#pragma once

#include "app/configuration/config_plugins/config_plugin_helper/src/ListColumnsBase.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainDatabaseAccessor.h"

class TrTrainListColumns : public ListColumnsBase<TA_IRS_App::TrTrainDatabaseAccessor::CfgTrTrainRecord, TrTrainListColumns>
{
public:

    TrTrainListColumns();

    enum EColumn
    {
        COL_MOD_STATUS,
        //COL_KEY,                // ::getKey()
        COL_TRAIN_ID,		    // ::getTrainId
        COL_PRIMARY_TSI,           // ::getPrimaryTsi()
        COL_SECONDARY_TSI,      // ::getSecondaryTsi()	
        COL_MAX        
    };    

    static DWORD getDefaultSortColumn() { return COL_TRAIN_ID; }

    /**
     * getFieldFromRecord
     *
     * @return a string representing the specific field of this record corresponding to the input row
     * @param column the column defining which field we want to extract from the record
     * @param details the record for which the reference to a member is returned
     */
    static const CString getFieldFromRecord(
	         	DWORD column,
                const TA_IRS_App::TrTrainDatabaseAccessor::CfgTrTrainRecord& details);



};
