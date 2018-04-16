/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * This class is for the action that displayed the schematic related to the alarm.
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/RightsChecker.h"
#include "bus/alarm/alarm_list_control/src/DatabaseCache.h"

#include "bus/security/access_control/alarm_actions/src/AlarmActions.h"

#include "bus/generic_gui/src/AppLauncher.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ApplicationException.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/alarm/alarm_list_control/src/resource.h"

using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{

    ActionRelatedSchematic::ActionRelatedSchematic(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu)
    {
    }
   
    ActionRelatedSchematic::~ActionRelatedSchematic()
    {
    }


    bool ActionRelatedSchematic::isAvailable(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("isAvailable");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return false;
        }

        // First retrieve the entity name from the alarm
        unsigned long entityKey;
        try
        {
			AlarmDataItem * pdataTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
			if (pdataTemp == NULL) return false;

			std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm (pdataTemp);
            entityKey = alarm->assocEntityKey;
           // delete alarm;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            return false;
        }

		try
		{
            TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
			std::string display = entity.getAlarmDisplay();

            // If the display string is not empty then this option is available
			if(!display.empty())
			{
                FUNCTION_EXIT;
                return true;
			}

		}
		catch(TA_Base_Core::DataException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not determine the alarm display for the selected alarm");
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not determine the alarm display for the selected alarm");
		}

        FUNCTION_EXIT;
		return false;
    }


    void ActionRelatedSchematic::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if (selectedAlarms.empty() || selectedAlarms.size() > 1)
        {
            // More than one alarm (or no alarms) selected.
            FUNCTION_EXIT;
            return;
        }

        // First retrieve the entity name from the alarm
		std::auto_ptr<TA_Base_Bus::AlarmDataItem> alarm;
        unsigned long entityKey;
        try
        {
			AlarmDataItem * pdataTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
			if (pdataTemp == NULL) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to retrieve alarm details.");
				TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
			}

			alarm.reset( pdataTemp);
            entityKey = alarm->assocEntityKey;
		
            //delete alarm;
        }
        catch( const TA_Base_Core::AlarmListCtrlException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "AlarmListCtrlException", "Could not retrieve the alarm from the alarm store to determine related entity");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
        }

        std::string display("");
        std::string entityName("");
		try
		{
            TA_Base_Core::IEntityData& entity = alarmRetriever.getDatabaseCache().getEntity(entityKey);
			display = entity.getAlarmDisplay();
            entityName = entity.getName();

            // If the display string is not empty then this option is available
			if(display.empty())
			{
                TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
			}
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", "Could not determine the alarm display for the selected alarm");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
		}

		// Get x coordinates of the selected item so we know which GraphWorx screen to update
		CPoint pt(GetMessagePos());
        try
        {
			TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(display, entityName, pt.x);
        }
		catch(TA_Base_Core::ApplicationException&)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Failed to launch graphworx screen with the specified display");
            TA_THROW( AlarmActionException(AlarmActionException::CANNOT_LAUNCH_SCHEMATIC) );
		}

        FUNCTION_EXIT;
    }
}
