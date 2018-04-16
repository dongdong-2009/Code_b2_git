/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: 
  * @author:  
  * @version: $Revision:
  *
  * Last modification: $DateTime: 
  * Last modified by:  $Author: 
  *
  * This is the socket class that will manage the connection 
  * to the H/W server.  This class works with a observer pattern, 
  * running on its own thread.  It will notify the observer when  
  * the connection is lost or established as well as when there
  * is incoming data.
  */

#include "ace/OS.h"
#include "ace/os_include/netinet/os_tcp.h"

#include <iostream>
#include <string>
#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/ISocketConnectionObserver.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/RadioSDSSocket.h"

const short RadioSDSSocket::CONNECTIION_RETRY_TIMER_ID = 100;
const short RadioSDSSocket::CONNECTIION_RETRY_PERIOD_IN_SEC = 1;

const short RadioSDSSocket::POLL_READ_TIMER_ID = 200;
const short RadioSDSSocket::POLL_READ_TIMER_PERIOD_IN_SEC = 1;

const short RadioSDSSocket::MAX_SLEEP_TIME_IN_MSEC = 100;
const int   RadioSDSSocket::MAX_RECEIVE_BUFFER = 51200; //~50kbytes

RadioSDSSocket::RadioSDSSocket( const std::string& host,
                      const std::string& service,
                      ISocketConnectionObserver& observer,
                      long socketTimeout,
                      bool tcpNoDelay)
    : m_toTerminate(false),
      m_isConnected(false),
      m_isPrevConnected(false),
      m_host(host),
      m_service(service),
      m_tcpNoDelay(tcpNoDelay ? 1 : 0),
      m_socketTimeout( socketTimeout / 1000,  (socketTimeout % 1000) * 1000 ),
      m_observer(observer)
{
    m_internalDataBuffer = new char[MAX_RECEIVE_BUFFER];
	memset(m_internalDataBuffer, 0, MAX_RECEIVE_BUFFER);
}

RadioSDSSocket::~RadioSDSSocket()
{
    delete[] m_internalDataBuffer;
}

void RadioSDSSocket::run()
{
    connect();

    while(m_toTerminate == false)
    {
        if(m_isConnected == true)
        {
            // Check if there are things to write 
            // If there is, send out to server first
            {
                //TA_Base_Core::ThreadGuard guard(m_writeBufferLock);  //move lock into sendData
                //if(m_writeBuffer.size() != 0)
                //{
                    sendData();
                //}
            }

            // Check if there are information from remote party
            getIncomingData();
            if(m_isConnected == true && m_readBuffer.size() != 0)  // Socket is still connected
            { 
                notifyObserverOfIncomingData();
            }
			if (m_isConnected == false)
			{
				notifyConnectionLost();
			}
        }
        else // try to connect until success
        {
            connect();
        }
        
        Thread::sleep(MAX_SLEEP_TIME_IN_MSEC);
    }
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"Stop thread, IP address %s, port number is %s", m_host.c_str(), m_service.c_str());
}

void RadioSDSSocket::terminate()
{
    m_toTerminate = true;
    close();
}

bool RadioSDSSocket::isConnected()
{
    return m_isConnected;
}

void RadioSDSSocket::connect()
{
	TA_Base_Core::ThreadGuard guard(m_socketLock);
    if(m_isConnected == true)
    {
        return;
    }

    u_short port_number;
    std::stringstream ssPortNumber;
    ssPortNumber << m_service;
    ssPortNumber >> port_number;
    
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		"Contructing the ACE_INET_Addr with IP address %s, port number is %s", m_host.c_str(), m_service.c_str());
	
	ACE_INET_Addr m_remoteAddress( port_number, m_host.c_str() );
    ACE_Time_Value timeout( m_socketTimeout );

    close();
   
    // Initiate timeout - nonblocking connection with server.
    int status = m_connector.connect (m_clientStream, m_remoteAddress, &m_socketTimeout);
    int error = errno;

    if (status == -1/* && errno != EISCONN*/)
    {
        m_isConnected = false;
        if(m_isPrevConnected != m_isConnected)
        {
            notifyConnectionLost();
            m_isPrevConnected = m_isConnected;
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Failed to connect to %s:%d. Error[%d][%s]", 
			m_remoteAddress.get_host_name(), m_remoteAddress.get_port_number(),
			error, ACE_OS::strerror(error) );
    }
    else
    {
        m_isConnected = true;

        m_clientStream.enable (ACE_NONBLOCK); 
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Connected to %s:%d.", m_remoteAddress.get_host_name(), m_remoteAddress.get_port_number());

        status = m_clientStream.set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );
        if (status != 0)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to set TCP_NODELAY to %s socket option for connection to %s:%s.",
                m_tcpNoDelay ? "TRUE" : "FALSE", m_host.c_str(), m_service.c_str() );
        }

//         if(m_isPrevConnected != m_isConnected)
//         {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"notifyConnectionEstablished, m_isConnected is %d, m_isPrevConnected is %d", m_isConnected, m_isPrevConnected);
			notifyConnectionEstablished();
            m_isPrevConnected = m_isConnected;
//         }
    }
}

void RadioSDSSocket::close()
{	
	{
		TA_Base_Core::ThreadGuard guard(m_socketLock);
		if (m_clientStream.close () == -1)
		{
			int error = errno;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Error closing socket connection to %s:%s, Error:[%d][%s]",
				m_host.c_str(), m_service.c_str(), error, ACE_OS::strerror(error));
		}
		m_isConnected=false;
		m_isPrevConnected=m_isConnected;
	}
	{		
		TA_Base_Core::ThreadGuard guard(m_writeBufferLock);
		 m_writeBuffer.erase( m_writeBuffer.begin(), m_writeBuffer.end());
	}
}

void RadioSDSSocket::sendData()
{		
	{		
		TA_Base_Core::ThreadGuard guard(m_writeBufferLock);
		if(m_writeBuffer.size() == 0) return;
	}
	
	if(m_isConnected == true)
    {         			 
		std::vector<unsigned char> writeBuffer;
		{ 
			TA_Base_Core::ThreadGuard guard(m_writeBufferLock);
			
			writeBuffer.reserve(m_writeBuffer.size());
			writeBuffer.assign(m_writeBuffer.begin(), m_writeBuffer.end());
			m_writeBuffer.erase( m_writeBuffer.begin(), m_writeBuffer.end()); // Clear the buffer only if connection exist
		}		
		std::ostringstream stream;
		for(unsigned int i = 0; i < writeBuffer.size(); i++)
		{
			stream << writeBuffer[i];
		}
		stream << std::ends;
		TA_Base_Core::ThreadGuard guard(m_socketLock);
		int status = m_clientStream.send_n(stream.str().c_str(), writeBuffer.size()/*, &timeout*/); // ss++		 
        if(status <= 0 /*== -1*/)
        {
			int error = errno;
            if(isConnectionLost(error) == true || error == ETIME)
            {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Socket %s:%s send_n error: Native Error Code:[%d][%s]",
					m_host.c_str(), m_service.c_str(), error, ACE_OS::strerror(error));

                m_isPrevConnected = m_isConnected;
                m_isConnected = false;
                notifyObserverOfWriteFailed(writeBuffer);
            }
        }
		 writeBuffer.clear(); // Clear the buffer only if connection exist        
    }
}

void RadioSDSSocket::getIncomingData()
{
	if(m_isConnected == true) // already connected
	{
		size_t byteCount = 0;
		int status = 0;
		size_t bytes_transferred = 0;
		TA_Base_Core::ThreadGuard guard(m_socketLock);
		// must be called before every operation - it seems it doesnt stick
		m_clientStream.enable(ACE_NONBLOCK);

		// also no timeout on read operations - otherwise they block for the timeout period
		status = m_clientStream.recv(m_internalDataBuffer, MAX_RECEIVE_BUFFER);

		if (status > 0)
		{
			bytes_transferred = status;

			for(unsigned int i = 0 ; i < bytes_transferred; i++)
			{
				m_readBuffer.push_back(m_internalDataBuffer[i]);
			}

			// debugging
			if(bytes_transferred > 0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"Socket %s:%s incoming %d bytes of data",
					m_host.c_str(), m_service.c_str(), bytes_transferred);
			}
		}
		m_isPrevConnected = m_isConnected;
		if(status < 0) // error
		{
			int error = errno;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Socket %s:%s status = %d, Native Error Code:[%d][%s], isConnected:%s", 
				m_host.c_str(), m_service.c_str(), status, error,
				ACE_OS::strerror(error), (m_isConnected)?"True":"False");

			m_isConnected = !isConnectionLost(error);

			// only log on error
			if (false == m_isConnected) // (EWOULDBLOCK != error)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Socket %s:%s status = %d, Native Error Code:[%d][%s], isConnected:%s", 
					m_host.c_str(), m_service.c_str(), status, error,
					ACE_OS::strerror(error), (m_isConnected)?"True":"False");
			}
		}
		else if (0 == status) // graceful remote close
		{
			m_isConnected = false;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Socket %s:%s closed normally",
				m_host.c_str(), m_service.c_str() );
		}
	}
}
void RadioSDSSocket::sendPacket(const std::vector<unsigned char>& packets)
{
    TA_Base_Core::ThreadGuard guard(m_writeBufferLock);

    std::vector<unsigned char>::const_iterator iter = packets.begin();
    for(; iter != packets.end(); ++iter)
    {
        m_writeBuffer.push_back(*iter);
    }
}

void RadioSDSSocket::notifyConnectionLost()
{
    m_observer.connectionLost();
}

void RadioSDSSocket::notifyConnectionEstablished()
{
    m_observer.connectionEstablished();
}

void RadioSDSSocket::notifyObserverOfIncomingData()
{
    m_observer.processReceivedData(m_readBuffer);
}

void RadioSDSSocket::notifyObserverOfWriteFailed(const std::vector<unsigned char>& writeBuffer)
{
    m_observer.writeFailed();
}

bool RadioSDSSocket::isConnectionLost(int state)
{
    switch(state)
    {
        case ECONNABORTED:
        case ECONNREFUSED:
        case ECONNRESET:
        case EHOSTDOWN:
        case EHOSTUNREACH:
        case ENETDOWN:
        case ENETUNREACH:
        case ENOTCONN:
        case ETIMEDOUT:
            return true;
    }
    return false;
}

