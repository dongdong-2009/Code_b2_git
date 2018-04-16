/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSCommunicationsHandler.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include <bitset>
#include "app/signs/pids_agent/src/PIDSCommunicationsHandler.h"
#include "app/signs/pids_agent/src/PIDSManager.h"
//#include "app/signs/pids_agent/src/StatusMonitor.h"
#include "app/signs/pids_agent/src/Exceptions.h"

#include "bus/modbus_comms/src/ModbusException.h"
#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/message/types/PIDSAlarms_MessageTypes.h"
#include "core/message/types/TISAlarms_MessageTypes.h"
#include "core/alarm/src/AlarmHelper.h"
#include "app/signs/pids_agent/src/StatusMonitor.h"


namespace TA_IRS_App
{
	PIDSCommunicationsHandler*                PIDSCommunicationsHandler::m_instance = 0;
	TA_Base_Core::ReEntrantThreadLockable     PIDSCommunicationsHandler::m_lock;

	PIDSCommunicationsHandler* PIDSCommunicationsHandler::getInstance()
	{
		FUNCTION_ENTRY("getInstance");

		if ( 0 == m_instance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			TA_Base_Core::ThreadGuard guard( m_lock );

			if ( 0 == m_instance )
			{
				m_instance = new PIDSCommunicationsHandler();
			}
		}

		return m_instance;

		FUNCTION_EXIT;
	}

	void PIDSCommunicationsHandler::removeInstance( )
	{
		FUNCTION_ENTRY("removeInstance");

		// 
		// Guard this to prevent multiple threads atempting
		// to delete/create simultaneously
		//
		TA_Base_Core::ThreadGuard guard( m_lock );
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL;
		}

		FUNCTION_EXIT;
	}

	short PIDSCommunicationsHandler::sendMessage( PIDSMessage* data, bool isSigMsg/* = false*/, WORD startAddress/*= 1*/)
	{
		FUNCTION_ENTRY("sendMessage");

		if ( 0 == m_pModbusHelper )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "No modbus helper yet, wait for config update" );
			return -1;
		}

		TA_Base_Bus::DataBlock<WORD> dbwData = data->createProtocolMessage();

		if ( 0 == dbwData.length() )
		{
			return -2;
		}

		try
		{
			logDataBlockToBinaryString( __LINE__, dbwData );
			if(!isSigMsg)
			{
				m_pModbusHelper->SendData( dbwData );
			}
			else //should be sig msg
			{
				m_pModbusHelper->SendData( dbwData, startAddress);
			}			
		}
		catch ( TA_Base_Bus::ModbusException& expServer )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ModbusException", expServer.what() );
//			TA_Base_Bus::IPIDSManagerCorbaDef::PIDSServerNotConnectedException e;
//			throw(e);			
			unsigned int errorType = static_cast<unsigned int>(expServer.getFailType());
			std::ostringstream strErrorType;
			strErrorType << errorType;
			TA_Base_Core::NameValuePair errorTypeValue("errorType", strErrorType.str());
			TA_Base_Core::DescriptionParameters dp;
			dp.push_back(&errorTypeValue);
			
			PIDSManager * pidsManager = PIDSManager::getInstance();//it should not be NONE
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset( TA_Base_Core::PIDSAlarms::PidsCommFailure,
				pidsManager->getEntityKey(),
				pidsManager->getEntityKeyType(),
				dp,
				pidsManager->getEntityName(),
				pidsManager->getLocationKey(),				
				pidsManager->getSubsystemKey(),
				StatusMonitor::getInstance()->getAgentAssetName(),
				TA_Base_Core::AlarmConstants::defaultTime );
			return -1;
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
			return -1;
		}
		
		return 0;
		FUNCTION_EXIT;
	}

	PIDSCommunicationsHandler::PIDSCommunicationsHandler() : m_pModbusHelper( static_cast<TA_Base_Bus::IModbusHelper*>(0) )
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}

	PIDSCommunicationsHandler::~PIDSCommunicationsHandler()
	{
	}

	// ExceptionChecked
	void PIDSCommunicationsHandler::initialise(unsigned long agentLocation)
	{
		FUNCTION_ENTRY("createModbusHelper");

		try
		{
			// boost smart pointer
			m_pModbusHelper = TA_Base_Bus::ModbusHelperFactory::createHelper(
				agentLocation, TA_Base_Bus::ModbusHelperFactory::PIDS );
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

	void PIDSCommunicationsHandler::logDataBlockToBinaryString( int nFileLine, TA_Base_Bus::DataBlock<WORD>& dbwToBeLoged )
	{
		std::stringstream stmLog;
		stmLog << "DataBlock[" << dbwToBeLoged.start() << "," << dbwToBeLoged.end() << "] binary: ";
		for ( int nLoop = dbwToBeLoged.start(); nLoop <= dbwToBeLoged.end(); ++nLoop )
		{
			std::bitset<sizeof(WORD) * 8> bsBitsConverter(dbwToBeLoged[nLoop]);
			stmLog << bsBitsConverter << " ";
		}

		//LOG_GENERIC( __FILE__, nFileLine, TA_Base_Core::DebugUtil::DebugInfo, stmLog.str().c_str() );
		LOGLARGESTRING( __FILE__, nFileLine, TA_Base_Core::DebugUtil::DebugInfo, stmLog.str().c_str() );
	}

}