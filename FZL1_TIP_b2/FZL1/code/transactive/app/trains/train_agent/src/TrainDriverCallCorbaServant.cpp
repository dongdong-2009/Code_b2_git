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

#include "app/trains/train_agent/src/ITimsDriverCallManager.h"
#include "app/trains/train_agent/src/TrainDriverCallCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"


namespace TA_IRS_App
{

    TrainDriverCallCorbaServant::TrainDriverCallCorbaServant( ITimsDriverCallManager& timsDriverCallManager )
        : m_timsDriverCallManager( timsDriverCallManager )
    {
        FUNCTION_ENTRY( "TrainDriverCallCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainDriverCallObjectName );

        FUNCTION_EXIT;
    }


    TrainDriverCallCorbaServant::~TrainDriverCallCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainDriverCallCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList* TrainDriverCallCorbaServant::getAllOccCallRequests()
    {
        FUNCTION_ENTRY( "getAllOccCallRequests" );

        OperationModeManager::ensureControlState();//limin

        // Delegate the call to m_timsDriverCallManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::CallTypes::OccCallRequestList result;

        CPP_CATCH( result = m_timsDriverCallManager.getAllOccCallRequests() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList( TA_IRS_Bus::CallTypesConversion::convertToCorba( result ) );
    }


    void TrainDriverCallCorbaServant::answerOccCallRequest( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                            const char* sessionId )
    {
        FUNCTION_ENTRY( "answerOccCallRequest" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsDriverCallManager:
        // Do parameter conversion using the appropriate convertToCpp methods

        CPP_CATCH( m_timsDriverCallManager.answerOccCallRequest( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                                 sessionId ) );

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult* TrainDriverCallCorbaServant::makeStaticGroupDriverCall( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainList,
                                                                                                                  const char* groupTsi,
                                                                                                                  CORBA::Boolean override,
                                                                                                                  TA_Base_Bus::IRadioSessionCorbaDef::ECallType callType,
                                                                                                                  TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority callPriority,
                                                                                                                  const char* sessionId )
    {
        FUNCTION_ENTRY( "makeStaticGroupDriverCall" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        TA_IRS_Bus::CallTypes::DriverCallResult result;

        // Delegate the call to m_timsDriverCallManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( result = m_timsDriverCallManager.makeDriverCall( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainList ),
                                                                    groupTsi,
                                                                    override,
                                                                    TA_IRS_Bus::CallTypesConversion::convertToCpp( callType ),
                                                                    TA_IRS_Bus::CallTypesConversion::convertToCpp( callPriority ),
                                                                    sessionId ) );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult( TA_IRS_Bus::CallTypesConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult* TrainDriverCallCorbaServant::makeDgnaDriverCall( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainList,
                                                                                                           const TA_Base_Bus::ITrainDriverCallCorba::ItsiList& individualSubscribers,
                                                                                                           CORBA::Boolean override,
                                                                                                           TA_Base_Bus::IRadioSessionCorbaDef::ECallType callType,
                                                                                                           TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority callPriority,
                                                                                                           const char* sessionId )
    {
        FUNCTION_ENTRY( "makeDgnaDriverCall" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsDriverCallManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::CallTypes::DriverCallResult result;

        CPP_CATCH( result = m_timsDriverCallManager.makeDriverCall( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainList ),
                                                                    TA_IRS_Bus::CallTypesConversion::convertToCpp( individualSubscribers ),
                                                                    override,
                                                                    TA_IRS_Bus::CallTypesConversion::convertToCpp( callType ),
                                                                    TA_IRS_Bus::CallTypesConversion::convertToCpp( callPriority ),
                                                                    sessionId ) );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult( TA_IRS_Bus::CallTypesConversion::convertToCorba( result ) );
    }


    CORBA::UShort TrainDriverCallCorbaServant::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // Delegate the call to m_timsDriverCallManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CORBA::UShort result;

        CPP_CATCH( result = m_timsDriverCallManager.getFlexibleTimeOutValue() );

        FUNCTION_EXIT;
        return  result;
    }

} // TA_IRS_App
