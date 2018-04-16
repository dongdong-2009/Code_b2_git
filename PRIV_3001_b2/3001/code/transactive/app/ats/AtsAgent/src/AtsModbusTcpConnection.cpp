/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsModbusTcpConnection.cpp $
  * @author  A. Ivankovic
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
  *
  */

#include    <sstream>

#include	<iomanip>
#include <ace/Time_Value.h>
#include <ace/INET_Addr.h>
#include <ace/os_include/netinet/os_tcp.h> // ACE_5.6

#include    "app/ats/AtsAgent/src/AtsModbusTcpConnection.h"
#include    "bus/modbus_comms/src/ModbusException.h"

#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include    "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

#define THROW_NOT_SUPPORTED_EXCEPTION() \
TA_THROW( TA_Base_Bus::ModbusException( "The functionality requested is not supported by the Alstom ATS Server.", TA_Base_Bus::ModbusException::ILLEGAL_FUNCTION ) )

namespace TA_IRS_App
{
    const int AtsModbusTcpConnection::DEFAULT_PORT = 502;
    const std::string AtsModbusTcpConnection::DEFAULT_SERVICE( "502" );
    const unsigned int AtsModbusTcpConnection::MBAP_HEADER_SIZE = 7;
    const int AtsModbusTcpConnection::MODBUS_PROTOCOL_ID = 0;
    // maximum of 16 bits
    const int AtsModbusTcpConnection::MAX_TRANSACTION_ID = 0xffff;
	const int AtsModbusTcpConnection::HEADER_SIZE = 1;
    const int AtsModbusTcpConnection::FOOTER_SIZE = 2;

    AtsModbusTcpConnection::AtsModbusTcpConnection( const std::string& host,
                                                    const std::string& service,
                                                    long socketTimeout,
                                                    bool tcpNoDelay )
    :
    m_transactionID( 0 ),
    m_socketTimeout( socketTimeout ),
    m_tcpNoDelay( tcpNoDelay ? 1 : 0 ),
    m_closed( true ),
    m_host( host ),
    m_service( service )
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    AtsModbusTcpConnection::~AtsModbusTcpConnection()
    {
        FUNCTION_ENTRY("Destructor");

        m_closed = true;

        TA_Base_Core::ThreadGuard guard( m_socketStreamLock );
		m_sockStream.close();

        FUNCTION_EXIT;
    }


    void AtsModbusTcpConnection::enable()
    {
        FUNCTION_ENTRY("enable");

        ModbusConnection::enable();
        
        FUNCTION_EXIT;
    }


    void AtsModbusTcpConnection::disable()
    {
        FUNCTION_ENTRY("disable");

        ModbusConnection::disable();
        close();

        FUNCTION_EXIT;
    }


    bool AtsModbusTcpConnection::isConnected()
    {
        return (false == m_closed);
    }


    void AtsModbusTcpConnection::close()
    {
        FUNCTION_ENTRY("close");

		m_closed = true;

        FUNCTION_EXIT;
    }


	void AtsModbusTcpConnection::writeRequest( const std::string& request )
    {
        FUNCTION_ENTRY("writeRequest");

        if ( request.size() == 0 )
        {
            TA_THROW( TA_Base_Bus::ModbusException( "No data in the request", TA_Base_Bus::ModbusException::NO_REQUEST_DATA ) );
        }
		if ( false == isEnabled() )
		{
			TA_THROW( TA_Base_Bus::ModbusException( "The connection is not enabled for communications.", TA_Base_Bus::ModbusException::CONNECTION_NOT_ENABLED ) );
		}

		std::string requestPacket;

		// Device Address - set to zero for Alstom ATS Modbus
        requestPacket += (char) 0;

		// Append the data
        requestPacket.append( request );

		// Append the CRC - set to zero for Alstom ATS Modbus
        requestPacket += (char) 0;
        requestPacket += (char) 0;

		drainReadBuffer();

		write( requestPacket.c_str(), requestPacket.size() );

        FUNCTION_EXIT;
    }

	void AtsModbusTcpConnection::readReply( std::string& reply, int expectedSize )
    {
        FUNCTION_ENTRY("readReply");
		if ( false == isEnabled() )
		{
			TA_THROW( TA_Base_Bus::ModbusException( "The connection is not enabled for communications.", TA_Base_Bus::ModbusException::CONNECTION_NOT_ENABLED ) );
		}

		if ( expectedSize >= sizeof(m_replyBuffer) )
		{
		    TA_THROW( TA_Base_Bus::ModbusException( 
				"The expected reply size exceeds internal buffer size allowances",
                TA_Base_Bus::ModbusException::SIZE_EXCEEDS_BUFFER) );
		}

		unsigned int bytesRead = 0;

        // Add an allowance for the device address and the CRC.
		expectedSize += sizeof(char) + sizeof(WORD);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Expecting %d bytes in the reply.", expectedSize);

		bytesRead = read( m_replyBuffer, expectedSize );

        if ( 0 == bytesRead )
        {
			TA_THROW( TA_Base_Bus::ModbusException( "Reply timed out", TA_Base_Bus::ModbusException::REPLY_TIMEOUT ) );
        }
        else if ( bytesRead <= HEADER_SIZE + FOOTER_SIZE )
        {
			TA_THROW( TA_Base_Bus::ModbusException( "Reply packet too short.", TA_Base_Bus::ModbusException::REPLY_PACKET_TOO_SHORT ) );
        }

        reply.assign( m_replyBuffer + HEADER_SIZE, bytesRead - HEADER_SIZE - FOOTER_SIZE );

		// Check CRC
		int startCrc = bytesRead - FOOTER_SIZE;
        if ( m_replyBuffer[ startCrc ] != 0 || m_replyBuffer[ startCrc + 1 ] )
        {
            TA_THROW( TA_Base_Bus::ModbusException( "The Packet CRC is non-zero", TA_Base_Bus::ModbusException::CRC_CALC_ERROR ) );
        }

		drainReadBuffer();
        FUNCTION_EXIT;
    }

	void AtsModbusTcpConnection::writeHoldingRegister( int address, WORD value )
    {
        THROW_NOT_SUPPORTED_EXCEPTION();
    }


    void AtsModbusTcpConnection::writeHoldingRegister( int address, WORD andMask, WORD orMask )
    {
        THROW_NOT_SUPPORTED_EXCEPTION();
    }


    void AtsModbusTcpConnection::writeHoldingRegisterBit( int address, int bit, bool value )
    {
        THROW_NOT_SUPPORTED_EXCEPTION();
    }

	void AtsModbusTcpConnection::appendByteCount(
        std::string & request, 
        int dataCount, 
        int elementSize )
    {
		// Byte count - set to zero for Alstom ATS Modbus
        request += LSB( 0 );
    }

    void AtsModbusTcpConnection::ensureConnected()
    {
        FUNCTION_ENTRY("ensureConnected");

		if ( true == isConnected() )
		{
			return;
		}

        u_short port_number;
        std::stringstream ssPortNumber;
        ssPortNumber << m_service;
        ssPortNumber >> port_number;
        
        ACE_INET_Addr remote_sap( port_number, m_host.c_str() );
        ACE_Time_Value timeout( m_socketTimeout / 1000,  (m_socketTimeout % 1000) * 1000 );

        TA_Base_Core::ThreadGuard guard( m_socketStreamLock );
        if ( m_sockStream.close() == -1 )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ACE_SOCK_Stream::close() failed before connecting to %s:%s",
                m_host.c_str(), m_service.c_str() );
        }
        
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Connecting to %s:%s...", m_host.c_str(), m_service.c_str() );

        int result = m_sockConnector.connect( m_sockStream, remote_sap, &timeout );
        
        if ( 0 != result )
        {
            if ( m_sockStream.close() == -1 )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "ACE_SOCK_Stream::close() failed while connecting to %s:%s",
                    m_host.c_str(), m_service.c_str() );
            }

            std::ostringstream desc;
            desc << "Failed to connect to " << m_host << ":" << port_number << "; errno: " << errno;
            TA_THROW( TA_Base_Bus::ModbusException( desc.str(), TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Connected to %s:%s", m_host.c_str(), m_service.c_str() );
		m_closed = false;

        result = m_sockStream.set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );
        if (result != 0)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to set TCP_NODELAY to %s socket option for connection to %s:%s.",
                m_tcpNoDelay ? "TRUE" : "FALSE", m_host.c_str(), m_service.c_str() );
        }

        FUNCTION_EXIT;
    }


	int AtsModbusTcpConnection::write( const char* buffer, const unsigned int length )
    {
		FUNCTION_ENTRY("write");

		hexDump( "Writing: ", buffer, length );

        unsigned int totalBytesWritten=0;

		while ( totalBytesWritten < length )
        {
			if ( false == isConnected() )
        	{
				TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a write.", TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
            }			

            ACE_Time_Value timeout( m_socketTimeout / 1000, (m_socketTimeout % 1000) * 1000 );
			
            TA_Base_Core::ThreadReadGuard guard( m_socketStreamLock );
            int result = m_sockStream.send_n( buffer + totalBytesWritten, length - totalBytesWritten, &timeout );
            
            if (result > 0)
			{
				// something was written update the total bytes written
				totalBytesWritten += result;
			}
			else
            {
                close();

                if (result < 0)
                {
                    int errorCode = errno;
                    if ( errorCode == ETIME )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Timeout writing to %s:%s",
                            m_host.c_str(), m_service.c_str() );
                    }
                    else
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to write to %s:%s; Error Code: %d",
                            m_host.c_str(), m_service.c_str(), errorCode);
                    }
                }

                TA_THROW( TA_Base_Bus::ModbusException( "Failed to write to device.", TA_Base_Bus::ModbusException::SOCKET_WRITE_ERROR ) );
            }

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,  "wrote %d of %d bytes", totalBytesWritten, length);
		}

        FUNCTION_EXIT;
        return totalBytesWritten;
    }


	int AtsModbusTcpConnection::read( char* buffer, const unsigned int bufferLength )
    {
		FUNCTION_ENTRY("read");

        unsigned int totalBytesRead = 0;
        size_t bytesRead = 0;
        int result;
        
        while ( totalBytesRead < bufferLength )
        {
			if ( false == isConnected() )
        	{
				TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a read.", TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
            }			
            
            TA_Base_Core::ThreadReadGuard guard( m_socketStreamLock );

            ACE_Time_Value timeout( m_socketTimeout / 1000,  (m_socketTimeout % 1000) * 1000 );
            result = m_sockStream.recv_n( buffer + totalBytesRead, bufferLength - totalBytesRead, &timeout, &bytesRead );
            if ( result > 0 )
            {
                hexDump( "Read: ", buffer, bytesRead );
            }
            else
			{
				// Any error should force a closure of the socket
				close();

				std::stringstream ss;
				ss << "Modbus socket read error (error code = " << errno << ")";
				TA_THROW( TA_Base_Bus::ModbusException( ss.str(), TA_Base_Bus::ModbusException::SOCKET_READ_ERROR ) );
			}
			
			totalBytesRead += bytesRead;

            //
            // peek into the buffer to see if an exception code has occurred.
            if ( (totalBytesRead == 5) && ((buffer[1] & 0x80)==0x80) )
            {
                char exceptionCode = buffer[2];
                TA_THROW( TA_Base_Bus::ModbusException( exceptionCode ) );
            }

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "read %d of %d bytes", totalBytesRead, bufferLength );
        }

		FUNCTION_EXIT;
        return totalBytesRead;
    }
    
    void AtsModbusTcpConnection::drainReadBuffer()
	{
	    int result = 0;
		unsigned int totalBytesRead = 0;
        unsigned int bufferLength = sizeof(m_replyBuffer);
        ACE_Time_Value timeout( 0,  1000 ); // 1 millisecond

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Draining tcp with read buffer of %d", bufferLength );

		if ( !isConnected() )
        {
			TA_THROW( TA_Base_Bus::ModbusException( "Socket not connected when attempting a read.", TA_Base_Bus::ModbusException::CONNECT_ERROR ) );
        }
		// else continue on

		try
		{
			TA_Base_Core::ThreadReadGuard guard(m_socketStreamLock);

			do
			{
				TA_Base_Core::ThreadReadGuard guard( m_socketStreamLock );

                result = m_sockStream.recv( m_replyBuffer + totalBytesRead, bufferLength - totalBytesRead, &timeout);

                /*
                Hereby ignoring the result of this crud, it will be caught some other time
                if ( result > 0 )
                {
                    // bytes were read
                    totalBytesRead += result;
                }
                else if (result < 0)
			    {
			        // error reading
    				std::stringstream ss;
	      			ss << "Modbus socket read error (error code = " << errno << ")";
				    TA_THROW( TA_Base_Bus::ModbusException( ss.str(), TA_Base_Bus::ModbusException::SOCKET_READ_ERROR ) );
				}
			    else
			    {
			         // result is 0, socket empty
			    }
                */
			}
			while (result > 0);
		}
		catch (TA_Base_Bus::ModbusException &e)
		{
			//
			// Any exception should force a closure of the socket
			close();

			throw e;
		}
		catch( std::exception& e )
		{
			//
			// Any exception should force a closure of the socket
			close();

			TA_THROW( TA_Base_Bus::ModbusException( e.what(), TA_Base_Bus::ModbusException::SOCKET_READ_ERROR ) );
		}
		catch ( ... )
		{
			//
			// Any exception should force a closure of the socket
			close();

			TA_THROW( TA_Base_Bus::ModbusException( "Failed to read from device", TA_Base_Bus::ModbusException::SOCKET_READ_ERROR ) );
		}

		if (totalBytesRead != 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                        "%d bytes of unexpected data read from socket",totalBytesRead);
		}
	}

}

