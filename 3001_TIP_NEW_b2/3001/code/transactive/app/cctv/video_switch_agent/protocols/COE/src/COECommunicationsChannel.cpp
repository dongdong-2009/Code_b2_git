/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.cpp $
  * @author:  Robert Young
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2016/10/03 18:36:49 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/Timer.h"

#include <sstream>
#include <iomanip>

// TODO Complete thread locking of queues.
namespace
{
	template<class T> std::string getPacketLogFormatStr(const T& packet)
	{
        FUNCTION_ENTRY("getPacketLogFormatStr");
		std::stringstream ss;
        typename T::const_iterator it = packet.begin();
		for(; it != packet.end(); ++it)
		{
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned short>(*it);
			ss << '.';
		}
		FUNCTION_EXIT;
		return ss.str();
	}
}

namespace TA_IRS_App
{
    const unsigned long COECommunicationsChannel::WAIT_FOR_OBSERVED_SOCKET_CONNECT_ATTEMPT_INTERVAL = 10;
    const unsigned long COECommunicationsChannel::DELAY_AFTER_SUCCESSFUL_SOCKET_CONNECT = 1000;

    //
    // COECommunicationsChannel
    //
    COECommunicationsChannel::COECommunicationsChannel( unsigned int commsChannelIndex, const std::string& address, const std::string& port, unsigned long connectTimeout, unsigned long maxResponseTime, 
        unsigned long pollInterval, unsigned long maxSummaryPollingResponseTime, unsigned long summaryPollingInterval, COECommunicationsHandler& commsHandler, ReceivedDataProcessor& receivedDataProcessor, 
		std::string entityName, unsigned long entityKey, unsigned long locationKey, unsigned long entityType, unsigned long subSystem, std::string assetName )
        : m_socket( 0 ), 
          m_doTerminate( false ),
          m_isConnected( false ),
          m_socketHasAttemptedConnection( false ),
          m_previousWriteFailed( false ),
          m_commsChannelIndex( commsChannelIndex ),
          m_address( address ),
          m_port( port ),
          m_connectTimeout( connectTimeout ),
          m_maxResponseTime( maxResponseTime ),
          m_pollInterval( pollInterval ),
		  m_maxSummaryPollingResponseTime( maxSummaryPollingResponseTime ),
		  m_summaryPollingInterval( summaryPollingInterval ),
		  m_commsHandler( commsHandler ),
          m_receivedDataProcessor( receivedDataProcessor ),
          m_commandInProgress( 0 ),
          m_currentReceivedPacketLength( 0 ),
          m_channelUsageLock(),
          m_threadStateLock(),
          m_commandInProgressCondition(),
		  m_entityKey(entityKey),
		  m_entityName(entityName),
		  m_locationKey(locationKey),
		  m_entityTypeKey(entityType),
		  m_subsystemKey(subSystem),
		  m_assetName(assetName),
		  m_heartBeatCommandFailureCount(0)
    {
        FUNCTION_ENTRY( "Constructor" );

        // Nothing else to initialise.

        FUNCTION_EXIT;
    }


    //
    // ~COECommunicationsChannel
    //
    COECommunicationsChannel::~COECommunicationsChannel()
    {
        FUNCTION_ENTRY( "Destructor" );

        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
            // If not done already clean up this thread and the socket.
            deactivateChannel();
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }

        FUNCTION_EXIT;
    }


    //
    // sendCommand
    //
    void COECommunicationsChannel::sendCommand( IAgentRequestCommand& command, unsigned long maxResponseTime )
    {
        FUNCTION_ENTRY( "sendCommand" );
		
		if ( maxResponseTime == 0 )
		{
			maxResponseTime = m_maxResponseTime;
		}

        TA_Base_Core::ThreadGuard channelUsageLock( m_channelUsageLock );

        TA_Base_Core::ThreadGuard commandInProgressGuard( m_commandInProgressCondition.getLinkedThreadLockable() );

        if ( false == m_isConnected )
        {
            command.signalCouldNotBeSent();
            FUNCTION_EXIT;
            return;
        }
        else
        {
			TA_ASSERT(m_commandInProgress == 0, "No command should be in progressing");

            m_commandInProgress = &command;
            std::string dataToSend( command.getDataToSend() );
            m_socket->write( dataToSend.c_str(), dataToSend.size() );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Send out command:[%s]",getPacketLogFormatStr(dataToSend).c_str());
        }

        //
        // Check if condition timed-out while waiting for command response or send failure.
        // NOTE: timedWait intrinsically releases the linked lockable just before the wait,
        // intrinsically re-acquires it immediately afterwards.
        //
        if ( 0 == m_commandInProgressCondition.timedWait( maxResponseTime ) ) 
        {
            // One last check to ensure received data thread didn't acquire lock inbetween
            // the condition time-out and this thread re-acquiring lock.
            if ( 0 != m_commandInProgress )
            {
                m_commandInProgress->signalTimedOut();
                m_commandInProgress = 0;
            }
            
        }
        
        //
        // As a precaution, make sure the socket is disconnected and 
        // destroyed if command wasn't sent due to write failure (note 
        // flag will be reset during helper).
        //
        if ( m_previousWriteFailed )
        {
            ensureSocketDisconnected();
        }

        FUNCTION_EXIT;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDCommunicationsHandler Methods to be overridden
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // connect
    //
    void COECommunicationsChannel::activateChannel()
    {
        FUNCTION_ENTRY( "activateChannel" );
        
        TA_Base_Core::ThreadGuard threadStateLock( m_threadStateLock );

        //
        // Check the thread is not currently running. Else log
        // warning and restart.
        //
        if ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "activateChannel() called while already active. Thread will be restarted." );

            terminateAndWait();
        }

        //
        // Try to connect communications channel and ensure an initial
        // state is reported (before this method returns) by attempting 
        // a polling command.

		// chenlei--
		// move this two function call into the beginning of run() so as that 
		// once the thread is running, 
		// these two processes will be proceeded before the loop in the run().
        //createAndConnectSocket();
        //sendSummaryPollingCommand();
        
        //
        // Now start this thread which will keep sending polling commands to 
        // maintain state.
        //
        start();

        FUNCTION_EXIT;
    }


    //
    // disconnect
    //
    void COECommunicationsChannel::deactivateChannel()
    {
        FUNCTION_ENTRY( "deactivateChannel" );

        TA_Base_Core::ThreadGuard threadStateLock( m_threadStateLock );

        // Clean up this thread and the socket if running.
        if ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState() )
        {
            // Note, the run() method will clean up the socket after exiting 
            // main loop.
            terminateAndWait();
        }

        // Ensure flag reset so thread will run correctly again if reactivated.
        m_doTerminate = false;

        FUNCTION_EXIT;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Thread Methods
    //
    ///////////////////////////////////////////////////////////////////////
    

    //
    // run
    //
	void COECommunicationsChannel::run()
	{
        FUNCTION_ENTRY( "run" );

        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this method.
        try
        {

			//chenlei++
			//moved from the function ActivateChannel()
			createAndConnectSocket();
			//sendSummaryPollingCommand();
			sendHeartbeatCommand();
			
			int nCycle = m_summaryPollingInterval / m_pollInterval;
			int i = 1;
		    while( false == m_doTerminate )
		    {
                try //if something roots up we dont want to give up and have the thread die
				{
					//
					// Sleep at start of loop as we already attempted an initial poll 
					// command during activateChannel()
					// 
					sleep( m_pollInterval );

					ensureSocketHealthy();
                
					sendHeartbeatCommand();
					
					if( i >= nCycle )
					{
						sendSummaryPollingCommand();
						i=1;
					}
					else
						i++;
				}
				catch( const TA_Base_Core::TransactiveException& e )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
				}
				catch( ... )
				{
					 LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
				}

		    }   // while( !m_isTerminated )

            //
            // Channel Thread has been terminated - terminate observed socket thread to ensure disconnection,
            // then set to "0" it so will be recreated next time the channel is activated.
            //
            ensureSocketDisconnected();
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
             LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }

        FUNCTION_EXIT;
	}


    //
    // terminate
    //
	void COECommunicationsChannel::terminate()
	{
		m_doTerminate = true;
	}
	
    
    ///////////////////////////////////////////////////////////////////////
    //
    // ITcpSocketObserver Methods
    //
    ///////////////////////////////////////////////////////////////////////


	//
    // processReceivedData
    //
    void COECommunicationsChannel::processReceivedData( std::vector< unsigned char >& pData, TA_Base_Core::ISocket* pSocket )
	{
        FUNCTION_ENTRY( "processReceivedData" );

        while ( false == pData.empty() )
        {
            m_currentReceivedPacket.push_back( *( pData.begin() ) );
            pData.erase( pData.begin() );
            if ( 0 == m_currentReceivedPacketLength )
            {
                // If we have packet heading length and command bytes, validate
                // them.  
                if ( m_currentReceivedPacket.size() >= ( TA_COE::NUM_LENGTH_BYTES + TA_COE::PACKET_HEADER_LENGTH ) )
                {
                    m_currentReceivedPacketLength = TA_COE::getPacketLength( 
                        m_currentReceivedPacket[ TA_COE::LSB_LENGTH_BYTE ], m_currentReceivedPacket[ TA_COE::MSB_LENGTH_BYTE ] );
                    bool isValidPacketHeader = m_receivedDataProcessor.validatePacketHeader( m_currentReceivedPacketLength, 
                        m_currentReceivedPacket[ TA_COE::COMMAND_BYTE ], m_currentReceivedPacket[ TA_COE::ERROR_BYTE ] );
                    if ( false == isValidPacketHeader )
                    {
                        invalidPacketHeaderReceived();
                    }
                }
            }

            //
            // IMPORTANT - don't replace with "else" here as we want to pass through on the cycle
            // that reaches minimum packet length.
            //

            if ( 0 != m_currentReceivedPacketLength )
            {
                // A valid packet header been has been received.
                if ( m_currentReceivedPacket.size() == ( TA_COE::NUM_LENGTH_BYTES + m_currentReceivedPacketLength ) )
                {
                    validPacketReceived();
                }
            }
        } //  while ( false == pData.empty() )           

        FUNCTION_EXIT;
	}


    //
    // writeFailed
    //
	void COECommunicationsChannel::writeFailed()
	{   
        FUNCTION_ENTRY( "writeFailed" );

        TA_Base_Core::ThreadGuard commandInProgressGuard( m_commandInProgressCondition.getLinkedThreadLockable() );

        //
        // As a precaution, the socket should be cleaned up and recreated in the event
        // a write fails. We can't clean up the observed socket thread from the observed socket
        // thread, so have to set a flag to make sure it will be cleaned up at next appropriate
        // time (i.e. end of SendCommand [if in progress] or during next call to ensureSocketHealthy()).
        //
        m_previousWriteFailed = true;

        if ( 0 != m_commandInProgress )
        {
            m_commandInProgress->signalCouldNotBeSent();
            m_commandInProgress = 0;
            m_commandInProgressCondition.broadcast();
        }

        FUNCTION_EXIT;
	}


    //
    // connectionLost
    //
	bool COECommunicationsChannel::connectionLost()
	{
        FUNCTION_ENTRY( "connectionLost" );

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			 "Connection to the switch has been lost." );
		
		try
		{
			TA_Base_Core::DescriptionParameters dp;
			TA_Base_Core::NameValuePair pair1 ("VideoSwitchAddress", m_address);
			dp.push_back(&pair1);

			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::VisualAlarms::VisualConnectionLost,
			m_entityKey,
			m_entityTypeKey,
			dp,
			m_entityName, 
			m_locationKey, 
			m_subsystemKey,
			m_assetName,
			TA_Base_Core::AlarmConstants::defaultTime );
		}
		catch(...){}
			
		m_isConnected = false;
        m_socketHasAttemptedConnection = true;

        FUNCTION_EXIT;
        return true;
	}


    //
    // connectionEstablished
    //
	void COECommunicationsChannel::connectionEstablished()
	{
        FUNCTION_ENTRY( "connectionEstablished" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			 "Connection to the switch has been established" );

		try
		{
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
				TA_Base_Core::VisualAlarms::VisualConnectionLost, m_entityKey, m_locationKey);
		}
		catch(...){}
			
		m_isConnected = true;
        m_socketHasAttemptedConnection = true;

        FUNCTION_EXIT;
	}


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////
  	

    //
    // createSocket
    //
    void COECommunicationsChannel::createAndConnectSocket()
	{
        FUNCTION_ENTRY( "createAndSocketSocket" ); 

        TA_Base_Core::ThreadGuard channelUsageLock( m_channelUsageLock );

        // Ensure we don't have existing socket already running.
        ensureSocketDisconnected();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Creating socket connection to %s:%s", m_address.c_str(), m_port.c_str() );

        try
        {
            m_socket = std::auto_ptr< TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > >(
                    new TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >( m_address, m_port, UINT_MAX, m_connectTimeout ) );
		    // Register this class as an observer of the socket.
            m_socket->addObserver( *this );
            m_socket->start();
        }
        catch( const TA_Base_Core::TcpSocketException& e )
        {
            // Couldn't even intialise default TcpSocket (something seriously wrong with operating system)
            // Will attempt again on next cycle.
            LOG_EXCEPTION_CATCH( SourceInfo, "TcpSocketException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TcpSocketException", "Failed to create socket connection." );
            m_socketHasAttemptedConnection = true;
        }

        //
        // Wait for Observed Socket thread to notify connectionLost or connectionEstablished
        // (ObservedSocket calls back connectionLost() the first time a connect attempt fails, and
        // calls connectionEstablished() whenever connection succeeds). Just in case the ObservedSocket
        // behaviour changes (or its thread fails) resulting in no callback to connectionLost() or 
        // connectionEstablished(), an added timeout clause is included so this loop can't possibly block
        // for longer than the configured connect timeout.
        //
        TA_Base_Core::Timer connectAttemptTimer; // returns elapsed time in <seconds.milliseconds>
        while ( ( false == m_socketHasAttemptedConnection ) && 
            ( static_cast< unsigned long >( connectAttemptTimer.elapsedTime() * 1000 ) <= m_connectTimeout ) )
        {
            sleep( WAIT_FOR_OBSERVED_SOCKET_CONNECT_ATTEMPT_INTERVAL );
        }

        //
        // If connection succeeded, sleep for arbritrary period to allow socket provider
        // to perform any initialisation before we begin sending commands.
        //
		/*
        if ( m_isConnected )
        {
            sleep( DELAY_AFTER_SUCCESSFUL_SOCKET_CONNECT );
        }
		*/

        FUNCTION_EXIT;
    }


    //
    // ensureSocketDisconnected
    //
	void COECommunicationsChannel::ensureSocketDisconnected()
	{
        FUNCTION_ENTRY( "ensureSocketDisconnected" );

        TA_Base_Core::ThreadGuard channelUsageLock( m_channelUsageLock );

		if ( 0 != m_socket.get() )
        {
            if ( TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != m_socket->getCurrentState() )
            {
                m_socket->terminateAndWait();
            }
            //
            // Assign to 0 so recreated and connected next time channel is activated.
            // Note - auto_ptr will clean up old memory automtically when reassigned, so
            // this is not a memory leak.
            //
            m_socket = std::auto_ptr< TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > >( 0 );
        }
        
        m_isConnected = false;
        m_socketHasAttemptedConnection = false;
        m_previousWriteFailed = false;

        FUNCTION_EXIT;
	}


    //
    // ensureSocketHealthy
    // 
    void COECommunicationsChannel::ensureSocketHealthy()
    {
        FUNCTION_ENTRY( "ensureSocketHealthy" );

        //
        // As a precaution, make sure the socket is disconnected and 
        // destroyed if previous write has failed (note flag will be reset
        // during helper). This is also checked at end of SendCommand, so
        // <m_previousWriteHasFailed> is unlikely to ever equal true here, 
        // but for consistency it is checked.
        //
        if ( m_previousWriteFailed )
        {
            ensureSocketDisconnected();
        }
         
        if ( false == m_isConnected )
        {
            //
            // If observed socket has not yet been successfully created then attempt to create,
            // kick it off, and wait until connection attempted at least once. If it has been created
            // but its thread is no longer active (unexpected behaviour as this observer always returns
            // true on connectionLost() and always starts after creating) recreate also.
            //
            if ( 0 == m_socket.get() ) 
            {
                createAndConnectSocket();
            }
            else
            {
                if ( TA_Base_Core::Thread::THREAD_STATE_RUNNING != m_socket->getCurrentState() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                        "Observed Socket Thread unexpectedly not running. Recreating.");
                    createAndConnectSocket();
                }
            }
        }

        FUNCTION_EXIT;
    }


    //
    // sendSummaryPollingCommand
    //
    void COECommunicationsChannel::sendSummaryPollingCommand()
	{
        FUNCTION_ENTRY( "sendSummaryPollingCommand" );

        std::auto_ptr< IAgentRequestCommand > pollingCommand = 
            std::auto_ptr< IAgentRequestCommand >( TA_COE::createSummaryPollingCommand( m_commsChannelIndex, m_commsHandler ) );

        TA_ASSERT( 0 != pollingCommand.get(), "Creation of new polling command failed!" );

        sendCommand( *pollingCommand.get(), m_maxSummaryPollingResponseTime);
        pollingCommand->waitForCompletion();

        /*if ( IAgentRequestCommand::SUCCEEDED != pollingCommand->getCompletionState() )
        {
            m_commsHandler.processCommunicationsChannelState( m_commsChannelIndex, COECommunicationsHandler::COMMUNICATIONS_FAILURE );
        }*/
        
        FUNCTION_EXIT;
	}


	//
	// sendHeartbeatCommand
	//
	void COECommunicationsChannel::sendHeartbeatCommand()
	{
		FUNCTION_ENTRY( "sendHeartbeatCommand" );

		std::auto_ptr< IAgentRequestCommand > heartbeatCommand = 
			std::auto_ptr< IAgentRequestCommand >( TA_COE::createHeartbeatCommand( m_commsChannelIndex, m_commsHandler ) );

		TA_ASSERT( 0 != heartbeatCommand.get(), "Creation of new heartbeat command failed!" );

		sendCommand( *heartbeatCommand.get() );
		heartbeatCommand->waitForCompletion();

		if ( IAgentRequestCommand::SUCCEEDED != heartbeatCommand->getCompletionState() )
		{
			// DTL-1973
			if(++m_heartBeatCommandFailureCount >= 3)
			{
				m_heartBeatCommandFailureCount = 0;
				m_commsHandler.processCommunicationsChannelState( m_commsChannelIndex, COECommunicationsHandler::COMMUNICATIONS_FAILURE );
			}
		}
		else
		{
			m_heartBeatCommandFailureCount = 0;
			m_commsHandler.processCommunicationsChannelState( m_commsChannelIndex, COECommunicationsHandler::ACTIVE_SWITCH_CHANNEL );
		}

		FUNCTION_EXIT;
	}


    //
    // invalidPacketHeaderPacketReceived
    //
    void COECommunicationsChannel::invalidPacketHeaderReceived()
    {
        FUNCTION_ENTRY( "invalidPacketHeaderPacketReceived" );

        std::stringstream badResponseHeaderMessage;
        badResponseHeaderMessage << "Invalid packet header received:[%s]";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			badResponseHeaderMessage.str().c_str(), getPacketLogFormatStr(m_currentReceivedPacket).c_str());
        LOGMORE( SourceInfo, "Throwing away first byte and reparsing." );
        
        m_currentReceivedPacket.erase( m_currentReceivedPacket.begin() );
        m_currentReceivedPacketLength = 0;        

        FUNCTION_EXIT;
    }


    //
    // validPacketReceived
    //
    void COECommunicationsChannel::validPacketReceived()
    {
        FUNCTION_ENTRY( "validPacketReceived" );

        TA_Base_Core::ThreadGuard commandInProgressGuard( m_commandInProgressCondition.getLinkedThreadLockable() );

        bool isCommandInProgressResponse( false );
        if ( 0 != m_commandInProgress )
        {
			try
			{
                isCommandInProgressResponse = m_commandInProgress->processResponse( m_currentReceivedPacket );
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Unexpected exception catch when processing response:[%s]", getPacketLogFormatStr(m_currentReceivedPacket).c_str());
				isCommandInProgressResponse = true;
			}
            if ( isCommandInProgressResponse )
            {
                m_commandInProgress->signalResponse();
                m_commandInProgress = 0;
                m_commandInProgressCondition.broadcast();
            }
        }
        
        if ( false == isCommandInProgressResponse )
        {
            COECommunicationsHandler::NotificationCommandSharedPtr notificationCommand = COECommunicationsHandler::NotificationCommandSharedPtr(
                m_receivedDataProcessor.createHardwareNotificationCommand( m_currentReceivedPacket ) );
            if ( 0 != notificationCommand.get() )
            {
                // Queue the NotificationCommand to be processed by Comms Handler thread,
                m_commsHandler.submitNotificationCommand( notificationCommand );
            }
            else
            {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Valid packet received, but is an unexpected type, or a response for a request that has already time out:[%s]",
					getPacketLogFormatStr(m_currentReceivedPacket).c_str());
            }
        }

        m_currentReceivedPacket.clear();
        m_currentReceivedPacketLength = 0;

        FUNCTION_EXIT;
    }
} // TA_IRS_App
