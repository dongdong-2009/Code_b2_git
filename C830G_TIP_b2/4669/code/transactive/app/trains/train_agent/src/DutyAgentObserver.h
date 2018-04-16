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
#if !defined(DutyAgentObserver_H)
#define DutyAgentObserver_H


#include "bus/security/duty_agent/IDL/src/DutyAgentTypeCorbaDef.h"

#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/BackwardsCompatibility.h"


namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ISubsystemDutyStateObserver;
}


namespace TA_IRS_App
{

    /**
     * This will be a observer for Duty Agent state changes
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:33 PM
     */
    class DutyAgentObserver : public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
    {

    public:

        /**
         * Constructor with required information
         *
         * @param configuration    Agent configuration object
         * @param dutyStateObserver    the observer for subsystem state changes
         */
        DutyAgentObserver( ITrainAgentConfiguration& configuration,
                           ISubsystemDutyStateObserver& dutyStateObserver );


        /**
         * Destructor
         */
        ~DutyAgentObserver();


        /**
         * This received update messages from the Duty Agent
         *
         * @param payload    The CORBA message data
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


    private:


        /**
         * Private default constructor
         */
        DutyAgentObserver();


        /**
         * This will subscribe to duty agent messages
         */
        void subscribeToMessages();


        /**
         * Handles a subsystem state update message from the duty agent
         *
         * @param subsystemStateSequence    The list of subsystem states
         */
        void processSubsystemStateSequence( TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence& subsystemStateSequence );


        /**
         * Handles a subsystem duty update message from the duty agent
         *
         * @param subsystemDutySequence    The list of subsystem duties
         * @param addDuty    true if duty is being added, false if it is being removed
         */
        void processSubsystemDutySequence( TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& subsystemDutySequence,
                                           bool addDuty );


        /**
         * Used to get configuration items
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * This is the observer to notify of duty changes
         */
        ISubsystemDutyStateObserver& m_dutyStateObserver;
    };

} // TA_IRS_App

#endif // !defined(DutyAgentObserver_H)


