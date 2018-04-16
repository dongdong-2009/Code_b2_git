/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(IAlarmUtility_H)
#define IAlarmUtility_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include <ctime>
#include <string>


namespace TA_IRS_App
{

    /**
     * This is used within the train agent to raise, modify, and close alarms.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:38 PM
     */
    class IAlarmUtility
    {

    public:

        /**
         * This will raise the unique alarm for an ATS Agent communication failure
         *
         * @param atsAgentName    The name of the ATS agent that communication failed with
         */
        virtual void raiseAtsAgentCommsAlarm( const std::string& atsAgentName ) = 0;


        /**
         * This will close the unique alarm for an ATS Agent communication failure
         */
        virtual void closeAtsAgentCommsAlarm() = 0;


        /**
         * This will raise an alarm for a train cctv pre-conflict
         *
         * @return the alarm ID of the alarm that was raised
         *
         * @param offendingTrainId    The offending train (that could have its carrier dropped)
         * @param owningTrainId    The train still broadcasting
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual std::string raiseCctvPreConflictAlarm( TA_IRS_Bus::CommonTypes::TrainIdType offendingTrainId,
                                                       TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId,
                                                       time_t timeStamp ) = 0;


        /**
         * This will raise an alarm for a train communication timeout
         *
         * @return the alarm ID of the alarm that was raised
         *
         * @param trainId    The train that timed out
         * @param timerType    The timer that expired
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual std::string raiseOa1AtcTimerAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                   const std::string& timerType,
                                                   time_t timeStamp ) = 0;


        /**
         * This will raise an alarm for a train that hasnt responded to a change mode request
         *
         * @return the alarm ID of the alarm that was raised
         *
         * @param trainId    The train that timed out
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual std::string raiseRadioModeTimerAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                      time_t timeStamp ) = 0;


        /**
         * This will raise an alarm for a train that changes MPU
         *
         * @return the alarm ID of the alarm that was raised
         *
         * @param trainId    The train that timed out
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual std::string raiseMpuChangeoverAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                     time_t timeStamp ) = 0;


        /**
         * This will close a non unique alarm
         *
         * @param alarmId    The alarm to close
         */
        virtual void closeAlarm( const std::string& alarmId ) = 0;


        /**
         * This will close all unique and non unique alarms for this agent entity.
         * This should only be used if this is the first agent starting at a location
         */
        virtual void closeAllAlarmsForThisAgent() = 0;

    };

} // TA_IRS_App

#endif // !defined(IAlarmUtility_H)
