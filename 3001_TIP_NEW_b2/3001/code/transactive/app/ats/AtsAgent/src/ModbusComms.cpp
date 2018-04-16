/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/ModbusComms.cpp $
  * @author:   Ross Tucker
  * @version:  $Revision: #3 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2015/01/21 17:42:44 $
  * Last mofified by:  $Author: peter.wong $
  *
  * This class wraps the modbus comms
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/sockets/src/TcpNoDelay.h"

#include "bus/modbus_comms/src/ModbusException.h"

#include "app/ats/AtsAgent/src/RawTable.h"
#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"

#include "app/ats/AtsAgent/src/ModbusComms.h"

namespace TA_IRS_App
{
    /**
      * Constructor
      */
    ModbusComms::ModbusComms(unsigned int connection, 
							 const std::string& ipAddress,
							 const std::string& servicePort,
							 unsigned long maxCommsRetries,
							 TableManager& tableManager) :
	m_connection (connection),
	m_ipAddress (ipAddress),
	m_servicePort (servicePort),
	m_maxCommsRetries (maxCommsRetries),
	m_modbusTcpConnection (NULL),
	m_keepRunning (true),
	m_ensureConnected (false),
	m_connected (false),
	m_readSecondaryWatchdog (false),
	m_tableManager(tableManager)
    {
        FUNCTION_ENTRY("ModbusComms()");

		m_modbusTcpConnection = new AtsModbusTcpConnection (
										m_ipAddress,
										m_servicePort,
										2000,
                                        true);
				
		if (m_modbusTcpConnection != NULL)
		{
			m_modbusTcpConnection->disable();
		}

		start();

        FUNCTION_EXIT;
    }

    /**
      * Destructor
      */
    ModbusComms::~ModbusComms()
    {
        FUNCTION_ENTRY("~ModbusComms()");

		if (m_modbusTcpConnection != NULL)
		{
			m_modbusTcpConnection->disable();
			
			delete m_modbusTcpConnection;
		}

		terminateAndWait();

        FUNCTION_EXIT;
    }

	bool ModbusComms::isConnected()
	{
		return m_connected;
	}

	void ModbusComms::enableConnection()
	{
		std::ostringstream festrstr; 
        festrstr << "enableConnection(" << m_connection << ")" << std::ends;
        FUNCTION_ENTRY(festrstr.str().c_str());

		TA_ASSERT(m_modbusTcpConnection != NULL, "Invalid connection");

		m_modbusTcpConnection->enable();

        FUNCTION_EXIT;
	}

	bool ModbusComms::connectConnection()
	{
		std::ostringstream festrstr; 
        festrstr << "connectConnection(" << m_connection << ")" << std::ends;
        FUNCTION_ENTRY(festrstr.str().c_str());

		TA_ASSERT(m_modbusTcpConnection != NULL, "Invalid connection");

		bool result = false;

		try
		{
			m_modbusTcpConnection->ensureConnected();

			result = true;
		}
		catch (...)
		{
		}

		FUNCTION_EXIT;

		return result;
	}

	void ModbusComms::disableConnection()
	{
        std::ostringstream festrstr; 
        festrstr << "disableConnection(" << m_connection << ")" << std::ends;
        FUNCTION_ENTRY(festrstr.str().c_str());

		TA_ASSERT(m_modbusTcpConnection != NULL, "Invalid connection");

		m_modbusTcpConnection->disable();

        FUNCTION_EXIT;
	}


	bool ModbusComms::modbusRead(RawTable& rawTable)
	{
        std::ostringstream festrstr; 
        festrstr << "modbusRead(rawTable.start()=" << rawTable.getDataBlock().start() << ", connection=" << m_connection << ")" << std::ends;
        FUNCTION_ENTRY(festrstr.str().c_str());

		bool dataRead = false;

		TA_ASSERT(m_modbusTcpConnection != NULL, "Invalid connection");

		if (m_connected)
		{
			// Retry before we give up
			for (unsigned long i = 0; i < m_maxCommsRetries && !dataRead; i++)
			{
				if (i != 0)
				{
					// Pause before retries
					Thread::sleep(400);
				}

				try
				{	
					m_modbusTcpConnection->readInputRegisters(rawTable.getDataBlock());

					dataRead = true;
				}
				catch ( TA_Base_Bus::ModbusException& e )
				{
					std::ostringstream text;
					text << "modbusRead() [" << m_connection << "] - " + std::string(e.what());
 					LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", text.str());

					//Check if ModbusException is due to Connection problem
					//If due to Connection problem, the retry must be stopped
					if(true == isModbusConError(e))
						break;
				}
				catch (...)
				{
					std::ostringstream text;
					text << "modbusRead() [" << m_connection << "] - Unknown exception";
 					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
				}
			}
		}

        FUNCTION_EXIT;
		return dataRead;
	}


	bool ModbusComms::modbusWrite(RawTable& rawTable)
	{
        std::ostringstream festrstr; 
        festrstr << "modbusWrite(rawTable.start()=" << rawTable.getDataBlock().start() << ", connection=" << m_connection << ")" << std::ends;
        FUNCTION_ENTRY(festrstr.str().c_str());

		bool dataWritten = false;

		TA_ASSERT(m_modbusTcpConnection != NULL, "Invalid connection");

		if (m_connected)
		{
			// Retry before we give up
			for (unsigned long i = 0; i < m_maxCommsRetries && !dataWritten; i++)
			{
				if (i != 0)
				{
					// Pause before retries
					Thread::sleep(400);
				}

				try
				{
					m_modbusTcpConnection->writeMultipleRegisters(rawTable.getDataBlock());
			
					dataWritten = true;
				}
				catch (TA_Base_Bus::ModbusException & e)
				{
					std::ostringstream text;
					text << "modbusWrite() [" << m_connection << "] - " + std::string(e.what());
 					LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", text.str());
				}
				catch (...)
				{
					std::ostringstream text;
					text << "modbusWrite() [" << m_connection << "] - Unknown exception";
 					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
				}
			}
		}

        FUNCTION_EXIT;

		return dataWritten;
	}

	void ModbusComms::ensureConnected()
	{
		m_ensureConnected = true;
	}

	void ModbusComms::ensureDisconnected()
	{
		m_ensureConnected = false;
	}

	void ModbusComms::readSecondaryWatchdog()
	{
		if (m_connected)
		{
			m_readSecondaryWatchdog = true;
		}
		else
		{
			m_readSecondaryWatchdog = false;
		}
	}

	void ModbusComms::run()
	{
		time_t lastTime=time(NULL);

		while (m_keepRunning)
		{
			if (m_readSecondaryWatchdog && m_connected)
			{
				m_tableManager.readAtsWatchdog(m_connection);

				m_readSecondaryWatchdog = false;
			}

			if (lastTime != time(NULL) && m_keepRunning)
			{
				if (m_ensureConnected)
				{
					if ( false == m_modbusTcpConnection->isEnabled() )
					{
						m_connected = false;

						enableConnection();
					}

					if ( false == m_modbusTcpConnection->isConnected() )
					{
						m_connected = connectConnection();
					}
					else
					{
						m_connected = true;
					}

					if (!m_connected)
					{
						m_tableManager.connectionFailed(m_connection);
					}
				}
				else if (m_connected)
				{
					m_modbusTcpConnection->disable();
					m_connected = false;
				}

				time(&lastTime);
			}

			Thread::sleep(400);
		}
	}

	void ModbusComms::terminate()
	{
		m_keepRunning = false;
	}

	bool ModbusComms::isModbusConError(const TA_Base_Bus::ModbusException& err)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ModbusException : %d",static_cast<int>(err.getFailType()));
		bool isConError = false;

		//Check the ModbusException if it is due to Connection problem
		if ( TA_Base_Bus::ModbusException::CONNECT_ERROR    == err.getFailType() ||
			TA_Base_Bus::ModbusException::SOCKET_READ_ERROR == err.getFailType() ||
			TA_Base_Bus::ModbusException::SOCKET_READ_ERROR == err.getFailType() )
		{
			isConError = true;
		}

		return isConError;
	}
}
