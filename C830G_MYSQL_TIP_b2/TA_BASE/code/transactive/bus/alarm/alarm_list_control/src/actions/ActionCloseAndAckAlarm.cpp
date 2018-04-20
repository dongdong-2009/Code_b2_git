/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_list_control/src/actions/ActionCloseAndAckAlarm.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * This class is for the action that acknowledges the selected alarms
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionCloseAndAckAlarm.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"

#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/NoAlarmAgentException.h"

using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionCloseAndAckAlarm::ActionCloseAndAckAlarm(bool isAvailableInContextMenu /*=true*/)
        : m_alarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper()),
          m_isAvailableInContextMenu(isAvailableInContextMenu)
    {

    }

    
    ActionCloseAndAckAlarm::~ActionCloseAndAckAlarm()
    {
        try
        {
            TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
        }
        catch( ... )
        {
        }
    }


    bool ActionCloseAndAckAlarm::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            // Already in the process of an action like this so this is not available
            FUNCTION_EXIT;
            return false;
        }

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty())
        {
            // No alarms selected.
            FUNCTION_EXIT;
            return false;
        }

        for(std::vector<std::string>::iterator iter = selectedAlarms.begin(); iter != selectedAlarms.end(); ++iter)
        {
            try
            {
				AlarmDataItem * pTemp = alarmRetriever.getAlarmItem(*iter);
				if (pTemp == NULL) continue;

				std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm(pTemp);
                if (alarm->state == TA_Base_Core::AlarmOpen) // Alarm still yet to be closed
                {
                    unsigned long action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                                TA_Base_Bus::AlarmActions::CLOSE_ALARM);

                    if( alarmRetriever.getRightsChecker().canPerformAction(alarm->assocEntityKey, action) )
                    {
                        // Alarm is open and the user has permission to close it. Now check that if the alarm
                        // is not acknowledged the user can acknowledge it.
						if (strcmp(alarm->alarmAcknowledgedBy.c_str(), "") == 0) // Alarm still yet to be acknowledged
                        {
                            // Now get the action we should be checking
                            action = TA_Base_Bus::AlarmActions::getAlarmAction(alarm->messageTypeKey,
                                                                          TA_Base_Bus::AlarmActions::ACK_ALARM);

                            if( alarmRetriever.getRightsChecker().canPerformAction(alarm->assocEntityKey, action) )
                            {
                                // Alarm is not acked and the user has permission to ack so we can enable this option
                                //delete alarm;
                                return true;
                            }
                        }
                        else
                        {
                            // Alarm is already acked and just needs to be closed. We've already determed we
                            // have permission to do this so return.
                          //  delete alarm;
                            return true;
						}
                    }
                }
               // delete alarm;
            }
            catch( const TA_Base_Core::AlarmListCtrlException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to close and acknowledge it");
            }
        }

        // No suitable alarms were found so this option is unavailable
        FUNCTION_EXIT;
        return false;
    }


    void ActionCloseAndAckAlarm::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        //TA_ASSERT(m_alarmHelper != NULL, "Alarm helper was null so this was not initialised correctly in the constructor");

        if (alarmRetriever.isActionAlreadyRunning())
        {
            FUNCTION_EXIT;
            return;
        }
        alarmRetriever.setIsActionAlreadyRunning(true, "Closing and acknowledging alarms . . .");

        std::vector<TA_Base_Core::AlarmModifyItem*> selectedAlarms = alarmRetriever.getSelectedModItems();
        if (selectedAlarms.empty())
        {
            // Nothing to do as no alarm is selected (or too many are selected)
            alarmRetriever.setIsActionAlreadyRunning(false);
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the alarm details for each of the alarms and add them to a vector
        bool wasSuccessful = true;

        // Now get those alarms acknowledged and closed
        try
        {
            //m_alarmHelper.operatorCloseAndAcknowledgeAlarms(selectedAlarms);
			std::vector<std::string>  alarmNoExists;
			wasSuccessful=!m_alarmHelper.operatorCloseAndAcknowledgeAlarmsEx(selectedAlarms,alarmNoExists);
			if (alarmNoExists.size()>0)
			{			 
				alarmRetriever.deleteSharedAlarm(alarmNoExists);				
			}
        }
        catch( const TA_Base_Core::NoAlarmAgentException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "NoAlarmAgentException", "Could not acknowledge or close some or all of the alarms");
            wasSuccessful = false;
        }

		alarmRetriever.freeAlarmModifyItems(selectedAlarms);
        alarmRetriever.setIsActionAlreadyRunning(false);
		
        if (!wasSuccessful)
        {
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_ACKNOWLEDGE_AND_CLOSE_ALARMS) );
        }
        FUNCTION_EXIT;
   }
   
}
