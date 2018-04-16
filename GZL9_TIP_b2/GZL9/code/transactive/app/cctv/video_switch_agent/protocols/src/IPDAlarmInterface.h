#ifndef IPDAlarmInterface_H
#define IPDAlarmInterface_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IPDAlarmInterface.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Handles the protocol dependent sections of the operations for a Quad.
 *
 */

#include "bus/cctv_gzl9/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"

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
		  *				TD15853 - changed parameters
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */
		virtual void acknowledgeAlarm( const std::string& alarmKey,/* unsigned long entityKey*/ unsigned long consoleId) = 0;
        
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
        
		/**
          * acknowledgeTrainCameraAlarm
          *
	      * acknowledges an trainborne camera alarm 
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be Acked
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */
       virtual void acknowledgeTrainCameraAlarm(unsigned long locationId, unsigned long trainId, const std::vector<unsigned long>& cameras, unsigned long consoleId ) = 0;
		
		/**
          * closeAlarm
          *
	      * closed an alarm on the videoswitch alarm stack that the switch cant close itself
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be closed
		  *				TD15853 - changed parameters; add flag for overload
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
		  *				3. The alarm not configured to be manually closed
	      */
        virtual void closeAlarm(const std::string& alarmTypeKey/*, unsigned long entityKey*/) = 0;

		/**
          * closeAlarm
          *
	      * closed an alarm on the videoswitch alarm stack that the switch cant close itself
          *
          * @param      string AlarmId
          *             the transactive alarm Id associated with the alarm to be closed
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
		  *				3. The alarm not configured to be manually closed
	      */
        virtual void closeAlarm( unsigned long stationId, unsigned long eventId ) = 0;

		/**
          * getAlarmStack
          *
	      * gets all of the alarms curently open by this alarm interface
          *
          * @return     TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack&
          *             The alarm stack object
		  *
		  */
		virtual TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack getAlarmStack() =0;

		/**
          * updateAlarmStack
          *
	      * returns all of the alarms curently open by this alarm interface
          *
          * @param     TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack& stack
          *             The alarm stack object
		  *
		  */
		virtual void updateAlarmStack(TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack& stack) =0;

		virtual unsigned long getEntityKey() =0;

	};
}

#endif // IPDAlarmInterface_H
