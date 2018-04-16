/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/alarm/alarm_list_control/src/columns/ColumnAvalanche.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * This class provides all the information for the avalanche column.
 */

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAvalanche.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/alarm/alarm_store_library/src/LogAlarmItem.h"

using TA_Base_Core::DebugUtil;
namespace TA_Base_Bus
{
    ColumnAvalanche::ColumnAvalanche(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction)
     : AbstractAlarmColumn(width, sortCol, isEnabled, doubleClickAction)
	{
    }

	const std::string ColumnAvalanche::getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex)
	{
		try
		{
			return pAlarmWrap->getItemString(ulIndex, AVALANCHE);
		}
		catch(...)
		{ 
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "wrong");
		}
		return "";
			//Utilities::getInstance().getAvalancheDisplayString(alarm).c_str();
    }


} //End of TA_Base_Bus

