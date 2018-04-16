/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date: 2011-01-21 10:00:00$
  * Last modified by:  $Author: Jian Huang $
  *
  */

#include "app/trains/train_agent/src/ITimsPaManager.h"
#include "app/trains/train_agent/src/TrainPaCorbaServant.h"
#include "app/trains/train_agent/src/OperationModeManager.h"

//#include "bus/managed_agent/src/AgentManager.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PaTypesConversion.h"


namespace TA_IRS_App
{

    TrainPaCorbaServant::TrainPaCorbaServant( ITimsPaManager& timsPaManager )
        : m_timsPaManager( timsPaManager )
    {
        FUNCTION_ENTRY( "TrainPaCorbaServant" );

        // activate the servant, with the given object name
        activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainPaObjectName );

        FUNCTION_EXIT;
    }


    TrainPaCorbaServant::~TrainPaCorbaServant()
    {
        FUNCTION_ENTRY( "~TrainPaCorbaServant" );

        deactivateServant();

        FUNCTION_EXIT;
    }


    CORBA::UShort TrainPaCorbaServant::getCommandTimeout()
    {
        FUNCTION_ENTRY( "getCommandTimeout" );

        // Delegate the call to m_timsPaManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        OperationModeManager::ensureControlState();

        CORBA::UShort result;

        CPP_CATCH( result = m_timsPaManager.getCommandTimeout() );

        FUNCTION_EXIT;
        return result;
    }


    //CORBA::UShort TrainPaCorbaServant::getTrainPaLibraryVersion( TA_Base_Bus::ITrainCommonCorba::TrainId trainId )
    CORBA::ULong TrainPaCorbaServant::getTrainPaLibraryVersion( TA_Base_Bus::ITrainCommonCorba::TrainId trainId )
    {
        FUNCTION_ENTRY( "getTrainPaLibraryVersion" );

        OperationModeManager::ensureControlState();//limin

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        //CORBA::UShort result;
        CORBA::ULong result = 0;

        CPP_CATCH( result = m_timsPaManager.getTrainPaLibraryVersion( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainId ) ) );

        FUNCTION_EXIT;
        return result;
    }


    TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList* TrainPaCorbaServant::getTrainPaLibraryVersionList()
    {
        FUNCTION_ENTRY( "getTrainPaLibraryVersionList" );

        OperationModeManager::ensureControlState();//limin

        // Delegate the call to m_timsPaManager:
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        TA_IRS_Bus::PaTypes::TrainLibraryInfoList result;

        CPP_CATCH( result = m_timsPaManager.getTrainPaLibraryVersionList() );

        FUNCTION_EXIT;
        return new TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList( TA_IRS_Bus::PaTypesConversion::convertToCorba( result ) );
    }


    void TrainPaCorbaServant::preRecordedAnnouncement( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                                                       TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                                       //CORBA::Octet messageNumber,
                                                       CORBA::UShort messageNumber,
                                                       //CORBA::UShort libraryVersion,
                                                       CORBA::ULong libraryVersion,
                                                       CORBA::Boolean override,
                                                       const char* sessionId,
													   TA_Base_Bus::ITrainPaCorba::EDvaMsgPriority messagePriority)
    {
        FUNCTION_ENTRY( "preRecordedAnnouncement" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPaManager.preRecordedAnnouncement( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainIdList ),
                                                            TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementId ),
                                                            messageNumber,
                                                            libraryVersion,
                                                            override,
                                                            sessionId,
															messagePriority) );

        FUNCTION_EXIT;
    }


    void TrainPaCorbaServant::cyclicAnnouncement( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                                                  TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                                  //CORBA::Octet messageNumber,
                                                  CORBA::UShort messageNumber,
                                                  //CORBA::UShort libraryVersion,
                                                  CORBA::ULong libraryVersion,
                                                  const TA_Base_Bus::ITrainPaCorba::MessageSchedule& announcementPeriod,
                                                  CORBA::Boolean override,
                                                  const char* sessionId )
    {
        FUNCTION_ENTRY( "cyclicAnnouncement" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPaManager.cyclicAnnouncement( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainIdList ),
                                                       TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementId ),
                                                       messageNumber,
                                                       libraryVersion,
                                                       TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementPeriod ),
                                                       override,
                                                       sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainPaCorbaServant::setupLiveAnnouncement( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                                                     TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                                     CORBA::Boolean override,
                                                     const char* staticGroup,
                                                     const char* sessionId )
    {
        FUNCTION_ENTRY( "setupLiveAnnouncement" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPaManager.setupLiveAnnouncement( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainIdList ),
                                                          TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementId ),
                                                          override,
                                                          staticGroup,
                                                          sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainPaCorbaServant::beginLiveAnnouncement( TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                                     const char* sessionId )
    {
        FUNCTION_ENTRY( "beginLiveAnnouncement" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPaManager.beginLiveAnnouncement( TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementId ),
                                                          sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainPaCorbaServant::continueLiveAnnouncement( TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                                        const char* sessionId )
    {
        FUNCTION_ENTRY( "continueLiveAnnouncement" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPaManager.continueLiveAnnouncement( TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementId ),
                                                             sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainPaCorbaServant::endLiveAnnouncement( TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                                   const char* sessionId )
    {
        FUNCTION_ENTRY( "endLiveAnnouncement" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPaManager.endLiveAnnouncement( TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementId ),
                                                        sessionId ) );

        FUNCTION_EXIT;
    }


    void TrainPaCorbaServant::endBroadcast( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIdList,
                                            TA_Base_Bus::ITrainPaCorba::AnnouncementId announcementId,
                                            const char* sessionId )
    {
        FUNCTION_ENTRY( "endBroadcast" );

        // First verify the operation state using AgentManager::ensureControlState()
        OperationModeManager::ensureControlState();

        // Delegate the call to m_timsPaManager:
        // Do parameter conversion using the appropriate convertToCpp methods
        // call the cpp method, catching and wrapping any C++ exceptions in CORBA exceptions

        CPP_CATCH( m_timsPaManager.endBroadcast( TA_IRS_Bus::CommonTypesConversion::convertToCpp( trainIdList ),
                                                 TA_IRS_Bus::PaTypesConversion::convertToCpp( announcementId ),
                                                 sessionId ) );

        FUNCTION_EXIT;
    }


} // TA_IRS_App
