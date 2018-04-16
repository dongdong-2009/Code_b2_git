/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/RadioAgentInterface.h"
#include "app/trains/train_agent/src/RadioSessionData.h"

#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"

#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/trains/train_agent/src/TrainAgentConstants.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"




namespace TA_IRS_App
{

    RadioAgentInterface::RadioAgentInterface( ITrainAgentConfiguration& configuration,
                                              ITrainStatusManager& statusManager )
            : m_authLibrary(),
              m_statusManager( statusManager ),
              m_serverSession(),
			  m_radioServerAgentSession(),
              m_radioEntityKey( 0 ) // populated in loadServerSession
    {
        FUNCTION_ENTRY( "RadioAgentInterface" );

        loadServerSession( configuration );
        loadConsoleSessions();

        FUNCTION_EXIT;
    }


    RadioAgentInterface::~RadioAgentInterface()
    {
        FUNCTION_ENTRY( "~RadioAgentInterface" );

        // for each entry in m_consoleKeyToRadioSessionMap
        // delete it, then clear the map

        for ( ConsoleKeyToRadioSessionMap::iterator iter = m_consoleKeyToRadioSessionMap.begin();
              m_consoleKeyToRadioSessionMap.end() != iter; ++iter )
        {
            delete iter->second;
            iter->second = NULL;
        }

        m_consoleKeyToRadioSessionMap.clear();

        FUNCTION_EXIT;
    }


    void RadioAgentInterface::sendData( const std::string& radioId,
                                        const TA_IRS_Bus::TrainMessageData& data )
    {
        FUNCTION_ENTRY( "sendData" );

        // convert the data to the corba type
        TA_Base_Bus::IRadioCorbaDef::MessageDataType corbaData;
        corbaData.length( data.size() );

        unsigned int index = 0;

        for ( TA_IRS_Bus::TrainMessageData::const_iterator iter = data.begin();
              data.end() != iter; ++iter, ++index )
        {
            corbaData[ index ] = *iter;
        }

        try
        {
            // use the server session to send the SDS message
            CORBA_CALL( m_serverSession, sendSDSMessage, ( radioId.c_str(), corbaData ) );//limin
        }
        // catch any exceptions and wrap them in a TrainTransmissionFailureException
        catch ( TA_Base_Bus::IRadioCorbaDef::radioException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioCorbaDef::radioException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( e.what.in() ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( error ) );
        }

        FUNCTION_EXIT;
    }


    std::string RadioAgentInterface::createDynamicGroup( const TA_IRS_Bus::CallTypes::ItsiList& tsiList,
                                                         const std::string& sessionId )
    {
        FUNCTION_ENTRY( "createDynamicGroup" );

        // get the radio session data from the operators session
        // assert that it is not NULL (an exception should have been thrown)

        RadioSessionData* operatorSession = getRadioSessionData( sessionId, TA_IRS_Bus::CommonTypes::DgnaFailed );
        TA_ASSERT( NULL != operatorSession, "This can not be NULL, an exception should be thrown" );


        // convert the types to corba
        TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList corbaList;
        corbaList.length( tsiList.size() );

        unsigned int index = 0;

        for ( TA_IRS_Bus::CallTypes::ItsiList::const_iterator iter = tsiList.begin();
              tsiList.end() != iter; ++iter, ++index )
        {
            corbaList[ index ] = iter->c_str();
        }

        std::string groupTsi;

        try
        {
            // call the radio agent
			// Bohong++
			CORBA_CALL_RETURN( groupTsi, operatorSession->getRadioSessionObject(), makeDynamicGroup, 
				( corbaList, sessionId.c_str() ) );
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Created dynamic group %s", groupTsi.c_str() );
        }
        // catch any exceptions and wrap them in a TrainRadioException
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::DgnaFailed ) );
        }
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::DgnaFailed ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.reason.in(), TA_IRS_Bus::CommonTypes::DgnaFailed ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what(), TA_IRS_Bus::CommonTypes::DgnaFailed ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( error, TA_IRS_Bus::CommonTypes::DgnaFailed ) );
        }

        // return the tsi string given by the radio agent

        FUNCTION_EXIT;
        return groupTsi;
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails RadioAgentInterface::makePecCall( const std::string& trainItsi,
            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makePecCall" );

        // make a call with the appropriate parameters:
        // TrainPecResource
        // single call
        // full duplex
        // emergency priority
        // direct (hook)
        // invisible to GUIs

        FUNCTION_EXIT;
        return makeCall( TA_IRS_Bus::CommonTypes::TrainPecResource,
                         trainItsi,
                         sessionId,
                         TA_IRS_Bus::CallTypes::SingleCall,
                         true,
                         TA_IRS_Bus::CallTypes::DefaultEmergencyPriority,
                         true,
                         true );
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails RadioAgentInterface::makeOccCall( const std::string& trainItsi,
            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeOccCall" );

        // make a call with the appropriate parameters:
        // TrainCallResource
        // single call
        // full duplex
        // normal priority
        // direct (hook)
        // visible to GUIs

        FUNCTION_EXIT;
        return makeCall( TA_IRS_Bus::CommonTypes::TrainCallResource,
                         trainItsi,
                         sessionId,
                         TA_IRS_Bus::CallTypes::SingleCall,
                         true,
                         TA_IRS_Bus::CallTypes::DefaultNormalPriority,
                         true,
                         false );
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails RadioAgentInterface::makeSingleDriverCall( const std::string& trainItsi,
                                                                                              const std::string& sessionId,
                                                                                              TA_IRS_Bus::CallTypes::ECallPriority callPriority )
    {
        FUNCTION_ENTRY( "makeSingleDriverCall" );

        // make a call with the appropriate parameters:
        // TrainCallResource
        // single call
        // full duplex
        // normal priority
        // direct (hook)
        // visible to GUIs

        FUNCTION_EXIT;
        return makeCall( TA_IRS_Bus::CommonTypes::TrainCallResource,
                         trainItsi,
                         sessionId,
                         TA_IRS_Bus::CallTypes::SingleCall,
                         true,
                         callPriority,
                         true,
                         false );
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails RadioAgentInterface::makeGroupDriverCall( const std::string& groupTsi,
            const std::string& sessionId,
            TA_IRS_Bus::CallTypes::ECallType callType,
            TA_IRS_Bus::CallTypes::ECallPriority callPriority )
    {
        FUNCTION_ENTRY( "makeGroupDriverCall" );

        // make a call with the appropriate parameters:
        // TrainCallResource
        // group call (type specified by parameters)
        // if the call type given is a single call, use a GroupUnacknowledged call instead
        // half duplex
        // priority specified by parameters
        // direct (hook)
        // visible to GUIs

        if ( TA_IRS_Bus::CallTypes::SingleCall == callType )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Overriding call type for a Group Driver call to %s from a Single Call to a Group Unacknowledge Call",
                         groupTsi.c_str() );

            callType = TA_IRS_Bus::CallTypes::GroupUnacknowledgeCall;
        }

        FUNCTION_EXIT;

        return makeCall( TA_IRS_Bus::CommonTypes::TrainCallResource,
                         groupTsi,
                         sessionId,
                         callType,
                         false,
                         callPriority,
                         true,
                         false );
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails RadioAgentInterface::makeSinglePaCall( const std::string& trainItsi,
            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeSinglePaCall" );

        // make a call with the appropriate parameters:
        // TrainPaResource
        // single call
        // half duplex
        // normal priority
        // direct (hook)
        // invisible to GUIs

        FUNCTION_EXIT;
        return makeCall( TA_IRS_Bus::CommonTypes::TrainPaResource,
                         trainItsi,
                         sessionId,
                         TA_IRS_Bus::CallTypes::SingleCall,
                         true,
                         TA_IRS_Bus::CallTypes::DefaultNormalPriority,
                         true,
                         true );
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails RadioAgentInterface::makeGroupPaCall( const std::string& groupTsi,
            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeGroupPaCall" );

        // make a call with the appropriate parameters:
        // TrainPaResource
        // group broadcast call
        // half duplex
        // normal priority
        // direct (hook)
        // invisible to GUIs

        FUNCTION_EXIT;
        return makeCall( TA_IRS_Bus::CommonTypes::TrainPaResource,
                         groupTsi,
                         sessionId,
                         TA_IRS_Bus::CallTypes::GroupBroadcastCall,
                         false,
                         TA_IRS_Bus::CallTypes::DefaultNormalPriority,
                         true,
                         true );
    }


    void RadioAgentInterface::endCall( unsigned long callId,
                                       const std::string& sessionId )
    {
        FUNCTION_ENTRY( "endCall" );

        // get the radio session data from the operators session
        // assert that it is not NULL (an exception should have been thrown)
        RadioSessionData* operatorSession = getRadioSessionData( sessionId, TA_IRS_Bus::CommonTypes::CallEndFailed );
        TA_ASSERT( NULL != operatorSession, "This can not be NULL, an exception should be thrown" );

        // get the console ID from the data
        unsigned long consoleKey = operatorSession->getConsoleKey();

        // remove the call from m_statusManager
        m_statusManager.removeRadioCall( consoleKey, callId );

        // end the call on the radio agent (using the RadioSessionData::getRadioSessionObject method)
        try
        {
            // call the radio agent
			//Bohong++
			CORBA_CALL( operatorSession->getRadioSessionObject(), releaseCall, (callId, sessionId.c_str()) );
        }
        // catch any exceptions and log them
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException", e.what.in() );
        }
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::invalidCallIDException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::invalidCallIDException", "" );
        }
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException", e.what.in() );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );
        }

        FUNCTION_EXIT;
    }


    void RadioAgentInterface::makeDataCall( const std::string& trainItsi,
                                            const TA_IRS_Bus::TrainMessageData& data,
                                            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "makeDataCall" );

        // convert the data to the corba type
        TA_Base_Bus::IRadioCorbaDef::CircuitDataType corbaData;
        corbaData.length( data.size() );

		//TA_IRS_Bus::RadioServerAgentIDL::PDSDataType corbaData;
		//corbaData.length( data.size());

        unsigned int index = 0;

        for ( TA_IRS_Bus::TrainMessageData::const_iterator iter = data.begin();
              data.end() != iter; ++iter, ++index )
        {
            corbaData[ index ] = *iter;
        }

		//Lucky
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugTrace,
			"trainITsi         : %s"
			"Library Data size : %d", trainItsi.c_str(),data.size() );

        try
        {
            // use the server session to send the data (sendData)
            // set the useXON parameter to true
            //CORBA_CALL( m_serverSession, sendData, ( trainItsi.c_str(), corbaData, true, sessionId.c_str() ) );//limin

			//CORBA_CALL( m_radioServerAgentSession, sendData,(trainItsi.c_str(), corbaData, true, sessionId.c_str() ) ); //Lucky
        }
        // catch any exceptions and wrap them in a TrainRadioException
       // catch ( TA_Base_Bus::IRadioCorbaDef::dataCallInProgressException& )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::dataCallInProgressException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::RadioServerAgentIDL::dataCallInProgressException", "" );

            TA_THROW( TA_IRS_Bus::TrainRadioException( "Data transfer already in progress", TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        //catch ( TA_Base_Bus::IRadioCorbaDef::radioException& e )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::radioException& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::RadioServerAgentIDL::radioException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.reason.in(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( error, TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }

        FUNCTION_EXIT;
    }

	void RadioAgentInterface::makeDataCall( const std::string& trainItsi, unsigned long trainID, const TA_IRS_Bus::TrainMessageData& data, const std::string& sessionId )
	{
		FUNCTION_ENTRY( "makeDataCall" );

		// convert the data to the corba type
		//TA_Base_Bus::IRadioCorbaDef::CircuitDataType corbaData;
		//corbaData.length( data.size() );

		TA_IRS_Bus::RadioServerAgentIDL::PDSDataType corbaData;
		corbaData.length( data.size());

		unsigned int index = 0;

		for ( TA_IRS_Bus::TrainMessageData::const_iterator iter = data.begin();
			data.end() != iter; ++iter, ++index )
		{
			corbaData[ index ] = *iter;
		}

		//Lucky
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugTrace,
			"trainID         : %d"
			"Library Data size : %d"
			"trainItsi : %s", trainID,data.size(),trainItsi.c_str() );

		try
		{
			// use the server session to send the data (sendData)
			// set the useXON parameter to true
			//CORBA_CALL( m_serverSession, sendData, ( trainItsi.c_str(), corbaData, true, sessionId.c_str() ) );//limin

			CORBA_CALL( m_radioServerAgentSession, sendData,( trainItsi.c_str() ,trainID, corbaData, true, sessionId.c_str() ) ); //Lucky
		}
		// catch any exceptions and wrap them in a TrainRadioException
		// catch ( TA_Base_Bus::IRadioCorbaDef::dataCallInProgressException& )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::dataCallInProgressException& )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::RadioServerAgentIDL::dataCallInProgressException", "" );

			TA_THROW( TA_IRS_Bus::TrainRadioException( "Data transfer already in progress", TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
		}
		//catch ( TA_Base_Bus::IRadioCorbaDef::radioException& e )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::radioException& e)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::RadioServerAgentIDL::radioException", e.what.in() );

			TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
		}
		catch ( TA_Base_Core::OperationModeException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

			TA_THROW( TA_IRS_Bus::TrainRadioException( e.reason.in(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
		}
		catch ( TA_Base_Core::TransactiveException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

			TA_THROW( TA_IRS_Bus::TrainRadioException( e.what(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
		}
		catch ( CORBA::Exception& e )
		{
			std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

			TA_THROW( TA_IRS_Bus::TrainRadioException( error, TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
		}

		FUNCTION_EXIT;
	}


    void RadioAgentInterface::cancelDataCall( const std::string& trainItsi,
                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "cancelDataCall" );

		//Lucky
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugTrace,
			"trainITsi : %s", trainItsi.c_str() );

        try
        {
            // use the server session to cancel the data call (cancelSendData)
            //CORBA_CALL( m_serverSession, cancelSendData, ( trainItsi.c_str() ) );//limin
	
			//CORBA_CALL( m_radioServerAgentSession, cancelSendData, ( trainItsi.c_str() ) );

        }
        // catch any exceptions and log them
        //catch ( TA_Base_Bus::IRadioCorbaDef::dataCallNotInProgressException& )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, " TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException", "" );
        }
        //catch ( TA_Base_Bus::IRadioCorbaDef::radioException& e )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::radioException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::RadioServerAgentIDL::radioException", e.what.in() );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );
        }

        FUNCTION_EXIT;
    }

	void RadioAgentInterface::cancelDataCall( unsigned long trainID, const std::string& sessionId )
	{
		FUNCTION_ENTRY( "cancelDataCall" );

		//Lucky
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugTrace,
			"trainID : %d", trainID );

		try
		{
			// use the server session to cancel the data call (cancelSendData)
			//CORBA_CALL( m_serverSession, cancelSendData, ( trainItsi.c_str() ) );//limin

			CORBA_CALL( m_radioServerAgentSession, cancelSendData, ( trainID, sessionId.c_str() ) );

		}
		// catch any exceptions and log them
		//catch ( TA_Base_Bus::IRadioCorbaDef::dataCallNotInProgressException& )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException& )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, " TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException", "" );
		}
		//catch ( TA_Base_Bus::IRadioCorbaDef::radioException& e )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::radioException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::RadioServerAgentIDL::radioException", e.what.in() );
		}
		catch ( TA_Base_Core::OperationModeException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );
		}
		catch ( TA_Base_Core::TransactiveException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
		}
		catch ( CORBA::Exception& e )
		{
			std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );
		}

		FUNCTION_EXIT;
	}

	void RadioAgentInterface::notifyEndOfTtisDownload( const unsigned long& lTrainID, const unsigned short& sStatus )
	{
		FUNCTION_ENTRY( "notifyEndOfTtisDownload" );

		//Lucky
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"trainID : %d, Status : %d ", lTrainID, sStatus );

		try
		{

			CORBA_CALL( m_radioServerAgentSession, notifyEndofTtisDownloadStatus , ( lTrainID, sStatus) );
		}
		// catch any exceptions and log them
		//catch ( TA_Base_Bus::IRadioCorbaDef::dataCallNotInProgressException& )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException& )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, " TA_IRS_Bus::RadioServerAgentIDL::dataCallNotInProgressException", "" );
		}
		//catch ( TA_Base_Bus::IRadioCorbaDef::radioException& e )
		catch ( TA_IRS_Bus::RadioServerAgentIDL::radioException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::RadioServerAgentIDL::radioException", e.what.in() );
		}
		catch ( TA_Base_Core::OperationModeException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );
		}
		catch ( TA_Base_Core::TransactiveException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
		}
		catch ( CORBA::Exception& e )
		{
			std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );
		}

		FUNCTION_EXIT;
	}


    bool RadioAgentInterface::inFallback()
    {
        FUNCTION_ENTRY( "inFallback" );

        bool inFallback = false;

        try
        {
            // calls the inFallback method on the server session
            CORBA_CALL_RETURN( inFallback, m_serverSession, inFallback, () );
        }
        // catches any exceptions and wraps in an AgentCommunicationException
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;

        return inFallback;
    }


    void RadioAgentInterface::resetConnection()
    {
        FUNCTION_ENTRY( "resetConnection" );

        try
        {
            // calls the resetConnection method on the server session
            CORBA_CALL( m_serverSession, resetConnection, () );//limin
        }
        // catches any exceptions and wraps in an AgentCommunicationException
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );
        }

        FUNCTION_EXIT;
    }


    unsigned long RadioAgentInterface::getRadioAgentEntityKey()
    {
        FUNCTION_ENTRY( "getRadioAgentEntityKey" );
        FUNCTION_EXIT;
        return m_radioEntityKey;
    }


    void RadioAgentInterface::loadServerSession( ITrainAgentConfiguration& configuration )
    {
        FUNCTION_ENTRY( "loadServerSession" );

        // get the entity key from the configuration object
        unsigned long locationKey = configuration.getLocationKey();

        // load the RadioEntityData for this location (there should only be one)
        TA_Base_Core::IEntityDataList entityList =
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
                TA_Base_Core::RadioEntityData::getStaticType(),
                locationKey );

        // check the number of records
        if ( entityList.size() != 1 )
        {
            for ( TA_Base_Core::IEntityDataList::iterator iter = entityList.begin();
                  entityList.end() != iter; ++iter )
            {
                delete *iter;
                *iter = NULL;
            }

            std::ostringstream error;

            error << "Incorrect configuration, there are "
            << entityList.size()
            << " radio entities configured at this location";

            TA_THROW( TA_Base_Core::FatalConfigurationException( error.str() ) );
        }

        TA_Base_Core::RadioEntityDataPtr radioEntity;

        radioEntity.reset( dynamic_cast< TA_Base_Core::RadioEntityData* >( entityList[0] ) );
        TA_ASSERT( NULL != radioEntity.get(), "Invalid radio entity object" );

        // from this, populate m_serverSession with the loaded corba name
        // also populate the radio entity key from this data
        m_radioEntityKey = radioEntity->getKey();

        m_serverSession.setCorbaName( radioEntity->getName(), radioEntity->getAgentName() );
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugTrace,
			"locationKey: %u, EntityKey: %u getName : %s  , getAgentName : %s",locationKey, m_radioEntityKey, radioEntity->getName().c_str(), radioEntity->getAgentName().c_str() );


		try
		{
			TA_Base_Core::CorbaNameList radioAgentCorbaNameList = 
				TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( TrainAgentConstants::RADIO_SERVER_COMMAND_TYPE, locationKey,true);

			TA_ASSERT(radioAgentCorbaNameList.size() == 1,"No or more than one Radio entity found in OCC");

			m_radioServerAgentSession.setCorbaName(radioAgentCorbaNameList[0]);
		}
		catch (const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
		}
		catch (const TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "...", 
				"Exception caught while attempting to resolve the RadioAgent.");
		}  

        FUNCTION_EXIT;
    }


    void RadioAgentInterface::loadConsoleSessions()
    {
        FUNCTION_ENTRY( "loadConsoleSessions" );

        // Load all RadioSession entities from the database.
        TA_Base_Core::IEntityDataList entityList =
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(
                TA_Base_Core::RadioSessionEntityData::getStaticType() );

        // for each entity

        for ( TA_Base_Core::IEntityDataList::iterator iter = entityList.begin();
              entityList.end() != iter; ++iter )
        {
            TA_Base_Core::RadioSessionEntityDataPtr radioSessionEntity;
            radioSessionEntity.reset( dynamic_cast< TA_Base_Core::RadioSessionEntityData* >( *iter ) );

            TA_ASSERT( NULL != radioSessionEntity.get(), "Invalid radio entity object" );

            // get the console key, and if it is 0 skip this entity
            if ( 0 == radioSessionEntity->getAssociatedConsoleKey() )
            {
                continue;
            }

            // otherwise create a RadioSessionData object
            RadioSessionData* sessionInfo = new RadioSessionData( radioSessionEntity );

            // insert the new object into the m_consoleKeyToRadioSessionMap
            m_consoleKeyToRadioSessionMap.insert(
                ConsoleKeyToRadioSessionMap::value_type( sessionInfo->getConsoleKey(), sessionInfo ) );
        }

        FUNCTION_EXIT;
    }


    RadioSessionData* RadioAgentInterface::getRadioSessionData( const std::string& operatorSessionId,
                                                                TA_IRS_Bus::CommonTypes::ERadioErrorType failureType )
    {
        FUNCTION_ENTRY( "getRadioSessionData" );

        unsigned long consoleKey = 0;

        // Using the authentication library, get the session information for the given session ID
        // get the workstation ID from the session information
        try
        {
            TA_Base_Bus::SessionInfo sessionDetails = m_authLibrary.getSessionInfo(
                                                          operatorSessionId,
                                                          TA_Base_Core::SessionAccessFactory::getSuperSessionId() );

            consoleKey = sessionDetails.WorkstationId;
        }
        catch ( TA_Base_Core::AuthenticationAgentException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AuthenticationAgentException", e.what() );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );
        }

        // if this fails, fall back to getting the console from the session using ConsoleAccessFactory
        if ( 0 == consoleKey )
        {
            try
            {
                TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( operatorSessionId );

                consoleKey = console->getKey();

                delete console;
                console = NULL;
            }
            catch ( TA_Base_Core::TransactiveException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
            }
        }

        // if the details are still not found, throw a TrainRadioException
        if ( 0 == consoleKey )
        {
            TA_THROW( TA_IRS_Bus::TrainRadioException( "Console not found for operator session", failureType ) );
        }

        // if the details are found, look up the console key in m_consoleKeyToRadioSessionMap
        ConsoleKeyToRadioSessionMap::iterator findIter = m_consoleKeyToRadioSessionMap.find( consoleKey );

        // if it is found, return the RadioSessionData object
        // otherwise throw a TrainRadioException
        if ( m_consoleKeyToRadioSessionMap.end() == findIter )
        {
            TA_THROW( TA_IRS_Bus::TrainRadioException( "Radio Session not found for console", failureType ) );
        }

        FUNCTION_EXIT;

        return findIter->second;
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails RadioAgentInterface::makeCall( TA_IRS_Bus::CommonTypes::ETrainResource resource,
                                                                                  const std::string& tsi,
                                                                                  const std::string& sessionId,
                                                                                  TA_IRS_Bus::CallTypes::ECallType callType,
                                                                                  bool fullDuplex,
                                                                                  TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                                  bool hook,
                                                                                  bool hideFromGui )
    {
        FUNCTION_ENTRY( "makeCall" );

        // get the radio session data from the operators session
        // assert that it is not NULL (an exception should have been thrown)
        RadioSessionData* operatorSession = getRadioSessionData( sessionId, TA_IRS_Bus::CommonTypes::CallSetupFailed );
        TA_ASSERT( NULL != operatorSession, "This can not be NULL, an exception should be thrown" );

        TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails;
        callDetails.consoleId = operatorSession->getConsoleKey();
        callDetails.callId = 0;
        callDetails.sessionId = sessionId;
        callDetails.calledTsi = tsi;
        callDetails.resourceType = resource;

        try
        {
            // call the radio agent to make the call
			// Bohong++
			CORBA_CALL_RETURN( callDetails.callId, operatorSession->getRadioSessionObject(), makeCall, 
					(
						tsi.c_str(),
						TA_IRS_Bus::CallTypesConversion::convertToCorba( callType ),
						fullDuplex,
						TA_IRS_Bus::CallTypesConversion::convertToCorba( callPriority ),
						hook,
						sessionId.c_str(),
						hideFromGui,
						operatorSession->getLocationKey() 
					) 
				 );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Made call %d to %s", callDetails.callId, tsi.c_str() );
        }
        // catch any exceptions and wrap them in a TrainRadioException
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.reason.in(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what(), TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( error, TA_IRS_Bus::CommonTypes::CallSetupFailed ) );
        }

        // this only needs to be in a separate try catch block to pass the correct error back
        // if one of the operations fail
        try
        {
            // select the call
            operatorSession->getRadioSessionObject()->selectCall( callDetails.callId, sessionId.c_str() );
        }
        // catch any exceptions and wrap them in a TrainRadioException
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::CallSelectFailed ) );
        }
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::invalidCallIDException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::invalidCallIDException", "" );

            TA_THROW( TA_IRS_Bus::TrainRadioException( "Error selecting call", TA_IRS_Bus::CommonTypes::CallSelectFailed ) );
        }
        catch ( TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException", e.what.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what.in(), TA_IRS_Bus::CommonTypes::CallSelectFailed ) );
        }
        catch ( TA_Base_Core::OperationModeException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.reason.in(), TA_IRS_Bus::CommonTypes::CallSelectFailed ) );
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( e.what(), TA_IRS_Bus::CommonTypes::CallSelectFailed ) );
        }
        catch ( CORBA::Exception& e )
        {
            std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );

            TA_THROW( TA_IRS_Bus::TrainRadioException( error, TA_IRS_Bus::CommonTypes::CallSelectFailed ) );
        }

        // if the call was successful, create a RadioCallDetails struct, and add it to m_statusManager
        m_statusManager.addRadioCall( callDetails );

        FUNCTION_EXIT;
        return callDetails;
    }

} // TA_IRS_App
