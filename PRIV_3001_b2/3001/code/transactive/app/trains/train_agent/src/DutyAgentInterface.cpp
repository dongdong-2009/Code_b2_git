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

#include "app/trains/train_agent/src/DutyAgentInterface.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"

#include "core/exceptions/src/TransactiveException.h"
#include "bus/security/duty_monitor_framework/src/DutyMonitorFrameworkException.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    DutyAgentInterface::DutyAgentInterface( ITrainAgentConfiguration& configuration )
        : m_configuration( configuration ),
          m_dutyAgent()
    {
        FUNCTION_ENTRY( "DutyAgentInterface" );
        FUNCTION_EXIT;
    }


    DutyAgentInterface::~DutyAgentInterface()
    {
        FUNCTION_ENTRY( "~DutyAgentInterface" );
        FUNCTION_EXIT;
    }


    void DutyAgentInterface::degradeLocalTrainSubsystem()
    {
        FUNCTION_ENTRY( "degradeLocalTrainSubsystem" );

        // for the subsystem list, build a list containing one item, m_configuration.getDutySubsystem
        TA_Base_Bus::SubsystemKeyList subsystems;
        subsystems.push_back( m_configuration.getDutySubsystem() );

        // call m_dutyAgent.degradeSubsystemsAtLocation
        // for the session ID use m_configuration.getAgentSessionId
        // for the location use m_configuration.getLocationKey
        m_dutyAgent.degradeSubsystemsAtLocation( m_configuration.getAgentSessionId(),
                                                 subsystems,
                                                 m_configuration.getLocationKey() );

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ESubsystemState DutyAgentInterface::getLocalSubsystemState()
    {
        FUNCTION_ENTRY( "getLocalSubsystemState" );

        // for the subsystem list, build a list containing one item, m_configuration.getDutySubsystem
        TA_Base_Bus::SubsystemKeyList subsystems;
        subsystems.push_back( m_configuration.getDutySubsystem() );

        // call m_dutyAgent.getSubsystemDutyState
        // for the session ID use m_configuration.getAgentSessionId
        // for the location use m_configuration.getLocationKey
        TA_Base_Bus::SubsystemStateList stateList = m_dutyAgent.getSubsystemDutyState( m_configuration.getAgentSessionId(),
                                                                                       subsystems,
                                                                                       m_configuration.getLocationKey() );

        if ( true == stateList.empty() )
        {
            throw TA_Base_Bus::DutyMonitorFrameworkException( "No duty state returned for local subsystem" );
        }

        // the result should only have 1 item.
        TA_ASSERT( 1 == stateList.size(), "stateList should only have 1 item" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Duty state [regionKey: %d][subsystemKey: %d][prevState: %d][currState: %d]",
                     stateList[0].regionKey, stateList[0].subsystemKey,
                     stateList[0].prevState, stateList[0].currState );

        // Get the addState, reinterpret cast this to a ESubsystemState, and return it
        FUNCTION_EXIT;
        return static_cast<TA_Base_Bus::ESubsystemState>( stateList[0].currState );
    }


    SubsystemDutyStateList DutyAgentInterface::getSubsystemStateForAllLocations()
    {
        FUNCTION_ENTRY( "getSubsystemStateForAllLocations" );

        SubsystemDutyStateList result;

        // for the subsystem list, build a list containing one item, m_configuration.getDutySubsystem
        TA_Base_Bus::SubsystemKeyList subsystems;
        subsystems.push_back( m_configuration.getDutySubsystem() );

        try
        {
            // call m_dutyAgent.getSubsystemDutyState
            // for the session ID use m_configuration.getAgentSessionId
            // for the location use 0 (all locations)
            TA_Base_Bus::SubsystemStateList stateList = m_dutyAgent.getSubsystemDutyState( m_configuration.getAgentSessionId(),
                                                                                           subsystems,
                                                                                           0 );

            for( TA_Base_Bus::SubsystemStateList::iterator stateListIter = stateList.begin();
                  stateList.end() != stateListIter; ++stateListIter )
            {

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "Duty state [regionKey: %d][subsystemKey: %d][prevState: %d][currState: %d]",
                             stateListIter->regionKey, stateListIter->subsystemKey,
                             stateListIter->prevState, stateListIter->currState );

                // based on the result, build a SubsystemDutyState structure, and add it to the return list
                SubsystemDutyState dutyState =
                {
                    stateListIter->regionKey,
                    static_cast<TA_Base_Bus::ESubsystemState>( stateListIter->currState )
                };

                result.push_back( dutyState );
            }
        }
        // if there are any exceptions getting the state from the duty agent,
        // catch the exception, and do not put an entry for it in the return list
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }

        FUNCTION_EXIT;
        return result;
    }

}

