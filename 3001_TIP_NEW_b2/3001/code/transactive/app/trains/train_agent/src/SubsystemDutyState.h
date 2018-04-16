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

#if !defined(SubsystemDutyState_H)
#define SubsystemDutyState_H

// these need to be included before DMFTypeDefs.h, because it doesnt include all its prerequisites
#include "bus/security/authentication_agent/IDL/src/SessionInfoCorbaDef.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/security/duty_agent/IDL/src/DutyAgentTypeCorbaDef.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include <string>
#include <vector>
#include <map>

#include "bus/security/duty_monitor_framework/src/DMFTypeDefs.h"

namespace TA_IRS_App
{

    /**
     * This represents the state of the train subsystem at the given location.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:05 PM
     */
    struct SubsystemDutyState
    {

        /**
         * The location this duty state applies to
         */
        unsigned long locationKey;


        /**
         * The current subsystem duty state
         */
        TA_Base_Bus::ESubsystemState dutyState;

        bool operator < ( const SubsystemDutyState& rhs ) const //limin++, CL-21120
        {
            return ( locationKey < rhs.locationKey ) ||
                   ( locationKey == rhs.locationKey && dutyState < rhs.dutyState );
        }

        bool operator == ( const SubsystemDutyState& rhs ) const //limin++, CL-21120
        {
            return ( locationKey == rhs.locationKey && dutyState == rhs.dutyState );
        }
    };


    /**
     * A list of subsystem duty states
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:05 PM
     */
    typedef std::vector< SubsystemDutyState > SubsystemDutyStateList;

} // TA_IRS_App

#endif // !defined(SubsystemDutyState_H)
