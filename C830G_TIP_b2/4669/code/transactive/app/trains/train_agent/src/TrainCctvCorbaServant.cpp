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


#include "app/trains/train_agent/src/ITimsCctvManager.h"
#include "app/trains/train_agent/src/TrainCctvCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"


namespace TA_IRS_App
{

    TrainCctvCorbaServant::TrainCctvCorbaServant( ITimsCctvManager& timsCctvManager )
        : m_timsCctvManager( timsCctvManager )
    {
        FUNCTION_ENTRY( "TrainCctvCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainCctvObjectName );

        FUNCTION_EXIT;
    }


    TrainCctvCorbaServant::~TrainCctvCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainCctvCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::setCarrierOn( TA_Base_Bus::ITrainCommonCorba::TrainId trainId, const char* sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOn" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.setCarrierOn( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                   sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::setCarrierOnDelocalisedTrain( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                              CORBA::ULong trainLocation,
                                                              const char* sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOnDelocalisedTrain" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.setCarrierOnDelocalisedTrain( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                                   trainLocation,
                                                                   sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::setCarrierOff( TA_Base_Bus::ITrainCommonCorba::TrainId trainId, const char* sessionId )
    {
        FUNCTION_ENTRY( "setCarrierOff" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.setCarrierOff( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                    sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::setSingleScreen( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                 TA_Base_Bus::ITrainCctvCorba::ECctvCamera camera,
                                                 const char* sessionId )
    {
        FUNCTION_ENTRY( "setSingleScreen" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.setSingleScreen( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                      TA_IRS_Bus::CctvTypesConversion::convertToCpp( camera ),
                                                      sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::setQuadScreen( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                               TA_Base_Bus::ITrainCctvCorba::ECctvCamera cameraW,
                                               TA_Base_Bus::ITrainCctvCorba::ECctvCamera cameraX,
                                               TA_Base_Bus::ITrainCctvCorba::ECctvCamera cameraY,
                                               TA_Base_Bus::ITrainCctvCorba::ECctvCamera cameraZ,
                                               const char* sessionId )
    {
        FUNCTION_ENTRY( "setQuadScreen" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.setQuadScreen( trainId,
                                                    TA_IRS_Bus::CctvTypesConversion::convertToCpp( cameraW ),
                                                    TA_IRS_Bus::CctvTypesConversion::convertToCpp( cameraX ),
                                                    TA_IRS_Bus::CctvTypesConversion::convertToCpp( cameraY ),
                                                    TA_IRS_Bus::CctvTypesConversion::convertToCpp( cameraZ ),
                                                    sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::setSequence( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                             TA_Base_Bus::ITrainCctvCorba::ECctvSequenceNumber sequenceNo,
                                             TA_Base_Bus::ITrainCctvCorba::ECctvSequenceMode sequenceMode,
                                             const char* sessionId )
    {
        FUNCTION_ENTRY( "setSequence" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.setSequence( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                  TA_IRS_Bus::CctvTypesConversion::convertToCpp( sequenceNo ),
                                                  TA_IRS_Bus::CctvTypesConversion::convertToCpp( sequenceMode ),
                                                  sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::addAlarmImage( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                               const TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence& cameraIds,
                                               const char* sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImage" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.addAlarmImage( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                    TA_IRS_Bus::CctvTypesConversion::convertToCpp( cameraIds ),
                                                    sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::addAlarmImageDelocalisedTrain( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                               const TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence& cameraIds,
                                                               CORBA::ULong trainLocation,
                                                               const char* sessionId )
    {
        FUNCTION_ENTRY( "addAlarmImageDelocalisedTrain" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.addAlarmImageDelocalisedTrain( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                                    TA_IRS_Bus::CctvTypesConversion::convertToCpp( cameraIds ),
                                                                    trainLocation,
                                                                    sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainCctvCorbaServant::removeAlarmImage( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                  TA_Base_Bus::ITrainCctvCorba::ECctvCamera cameraId,
                                                  const char* sessionId )
    {
        FUNCTION_ENTRY( "removeAlarmImage" );

        // First verify the operation state using AgentManager::ensureControlState()
        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CPP_CATCH( m_timsCctvManager.removeAlarmImage( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ),
                                                       TA_IRS_Bus::CctvTypesConversion::convertToCpp( cameraId ),
                                                       sessionId ) );

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainCctvCorba::CctvSwitchState TrainCctvCorbaServant::getActiveTrainForStage( CORBA::ULong stageID )
    {
        FUNCTION_ENTRY( "getActiveTrainForStage" );

        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::CctvTypes::CctvSwitchState result;

        CPP_CATCH( result = m_timsCctvManager.getActiveTrainForStage( stageID ) );

        FUNCTION_EXIT;
        return  TA_IRS_Bus::CctvTypesConversion::convertToCorba( result );
    }

	TA_Base_Bus::ITrainCctvCorba::CctvSwitchState TrainCctvCorbaServant::getCctvState( TA_Base_Bus::ITrainCommonCorba::TrainId trainId )
	{
		TA_IRS_Bus::CctvTypes::CctvSwitchState result;
		CPP_CATCH( result = m_timsCctvManager.getCctvState( trainId ) );
		return  TA_IRS_Bus::CctvTypesConversion::convertToCorba( result );
	}

    CORBA::UShort TrainCctvCorbaServant::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // Delegate the call to m_timsCctvManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        unsigned short  result;

        CPP_CATCH( result = m_timsCctvManager.getFlexibleTimeOutValue() );

        FUNCTION_EXIT;
        return  result;
    }

} // TA_IRS_App
