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

#include "app/trains/train_agent/src/OccOperationModeManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IDutyAgentInterface.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"

//#include "bus/managed_agent/src/AgentManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    OccOperationModeManager::OccOperationModeManager( ITrainAgentConfiguration& configuration,
                                                      IStateUpdateMessageSender& stateUpdateSender,
                                                      IMessageSender& messageSender,
                                                      ITrainStatusManager& statusManager,
                                                      IDutyAgentInterface& dutyAgent )
            : OperationModeManager( configuration,
                                    stateUpdateSender,
                                    messageSender,
                                    statusManager,
                                    dutyAgent )
    {
        FUNCTION_ENTRY( "OccOperationModeManager" );

        // Set this OCC object up to be initially in control of all station locations:
        // Get all station locations using m_configuration.getAllLocationsOfType
        LocationInfoMap stations = m_configuration.getAllLocationsOfType( TA_Base_Core::ILocation::STATION );

        // for each location returned, add it to the map m_locationControlMap[location key] = true
        for ( LocationInfoMap::iterator station = stations.begin();
              stations.end() != station; ++station )
        {
            m_locationControlMap[ station->second.key ] = true;
        }

        FUNCTION_EXIT;
    }


    OccOperationModeManager::~OccOperationModeManager()
    {
        FUNCTION_ENTRY( "~OccOperationModeManager" );
        FUNCTION_EXIT;

    }


    void OccOperationModeManager::processSubsystemStateChange( const SubsystemDutyState& dutyState )
    {
        FUNCTION_ENTRY( "processSubsystemStateChange" );

        // check the agent is in the control state using AgentManager, if not return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // check the subsystem state
        // if the state is E_SS_NORMAL
        if ( TA_Base_Bus::E_SS_NORMAL == dutyState.dutyState )
        {
            // the OCC Agent is in control
            takeControlOfLocation( dutyState.locationKey );
        }

        // if the state is E_SS_DEGRADED or E_SS_DELEGATED
        else
        {
            // the location agent is in control
            releaseControlOfLocation( dutyState.locationKey );
        }

        FUNCTION_EXIT;
    }


    void OccOperationModeManager::controlModeInitialisation()
    {
        FUNCTION_ENTRY( "controlModeInitialisation" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // get the state of the subsystem for each location in m_locationControlMap
        // note this does not throw any exceptions
        SubsystemDutyStateList subsystemStates = m_dutyAgent.getSubsystemStateForAllLocations();

        for ( SubsystemDutyStateList::iterator dutyStateIter = subsystemStates.begin();
              subsystemStates.end() != dutyStateIter; ++dutyStateIter )
        {
            // only populate the states for station locations
            LocationControlMap::iterator locationControlMapIter = m_locationControlMap.find( dutyStateIter->locationKey );
            
            if ( m_locationControlMap.end() != locationControlMapIter )
            {
                // if the state is E_SS_NORMAL, set the corresponding value to true
                // if the state is E_SS_DEGRADED or E_SS_DELEGATED set the value to false
                if ( TA_Base_Bus::E_SS_NORMAL == dutyStateIter->dutyState )
                {
                    // the OCC Agent is in control
                    locationControlMapIter->second = true;
                }

                // if the state is E_SS_DEGRADED or E_SS_DELEGATED
                else
                {
                    // the location agent is in control
                    locationControlMapIter->second = false;
                }
            }
        }

        notifyStateChange();

        FUNCTION_EXIT;
    }

} // TA_IRS_App
