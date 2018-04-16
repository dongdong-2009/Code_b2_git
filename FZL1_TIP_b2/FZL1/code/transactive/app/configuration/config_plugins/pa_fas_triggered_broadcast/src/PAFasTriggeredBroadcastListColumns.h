/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/3002/transactive/app/configuration/config_plugins/pa_fas_triggered_broadcast/src/PAFasTriggeredBroadcastListColumns.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by: $Author: grace.koh $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma once

#include "app/configuration/config_plugins/config_plugin_helper/src/ListColumnsBase.h"

class PaFasTriggeredBroadcastListColumns 
:
public ListColumnsBase<TA_IRS_App::PaFasTriggeredBroadcastDatabaseAccessor::CfgPaFasTriggeredBroadcastRecord, PaFasTriggeredBroadcastListColumns>
{
public:

    PaFasTriggeredBroadcastListColumns();

    enum EColumn
    {
        COL_MOD_STATUS,
        COL_KEY,                // ::getKey()
        COL_FAS_ALARM_ENTITY,     // ::getFasEventType
        COL_LOCATION,           // ::getLocation()
        COL_CYCLIC_TIMES,      // ::getDestinationLocationKey()
        COL_ZONE_GROUP,         // ::getPaZoneGroupKey()
        COL_DVA_MESSAGE,        // ::getPaDvaMessageKey()
        COL_IS_ENABLED,         // ::getIsEnabledParameter
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
                const TA_IRS_App::PaFasTriggeredBroadcastDatabaseAccessor::CfgPaFasTriggeredBroadcastRecord& details);
};
