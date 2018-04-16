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
#if !defined(DutyAgentInterface_H)
#define DutyAgentInterface_H

#include "app/trains/train_agent/src/IDutyAgentInterface.h"

#include "bus/security/duty_monitor_framework/src/DutyAgentAccess.h"


namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
}


namespace TA_IRS_App
{

    /**
     * This is an interface to the Duty Agent functions
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:32 PM
     */
    class DutyAgentInterface : public IDutyAgentInterface
    {

    public:

        /**
         * Constructor with required information
         *
         * @param configuration    Agent configuration object
         */
        DutyAgentInterface( ITrainAgentConfiguration& configuration );


        /**
         * Destructor
         */
        virtual ~DutyAgentInterface();


        /**
         * This is only applicable at a station.
         * It will degrade the train agent's subsystem at the local location,
         * so all train operations transfer to this local station.
         *
         * @exception DutyMonitorFrameworkException if there is an error performing the operation
         */
        virtual void degradeLocalTrainSubsystem();


        /**
         * This will get the subsystem duty state for the local train agent subsystem
         * at the local tain agent location. It is only applicable to station train agents.
         *
         * @return the state of the train subsystem at the local agent location
         *
         * @exception DutyMonitorFrameworkException if there is an error performing the operation
         */
        virtual TA_Base_Bus::ESubsystemState getLocalSubsystemState();


        /**
         * This will get the subsystem duty state for the train agent subsystem at all locations.
         * It is only applicable to OCC train agents.
         *
         * If there is an error checking a location, the OCC agent assumes it is in control
         *
         * @return the state of the train subsystem at all
         */
        virtual SubsystemDutyStateList getSubsystemStateForAllLocations();


    private:


        /**
         * Private default constructor
         */
        DutyAgentInterface();


        /**
         * Used to get configuration items
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * This is used to perform operations on the local duty agent
         */
        TA_Base_Bus::DutyAgentAccess m_dutyAgent;

    };

} // TA_IRS_App

#endif // !defined(DutyAgentInterface_H)

