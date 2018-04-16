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
#if !defined(IOperationModeManager_H)
#define IOperationModeManager_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "app/trains/train_agent/src/ISpecializedMsgProc.h"

// forward declarations
namespace TA_IRS_App
{
    class IOperationModeObserver;
}

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:41 PM
     */
    class IOperationModeManager
    {

    public:

        /**
         * This will register for operation mode updates.
         *
         * @param observer    The observer to add
         */
        virtual void registerForOperationModeChanges( IOperationModeObserver* observer ) = 0;


        /**
         * This will deregister a previously registered observer for operation mode updates
         *
         * @param observer    The previously registered observer to remove
         */
        virtual void deregisterForOperationModeChanges( IOperationModeObserver* observer ) = 0;


        /**
         * This will check the current agent state to determine if this agent is responsible for trains at the given location.
         *
         * @return true if trains at this locaiton are controlled by this agent, false otherwise.
         *
         * @param locationKey    The location to check
         */
        virtual bool isInControlOfLocation( unsigned long locationKey ) const = 0;


        /**
         * Given a train ID, this will check if this agent should be in control of it.
         * This will be true if the train is at one of this agent's controlled locations,
         * optionally if the train's location is unknown or its ats data is delocalised.
         *
         * @return whether this train can be controlled
         *
         * @param trainId    The train ID to check
         * @param allowDelocalised    Whether trains with invalid localisation are to be considered controllable, defaults to true
         * @param allowInvalid    Whether trains with invalid ATS are to be considered controllable, defaults to true
         */
        virtual bool isInControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         bool allowDelocalised = true,
                                         bool allowInvalid = true ) const = 0;


        /**
         * Get the list of locations this agent is in control of.
         *
         * @return the list of locations this agent is responsible for
         */
        virtual std::set< unsigned long > getControlledLocations() const = 0;


        /**
         * Checks if this agent is in degraded mode. This means the OCC is not contactable.
         *
         * @return true if in degraded mode, false otherwise
         */
        virtual bool isIscsDegradedMode() const = 0;


        /**
         * Checks if the local radio base station is in fallback. This is only applicable at a station.
         *
         * @return true if in degraded mode, false otherwise
         */
        virtual bool isLocalRadioInFallback() const = 0;


		virtual void registerDutyReleaseUpdate (ISpecializedMsgProc * objObserver) = 0;

    };

} // TA_IRS_App

#endif // !defined(IOperationModeManager_H)
