/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/PASConnection.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <algorithm>
#include <bitset>
#include <sstream>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/TransactiveException.h"

#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "bus/pa/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/ModbusPacket.h"
#include "app/pa/PAAgent/src/PASConnection.h"



namespace TA_IRS_App
{

PASConnection*                            PASConnection::m_me = 0;
TA_Base_Core::ReEntrantThreadLockable     PASConnection::m_lock;

PASConnection* PASConnection::getInstance()
{
    FUNCTION_ENTRY("getInstance");

    if ( 0 == m_me )
    {
        // Double checking to prevent multiple threads
        // creating multiple instances.
        
        TA_Base_Core::ThreadGuard guard( m_lock );
        
        if ( 0 == m_me )
        {
            m_me = new PASConnection();
        }
    }
    
    return m_me;

    FUNCTION_EXIT;
}

void PASConnection::removeInstance( )
{
    FUNCTION_ENTRY("removeInstance");

    // 
    // Guard this to prevent multiple threads atempting
    // to delete/create simultaneously
    //
    TA_Base_Core::ThreadGuard guard( m_lock );
    if ( m_me != NULL )
    {
        delete m_me;
        m_me = NULL;
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
PASConnection::PASConnection () : ICachedConfigObserver(),
m_pModbusHelper( static_cast<TA_Base_Bus::IModbusHelper*>(0) )
{
    FUNCTION_ENTRY("Constructor");

    createModbusHelper();

    CachedConfig::getInstance()->attachObserver( this, CachedConfig::CONFIG_PRIMARY_FEPC_ADDRESS );
    CachedConfig::getInstance()->attachObserver( this, CachedConfig::CONFIG_SECONDARY_FEPC_ADDRESS );
    CachedConfig::getInstance()->attachObserver( this, CachedConfig::CONFIG_PRIMARY_FEPC_PORT );
    CachedConfig::getInstance()->attachObserver( this, CachedConfig::CONFIG_SECONDARY_FEPC_PORT );
    CachedConfig::getInstance()->attachObserver( this, CachedConfig::CONFIG_SOCKET_TIMEOUT );

    FUNCTION_EXIT;
}


// ExceptionChecked
PASConnection::~PASConnection()
{
    FUNCTION_ENTRY("Destructor");

    CachedConfig::getInstance()->detachObserver( this );

	FUNCTION_EXIT;
}

bool PASConnection::writeToHardwareImmediately( ModbusPacket* pModbusPacket )
    // throw TA_Base_Bus::IPAAgentCorbaDef::FepcConfigNotFound, 
    //       TA_Base_Bus::IPAAgentCorbaDef::FepcCommsFailure,
    //       TA_Base_Core::OperationModeException
{
    FUNCTION_ENTRY("writeToHardwareImmediately");

    if ( 0 == pModbusPacket )
    {
        TA_THROW( TA_Base_Core::TransactiveException("Unexperted NULL pointer") );
    }

    if ( 0 == m_pModbusHelper )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "No modbus helper yet, wait for config update" );
        throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure, "The FEP config is not found" );
    }

    TA_Base_Bus::DataBlock<WORD>& dbwPacket = pModbusPacket->getModbusPacket();

    if ( 0 == dbwPacket.length() )
    {
        return true;
    }

    try
    {
        logDataBlockToBinaryString( __LINE__, dbwPacket );
        m_pModbusHelper->SendData( dbwPacket );
    }
    catch ( TA_Base_Bus::ModbusException& expServer )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ModbusException", expServer.what() );
        throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure, expServer.what() );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
        throw;
    }

	FUNCTION_EXIT;
    return true;
}

// ExceptionChecked
void PASConnection::processCachedConfigUpdate( CachedConfig::ECachedConfigItemKey key )
{
    FUNCTION_ENTRY("processCachedConfigUpdate");

    TA_Base_Core::ThreadGuard guard( m_lock );
    createModbusHelper();

	FUNCTION_EXIT;
}

// ExceptionChecked
void PASConnection::createModbusHelper()
{
    FUNCTION_ENTRY("createModbusHelper");
    
    try
    {
        // boost smart pointer
        m_pModbusHelper = TA_Base_Bus::ModbusHelperFactory::createHelper(
            CachedConfig::getInstance()->getAgentLocationKey(), TA_Base_Bus::ModbusHelperFactory::PA );
    }
    catch ( TA_Base_Bus::NotFoundFEPConfigException& expConfigError )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::NotFoundFEPConfigException", expConfigError.what() );
    }
    catch ( ...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unkonw exception", "Failed to create modbus helper instance" );
    }

	FUNCTION_EXIT;
}

void PASConnection::logDataBlockToBinaryString( int nFileLine, TA_Base_Bus::DataBlock<WORD>& dbwToBeLoged )
{
    std::stringstream stmLog;
    stmLog << "DataBlock[" << dbwToBeLoged.start() << "," << dbwToBeLoged.end() << "] binary: ";
    for ( int nLoop = dbwToBeLoged.start(); nLoop <= dbwToBeLoged.end(); ++nLoop )
    {
        std::bitset<sizeof(WORD) * 8> bsBitsConverter(dbwToBeLoged[nLoop]);
        stmLog << bsBitsConverter << " ";
    }

    LOG_GENERIC( __FILE__, nFileLine, TA_Base_Core::DebugUtil::DebugInfo, stmLog.str().c_str() );
}

} // namespace TA_IRS_App

