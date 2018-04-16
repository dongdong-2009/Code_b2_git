/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/PasModbusConnection.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2012/06/19 17:59:15 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "ace/Time_Value.h"
#include "ace/OS.h"

#include "bus/modbus_comms/src/ModbusException.h"
#include "app/pa/PAAgent/src/PasModbusConnection.h"

namespace TA_IRS_App
{

const int COMMUNICATION_RETRY_INTERVAL = 50; // millisecond

PasModbusConnection::PasModbusConnection( const std::string& host, const std::string& service ) : 
BusModbusTcpConnection( host, service )
{
}

PasModbusConnection::~PasModbusConnection()
{
}

bool PasModbusConnection::connect()
{
    try
    {
        ensureConnected();
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when calling ModbusTcpConnection::ensureConnected" );
    }

    return isConnected();
}

void PasModbusConnection::disable()
{
    BusModbusTcpConnection::disable();

    {
        TA_Base_Core::ThreadGuard guard( m_socketStreamLock );
        try
        {
            // Close the socket anyway since open state is unknown
            m_sockStream.close();
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "Caught unknown exception when calling ACE_SOCK_Stream::close" );
        }
        m_closed = true;
    }
}

bool PasModbusConnection::isAlive()
{
    if ( !isConnected() )
    {
        return false;
    }

    bool bIsAlive = false;

    try
    {
        TA_Base_Core::ThreadGuard guard( m_socketStreamLock );

        char pAliveBuffer[8] = { 0 };
        // Should enable Non-Blocking before every recv
        m_sockStream.enable( ACE_NONBLOCK );
        ACE_OS::last_error( EWOULDBLOCK );
        int nResult = m_sockStream.recv( pAliveBuffer, 0 );
        int nErrorNo = ACE_OS::last_error();
        m_sockStream.disable( ACE_NONBLOCK );

        if (( 0 == nResult ) && ( EWOULDBLOCK == nErrorNo ))
        {
            // Good connection in Solaris
            bIsAlive = true;
        }
        else if (( -1 == nResult ) && ( EWOULDBLOCK == nErrorNo ))
        {
            // Good connection in Windows
            #ifdef WIN32
            bIsAlive = true;
            #else
            bIsAlive = false;
            #endif
        }
        else if ( ECONNRESET == nErrorNo )
        {
            // Connection reset, must be disconnected
            bIsAlive = false;
        }
        else
        {
            bIsAlive = false;
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Ping with error[%d]: %s, Result %d", nErrorNo, ACE_OS::strerror( nErrorNo ), nResult );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Ping with error[%d]: %s, Result %d", nErrorNo, ACE_OS::strerror( nErrorNo ), nResult );
    }
    catch ( std::exception& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch( ... )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
            "Caught unknown exception when calling ACE_SOCK_Stream::recv_n" );
    }

    if ( !bIsAlive )
    {
        close();
    }

    return bIsAlive;
}


bool PasModbusConnection::readRegisterWithRetry( TA_Base_Bus::DataBlock<unsigned short>& refData, const int nTryTimes )
{
    bool bSuccess = false;

    ACE_Time_Value atmRetryInterval = ACE_Time_Value::zero;
    atmRetryInterval.msec( COMMUNICATION_RETRY_INTERVAL );
    int nRetryLoop = 0;

    while ( !bSuccess )
    {
        try
        {
            readInputRegisters( refData );
            bSuccess = true;
            break;
        }
        catch ( TA_Base_Bus::ModbusException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ModbusException", expWhat.what() );
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception when calling readInputRegisters" );
        }

        ++nRetryLoop;
        if ( nTryTimes <= nRetryLoop )
        {
            bSuccess = false;
            break;
        }

        ACE_OS::sleep( atmRetryInterval );
    }

    return bSuccess;
}

bool PasModbusConnection::writeRegisterWithRetry( const TA_Base_Bus::DataBlock<unsigned short>& refData, const int nTryTimes )
{
    bool bSuccess = false;

    ACE_Time_Value atmRetryInterval = ACE_Time_Value::zero;
    atmRetryInterval.msec( COMMUNICATION_RETRY_INTERVAL );
    int nRetryLoop = 0;

    while ( !bSuccess )
    {
        try
        {
            writeMultipleRegisters( refData );
            bSuccess = true;
            break;
        }
        catch ( TA_Base_Bus::ModbusException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ModbusException", expWhat.what() );
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Caught unknown exception when calling readInputRegisters" );
        }

        ++nRetryLoop;
        if ( nTryTimes <= nRetryLoop )
        {
            bSuccess = false;
            break;
        }

        ACE_OS::sleep( atmRetryInterval );
    }

    return bSuccess;
}

}
