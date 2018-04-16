/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastListColumns.cpp $
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

#pragma warning (disable : 4284 4786)

#include "StdAfx.h"
#include "PaAtsTriggeredBroadcastListColumns.h"

PaAtsTriggeredBroadcastListColumns::PaAtsTriggeredBroadcastListColumns()
{
    // Set up various column widths
    setColumnParams(COL_MOD_STATUS,     90,     "�༭״̬",       COMPARE_STRING);
    setColumnParams(COL_KEY,            70,     "���",          COMPARE_INTEGER);
    setColumnParams(COL_LOCATION,       100,     "��ʼ��վ",     COMPARE_STRING);
    setColumnParams(COL_DEST_LOCATION,  100,     "���ﳵվ", COMPARE_STRING);    
    setColumnParams(COL_ZONE_GROUP,     120,    "�㲥������",   COMPARE_STRING);
    setColumnParams(COL_PLATFORM_ID,    80,     "վ̨",     COMPARE_STRING);
    setColumnParams(COL_DVA_MESSAGE,    120,    "Ԥ��㲥��Ϣ",  COMPARE_STRING);
    setColumnParams(COL_ATS_EVENT_TYPE, 90,     "�¼�����",   COMPARE_STRING);
    setColumnParams(COL_IS_LAST_TRAIN,  75,     "ĩ�೵",   COMPARE_STRING);
    setColumnParams(COL_IS_ENABLED,     70,     "����",      COMPARE_STRING);
}


const CString PaAtsTriggeredBroadcastListColumns::getFieldFromRecord(DWORD column, 
    const TA_IRS_App::PaAtsTriggeredBroadcastDatabaseAccessor::CfgPaAtsTriggeredBroadcastRecord& details)
{
    switch (column)
    {
        case COL_MOD_STATUS:    
            return details.status;
        case COL_KEY:           
            return details.key;     
        case COL_ATS_EVENT_TYPE:     
            return details.eventType;
        case COL_LOCATION:        
            return details.location;
        case COL_DEST_LOCATION:   
            return details.destLocation;
        case COL_PLATFORM_ID:     
            return details.platformId;
        case COL_IS_LAST_TRAIN:    
            return details.isLastTrain;
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

