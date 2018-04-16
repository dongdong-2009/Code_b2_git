/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(ISubsystemDutyStateObserver_H)
#define ISubsystemDutyStateObserver_H

#include "app/trains/train_agent/src/SubsystemDutyState.h"

namespace TA_IRS_App
{

    /**
     * This is an observer for subsystem state changes
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:44 PM
     */
    class ISubsystemDutyStateObserver
    {

    public:

        /**
         * This processes a change of state for the train agent subsystem at a location.
         *
         * @param dutyState    The duty state that changed
         */
        virtual void processSubsystemStateChange( const SubsystemDutyState& dutyState ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ISubsystemDutyStateObserver_H)
