/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/3002/transactive/app/configuration/config_plugins/pa_fas_triggered_broadcast/src/PAFasTriggeredBroadcastListColumns.cpp $
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

#pragma warning (disable : 4284 4786)

#include "StdAfx.h"
#include "PaFasTriggeredBroadcastListColumns.h"

PaFasTriggeredBroadcastListColumns::PaFasTriggeredBroadcastListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     90,     "�༭״̬",       COMPARE_STRING);
    setColumnParams(COL_KEY,            70,     "���",          COMPARE_INTEGER);
    setColumnParams(COL_LOCATION,       80,     "��վ",     COMPARE_STRING);
    setColumnParams(COL_CYCLIC_TIMES,  90,     "ѭ������", COMPARE_STRING);    
    setColumnParams(COL_ZONE_GROUP,     150,    "�㲥������",   COMPARE_STRING);
    setColumnParams(COL_DVA_MESSAGE,    150,    "Ԥ��㲥��Ϣ",  COMPARE_STRING);
    setColumnParams(COL_FAS_ALARM_ENTITY, 250,     "���ֱ���������",   COMPARE_STRING);
    setColumnParams(COL_IS_ENABLED,     70,     "����",      COMPARE_STRING);
}


const CString PaFasTriggeredBroadcastListColumns::getFieldFromRecord(DWORD column, 
    const TA_IRS_App::PaFasTriggeredBroadcastDatabaseAccessor::CfgPaFasTriggeredBroadcastRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_KEY:           
            return details.key;     
        case COL_FAS_ALARM_ENTITY:     
            return details.fasAlarmEntityName;
        case COL_LOCATION:        
            return details.location;
        case COL_CYCLIC_TIMES:   
            return details.cyclicTimes;
        case COL_ZONE_GROUP:   
            return details.zoneGroupName;
        case COL_DVA_MESSAGE:   
            return details.dvaMessage;
        case COL_IS_ENABLED:   
            return details.isEnabled;            
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    };

    return _T("");
}

