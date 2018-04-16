#ifndef IPDAlarmInterface_H
#define IPDAlarmInterface_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/src/IPDAlarmInterface.h $
 * @author:  Jade Welton
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2013/05/16 11:23:36 $
 * Last modified by:  $Author: huang.wenbo $
 * 
 * Handles the protocol dependent sections of the operations for a Quad.
 *
 */

#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include <vector>

namespace TA_IRS_App
{
	class IPDAlarmInterface
	{

	public:

		virtual ~IPDAlarmInterface() {};
		/**
          * acknowledgeAlarm
          *
	      * acknowledges an alarm on the videoswitch alarm stack
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be Acked
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */
       virtual void acknowledgeAlarm( unsigned long stationId, unsigned long eventId, unsigned long monitorId ) = 0;


	   virtual void acknowledgeAlarm( unsigned long entityKey, unsigned long consoleId) = 0;
        

		/**
          * getAlarmStack
          *
	      * gets all of the alarms curently open by this alarm interface
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack&
          *             The alarm stack object
		  *
		  */
		virtual TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack getAlarmStack() =0;

		/**
          * updateAlarmStack
          *
	      * returns all of the alarms curently open by this alarm interface
          *
          * @param     TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack& stack
          *             The alarm stack object
		  *
		  */
		virtual void updateAlarmStack(TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack& stack) =0;

		virtual unsigned long getEntityKey() =0;

	};
}

#endif // IPDAlarmInterface_H
