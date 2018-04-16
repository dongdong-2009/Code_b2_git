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
#if !defined(IDutyAgentInterface_H)
#define IDutyAgentInterface_H

#include "app/trains/train_agent/src/SubsystemDutyState.h"

#include <set>

namespace TA_IRS_App
{

    /**
     * This is used to query subsystem duty state, and to degrade subsystems.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:40 PM
     */
    class IDutyAgentInterface
    {

    public:

        /**
         * This is only applicable at a station.
         * It will degrade the train agent's subsystem at the local location,
         * so all train operations transfer to this local station.
         *
         * @exception DutyMonitorFrameworkException if there is an error performing the operation
         */
        virtual void degradeLocalTrainSubsystem() = 0;


        /**
         * This will get the subsystem duty state for the local train agent subsystem
         * at the local tain agent location. It is only applicable to station train agents.
         *
         * @return the state of the train subsystem at the local agent location
         *
         * @exception DutyMonitorFrameworkException if there is an error performing the operation
         */
        virtual TA_Base_Bus::ESubsystemState getLocalSubsystemState() = 0;


        /**
         * This will get the subsystem duty state for the train agent subsystem at all locations.
         * It is only applicable to OCC train agents.
         *
         * If there is an error checking a location, the OCC agent assumes it is in control
         *
         * @return the state of the train subsystem at all
         */
        virtual SubsystemDutyStateList getSubsystemStateForAllLocations() = 0;

    };

} // TA_IRS_App

#endif // !defined(IDutyAgentInterface_H)
