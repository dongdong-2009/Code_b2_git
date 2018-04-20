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


#include "app/trains/train_agent/src/DutyAgentObserver.h"
#include "app/trains/train_agent/src/ISubsystemDutyStateObserver.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/SubsystemDutyState.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"

#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/MessageTypes.h"


namespace TA_IRS_App
{

    DutyAgentObserver::DutyAgentObserver( ITrainAgentConfiguration& configuration,
                                          ISubsystemDutyStateObserver& dutyStateObserver )
        : m_configuration( configuration ),
          m_dutyStateObserver( dutyStateObserver )
    {
        FUNCTION_ENTRY( "DutyAgentObserver" );

        subscribeToMessages();

        FUNCTION_EXIT;
    }


    DutyAgentObserver::~DutyAgentObserver()
    {
        FUNCTION_ENTRY( "~DutyAgentObserver" );

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        FUNCTION_EXIT;
    }


    void DutyAgentObserver::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // Using agent manager, get the agent state
        // if it is Control, then continue to handle the message, otherwise return.
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // extract the message to either:
        TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* stateSequence = NULL;
        TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence* dutySequence = NULL;

        // Call the appropriate handler function:
        if ( payload.messageState >>= stateSequence )
        {
            processSubsystemStateSequence( *stateSequence );
        }
        else if ( payload.messageState >>= dutySequence )
        {
            processSubsystemDutySequence( *dutySequence,
                                          payload.messageTypeKey.in() == TA_Base_Core::DutyAgentBroadcastComms::AddSubsystemDuty.getTypeKey() );
        }

        FUNCTION_EXIT;
    }


    void DutyAgentObserver::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        // Using MessageSubscriptionManager::subscribeToBroadcastCommsMessage
        // subscribe to TA_Base_Core::DutyAgentBroadcastComms::SubsystemStateChangeNotification
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::SubsystemStateChangeNotification,
                                                                                                  this );

        // but for these two subscriptions, leave them commented out, they may not be necessary, but we may need to add them at a later stage
        // TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::AddSubsystemDuty,
        //                                                                                           this );
        // TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( TA_Base_Core::DutyAgentBroadcastComms::RemoveSubsystemDuty,
        //                                                                                           this );

        FUNCTION_EXIT;
    }


    void DutyAgentObserver::processSubsystemStateSequence( TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence& subsystemStateSequence )
    {
        FUNCTION_ENTRY( "processSubsystemStateSequence" );

        const unsigned long dutySubsystem = m_configuration.getDutySubsystem();

        // for each item in the sequence
        for ( unsigned long i = 0; i < subsystemStateSequence.length(); ++i )
        {
            // if the subsystem key matches m_configuration.getDutySubsystem()
            if ( dutySubsystem == subsystemStateSequence[i].subsystemKey )
            {
                // create a SubsystemDutyState structure
                // populate the locationKey with the region key from the message
                // populate the dutyState with the currState from the message (must use reinterpret_cast)
                SubsystemDutyState dutyState =
                {
                    subsystemStateSequence[i].regionKey,
                    static_cast< TA_Base_Bus::ESubsystemState >( subsystemStateSequence[i].currState )
                };

                // Log the state change - the subsystem key, location key, previous and current states
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "State changed from previous [%d] to current [%d] for subsystem [%d], location [%d]",
                    subsystemStateSequence[i].prevState,
                    subsystemStateSequence[i].currState,
                    dutySubsystem,
                    dutyState.locationKey );

                // call m_dutyStateObserver.processSubsystemStateChange
                m_dutyStateObserver.processSubsystemStateChange( dutyState );
            }
        }

        FUNCTION_EXIT;
    }


    void DutyAgentObserver::processSubsystemDutySequence( TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence& subsystemDutySequence,
                                                          bool addDuty )
    {
        FUNCTION_ENTRY( "processSubsystemDutySequence" );

        const unsigned long dutySubsystem = m_configuration.getDutySubsystem();

        // for each item in the sequence
        for ( unsigned long i = 0; i < subsystemDutySequence.length(); ++i )
        {
            // if the subsystem key matches m_configuration.getDutySubsystem()
            if ( dutySubsystem == subsystemDutySequence[i].subsystemKey )
            {
                // create a SubsystemDutyState structure
                // populate the locationKey with the region key from the message
                SubsystemDutyState dutyState;
                dutyState.locationKey = subsystemDutySequence[i].regionKey;

                // Now, from the session ID, the location must be populated
                // Using AuthenticationLibrary, get the session info for the given session:
                // getSessionInfo( subsystemDutyItem.session, m_configuration.getAgentSessionId() );
                // From the session Info, LocationId is available, populate the location key in the SubsystemDutyState structure
                TA_Base_Bus::SessionInfo sessionInfo = TA_Base_Bus::AuthenticationLibrary().getSessionInfo( subsystemDutySequence[i].session.in(),
                                                                                                           m_configuration.getAgentSessionId() );

                // the operator location can be obtained by the WorkstationId
                // m_configuration.getLocationFromConsole( WorkstationId )
                // and from this, the console location type can be obtained
                // m_configuration.getLocationType( console location )
                unsigned long operatorLocation = m_configuration.getLocationFromConsole( sessionInfo.WorkstationId );

                TA_Base_Core::ILocation::ELocationType locationType = m_configuration.getLocationType( operatorLocation );

                // Now based on the location, we can derive the subsystem state
                // if duty is being added (addDuty)
                if ( addDuty )
                {
                    // if the console is at a station, the duty is E_SS_DELEGATED
                    if ( TA_Base_Core::ILocation::STATION == locationType )
                    {
                        dutyState.dutyState = TA_Base_Bus::E_SS_DELEGATED;
                    }
                    // if the console is at the OCC, the duty is E_SS_NORMAL
                    else if ( TA_Base_Core::ILocation::OCC == locationType )
                    {
                        dutyState.dutyState = TA_Base_Bus::E_SS_NORMAL;
                    }
                    // if the console is anywhere else, return
                    else
                    {
                        FUNCTION_EXIT;
                        return;
                    }
                }
                // otherwise duty is being removed
                else
                {
                    // if the console is at a station, the duty is E_SS_NORMAL
                    if ( TA_Base_Core::ILocation::STATION == locationType )
                    {
                        dutyState.dutyState = TA_Base_Bus::E_SS_NORMAL;
                    }
                    // if the console is at the OCC, the duty is E_SS_DELEGATED
                    else if ( TA_Base_Core::ILocation::OCC == locationType )
                    {
                        dutyState.dutyState = TA_Base_Bus::E_SS_DELEGATED;
                    }
                    // if the console is anywhere else, return
                    else
                    {
                        FUNCTION_EXIT;
                        return;
                    }
                }

                // Log the state change - the subsystem key, location key, and current state
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugInfo,
                             "Duty state changed to SubsystemState [%d] for subsystem [%d], location [%d]",
                             dutyState.dutyState,
                             dutySubsystem,
                             dutyState.locationKey );

                // call m_dutyStateObserver.processSubsystemStateChange
                m_dutyStateObserver.processSubsystemStateChange( dutyState );
            }
        }

        FUNCTION_EXIT;
    }

} // TA_IRS_App
