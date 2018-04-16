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


#include "app/trains/train_agent/src/StateUpdateManager.h"
#include "app/trains/train_agent/src/TrainStateUpdateCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainCorbaProxy/src/StateUpdateTypesConversion.h"


namespace TA_IRS_App
{

    TrainStateUpdateCorbaServant::TrainStateUpdateCorbaServant( StateUpdateManager& stateUpdateManager )
        : m_stateUpdateManager( stateUpdateManager )
    {
        FUNCTION_ENTRY( "TrainStateUpdateCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainStateUpdateObjectName );

        FUNCTION_EXIT;
    }


    TrainStateUpdateCorbaServant::~TrainStateUpdateCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainStateUpdateCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState* TrainStateUpdateCorbaServant::getLocalState()
    {
        FUNCTION_ENTRY( "getLocalState" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        TA_IRS_Bus::StateUpdateTypes::FullLocalState result;

        // Delegate the call to m_stateUpdateManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( result = m_stateUpdateManager.getLocalState() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState( TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState* TrainStateUpdateCorbaServant::getGlobalState()
    {
        FUNCTION_ENTRY( "getGlobalState" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        TA_IRS_Bus::StateUpdateTypes::FullGlobalState result;

        // Delegate the call to m_stateUpdateManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( result = m_stateUpdateManager.getGlobalState() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState( TA_IRS_Bus::StateUpdateTypesConversion::convertToCorba( result ) );
    }


} // TA_IRS_App
