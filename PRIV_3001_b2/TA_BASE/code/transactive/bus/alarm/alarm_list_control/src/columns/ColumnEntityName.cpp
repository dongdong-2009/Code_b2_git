/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/alarm/alarm_list_control/src/columns/ColumnEntityName.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * This class provides all the information for the entity name column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnEntityName.h"

namespace TA_Base_Bus
{
    ColumnEntityName::ColumnEntityName(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
	{
    }


    const std::string ColumnEntityName::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
    {
		std::string strEntityName = "";
		
		try
		{
			strEntityName = pAlarmWrap->getItemString(ulIndex, ENTITY_NAME);
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "wrong");			
		}

        return strEntityName;
    }


} //End of TA_Base_Bus

