/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastListColumns.h $
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

#pragma once

#include "app/configuration/config_plugins/config_plugin_helper/src/ListColumnsBase.h"

class PaAtsTriggeredBroadcastListColumns 
:
public ListColumnsBase<TA_IRS_App::PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord, PaAtsTriggeredBroadcastListColumns>
{
public:

    PaAtsTriggeredBroadcastListColumns();

    enum EColumn
    {
        COL_MOD_STATUS,
        COL_KEY,                // ::getKey()
        COL_ATS_EVENT_TYPE,     // ::getAtsEventType
        COL_LOCATION,           // ::getLocation()
        COL_DEST_LOCATION,      // ::getDestinationLocationKey()
        COL_PLATFORM_ID,        // ::getPlatformId()
        COL_IS_LAST_TRAIN,      // ::getIsLastTrain()
        COL_ZONE_GROUP,         // ::getPaZoneGroupKey()
        COL_DVA_MESSAGE,        // ::getPaDvaMessageKey()
        COL_IS_ENABLED,         // ::getIsEnabledParameter
        COL_DVA_MESSAGE_VERSION,    // ::getDvaMessageVersion
        COL_MAX        
    };    

    static DWORD getDefaultSortColumn() { return COL_KEY; }

    /**
     * getFieldFromRecord
     *
     * @return a string representing the specific field of this record corresponding to the input row
     * @param column the column defining which field we want to extract from the record
     * @param details the record for which the reference to a member is returned
     */
    static const CString getFieldFromRecord(
                DWORD column,
                const TA_IRS_App::PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord& details);
};
