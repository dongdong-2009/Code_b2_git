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

#include "app/trains/train_agent/src/ITimsPecManager.h"
#include "app/trains/train_agent/src/TrainPecCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PecTypesConversion.h"


namespace TA_IRS_App
{

    TrainPecCorbaServant::TrainPecCorbaServant( ITimsPecManager& timsPecManager )
        : m_timsPecManager( timsPecManager )
    {
        FUNCTION_ENTRY( "TrainPecCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainPecObjectName );

        FUNCTION_EXIT;
    }


    TrainPecCorbaServant::~TrainPecCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainPecCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainPecCorba::PecUpdateList* TrainPecCorbaServant::getAllActivePec()
    {
        FUNCTION_ENTRY( "getAllActivePec" );

        // Delegate the call to m_timsPecManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::PecTypes::PecUpdateList result;

        CPP_CATCH( result = m_timsPecManager.getAllActivePec() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainPecCorba::PecUpdateList( TA_IRS_Bus::PecTypesConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence* TrainPecCorbaServant::activatePecCamera( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                                                                TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                                                                                                TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                                                                                                const char* sessionId )
    {
        FUNCTION_ENTRY( "activatePecCamera" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPecManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::CctvTypes::TrainCameraSequence result;

        CPP_CATCH( result = m_timsPecManager.activatePecCamera( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                                TA_IRS_Bus::PecTypesConversion::convertToCpp( car ),
                                                                TA_IRS_Bus::PecTypesConversion::convertToCpp( pec ),
                                                                sessionId ) );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence( TA_IRS_Bus::CctvTypesConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence* TrainPecCorbaServant::activatePecCameraDelocalisedTrain( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                                                                                TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                                                                                                                TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                                                                                                                CORBA::ULong trainLocation,
                                                                                                                const char* sessionId )
    {
        FUNCTION_ENTRY( "activatePecCameraDelocalisedTrain" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPecManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::CctvTypes::TrainCameraSequence result;

        CPP_CATCH( result = m_timsPecManager.activatePecCameraDelocalisedTrain( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                                                TA_IRS_Bus::PecTypesConversion::convertToCpp( car ),
                                                                                TA_IRS_Bus::PecTypesConversion::convertToCpp( pec ),
                                                                                trainLocation,
                                                                                sessionId ) );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence( TA_IRS_Bus::CctvTypesConversion::convertToCorba( result ) );
    }


    void TrainPecCorbaServant::answerPec( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                          TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                                          TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                                          const char* sessionId )
    {
        FUNCTION_ENTRY( "answerPec" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPecManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPecManager.answerPec( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                               TA_IRS_Bus::PecTypesConversion::convertToCpp( car ),
                                               TA_IRS_Bus::PecTypesConversion::convertToCpp( pec ),
                                               sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainPecCorbaServant::resetPec( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                         TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                                         TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                                         const char* sessionId )
    {
        FUNCTION_ENTRY( "resetPec" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPecManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPecManager.resetPec( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                              TA_IRS_Bus::PecTypesConversion::convertToCpp( car ),
                                              TA_IRS_Bus::PecTypesConversion::convertToCpp( pec ),
                                              sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainPecCorbaServant::resetAllPec( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                            const char* sessionId )
    {
        FUNCTION_ENTRY( "resetAllPec" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPecManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPecManager.resetAllPec( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                 sessionId ) );

        FUNCTION_EXIT;
    }


    CORBA::UShort TrainPecCorbaServant::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // Delegate the call to m_timsPecManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CORBA::UShort result;

        CPP_CATCH( result = m_timsPecManager.getFlexibleTimeOutValue() );

        FUNCTION_EXIT;
        return result;
    }

} // TA_IRS_App
