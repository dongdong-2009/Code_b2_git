/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/TrainAgentCommunicator.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Singleton to provide access to Agent data and functions
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // defined( WIN32 )

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

#include "app/cctv/video_display_manager/src/TrainAgentCommunicator.h"

#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/exceptions/src/TransActiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "bus/trains/trainagentcorbadef/IDL/src/ICCTVTrainCommandsCorbaDef.h"

using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{
    // Initialise singleton and threadlock Statics
    TrainAgentCommunicator* TrainAgentCommunicator::m_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable TrainAgentCommunicator::m_singletonLock;


    // 
    // getInstance
    // 
    TrainAgentCommunicator& TrainAgentCommunicator::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if( m_theInstance == 0 )
        {
            m_theInstance = new TrainAgentCommunicator();
        }

        FUNCTION_EXIT;
        return *m_theInstance;
    }


    // 
    // removeInstance
    // 
    void TrainAgentCommunicator::removeInstance()
    {
        FUNCTION_ENTRY( "removeInstance" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if( m_theInstance != 0 )
        {
            delete m_theInstance;
            m_theInstance = 0;
        }

        FUNCTION_EXIT;
    }


    // 
    // Constructor
    // 
    TrainAgentCommunicator::TrainAgentCommunicator()
        : m_isTrainAgentNamedObjIntialised( false )
    {
        FUNCTION_ENTRY( "TrainAgentCommunicator" );
        FUNCTION_EXIT;
    }


    bool TrainAgentCommunicator::sendTrainCarrier( CORBA::Octet trainID, bool enable )
    {
        FUNCTION_ENTRY( "sendTrainCarrier" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_lastErrorMessage = "IDS_UE_090083";

        std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
            initialiseTrainAgentNamedObj();
            CORBA_CALL( m_cctvTrainAgentCommands, CCTVCarrier, ( trainID, enable, session.c_str() ) );//limin

            FUNCTION_EXIT;
            return true;
        }
        catch (TA_Base_Bus::ICCTVTrainCommandsCorbaDef::TrainCCTVConflictException& e)
        {
            m_lastErrorMessage = e.what;
        }
        catch (TA_Base_Bus::ICCTVTrainCommandsCorbaDef::TrainAccessDeniedException& e)
        {
            m_lastErrorMessage = e.what;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainAgentCommunicator", "unknown exception");
        }

        FUNCTION_EXIT;
        return false;
    }


    bool TrainAgentCommunicator::sendTrainSingleScreen( CORBA::Octet trainID, CORBA::Octet cameraX )
    {
        FUNCTION_ENTRY( "sendTrainSingleScreen" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_lastErrorMessage = "IDS_UE_090084";

        std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
            initialiseTrainAgentNamedObj();
            CORBA_CALL( m_cctvTrainAgentCommands, CCTVSingleScreen, ( trainID, cameraX, session.c_str() ) );//limin

            FUNCTION_EXIT;
            return true;
        }
        catch (TA_Base_Bus::ICCTVTrainCommandsCorbaDef::TrainAccessDeniedException& e)
        {
            m_lastErrorMessage = e.what;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainAgentCommunicator", "unknown exception");
        }

        FUNCTION_EXIT;
        return false;
    }


    bool TrainAgentCommunicator::sendTrainQuadScreen( CORBA::Octet trainID, CORBA::Octet cameraW, CORBA::Octet cameraX, CORBA::Octet cameraY, CORBA::Octet cameraZ )
    {
        FUNCTION_ENTRY( "sendTrainQuadScreen" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_lastErrorMessage = "IDS_UE_090085";

        std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
            initialiseTrainAgentNamedObj();
            CORBA_CALL( m_cctvTrainAgentCommands, CCTVQuadScreen, ( trainID, cameraW, cameraX, cameraY, cameraZ, session.c_str() ) );//limin

            FUNCTION_EXIT;
            return true;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainAgentCommunicator", "unknown exception");
        }

        FUNCTION_EXIT;
        return false;
    }


    bool TrainAgentCommunicator::sendTrainSequence( CORBA::Octet trainID, CORBA::Octet sequenceNo )
    {
        FUNCTION_ENTRY( "sendTrainSequence" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_lastErrorMessage = "IDS_UE_090086";

        std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
            initialiseTrainAgentNamedObj();
            CORBA_CALL( m_cctvTrainAgentCommands, CCTVSequence, ( trainID, sequenceNo, 0x1, session.c_str() ) );//limin

            FUNCTION_EXIT;
            return true;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainAgentCommunicator", "unknown exception");
        }

        FUNCTION_EXIT;
        return false;
    }


    bool TrainAgentCommunicator::clearTrainAlarmImage( CORBA::Octet trainID, CORBA::Octet cameraId )
    {
        FUNCTION_ENTRY( "clearTrainAlarmImage" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_lastErrorMessage = "IDS_UE_090086";

        std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
            initialiseTrainAgentNamedObj();
            CORBA_CALL( m_cctvTrainAgentCommands, CCTVRemoveAlarmImage, ( trainID, cameraId, session.c_str() ) );//limin

            FUNCTION_EXIT;
            return true;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainAgentCommunicator", "unknown exception");
        }

        FUNCTION_EXIT;
        return false;
    }


    // 
    // initialiseTrainAgentNamedObj
    // 
    void TrainAgentCommunicator::initialiseTrainAgentNamedObj()
    {
        FUNCTION_ENTRY( "initialiseTrainAgentNamedObj" );

        if ( m_isTrainAgentNamedObjIntialised )
        {
            FUNCTION_EXIT;
            return;
        }

        // Get the Corba name of the agent.

        TA_Base_Core::CorbaNameList agents;
        TA_Base_Core::IConsole* console = 0;
        try
        {
            // Determine the local agent for the physical Location of this Console Session.
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
            console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
            agents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
                TA_Base_Core::TrainAgentEntityData::getStaticType(), console->getLocationName() ,true);
            delete console;
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            delete console;
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException",
                "The Corba name for the Video Switch Agent could not be determined." );
            throw TA_Base_Core::TransactiveException( "The Corba name for the Video Switch Agent could not be determined." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            delete console;
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException",
                "The Corba name for the Video Switch Agent could not be determined." );
            throw TA_Base_Core::TransactiveException( "The Corba name for the Video Switch Agent could not be determined." );
        }

        if ( 1 != agents.size() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "The Corba name for the Video Switch Agent could not be uniquely determined, because there were %d names returned.",
                agents.size() );
            throw TA_Base_Core::TransactiveException( "The Corba name for the Video Switch Agent could not be determined." );
        }

        m_cctvTrainAgentCommands.setCorbaName( agents[ 0 ] );

        m_isTrainAgentNamedObjIntialised = true;

        FUNCTION_EXIT;
    }


 } // namespace TA_IRS_App

