/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/alarm/alarm_list_control/src/actions/ActionDisplayComment.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * This class is for the action that displays a read-only alarm comment
 */

#pragma warning(disable:4786)

#include "bus/alarm/alarm_list_control/src/StdAfx.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionDisplayComment.h"
#include "bus/alarm/alarm_list_control/src/AlarmRetriever.h"
#include "bus/alarm/alarm_list_control/src/CommentDialog.h"

#include "core/exceptions/src/AlarmListCtrlException.h"
#include "core/exceptions/src/AlarmActionException.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

using TA_Base_Core::AlarmActionException;

namespace TA_Base_Bus
{
    ActionDisplayComment::ActionDisplayComment(bool isAvailableInContextMenu /*=true*/)
        : m_isAvailableInContextMenu(isAvailableInContextMenu)
    {
    }

    
    bool ActionDisplayComment::isAvailable(IAlarmRetriever& alarmRetriever)
    {
		// Display comment is available if at least one alarm is selected
        std::vector<std::string>& selectedAlarms = alarmRetriever.getSelectedAlarms();
        return (selectedAlarms.size() >= 1);
    }


    void ActionDisplayComment::performAction(IAlarmRetriever& alarmRetriever)
    {
        FUNCTION_ENTRY("performAction");

        // By default the comment is empty. This applies in the case when either there is no
		// comment for the single selected alarm, of if multiple alarms are selected.
		std::string comment = "";

        std::vector<std::string> selectedAlarms = alarmRetriever.getSelectedAlarms();
        if(selectedAlarms.size() == 1)
        {
			AlarmDataItem * pTemp = alarmRetriever.getAlarmItem(selectedAlarms[0]);
			if (pTemp == NULL) return;

			std::auto_ptr<TA_Base_Bus::AlarmDataItem> pAlarm(pTemp);
			comment = pAlarm->alarmComments;
		}
        else if(selectedAlarms.size() > 1)
        {
            // TD 8705: When multiple alarms have been selected and 'Comment' is selected, indicate
            // that comments cannot be viewed simultaneously, but can be created, or deleted.
			// When operator attempts to edit comments of multiple alarms "the edit alarm comment dialog is displayed
			comment = "Comments cannot be viewed simultaneously, as multiple Alarms are selected. \r\n";
			comment += "If you wish to delete comment of selected alarms, click Delete button.\r\n";
			comment += "If you wish to replace existing/create new comment for selected alarms, click Edit button.";
        }
		else // A case that the selected alarm is zero or retriever is faulty
		{
			comment = "No alarm is selected or system is not working fine.";
		}

		// Create view comment dialog
        CommentDialog dlg(alarmRetriever);
		dlg.m_comment = comment.c_str();
        dlg.DoModal();

		FUNCTION_EXIT;
    }
}
