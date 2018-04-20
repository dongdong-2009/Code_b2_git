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

#include "app/trains/train_agent/src/ITimsTisManager.h"
#include "app/trains/train_agent/src/TrainTisCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TisTypesConversion.h"


namespace TA_IRS_App
{

    TrainTisCorbaServant::TrainTisCorbaServant( ITimsTisManager& timsTisManager )
        : m_timsTisManager( timsTisManager )
    {
        FUNCTION_ENTRY( "TrainTisCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainTisObjectName );

        FUNCTION_EXIT;
    }


    TrainTisCorbaServant::~TrainTisCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainTisCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList* TrainTisCorbaServant::getLibraryVersions()
    {
        FUNCTION_ENTRY( "getLibraryVersions" );

        // Delegate the call to m_timsTisManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::TtisTypes::TisLibraryVersionList result;

        CPP_CATCH( result = m_timsTisManager.getLibraryVersions() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList( TA_IRS_Bus::TisTypesConversion::convertToCorba( result ) );
    }


    TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate TrainTisCorbaServant::getLibraryVersionsForTrain( TA_Base_Bus::ITrainCommonCorba::TrainId trainId )
    {
        FUNCTION_ENTRY( "getLibraryVersionsForTrain" );

        // Delegate the call to m_timsTisManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate result;

        CPP_CATCH( result = m_timsTisManager.getLibraryVersionsForTrain( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ) ) );

        FUNCTION_EXIT;
        return TA_IRS_Bus::TisTypesConversion::convertToCorba( result );
    }


    void TrainTisCorbaServant::displayStoredMessage( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                                     const TA_Base_Bus::ITrainTisCorba::TtisPredefinedDisplayParameters& displayParameters,
                                                     CORBA::Long timestamp,
                                                     const char* sessionId )
    {
        FUNCTION_ENTRY( "displayStoredMessage" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsTisManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsTisManager.displayStoredMessage( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trains ),
                                                          TA_IRS_Bus::TisTypesConversion::convertToCpp( displayParameters ),
                                                          timestamp,
                                                          sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainTisCorbaServant::displayFreeTextMessage( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                                       const TA_Base_Bus::ITrainTisCorba::TtisFreeTextDisplayParameters& displayParameters,
                                                       CORBA::Long timestamp,
                                                       const char* sessionId )
    {
        FUNCTION_ENTRY( "displayFreeTextMessage" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsTisManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsTisManager.displayFreeTextMessage( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trains ),
                                                            TA_IRS_Bus::TisTypesConversion::convertToCpp( displayParameters ),
                                                            timestamp,
                                                            sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainTisCorbaServant::resetMessage( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                             const TA_Base_Bus::ITrainTisCorba::TtisResetParameters& displayParameters,
                                             CORBA::Long timestamp,
                                             const char* sessionId )
    {
        FUNCTION_ENTRY( "resetMessage" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsTisManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsTisManager.resetMessage( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trains ),
                                                  TA_IRS_Bus::TisTypesConversion::convertToCpp( displayParameters ),
                                                  timestamp,
                                                  sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainTisCorbaServant::libraryDownload( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                                const TA_Base_Bus::ITrainTisCorba::LibraryDataType& libraryData,
                                                CORBA::UShort libraryVersion,
                                                const char* sessionId )
    {
        FUNCTION_ENTRY( "libraryDownload" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsTisManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsTisManager.libraryDownload( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trains ),
                                                     TA_IRS_Bus::TisTypesConversion::convertToCpp( libraryData ),
                                                     libraryVersion,
                                                     sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainTisCorbaServant::scheduleDownload( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                                 const TA_Base_Bus::ITrainTisCorba::LibraryDataType& scheduleData,
                                                 CORBA::UShort scheduleVersion,
                                                 const char* sessionId )
    {
        FUNCTION_ENTRY( "scheduleDownload" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsTisManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsTisManager.scheduleDownload( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trains ),
                                                      TA_IRS_Bus::TisTypesConversion::convertToCpp( scheduleData ),
                                                      scheduleVersion,
                                                      sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainTisCorbaServant::libraryUpgrade( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                               const char* sessionId )
    {
        FUNCTION_ENTRY( "libraryUpgrade" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsTisManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsTisManager.libraryUpgrade( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trains ),
                                                    sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainTisCorbaServant::scheduleUpgrade( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                                const char* sessionId )
    {
        FUNCTION_ENTRY( "scheduleUpgrade" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsTisManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsTisManager.scheduleUpgrade( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trains ),
                                                     sessionId ) );

        FUNCTION_EXIT;
    }


    CORBA::UShort TrainTisCorbaServant::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );

        // Delegate the call to m_timsTisManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CORBA::UShort result;

        CPP_CATCH( result = m_timsTisManager.getFlexibleTimeOutValue() );

        FUNCTION_EXIT;
        return result;
    }

} // TA_IRS_App

