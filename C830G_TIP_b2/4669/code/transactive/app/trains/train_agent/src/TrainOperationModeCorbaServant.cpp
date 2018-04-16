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

#include "app/trains/train_agent/src/IOperationModeManagerServantInterface.h"
#include "app/trains/train_agent/src/ITimsModeManager.h"
#include "app/trains/train_agent/src/TrainOperationModeCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"


namespace TA_IRS_App
{

    TrainOperationModeCorbaServant::TrainOperationModeCorbaServant( IOperationModeManagerServantInterface& operationModeManager,
                                                                    ITimsModeManager& timsModeManager )
        : m_operationModeManager( operationModeManager ),
          m_timsModeManager( timsModeManager )
    {
        FUNCTION_ENTRY( "TrainOperationModeCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainOperationModeObjectName );

        FUNCTION_EXIT;
    }


    TrainOperationModeCorbaServant::~TrainOperationModeCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainOperationModeCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    void TrainOperationModeCorbaServant::inFallback( CORBA::Boolean fallback )
    {
        FUNCTION_ENTRY( "inFallback" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_operationModeManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_operationModeManager.inFallback( fallback ) );

        FUNCTION_EXIT;
    }


    void TrainOperationModeCorbaServant::inDoubleAtsFailure( CORBA::Boolean doubleAtsFailure )
    {
        FUNCTION_ENTRY( "inDoubleAtsFailure" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_operationModeManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_operationModeManager.inDoubleAtsFailure( doubleAtsFailure ) );

        FUNCTION_EXIT;
    }


    bool TrainOperationModeCorbaServant::radioInFallback()
    {
        FUNCTION_ENTRY( "radioInFallback" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_operationModeManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        bool result = false;

        CPP_CATCH( result = m_operationModeManager.radioInFallback() );

        FUNCTION_EXIT;
        return result;
    }


    void TrainOperationModeCorbaServant::manuallyTakeTrainControl( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                                   const char* sessionId )
    {
        FUNCTION_ENTRY( "manuallyTakeTrainControl" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsModeManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsModeManager.manuallyTakeTrainControl( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                               sessionId ) );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
