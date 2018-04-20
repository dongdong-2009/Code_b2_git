/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/bus/alarm/alarm_list_control/src/columns/ColumnSeverity.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2011/05/27 12:16:41 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * This class provides all the information for the severity column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSeverity.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/alarm/alarm_store_library/src/LogAlarmItem.h"

using TA_Base_Core::DebugUtil;
namespace TA_Base_Bus
{
    ColumnSeverity::ColumnSeverity(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
    {
    }

	const std::string ColumnSeverity::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
	{
		try
		{
			memset(m_textbuf, 0, sizeof(m_textbuf));
			int nSeverity = pAlarmWrap->getItemNumber(ulIndex, ALARM_SEVERITY);
			if (nSeverity != 0)
			{
				itoa(nSeverity, m_textbuf, 10);
			}
			return m_textbuf;
		}
		catch(...)
		{ 
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");			
		}
		return "";
			//databaseCache.getAlarmSeverityName(alarm->alarmSeverity).c_str();
    }


} //End of TA_Base_Bus

