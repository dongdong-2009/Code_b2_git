/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/ModbusComms.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This class wraps the Modbus Comms infrastructure
  */

#ifndef MODBUSCOMMS_H
#define MODBUSCOMMS_H

#include "core/threads/src/Thread.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "app/ats/AtsAgent/src/AtsModbusTcpConnection.h"

#include "app/ats/AtsAgent/src/IAtsTable.h"
#include "app/ats/AtsAgent/src/AtsWatchDogTable.h"

#include "app/ats/AtsAgent/src/IIscsTable.h"
#include "app/ats/AtsAgent/src/IscsWatchDogTable.h"

#include "app/ats/AtsAgent/src/TableManager.h"
#include "bus/modbus_comms/src/ModbusException.h"

namespace TA_Core
{
	class AlarmHelper;
};

namespace TA_IRS_App
{
	#define MODBUS_MAX_CONNECTIONS 2

    /**
      * ModbusComms is a Thread class that periodically queries the Ats for
      * its tables and sends Iscs tables to the Ats.
      */
    class ModbusComms : public TA_Base_Core::Thread
    {
    public:

        /**
          * Constructor
          */
        ModbusComms(unsigned int connection, 
					const std::string& ipAddress,
					const std::string& servicePort,
					unsigned long maxCommsRetries,
					TableManager& tableManager);

        /**
          * Destructor
          */
        virtual ~ModbusComms();

		void		ensureConnected();
		bool		isConnected();
		void		ensureDisconnected();

		void		readSecondaryWatchdog();

		bool		modbusRead(RawTable& rawTable);
		bool		modbusWrite(RawTable& rawTable);

		void		run();
		void		terminate();

    private:
        // Disable default constructor, copy constructor and assignment operator
		ModbusComms();
        ModbusComms( const ModbusComms& modbusComms);
        ModbusComms& operator=(const ModbusComms &);
		
		void	enableConnection();
		bool	connectConnection();
		void	disableConnection();
		bool    isModbusConError(const TA_Base_Bus::ModbusException& err);

	protected:

		unsigned int							m_connection;
		std::string								m_ipAddress;
		std::string								m_servicePort;
		unsigned long							m_maxCommsRetries;

		AtsModbusTcpConnection*					m_modbusTcpConnection;

		volatile bool							m_keepRunning;
		volatile bool							m_ensureConnected;
		volatile bool							m_connected;
		volatile bool							m_readSecondaryWatchdog;

		TableManager&							m_tableManager;
    };
}

#endif 
