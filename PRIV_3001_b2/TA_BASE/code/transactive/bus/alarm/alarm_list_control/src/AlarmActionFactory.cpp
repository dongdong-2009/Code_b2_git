/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/alarm/alarm_list_control/src/AlarmActionFactory.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * This factory holds all the alarm actions that can be performed by this instance
 * of the alarm list. The main application needs to add the actions it wants to be
 * available from the list context menu. This class is not intended to be dynamic. All
 * actions should be added at the start and 
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/AlarmActionFactory.h"
#include "bus/alarm/alarm_list_control/src/IAlarmAction.h"
#include "bus/alarm/alarm_list_control/src/IAlarmRetriever.h"

namespace TA_Base_Bus
{
    AlarmActionFactory::AlarmActionFactory(const AlarmActionList& actionList)
        : m_alarmActions(actionList)
    {
    }


    AlarmActionFactory::~AlarmActionFactory()
    {
        try
        {
            for(AlarmActionList::iterator iter = m_alarmActions.begin(); iter != m_alarmActions.end(); ++iter)
            {
                 if (*iter != NULL)
                 {
                     delete *iter;
                     *iter = NULL;
                 }
            }
        }
        catch( ... )
        {
        }
    }

} //End of TA_Base_Bus

