/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/TableManager.cpp $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>
#include <time.h>

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/configuration_updates/src/ConfigUpdateDetails.h"

#include "core/utilities/src/RunParams.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/AtsAgentAlarms_MessageTypes.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/exceptions/src/AtsAgentException.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"

#include "app/ats/AtsAgent/src/AtcDataTable.h"
#include "app/ats/AtsAgent/src/DssTable.h"
#include "app/ats/AtsAgent/src/ElectricalSubsectionTable.h"
#include "app/ats/AtsAgent/src/PlatformTable.h"
#include "app/ats/AtsAgent/src/TrainTable.h"
#include "app/ats/AtsAgent/src/WashTable.h"
#include "app/ats/AtsAgent/src/WildTable.h"

#include "app/ats/AtsAgent/src/AtsWatchDogTable.h"
#include "app/ats/AtsAgent/src/IscsWatchDogTable.h"

#include "app/ats/AtsAgent/src/WatchDogData.h"

#include "app/ats/AtsAgent/src/ModbusComms.h"

#include "app/ats/AtsAgent/src/TableManager.h"
#include "boost/algorithm/string.hpp"

namespace TA_IRS_App
{
    /**
      * Constructor
      */
	TableManager::TableManager(TA_Base_Core::AtsAgentEntityDataPtr entityData,
							   DataManager& dataManager)
	: m_currentConnection(-1),
	  m_keepRunning(true),
	  m_entityData(entityData),
	  m_dataManager(dataManager)
	{
		FUNCTION_ENTRY("TableManager()");

		unsigned long entityKey = m_entityData->getKey();
		unsigned long locationKey = m_entityData->getLocation();

		// Find the ATS location type
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType = m_entityData->getAgentAtsLocationType();

		std::string locationName = m_entityData->getLocationName()  ;
		boost::to_upper(locationName);
		if ( locationName.compare ( "GDCC") ==0 || locationName.compare("GBD") == 0) 
		{
           locationType = TA_Base_Core::AtsAgentEntityData::Depot ; //bihui 16/02/2011 hardcode
		}
	
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  locationType == TA_Base_Core::AtsAgentEntityData::Depot ||
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "Not a known location");

		// Get the base addresses of the Modbus tables
		int atsTablesBaseAddress = m_entityData->getAtsTablesBaseAddress();
		int iscsTablesBaseAddress = m_entityData->getIscsTablesBaseAddress();

		// Create the supporting classes

		//
		// Tables
		//
		m_atsTables.clear();
		m_iscsTables.clear();
		
		// All Locations
		if (locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
			locationType == TA_Base_Core::AtsAgentEntityData::Depot ||
			locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
			locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
			locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
		{
			m_atsWatchDogTable = NULL;
			DssTable* dssTable = NULL;
			TrainTable* trainTable = NULL;
			m_iscsWatchDogTable = NULL;
			AtcDataTable* atcDataTable = NULL;

			try
			{		
				trainTable = new TrainTable(entityKey, locationKey, locationType, 
											atsTablesBaseAddress, iscsTablesBaseAddress, 
											dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
								 "TableManager() - Unknown Exception creating TrainTable");
			}

			TA_ASSERT(trainTable != NULL, "Train Table not created");

			try
			{
				dssTable = new DssTable(m_entityData, m_dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
								 "TableManager() - Unknown Exception creating DssTable");
			}
			
			TA_ASSERT(dssTable != NULL, "DSS Table not created");

			try
			{
				m_iscsWatchDogTable = new IscsWatchDogTable(locationType, iscsTablesBaseAddress, m_dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
								 "TableManager() - Unknown Exception creating IscsWatchDogTable");
			}
			
			TA_ASSERT(m_iscsWatchDogTable != NULL, "ISCS Watchdog Table not created");

			try
			{
				atcDataTable = new AtcDataTable(locationKey, locationType, iscsTablesBaseAddress, m_dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
								 "TableManager() - Unknown Exception creating AtcDataTable");
			}
							
			TA_ASSERT(atcDataTable != NULL, "ISCS ATC Alarm Table not created");

			m_atsTables[TrainTable::getStaticType()] = trainTable;
			m_atsTables[DssTable::getStaticType()] = dssTable;

			m_iscsTables[TrainTable::getStaticType()] = trainTable;
			m_iscsTables[AtcDataTable::getStaticType()] = atcDataTable;
		}

		PlatformTable* platformTable = NULL;
		
		// OCC and Stations only
		if (locationType == TA_Base_Core::AtsAgentEntityData::Occ ||
			locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
			locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
			locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
		{
			ElectricalSubsectionTable* electricalSubsectionTable = NULL;

			try
			{
				platformTable = new PlatformTable(entityKey, locationKey, locationType, 
												  atsTablesBaseAddress, iscsTablesBaseAddress, 
												  m_dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", 
									"TableManager() - Unknown Exception creating PlatformTable");
			}

			TA_ASSERT(platformTable != NULL, "ATS Platform Table not created");

			try
			{
				electricalSubsectionTable = new ElectricalSubsectionTable(locationKey, locationType, iscsTablesBaseAddress, 
																		  m_dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", 
									"TableManager() - Unknown Exception creating ElectricalSubsectionTable");
			}

			TA_ASSERT(electricalSubsectionTable != NULL, "ISCS Electrical Subsection Table not created");

			m_atsTables[PlatformTable::getStaticType()] = platformTable;
			m_iscsTables[ElectricalSubsectionTable::getStaticType()] = electricalSubsectionTable;
		}

		
		// OCC only
		if (locationType == TA_Base_Core::AtsAgentEntityData::Occ)
		{
			WildTable* wildTable = NULL;

			try
			{
				wildTable = new WildTable(entityKey, locationKey, locationType, 
										  atsTablesBaseAddress, dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", 
									"TableManager() - Unknown Exception creating WildTable");
			}

			TA_ASSERT(wildTable != NULL, "WILD Table not created");

			m_atsTables[WildTable::getStaticType()] = wildTable;
		}

		// Stations only
		if (locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
			locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
			locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
		{
			TA_ASSERT(platformTable != NULL, "ISCS Platform Table not created");
			
			//m_atsTables[PlatformTable::getStaticType()] = platformTable;
			m_iscsTables[PlatformTable::getStaticType()] = platformTable;
		}

		// Depot only
		if (locationType == TA_Base_Core::AtsAgentEntityData::Depot)
		{
			WashTable* washTable = NULL;

			try
			{
				washTable = new WashTable(entityKey, locationKey, locationType, 
										  atsTablesBaseAddress, iscsTablesBaseAddress, 
										  m_dataManager);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", 
									"TableManager() - Unknown Exception creating WashTable");
			}

			TA_ASSERT(washTable != NULL, "Wash Table not created");

			m_atsTables[WashTable::getStaticType()] = washTable;
			m_iscsTables[WashTable::getStaticType()] = washTable;
		}

		//
        // Still need to be able to connect to a non-configured,
        // command-line specified simulator.
        if (TA_Base_Core::RunParams::getInstance().isSet(AtsAgentRunParams::SIMULATOR_HOST.c_str()))
        {
			std::string ipAddresses = TA_Base_Core::RunParams::getInstance().get(AtsAgentRunParams::SIMULATOR_HOST.c_str());

			// Check the length first
			if (ipAddresses.length() != 0)
			{
				// Now extract the comma separated ip addresses
				char comma = ',';
				int  numIPs;
				int  indexes[MODBUS_MAX_CONNECTIONS+1]; 
				int  commaIndex;

				// Find the start and ending indexes for each ip address
				indexes[0] = -1;
				numIPs = 1;  // Assume at least one ip address
				while ((commaIndex = ipAddresses.find(comma, indexes[numIPs-1]+1)) != -1  
						&& numIPs < MODBUS_MAX_CONNECTIONS) 
				{
					indexes[numIPs] = commaIndex;
					numIPs++;
				}
				indexes[numIPs] = ipAddresses.length();

				int i;

				for (i = 0; i < numIPs; i++)
				{
					ModbusComms* modbusComms = NULL;
					std::string ipAddress = "", servicePort = "";

					std::string ip;
					ip.assign(ipAddresses, indexes[i]+1, indexes[i+1]-indexes[i]-1);
					
					std::string::size_type colonpos = ip.find(':');
            
					if ( colonpos != std::string::npos )
					{
						ipAddress = ip.substr(0,colonpos);
						servicePort = ip.substr(colonpos+1,ip.length()-colonpos-1);
					}
					else
					{
						ipAddress = ip;

						std::ostringstream servicePortStream;

						servicePortStream << (25500 + (i*100));
						servicePort = servicePortStream.str();	// Default for ATS Simulator
					}

					modbusComms = new ModbusComms(i, ipAddress, servicePort, 3, *this);

					if (modbusComms != NULL)
					{
						m_modbusComms.push_back(modbusComms);
					}
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ModbusComms() - added simulator connection %s:%s", ipAddress.c_str(), servicePort.c_str());
				}
			}
        }
		else
		{
			// Get Modbus Connection details
			ModbusComms* modbusComms;

			modbusComms = new ModbusComms(0, 
										  m_entityData->getAtsServer1IpAddress(),
										  m_entityData->getAtsServer1ServicePort(),
										  m_entityData->getMaxCommsRetries(),
										  *this);

			if (modbusComms != NULL)
			{
				m_modbusComms.push_back(modbusComms);
			}

			modbusComms = NULL;
			modbusComms = new ModbusComms(1, 
										  m_entityData->getAtsServer2IpAddress(),
										  m_entityData->getAtsServer2ServicePort(),
										  m_entityData->getMaxCommsRetries(),
										  *this);

			if (modbusComms != NULL)
			{
				m_modbusComms.push_back(modbusComms);
			}
		}

		m_dataManager.getWatchDogData().setNumberOfConnections(m_modbusComms.size());

		FUNCTION_EXIT;
	} 

    /**
      * Destructor
      */
	TableManager::~TableManager()
	{
		FUNCTION_ENTRY("~TableManager()");

		terminateAndWait();

		std::vector<ModbusComms*>::iterator modbusCommsIt;

		for (modbusCommsIt = m_modbusComms.begin(); modbusCommsIt != m_modbusComms.end(); modbusCommsIt++)
		{
			delete (*modbusCommsIt);
			(*modbusCommsIt) = NULL;
		}

		// ATS Tables
		std::map<std::string, IAtsTable*>::iterator atsIt;
		for (atsIt = m_atsTables.begin(); atsIt != m_atsTables.end(); atsIt++)
		{
			if ((*atsIt).second != NULL)
			{
				// Can't delete the train and platform tables twice.
				if ((*atsIt).first != TrainTable::getStaticType() &&
					(*atsIt).first != PlatformTable::getStaticType())
				{
					delete (*atsIt).second;
				}
				(*atsIt).second = NULL;	
			}
		}
		m_atsTables.clear();

		// ISCS Tables
		if (m_iscsWatchDogTable != NULL)
		{
			delete m_iscsWatchDogTable;
			m_iscsWatchDogTable = NULL;
		}

		std::map<std::string, IIscsTable*>::iterator iscsIt;
		for (iscsIt = m_iscsTables.begin(); iscsIt != m_iscsTables.end(); iscsIt++)
		{
			if ((*iscsIt).second != NULL)
			{
				// Can't delete the train and platform tables twice.
				if ((*iscsIt).first != TrainTable::getStaticType() &&
					(*iscsIt).first != PlatformTable::getStaticType())
				{
					delete (*iscsIt).second;
				}
				(*iscsIt).second = NULL;
					
			}
		}
		m_iscsTables.clear();	

		FUNCTION_EXIT;
	}

	//
	// Thread functions
	//

	void TableManager::run()
    {
		time_t currentTime = 0, lastTime = 0;

		TA_ASSERT(m_modbusComms.size() != 0, "Modbus Comms not created");

		// Connect the connections
		std::vector<ModbusComms*>::iterator modbusCommsIt;

		for (modbusCommsIt = m_modbusComms.begin(); modbusCommsIt != m_modbusComms.end(); modbusCommsIt++)
		{
			TA_ASSERT((*modbusCommsIt) != NULL, "Invalid Modbus Connection");

			(*modbusCommsIt)->ensureConnected();
		}

		m_read = false;

		// Have to set up the watchdog table here as the number of connections aren't known until here...
		if (m_atsWatchDogTable != NULL)
		{
			delete m_atsWatchDogTable;
			m_atsWatchDogTable = NULL;
		}

		m_atsWatchDogTable = new AtsWatchDogTable(m_entityData->getKey(), m_entityData->getLocation(), 
												  m_entityData->getAgentAtsLocationType(), 
												  m_entityData->getAtsTablesBaseAddress(), 
												  m_dataManager);	

		TA_ASSERT(m_atsWatchDogTable != NULL, "ATS Watchdog Table not created");

		time(&lastTime);

		OperationModeManager& operationModeManager = m_dataManager.getOperationModeManager();

        while (m_keepRunning)
        {
			time(&currentTime);
				
			// Update the ISCS Watchdog
			if (currentTime != lastTime)
			{
				m_iscsWatchDogTable->updateWatchdogCounter(currentTime-lastTime);

				lastTime = currentTime;
			}

			
			if ( ( !m_read ) && ( m_nextPoll <= currentTime ) && ( operationModeManager.inControlMode() ) )
			{
				//
				// Write to Ats
				//
				m_nextPoll = time(NULL) + 2;

				if (writeIscsTables())
				{
					m_read = true;
				}
				else
				{
					m_nextPoll +=2;
				}
			}
			else if ( ( m_read ) && ( m_nextPoll <= currentTime ) && ( operationModeManager.inControlMode() ) )
			{
				//
				// Read from Ats
				//
				m_nextPoll = time(NULL) + 2;

				readAtsTables();
				
				m_read = false;
			}
			
			Thread::sleep(400);
        }

		if (m_atsWatchDogTable != NULL)
		{
			delete m_atsWatchDogTable;
			m_atsWatchDogTable = NULL;
		}

		// Disconnect the connections
		for (modbusCommsIt = m_modbusComms.begin(); modbusCommsIt != m_modbusComms.end(); modbusCommsIt++)
		{
			TA_ASSERT((*modbusCommsIt) != NULL, "Invalid Modbus Connection");

			(*modbusCommsIt)->ensureDisconnected();
		}

		m_currentConnection = -1;

		m_keepRunning = true;
    }

	void TableManager::terminate()
    {
        m_keepRunning = false;
    }

	void TableManager::setToMonitorMode()
	{
		// Terminate the comms thread
        terminateAndWait();
	}

	void TableManager::setToControlMode()
	{
		// Read immediately
		m_read = true;
		m_nextPoll = 0;

		// Start the comms thread
        start();
	}

	bool TableManager::writeIscsTables()
	{
		/*LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"*** Begin ISCS Table Write ***");*/

		return true ; //bihui added
		/*
		int connection = -1;

		WatchDogData& watchDogData = m_dataManager.getWatchDogData();

		// Write Watchdog first
		{
			RawTable rawTable(m_iscsWatchDogTable->getIscsModbusStartAddress(), 
							  m_iscsWatchDogTable->getIscsModbusEndAddress());

			// Get the raw table
			try
			{
				m_iscsWatchDogTable->exportIscsTable(rawTable);
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
									"writeIscsTables() - Exception caught while exporting raw table");

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"*** End ISCS Table Write - Bad Watchdog Export ***");

				return false;
			}

			// Try to find a good connection
			connection = getConnection();
			int numConnections = getNumConnections();
			int i;

			for (i = 0; i < numConnections && connection != -1; i++)
			{
				TA_ASSERT(connection < (int)m_modbusComms.size() && m_modbusComms[connection] != NULL, "Invalid Modbus Connection");

				if (m_modbusComms[connection]->modbusWrite(rawTable))
				{
					watchDogData.updateConnectionState(connection, WatchDogData::ConnectionWrite);

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                        "writeIscsTables() - successful modbusWrite(rawTable=%d, connection=%d)", rawTable.getDataBlock().start(), connection);

					break;
				}
				else
				{
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                        "writeIscsTables() - failed modbusWrite(rawTable=%d, connection=%d)", rawTable.getDataBlock().start(), connection);

					connectionFailed(connection);
					connection = getAnotherConnection();
				}
			}

			// Still no good connection - give up for now
			if (connection == -1)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"*** End ISCS Table Write - no good connection ***");

				return false;
			}

			WatchDogData::EWatchDogState watchDogState = watchDogData.getWatchDogState(connection);
			if  (watchDogState == WatchDogData::WatchDogNotRead || 
				 watchDogState == WatchDogData::WatchDogNotValid)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
							"*** End ISCS Table Write - WatchDog Only ***");

				return true;
			}
		}

		// Now try the rest of the tables using this connection
		std::map<std::string, IIscsTable*>::iterator iscsIt;
		bool writeSuccessful = true;
		for (iscsIt = m_iscsTables.begin(); iscsIt != m_iscsTables.end() && writeSuccessful; iscsIt++)
		{
			if ((*iscsIt).second != NULL && ((*iscsIt).second)->tableWriteAllowed())
			{    
				RawTable rawTable(((*iscsIt).second)->getIscsModbusStartAddress(), 
								  ((*iscsIt).second)->getIscsModbusEndAddress());

				try
				{
					(*iscsIt).second->exportIscsTable(rawTable);

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                        "writeIscsTables() - successfully exported rawTable %d on connection %d", rawTable.getDataBlock().start(), connection);

				}
				catch (...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
										"writeIscsTables() - Exception caught while exporting raw table");

					TA_ASSERT(false, "writeIscsTables() - Exception caught while exporting raw table");
				}
				
				TA_ASSERT(connection < (int)m_modbusComms.size() && m_modbusComms[connection] != NULL, "Invalid Modbus Connection");

				if (!m_modbusComms[connection]->modbusWrite(rawTable))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"writeIscsTables() - failed modbusWrite(rawTable=%d, connection=%d)", rawTable.getDataBlock().start(), connection);

                    writeSuccessful = false;
				}
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                        "writeIscsTables() - successful modbusWrite(rawTable=%d, connection=%d)", rawTable.getDataBlock().start(), connection);
                }
			}
		}

		if (writeSuccessful)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"*** End ISCS Table Write ***");
		}
		else
		{
			connectionFailed(connection);
			nextConnection();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"*** End ISCS Table Write - New Connection ***");
		}
			
		return writeSuccessful;*/
	}

	void TableManager::readAtsTables()
	{
		int connection = -1;

		// Try to find a good connection
		connection = getConnection();

		WatchDogData& watchDogData = m_dataManager.getWatchDogData();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"*** Begin ATS Table Read (connection = %d) ***",
					connection);

		if (connection == -1)
		{
			// no valid connection
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"*** End ATS Table Read - No valid Connection ***");

			return;
		}

		{
			int i;

			// Get WatchDog first
			for (i = 0; i < (int)m_modbusComms.size(); i++)
			{
				if (i == connection)
				{
					readAtsWatchdog(i);
				}
				else
				{
					TA_ASSERT(i < (int)m_modbusComms.size() && m_modbusComms[i] != NULL, "Invalid Modbus Connection");

					m_modbusComms[i]->readSecondaryWatchdog();
				}
			}
		}

		WatchDogData::EWatchDogState watchDogState = watchDogData.getWatchDogState(connection);
		if  (watchDogState != WatchDogData::WatchDogValid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"*** End ATS Table Read - Watchdog not Valid ***");

			if (watchDogState != WatchDogData::WatchDogReadOnce)
			{
				nextConnection();
			}
			m_dataManager.processUpdates();

			return;
		}

		bool readSuccessful = true;

		// Then the rest of the tables
		std::map<std::string, IAtsTable*>::iterator atsIt;
		for (atsIt = m_atsTables.begin(); atsIt != m_atsTables.end(); atsIt++)
		{
			if ((*atsIt).second != NULL && ((*atsIt).second)->tableReadAllowed())
			{
				RawTable rawTable(((*atsIt).second)->getAtsModbusStartAddress(), 
								  ((*atsIt).second)->getAtsModbusEndAddress());
				
                int pre_length = rawTable.length();

				TA_ASSERT(connection < (int)m_modbusComms.size() && m_modbusComms[connection] != NULL, "Invalid Modbus Connection");

				if (m_modbusComms[connection]->modbusRead(rawTable))
				{
                    TA_ASSERT(rawTable.length() == pre_length, "readAtsTables() - RawTable data length modified");
					try
					{
						(*atsIt).second->importAtsTable(rawTable);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
							"readAtsTables() - successfully imported rawTable %d on connection %d", rawTable.getDataBlock().start(), connection);

					}
					catch(...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
											"readAtsTables() - Exception caught while importing raw table");

						readSuccessful = false;
						break;
					}
				}
				else
				{					
					readSuccessful = false;
					break;
				}
			}
		}

		if (readSuccessful)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"*** End ATS Table Read - Successful ***");
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"*** End ATS Table Read - Failed, New Connection ***");

			connectionFailed(connection);
			nextConnection();
		}

		// Process Updates
		m_dataManager.processUpdates();
	}

	void TableManager::readAtsWatchdog(int connection)
	{
		RawTable rawTable(m_atsWatchDogTable->getAtsModbusStartAddress(), 
						  m_atsWatchDogTable->getAtsModbusEndAddress());

		int pre_length = rawTable.length();

		TA_ASSERT(connection < (int)m_modbusComms.size() && m_modbusComms[connection] != NULL, "Invalid Modbus Connection");

		bool readSuccessful = false;

		if (m_modbusComms[connection]->modbusRead(rawTable))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"readAtsTables() - successful modbusRead(rawTable=%d, connection=%d)", rawTable.getDataBlock().start(), connection);

			TA_ASSERT(rawTable.length() == pre_length, "readAtsTables() - Watchdog RawTable data length modified");
			try
			{
				m_atsWatchDogTable->importAtsTable(rawTable, connection);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"readAtsTables() - successfully imported rawTable %d on connection %d", rawTable.getDataBlock().start(), connection);

				readSuccessful = true;
			}
			catch (...)
			{
				std::ostringstream text;
				text << "readAtsTables() - Error: Exception caught while importing WatchDog table for Connection " << connection;

				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text.str());
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"readAtsTables() - Error: Failed to read WatchDog for Connection %d", connection);
		}

		WatchDogData& watchDogData = m_dataManager.getWatchDogData();

		if (readSuccessful)
		{
			if (connection == m_currentConnection)
			{
				watchDogData.updateConnectionState(connection, WatchDogData::ConnectionRead);
			}
			else
			{
				watchDogData.updateConnectionState(connection, WatchDogData::ConnectionRead);
			}
		}
		else
		{
			connectionFailed(connection);
		}
	}

	int TableManager::getConnection()
	{
		if (m_currentConnection == -1)
		{
			nextConnection();
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"getConnection() - Using connection %d", 
					m_currentConnection);

		if(m_currentConnection ==-1)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"Now update PSD Datapoint for Double ATS failure, Set Not Connected");
			m_dataManager.updatePSDWhenDoubleATSFail();		//hongzhi, TD17760
		}

		return m_currentConnection;
	}

	int TableManager::getAnotherConnection()
	{
		int originalConnection = m_currentConnection;

		if (nextConnection() == originalConnection)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"getAnotherConnection() - No other connected connection");

			return -1;
		}

		return m_currentConnection;
	}

	int TableManager::nextConnection()
	{
		FUNCTION_ENTRY("getNextActiveConnection()");

		int originalConnection = m_currentConnection;
		int connection = 0;
		int numConnections = getNumConnections();
		
		for (connection = 0; connection < numConnections; connection++)
		{
			TA_ASSERT(m_modbusComms[connection] != NULL, "Invalid Modbus Connection");

			if (m_currentConnection != connection &&
				true == m_modbusComms[connection]->isConnected())
			{
	
				// If found a different connection that is OK, update the current connection state to ConnectionWrite.
				// Which means that the connection now is ready to accept and write the data. 
				m_dataManager.getWatchDogData().updateConnectionState(connection,WatchDogData::ConnectionRead); //Lucky 8/4/2011
	
				break;
			}
		}

		if (connection < numConnections)
		{
			// Found a different connection that is OK
			m_currentConnection = connection;
		}
		else if (m_currentConnection != -1)
		{
			// Checking to see if current connection is OK
			TA_ASSERT(m_modbusComms[m_currentConnection] != NULL, "Invalid Modbus Connection");

			if ( false == m_modbusComms[m_currentConnection]->isConnected())
			{
				m_currentConnection = -1;
			}
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"nextConnection() - Switching to connection %d from %d", 
			m_currentConnection, originalConnection);

		FUNCTION_EXIT;

		return m_currentConnection;
	}

	void TableManager::connectionFailed(int connection)
	{
		m_dataManager.getWatchDogData().
			updateConnectionState(connection, WatchDogData::ConnectionFailed);
	}

	int TableManager::getNumConnections()
	{
		int numConnections = (int)m_modbusComms.size();

		return (MODBUS_MAX_CONNECTIONS < numConnections)?MODBUS_MAX_CONNECTIONS:numConnections;
	}
}
