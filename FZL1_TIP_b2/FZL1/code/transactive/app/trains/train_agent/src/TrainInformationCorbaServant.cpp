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
#include "app/trains/train_agent/src/ITrainInformation.h"
#include "app/trains/train_agent/src/TrainInformationCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationConversion.h"


namespace TA_IRS_App
{

    TrainInformationCorbaServant::TrainInformationCorbaServant( ITrainInformation& trainInformation,
                                                                IOperationModeManagerServantInterface& operationModeManager )
        : m_operationModeManager( operationModeManager ),
          m_trainInformation( trainInformation )
    {
        FUNCTION_ENTRY( "TrainInformationCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainInformationObjectName );

        FUNCTION_EXIT;
    }


    TrainInformationCorbaServant::~TrainInformationCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainInformationCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainInformationCorba::TrainDetails* TrainInformationCorbaServant::getTrainDetails( TA_Base_Bus::ITrainCommonCorba::TrainId trainId )
    {
        FUNCTION_ENTRY( "getTrainDetails" );

        OperationModeManager::ensureControlState();//limin

        // Delegate the call to m_trainInformation:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::TrainInformationTypes::TrainDetails result;

        CPP_CATCH( result = m_trainInformation.getTrainDetails( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ) ) );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainInformationCorba::TrainDetails( TA_IRS_Bus::TrainInformationConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainInformationCorba::TrainDetailsList* TrainInformationCorbaServant::getTrainList()
    {
        FUNCTION_ENTRY( "getTrainList" );

        OperationModeManager::ensureControlState();//limin

        // Delegate the call to m_trainInformation:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions
        // CPP_CATCH( result = object.operation() )

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList result;

        CPP_CATCH( result = m_trainInformation.getTrainList() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainInformationCorba::TrainDetailsList( TA_IRS_Bus::TrainInformationConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainInformationCorba::TrainSelectionList* TrainInformationCorbaServant::getTrainSelectionList()
    {
        FUNCTION_ENTRY( "getTrainSelectionList" );

        OperationModeManager::ensureControlState();//limin

        // Delegate the call to m_trainInformation:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::TrainInformationTypes::TrainSelectionList result;

        CPP_CATCH( result = m_trainInformation.getTrainSelectionList() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainInformationCorba::TrainSelectionList( TA_IRS_Bus::TrainInformationConversion::convertToCorba( result ) );
    }


    void TrainInformationCorbaServant::saveTrainSelection( const char* selectionName,
                                                           const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIDs,
                                                           const char* sessionId )
    {
        FUNCTION_ENTRY( "saveTrainSelection" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_trainInformation:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions
        CPP_CATCH( m_trainInformation.saveTrainSelection( selectionName,
                                                          TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainIDs ),
                                                          sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainInformationCorbaServant::deleteTrainSelection( const char* selectionName, const char* sessionId )
    {
        FUNCTION_ENTRY( "deleteTrainSelection" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_trainInformation:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_trainInformation.deleteTrainSelection( selectionName, sessionId ) );

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainCommonCorba::TrainList* TrainInformationCorbaServant::getTrainSelection( const char* selectionName )
    {
        FUNCTION_ENTRY( "getTrainSelection" );

        OperationModeManager::ensureControlState();//limin

        // Delegate the call to m_trainInformation:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::CommonTypes::TrainIdList result;

        CPP_CATCH( result = m_trainInformation.getTrainSelection( selectionName ) );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainCommonCorba::TrainList( TA_IRS_Bus::CommonTypesConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainInformationCorba::AgentOperationMode* TrainInformationCorbaServant::getAgentOperationMode()
    {
        FUNCTION_ENTRY( "getAgentOperationMode" );

        // Delegate the call to m_operationModeManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::TrainInformationTypes::AgentOperationMode result;

        CPP_CATCH( result = m_operationModeManager.getCurrentOperationMode() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainInformationCorba::AgentOperationMode( TA_IRS_Bus::TrainInformationConversion::convertToCorba( result ) );
    }

} // TA_IRS_App
