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
#if !defined(AlarmUtility_H)
#define AlarmUtility_H

#include "app/trains/train_agent/src/IAlarmUtility.h"

#include "core/types/public_types/TypeDefs.h"


namespace TA_Base_Core
{
    class ItaAlarms;
    class MessageType;
}

namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ITrainStatusManager;
    class ITrainStatusManager;
}


namespace TA_IRS_App
{

    /**
     * This is the implementation of the alarm utility, it does the work of raising, modifying and closing alarms.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:15 PM
     */
    class AlarmUtility : public IAlarmUtility
    {

    public:

        /**
         * Constructor
         *
         * @param configuration    Agent configuration
         * @param statusManager    Stores train state
         * @param taAlarms    Used to submit and close alarms
         */
        AlarmUtility( ITrainAgentConfiguration& configuration,
                      ITrainStatusManager& statusManager,
                      TA_Base_Core::ItaAlarms& taAlarms );


        /**
         * Destructor
         */
        virtual ~AlarmUtility();


        /**
         * This will raise the unique alarm for an ATS Agent communication failure
         *
         * @param atsAgentName    The name of the ATS agent that communication failed with
         */
        virtual void raiseAtsAgentCommsAlarm( const std::string& atsAgentName );


        /**
         * This will close the unique alarm for an ATS Agent communication failure
         */
        virtual void closeAtsAgentCommsAlarm();


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
                                                       time_t timeStamp );


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
                                                   time_t timeStamp );


        /**
         * This will raise an alarm for a train that hasnt responded to a change mode request
         *
         * @return the alarm ID of the alarm that was raised
         *
         * @param trainId    The train that timed out
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual std::string raiseRadioModeTimerAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                      time_t timeStamp );


        /**
         * This will raise an alarm for a train that changes MPU
         *
         * @return the alarm ID of the alarm that was raised
         *
         * @param trainId    The train that timed out
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual std::string raiseMpuChangeoverAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                     time_t timeStamp );


        /**
         * This will close a non unique alarm
         *
         * @param alarmId    The alarm to close
         */
        virtual void closeAlarm( const std::string& alarmId );


        /**
         * This will close all unique and non unique alarms for this agent entity.
         * This should only be used if this is the first agent starting at a location
         */
        virtual void closeAllAlarmsForThisAgent();


    private:


        /**
         * Constructor
         */
        AlarmUtility();


        /**
         * This will raise an alarm that is unique for this train agent entity
         *
         * @param alarmType    The alarm type to raise
         * @param descriptionParameters    A list of parameters for the alarm description
         * @param timeStamp    The time the alarm condition occurred. If not specified the default time will be used.
         */
        void raiseUniqueAlarm( const TA_Base_Core::MessageType& alarmType,
                               TA_Base_Core::NameValuePairs& descriptionParameters,
                               time_t timeStamp = 0 );


        /**
         * This will close an alarm that is unique for this train agent entity
         *
         * @param alarmType    The alarm type to raise
         */
        void closeUniqueAlarm( const TA_Base_Core::MessageType& alarmType );


        /**
         * This will raise an alarm that is not unique for this train agent entity
         *
         * @return the alarm ID of the alarm that was raised
         *
         * @param alarmType    The alarm type to raise
         * @param descriptionParameters    A list of parameters for the alarm description
         * @param timeStamp    The time the alarm condition occurred. If not specified the default time will be used.
         */
        std::string raiseNonUniqueAlarm( const TA_Base_Core::MessageType& alarmType,
                                         TA_Base_Core::NameValuePairs& descriptionParameters,
                                         time_t timeStamp = 0 );


        /**
         * This will create the name value pairs used to raise a conflict or pre conflict alarm
         *
         * @return the name value pairs
         *
         * @param offendingTrainId    The offending train (that had its carrier dropped)
         * @param owningTrainId    The train still broadcasting
         */
        virtual TA_Base_Core::NameValuePairs createConflictParameters( TA_IRS_Bus::CommonTypes::TrainIdType ofendingTrainId,
                                                                       TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId );


        /**
         * Used to query agent configuration
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * Used to query train information
         */
        ITrainStatusManager& m_statusManager;


        /**
         * Used to do the actual alarm raising and closing
         */
        TA_Base_Core::ItaAlarms& m_taAlarms;

    };

} // TA_IRS_App

#endif // !defined(AlarmUtility_H)
