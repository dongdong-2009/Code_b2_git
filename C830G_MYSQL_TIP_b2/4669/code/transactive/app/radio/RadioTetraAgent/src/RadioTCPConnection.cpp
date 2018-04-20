/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
///////////////////////////////////////////////////////////
//
//  RadioTCPConnection.cpp
//  Implementation of the Class RadioTCPConnection
//  Generated by Enterprise Architect
//  Created on:      29-Oct-2003 17:46:16
//  Original author: Andy Parker
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif


#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/radio/RadioTetraAgent/src/ObservedSocket.h"
#include "app/radio/RadioTetraAgent/src/IRadioConnectionObserver.h"
#include "app/radio/RadioTetraAgent/src/RadioTCPConnection.h"
#include "app/radio/RadioTetraAgent/src/RadioKeepAliveHandlerThread.h"

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/ITcpMethod.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/ITcpEvent.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/radio/RadioTetraAgent/src/RadioAgentConstants.h"

using namespace TA_IRS_App;

using TA_Base_Core::DebugUtil;

RadioTCPConnection::RadioTCPConnection
(   IRadioEventProcessor & eventProcessor,
    IConnectionStatusObserver & observer,
    std::string host,
    std::string service,
    bool isPrimary)
: m_clientSocket(0)
, m_radioEventPocessor(eventProcessor)
, m_observer(observer)
, m_ignoreIncomingData(false)
, m_event(0)
, m_primary(isPrimary)
, m_identifier(host + ":" + service)
, isConnectionLost(false)
, m_RadioTimedEvent(NULL)
, m_keepAliveProcessor(NULL)
, m_terminited(false)
, m_semaphore(0)
{
    FUNCTION_ENTRY("Constructor");

	m_event = m_radioEventPocessor.createCompatibleTcpEvent();
    TA_ASSERT(m_event!=0,"Unable to create empty radio event.");
    
	
    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Creating socket to host: %s, port: %s",host.c_str(),service.c_str());
    m_clientSocket = new ObservedSocket(host,service, *this);
    TA_ASSERT(m_clientSocket!=0,"Unable to create client socket.");

    m_clientSocket->setReconnectInterval(4000);

	m_clientSocket->setPingOption(false);

	m_RadioTimedEvent = new TA_IRS_App::RadioTimedEvent(&m_radioEventPocessor, true);

	
	// change from 15 to 50 : expire timer for keep alive
	m_keepAliveProcessor = new TA_IRS_App::RadioKeepAliveHandlerThread( this, 5, 50 );

	FUNCTION_EXIT;
}


RadioTCPConnection::~RadioTCPConnection()
{
	FUNCTION_ENTRY("Destructor");
	m_clientSocket->terminateAndWait();
	terminateAndWait();
	
	delete m_clientSocket;
	delete m_event;

	delete m_RadioTimedEvent;
	m_RadioTimedEvent = NULL;

	delete m_keepAliveProcessor;
	m_keepAliveProcessor = NULL;

	FUNCTION_EXIT;
}

/**
* Sets the connection to primary or secondary.  If primary messages are passed to observer.  
* If secondary messages are discarded.
* @param    method
*/
void RadioTCPConnection::setIgnoreIncomingData(bool bIgnore)
{
    FUNCTION_ENTRY("setIgnoreIncomingData");

	if ( m_ignoreIncomingData != bIgnore )
	{
		m_ignoreIncomingData = bIgnore;
		
		m_RadioTimedEvent->setCacheEvent(m_ignoreIncomingData);
		
		// clear any partial events
		m_event->clear();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"current state for m_ignoreIncomingData is %s", 
		(m_ignoreIncomingData ? "true" : "false") );
	}

    FUNCTION_EXIT;
}

void RadioTCPConnection::setCheckConnection()
{
	if ( !m_ignoreIncomingData )
	{
		m_keepAliveProcessor->startCheckConnection();
	} 
	else
	{
		m_keepAliveProcessor->stopCheckConnection();
	}
}



/**
* A thread guard must be used to ensure that a complete message is always sent without interruption.
* @param    method
*/
void RadioTCPConnection::Send(const ITcpMethod& method)
{
    FUNCTION_ENTRY("Send");

    // locking is done by the actual socket

    // Log data to be sent
    std::string   hexText = "";
    std::string   consoleText = "";

    const unsigned char* messageData = method.getMessageData();
    const unsigned long dataSize = method.getMessageDataSize();


    // this is a lot of work to do when we're not in debug log level
    if (DebugUtil::DebugDebug <= DebugUtil::getInstance().getLevel())
    {
        method.getLogText(hexText, consoleText);

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
             "Connection %s Sending radio method: %s", m_identifier.c_str(), typeid(method).name());
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
             "Connection %s Radio method data: %s", m_identifier.c_str(), hexText.c_str());
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
             "Connection %s Radio method consoleText: %s", m_identifier.c_str(), consoleText.c_str());
    }

    m_clientSocket->sendData(messageData, dataSize);

    FUNCTION_EXIT;
}

void RadioTCPConnection::run()
        {
	m_clientSocket->start();
            while(!m_terminited)
			{
            SocketData data;
			{
				TA_Base_Core::ThreadGuard guard(socket_data_lock);
				data=socket_data;
#ifdef __WIN32__
			socket_data.erase();
#else
			socket_data.clear();
#endif    
			}
			if(data.size()==0)
			{
				if (!m_terminited) m_semaphore.timedWait( 200 );
				continue;
			}
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Data of %d bytes is being processed now", data.size() );
    SocketData::iterator it=data.begin();
	while (it!=data.end() && !m_terminited)
	{
		// Add to incoming message
		m_event->appendData(*it);

		if (m_event->isComplete())
		{
			RadioEvent * event = dynamic_cast<RadioEvent *>(m_event);

			if ( m_keepAliveProcessor->processEvent( *event ) )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"[KeepAlive] Received radio event: %s", event->getBufferDump().c_str() );
			}
			else if (!m_ignoreIncomingData)
			{
				// Pass the incoming communication to the 
				m_radioEventPocessor.onReceiveEvent(*m_event);
			}
			else
			{
				//get a copy of event 
              //  RadioEvent *cachEvent = new RadioEvent ( *event ) ;
				m_RadioTimedEvent->checkRadioEvent( event);
			}
			m_event->clear();
		}

		// Remove from passed buffer to indicate it was processed.
		//data.erase(data.begin());
		it++;
	}
#ifdef __WIN32__
			data.erase();
#else
			data.clear();
#endif
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Data Buffer cleared as processing is complete.");
            }
        }
        
void RadioTCPConnection::terminate()
{
	m_terminited=true;
	m_semaphore.post();
}

void RadioTCPConnection::processReceivedData( SocketData& data, ObservedSocket* socket )
{
	FUNCTION_ENTRY("processReceivedData");
	TA_Base_Core::ThreadGuard guard(socket_data_lock);
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Data of %d bytes is added to the buffer", data.size() );
	socket_data.insert(socket_data.end(),data.begin(),data.end());
#ifdef __WIN32__
			data.erase();
#else
			data.clear();
#endif
	m_semaphore.post();
	FUNCTION_EXIT;
}


void RadioTCPConnection::writeFailed( const SocketData& writeBuffer, ObservedSocket* socket )
{
	FUNCTION_ENTRY("writeFailed");
	FUNCTION_EXIT;
}


void RadioTCPConnection::connectionLost( ObservedSocket* socket )
{
    FUNCTION_ENTRY("connectionLost");

	if( !isConnectionLost )
	{
		isConnectionLost = true;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"[Link Lost] %s link connection lost detected.", (m_primary ? "primary" : "secondary") );

		// Notify observer
		m_observer.onConnectionFail(m_primary);

		// clear the write buffer - dont want old data being sent on the new connection
		socket->clearWriteBuffer();
	}

    FUNCTION_EXIT;
}

void RadioTCPConnection::connectionLost()
{
	if( !isConnectionLost )
	{
		isConnectionLost = true;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"[Link Lost] connection lost detected. is primary link = %d", m_primary);

		m_clientSocket->clearWriteBuffer();
		m_clientSocket->close(8);

		m_observer.onConnectionFail( m_primary );
	}
}

void RadioTCPConnection::connectionEstablished( ObservedSocket* socket )
{
    FUNCTION_ENTRY("connectionEstablished");

	isConnectionLost = false;
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"[Link Establish] connection establish. is primary link = %d", m_primary);
	
	// Notify observer
	m_observer.onConnectionResumed(m_primary);

    FUNCTION_EXIT;
}


bool RadioTCPConnection::isConnected() const
{
    return m_clientSocket->isConnected();
}

bool RadioTCPConnection::closeSocket()
{
	return m_clientSocket->close( 3 );
}

inline const std::string & RadioTCPConnection::getIdentifier() const
{
    return m_identifier;
}

