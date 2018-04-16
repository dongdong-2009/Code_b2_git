/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Ross Tucker
  * @version:  $Revision$
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime$
  * Last mofified by:  $Author$
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
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsPSDDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPSDData.h"
#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsDssPlanMapAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsPowerDataFactory.h"

#include "core/configuration_updates/src/ConfigUpdateDetails.h"

#include "core/utilities/src/RunParams.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/message/types/AtsAgentAlarms_MessageTypes.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"

#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/FatalConfigurationException.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"
#include "app/ats/AtsAgent/src/AtsCctvData.h"
#include "app/ats/AtsAgent/src/AtsEcsData.h"
#include "app/ats/AtsAgent/src/AtsTrainData.h"
#include "app/ats/AtsAgent/src/AtsPasData.h"
#include "app/ats/AtsAgent/src/AtsPsdData.h"
#include "app/ats/AtsAgent/src/AtsTisData.h"
#include "app/ats/AtsAgent/src/AtsDssData.h"
#include "app/ats/AtsAgent/src/AtsWildData.h"
#include "app/ats/AtsAgent/src/PlatformTable.h"
#include "app/ats/AtsAgent/src/TrainTable.h"
#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/OccUpdateProcessor.h"
#include "app/ats/AtsAgent/src/TrainCctvCamera.h"
#include "app/ats/AtsAgent/src/DataManager.h"
#include "app/ats/AtsAgent/src/AtsServerLinkStatusUpdateThread.h"

#include "core/message/types/TrainCctvCameraAlarms_MessageTypes.h"
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"

namespace
{
	const long INVALID_TINER_INTERVAL(-1);

}

namespace TA_IRS_App
{
    /**
      * Constructor
      */
	DataManager::DataManager(TA_Base_Core::AtsAgentEntityDataPtr entityData,
							   const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap,
							  OperationModeManager &operationModeManager,
                              AtsServerLinkStatusUpdateThread& linkStatusThread )
	: m_keepRunning(true), m_semaphore(0),
	  m_entityData(entityData), m_dataPointHelper(NULL),
	  m_stationZones (NULL), m_cctvZones (NULL), m_ecsZones (NULL),
	  m_locationKey (0),
	  m_locationType (TA_Base_Core::AtsAgentEntityData::Occ),
	  m_twpData (NULL),
	  m_planAgentLibrary(entityData->getLocation()),
	  m_dssPlanMap (NULL),
	  m_occUpdateProcessor (NULL),
	  m_occUpdateTime (0),
	  m_watchDogData (NULL),
	  m_occAtsAgent (NULL),
	  m_operationModeManager(operationModeManager),
	  m_atsTrainDataTimer(NULL),
	  m_dpControlStateProcessor(NULL),
	  m_bAtsRecoverLink(false),
      m_linkStatusThread( linkStatusThread )
	{
		FUNCTION_ENTRY("DataManager()");

		m_dataPointHelper = new DataPointHelper(m_entityData);

        //m_linkStatusThread.start(); //limin

		if (m_dataPointHelper == NULL)
		{
			TA_THROW(TA_Base_Core::AtsAgentException("Error: Ats Agent DataPointHelper can not be created"));
		}

		unsigned long entityKey = m_entityData->getKey();
		m_locationKey = m_entityData->getLocation();
		
		//get the atsTrainDataTimer configuration
		long oa1Timeout, atcTimeout;
		try
		{
			oa1Timeout = m_entityData->getOa1Timeout();
			atcTimeout = m_entityData->getAtcTimeout();
		}
		catch (...) 
		{
			oa1Timeout = INVALID_TINER_INTERVAL;
			atcTimeout = INVALID_TINER_INTERVAL;
		}
		m_atsTrainDataTimer = new AtsTrainDataTimer(oa1Timeout, atcTimeout);

		// Find the ATS location type
		m_locationType = m_entityData->getAgentAtsLocationType();

		TA_ASSERT(m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::Depot ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				  m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				  m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation,
				  "Not a known location");

		// Create the WatchDogData
		m_watchDogData = new WatchDogData(m_locationType,
										  m_entityData->getMaxWatchdogInterval());

		TA_ASSERT(m_watchDogData != NULL, "WatchDogData not created");

		std::vector<unsigned long> stationsInSector = m_sectorData.getStationsInRelatedMssSector(m_locationKey);

		// Create the supporting classes
		if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
		{
			m_occUpdateProcessor = new OccUpdateProcessor(m_locationKey, m_locationType, *m_watchDogData, m_sectorData);

			TA_ASSERT(m_occUpdateProcessor != NULL,
					  "Can't create OCC Update Processor");

            // if at OCC, we care about the server status changing
            m_watchDogData->setObserver(this);
		}
		else
		{
			// Create OCC ATS Agent named object
			try
			{
				TA_Base_Core::CorbaNameList occAtsAgentCorbaName = 
					TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
						TA_Base_Core::AtsAgentEntityData::getStaticType(), "OCC",true);

				if (occAtsAgentCorbaName.size() != 1)
				{
					throw TA_Base_Core::AtsAgentException("Can not locate OCC ATS Agent");
				}

				m_occAtsAgent = new OccUpdateProcessor::AtsAgentNamedObject;

				if (m_occAtsAgent != NULL)
				{
					m_occAtsAgent->setCorbaName(occAtsAgentCorbaName[0]);
				}
			}
			catch (...)
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
					"Configuration Error: Can not locate OCC ATS Agent"));
			}
		}

		// Get the zone maps
		try
		{
			m_stationZones = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().getAtsFixedBlockMap("IAGO_ZONES");
			m_cctvZones = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().getAtsFixedBlockMap("VI_ZONES");
			m_ecsZones = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().getAtsFixedBlockMap("EC_ZONES");
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			// Log and re-throw the exception
			std::string text = "DataManager() - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

			TA_THROW(TA_Base_Core::FatalConfigurationException(
						e.what()));
		}
		catch (TA_Base_Core::DataException& e)
		{
			// Log and re-throw the exception
			std::string text = "DataManager() - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

			TA_THROW(TA_Base_Core::FatalConfigurationException(
						e.what()));
		}	

		TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber;
		
		for (physicalTrainNumber = 1; physicalTrainNumber <= 99; physicalTrainNumber++)
		{
			AtsEcsData* ecsData = new AtsEcsData(physicalTrainNumber, entityKey, m_locationKey, 
												 *m_stationZones, *m_ecsZones, stationsInSector);

			m_ecsDataMap[physicalTrainNumber] = ecsData;
			
			//td 13947
			//zhou yuan++
			//start the AtsTrainDataTimer for ATC table
			AtsTrainData* trainData = new AtsTrainData(physicalTrainNumber, entityKey, m_locationKey, 
													   *m_stationZones, *m_cctvZones, stationsInSector,
													   m_atsTrainDataTimer);
			//++zhou yuan

			m_trainDataMap[physicalTrainNumber] = trainData;

			AtsCctvData* cctvData = new AtsCctvData(physicalTrainNumber, m_locationKey, *m_stationZones , m_locationType );
			m_cctvDataMap[physicalTrainNumber] = cctvData;
		}

		if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
		{
			createOccPlatformData(entityKey);
		}
		else if (m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				 m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				 m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
		{
			createStationPlatformData(entityKey);
		}

		// TWP - Only at Depot
		if (m_locationType == TA_Base_Core::AtsAgentEntityData::Depot)
		{
			m_twpData = new AtsTwpData(entityKey, m_locationKey, *m_dataPointHelper);

			TA_ASSERT(m_twpData != NULL, "Fatal: TWP Data can not be created");
		}

		unsigned int i;

		try
		{
			m_dssPlanMap = TA_IRS_Core::AtsDssPlanMapAccessFactory::getInstance().getIndexedAtsDssPlanMaps();
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "DataManager() - error when retrieving plan map - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "DataManager() - error when retrieving plan map - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
		}

		TA_ASSERT(m_dssPlanMap != NULL, "Could not retrieve DSS Plan Maps");

		// WILD - Only at OCC
		
		for (i = 0; i < 5; i++)
		{
			m_wildDataArray[i] = NULL;
			
			if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
			{
				m_wildDataArray[i] = new AtsWildData(entityKey, m_locationKey, i);
			}
		}

		// Power
		std::map<unsigned long, std::vector<TA_IRS_Core::IAtsPowerData*> > atsPowerData;

		if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
		{
			try
			{
				atsPowerData = TA_IRS_Core::AtsPowerDataFactory::getInstance().getAllPowerDataBySubsection();
			}
			catch (TA_Base_Core::DatabaseException& e)
			{
				std::string text = "DataManager() - error when retrieving Power configuration for all stations - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

				atsPowerData.clear();
			}
			catch (TA_Base_Core::DataException& e)
			{
				std::string text = "DataManager() - error when retrieving Power configuration for all stations - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
			
				atsPowerData.clear();
			}
		}
		else if (m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
				 m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
				 m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
				 m_locationType == TA_Base_Core::AtsAgentEntityData::Depot)
		{
			try
			{
				atsPowerData = TA_IRS_Core::AtsPowerDataFactory::getInstance().getAllPowerDataAtLocationBySubsection(m_locationKey);
			}
			catch (TA_Base_Core::DatabaseException& e)
			{
				std::string text = "DataManager() - error when retrieving Power configuration for this station - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

				atsPowerData.clear();
			}
			catch (TA_Base_Core::DataException& e)
			{
				std::string text = "DataManager() - error when retrieving Power configuration for this station - " + std::string(e.what());
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
				
				atsPowerData.clear();
			}
		}

		std::map<unsigned long, std::vector<TA_IRS_Core::IAtsPowerData*> >::iterator powerIt;

		for (powerIt = atsPowerData.begin(); 
			 powerIt != atsPowerData.end(); 
			 powerIt++)
		{
			std::map < unsigned short, PowerData* >::iterator it
				= m_powerDataMap.find((unsigned short)(*powerIt).first);

			if(it != m_powerDataMap.end())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
							"Configuration Error: Duplicate Electrical Subsection defined"));
			}

			PowerData* esRecord = NULL;

			try
			{
				esRecord = new PowerData(m_locationType, 
										 (unsigned short)(*powerIt).first, (*powerIt).second, 
										 *m_dataPointHelper);
				m_powerDataMap[(unsigned short)(*powerIt).first] = esRecord;
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",  "DataManager() - error adding power data to map");

				if (esRecord != NULL)
				{
					delete esRecord;
					esRecord = NULL;
				}
			}
		}

		if (m_powerDataMap.size() == 0)
		{
			TA_THROW(TA_Base_Core::FatalConfigurationException(
						"Configuration Error: No valid electrical subsection records defined"));
		}

		// Allocate the map of entities to the tables
		TA_IRS_Bus::PhysicalTrainNumberType tempCctvPhysicalTrainNumber = 0;

		//first check AlarmAgent is available or not, added by hongzhi
		bool isAlarmAgentAvailable = false;
        try
        {
            // use the getAlarm to detect if alarmAgent is available
            TA_Base_Core::AlarmDetailCorbaDef* alarm = 
                TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().getActiveAlarmForEntityOfType( TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm,
                                                              m_entityData->getKey(),
                                                              m_entityData->getLocation() );
			isAlarmAgentAvailable = true;
		}
		catch (TA_Base_Core::ObjectResolutionException& te)
		{
			isAlarmAgentAvailable = false;
			LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", te.what() );
		}
		catch (TA_Base_Core::TransactiveException& te)
        {
			isAlarmAgentAvailable = true;
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
        }
        catch (const TA_Base_Core::AlarmAgentCorbaDef::AlarmException& ae)
        {
			isAlarmAgentAvailable = true;
            // means the alarm wasnt found
            LOG_EXCEPTION_CATCH( SourceInfo, "AlarmAgentException", ae.reason.in() );
        }
        catch( const CORBA::Exception& e )
		{
			isAlarmAgentAvailable = false;
			std::string text = "closeAlarm()" 
								+ TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", text );
		}
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "while try to detect alarmagent exist", "..." );
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "isAlarmAgentAvailable=%d", isAlarmAgentAvailable);

		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator it; 
		for (it = entityMap->begin(); it != entityMap->end(); ++it)
		{
			TA_ASSERT((*it).second != NULL, "IEntity* in entity map is NULL");
			
			TA_Base_Bus::DataPoint* dataPoint = NULL;
			try
			{
				dataPoint = dynamic_cast<TA_Base_Bus::DataPoint *>((*it).second);
			}
			catch (...)
			{
			}
			
			if (dataPoint != NULL)
			{
				std::string address = dataPoint->getDataPointAddress();
					
				// Find the table which owns the datapoint
				
				// Platform - this is only PSD Data Points
				if (AddressUtil::caseInsensitiveCompare(AddressUtil::getTableName(address),
														PlatformTable::getStaticType()))
				{
					TA_IRS_Bus::PlatformIdType platformId = (TA_IRS_Bus::PlatformIdType) AddressUtil::getTableRecordKey(address);

					PsdDataMapIt psdIt
						= m_psdDataMap.find(platformId);

					if (psdIt != m_psdDataMap.end() && (*it).second != NULL)
					{
						(*psdIt).second->attachDataPoint(dataPoint);
					}
				}
				else
				{
					// Try the WatchDog
					m_watchDogData->attachDataPoint(dataPoint);
				}
			}
			else
			{
				TrainCctvCamera* cctvCamera = NULL;
				try
				{
					cctvCamera = dynamic_cast<TrainCctvCamera*>((*it).second);
				}
				catch (...)
				{
				}

				if (cctvCamera != NULL)
				{
					CctvDataMapIt cctvIt
						= m_cctvDataMap.find(cctvCamera->getPhysicalTrainNumber());

					if (cctvIt != m_cctvDataMap.end())
					{
						(*cctvIt).second->attachCamera(cctvCamera, isAlarmAgentAvailable);
					}
				}
			}
		}

		//TD 16335
		//zhou yuan++
		//create the platformId to stationId(location) map
		TA_IRS_Core::IAtsPlatforms platforms;
		
		try
		{
			platforms = TA_IRS_Core::AtsPlatformAccessFactory::getInstance().getAllAtsPlatforms();
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "creating m_platformIdToStationMap - error when retrieving platforms for all stations - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
			
			platforms.clear();
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "creating m_platformIdToStationMap - error when retrieving platforms for all stations - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
			
			platforms.clear();
		}
		
		if (platforms.size() == 0)
		{
			TA_THROW(TA_Base_Core::FatalConfigurationException(
				"Configuration Error: No Platforms Defined"));
		}
		
		for (TA_IRS_Core::IAtsPlatformsIt platformIt = platforms.begin(); platformIt != platforms.end(); platformIt++)
		{
			m_platformIdToStationMap[(*platformIt)->getPlatformId()] = (*platformIt)->getStationId();
			delete *platformIt;
		}
		//++zhou yuan

		if(m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
		   m_locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
		   m_locationType == TA_Base_Core::AtsAgentEntityData::MainStation) 
		{
			std::vector< AtsPsdData* > psdData;
			PsdDataMapIt psdIt;
			
			for (psdIt = m_psdDataMap.begin(); psdIt != m_psdDataMap.end(); psdIt++)
			{
				psdData.push_back(psdIt->second);
			}
			
			m_dpControlStateProcessor = new DataPointControlStateProcessor();

			m_dpControlStateProcessor->InitialisePsdData(psdData);

			m_dpControlStateProcessor->start();
		}
		

		// Register this object with the naming service
		this->activateServantWithName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));

		start();

		FUNCTION_EXIT;
	} 

    /**
      * Destructor
      */
	DataManager::~DataManager()
	{
		FUNCTION_ENTRY("~DataManager()");

		terminateAndWait();

		if (m_dpControlStateProcessor != NULL)
		{
			m_dpControlStateProcessor->terminateAndWait();
			delete m_dpControlStateProcessor;
			m_dpControlStateProcessor = NULL;
		}

		delete m_atsTrainDataTimer;
		m_atsTrainDataTimer = NULL;

		// Clean up the DataPointHelper
		delete m_dataPointHelper;
		m_dataPointHelper = NULL;

		// Clean up the OCC Update Processor
		delete m_occUpdateProcessor;
		m_occUpdateProcessor = NULL;

		FUNCTION_EXIT;
	}

	//
	//TA_Core::IAtsCorbaDef Implementation
	//

	/**
      * getEcsInformationList
      *
      * This method is used to access the full list of ECS information
	  * and will typlically used to synchronise at startup of the client
	  * application
      *
	  * @return A sequence of AtsEcsMessageCorbaDef for all trains known
	  *			to the ATS agent.
      */
	TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList* DataManager::getEcsInformationList()
	{
		FUNCTION_ENTRY("getEcsInformationList()");
		
		if (m_operationModeManager.inControlMode() == false)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "getEcsInformationList() - in monitor mode");

			throw TA_Base_Core::OperationModeException();
		}

		std::vector<TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef> ecsVector;
		EcsDataMapIt it;

		for (it=m_ecsDataMap.begin(); it != m_ecsDataMap.end(); it++)
		{
			TA_Base_Core::ThreadGuard guard(*(*it).second);

			ecsVector.push_back((*it).second->buildAtsEcsMessage());
		}

		TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList* ecsList = new TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList;
		ecsList->length(ecsVector.size());

		std::vector<TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef>::iterator evIt;
		unsigned int i = 0;

		for (evIt = ecsVector.begin(), i = 0; evIt != ecsVector.end(); evIt++, i++)
		{
			(*ecsList)[i] = *evIt;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "getEcsInformationList() - %d AtsEcsMessageCorbaDef records retrieved",
					ecsVector.size());

		FUNCTION_EXIT;
		return ecsList;
	};

	/**
      * processOccEcsUpdate
      *
      * This method is used to allow the OCC ATS Agent to pass on ECS
	  * information
      *
	  * @param	ecsUpdate	A sequence of AtsEcsMessageCorbaDef for updates  
	  *						to trains for the sector
      */
	void DataManager::processOccEcsUpdate(const TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList& ecsUpdate)
	{
		FUNCTION_ENTRY("processOccEcsUpdate()");
		
		if (m_operationModeManager.inControlMode() == false)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"processOccEcsUpdate() - in monitor mode");
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException();
		}

		if (ensureNormalMode())
		{
			localProcessOccEcsUpdate(ecsUpdate);
		}

		processUpdates();

		FUNCTION_EXIT;
	}

	void DataManager::localProcessOccEcsUpdate(const TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList& ecsUpdate)
	{
		unsigned int i;

		for (i = 0; i < ecsUpdate.length(); i++)
		{
			try
			{
				AtsEcsData& ecsData = getEcsData(ecsUpdate[i].physicalTrainNumber);
				TA_Base_Core::ThreadGuard guard(ecsData);

				ecsData.importAtsEcsMessage(ecsUpdate[i]);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", e.what());
			}
		}
	}

	/**
      * getWildInformationList
      *
      * This method is used to access the full list of WILD information
	  * and will typlically used to synchronise at startup of the client
	  * application
      *
	  * @return A sequence of AtsWildMessageCorbaDef for all trains known
	  *			to the ATS agent for WILD.
      */
	TA_IRS_Bus::IAtsWildCorbaDef::WildInfoList* DataManager::getWildInformationList()
	{
		FUNCTION_ENTRY("DataManager::getWildInformationList()");

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		TA_IRS_Bus::IAtsWildCorbaDef::WildInfoList* wildList = NULL;
			
		wildList = new TA_IRS_Bus::IAtsWildCorbaDef::WildInfoList;

		if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
		{
			wildList->length(5);

			for (unsigned int i=0;i < 5;i++)
			{
				TA_Base_Core::ThreadGuard guard(*m_wildDataArray[i]);

				(*wildList)[i] = m_wildDataArray[i]->buildAtsWildMessage();
			}
		}
		else
		{
			wildList->length(0);
		}

		FUNCTION_EXIT;
		return wildList;
	};


	/**
      * getTrainInformationList
      *
      * This method is used to access the full list of Train information
	  * and will typlically used to synchronise at startup of the client
	  * application
      *
	  * @return A sequence of AtsTrainMessageCorbaDef for all trains known
	  *			to the ATS agent.
      */
	TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList* DataManager::getTrainInformationList()
	{
		FUNCTION_ENTRY("getTrainInformationList()");

		if (m_operationModeManager.inControlMode() == false)
		{
			std::string message = "Agent in Monitor Mode";
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"getTrainInformationList() - %s",
				message.c_str());
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException(message.c_str());
		}

		std::vector<TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef> trainVector;
		TrainDataMapIt it;

		for (it=m_trainDataMap.begin(); it != m_trainDataMap.end(); it++)
		{
			TA_Base_Core::ThreadGuard guard(*(*it).second);

			trainVector.push_back((*it).second->buildAtsTrainMessage());
		}

		TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList* trainList = new TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList;
		trainList->length(trainVector.size());

		std::vector<TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef>::iterator tvIt;
		unsigned int i = 0;

		for (tvIt = trainVector.begin(), i = 0; tvIt != trainVector.end(); tvIt++, i++)
		{
			(*trainList)[i] = *tvIt;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "getTrainInformationList() - %d AtsTrainMessageCorbaDef records retrieved",
					trainVector.size());

		FUNCTION_EXIT;
		return trainList;
	};

	/**
      * processOccTrainUpdate
      *
      * This method is used to allow the OCC ATS Agent to pass on Train
	  * information
      *
	  * @param	trainUpdate	A sequence of AtsTrainMessageCorbaDef for updates  
	  *						to trains for the sector
      */
	void DataManager::processOccTrainUpdate(const TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList& trainUpdate)
	{
		FUNCTION_ENTRY("processOccTrainUpdate()");

		if (m_operationModeManager.inControlMode() == false)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 	"processOccTrainUpdate() - in monitor mode");
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException();
		}

		if (ensureNormalMode())
		{
			localProcessOccTrainUpdate(trainUpdate);
		}

		processUpdates();

		FUNCTION_EXIT;
	}

	void DataManager::localProcessOccTrainUpdate(const TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList& trainUpdate)
	{
		unsigned int i;

		for (i = 0; i < trainUpdate.length(); i++)
		{
			try
			{
				AtsTrainData& trainData = getTrainData(trainUpdate[i].physicalTrainNumber);
				TA_Base_Core::ThreadGuard guard(trainData);

				trainData.importAtsTrainMessage(trainUpdate[i]);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", e.what());
			}
		}
	}

	/**
      * sendOa1TableToAts
      *
      * This method is used to send OA1 tables to ATS
      *
	  * @param physicalTrainNumber	The identifier of the train that
	  *								originated the OA1 table
	  * @param oa1					A 24 byte OA1 table to send to ATS
      */
	void DataManager::sendOa1TableToAts(	TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, 
											const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& oa1)
	{
		FUNCTION_ENTRY("sendOa1TableToAts()");

		if (m_operationModeManager.inControlMode() == false)
		{
			std::string message = "Agent in Monitor Mode";
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"sendOa1TableToAts() - %s",
				message.c_str());
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException(message.c_str());
		}

		try
		{
			AtsTrainData& trainData = getTrainData(physicalTrainNumber);
			TA_Base_Core::ThreadGuard guard(trainData);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"sendOa1TableToAts() - OA1 Table received for train %d",
					physicalTrainNumber);

			if (trainData.isValidForExport(m_locationKey, m_locationType))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"sendOa1TableToAts() - Train record for train %d is valid for this location",
					physicalTrainNumber);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"sendOa1TableToAts() - Train record for train %d is NOT VALID for this location",
					physicalTrainNumber);
			}

			trainData.updateOa1Table(oa1);
		}
		catch (TA_Base_Core::AtsAgentException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", e.what());

			throw TA_Base_Core::NotAvailableException("ISCS Train Record not available");
		}
	};

	/**
      * sendAtcTrainStatusReportToAts
      *
      * This method is used to send Atc Alarm tables to ATS
      *
	  * @param physicalTrainNumber		The identifier of the train that
	  *									originated the OA1 table
	  *	@param atcTrainStatusCodeHeader	A code header for the Atc data
	  * @param atcTrainStatus			A 22 byte Atc data table to send to ATS
      */
	void DataManager::sendAtcTrainStatusReportToAts(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber, 
												     const TA_IRS_Bus::IAtsTrainCorbaDef::OctetSequence& atcTrainStatus)
	{
		FUNCTION_ENTRY("sendAtcAlarmTableToAts()");

		if (m_operationModeManager.inControlMode() == false)
		{
			std::string message = "Agent in Monitor Mode";
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"sendAtcAlarmTableToAts() - %s",
				message.c_str());
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException(message.c_str());
		}

		try
		{
			AtsTrainData& trainData = getTrainData(physicalTrainNumber);
			TA_Base_Core::ThreadGuard guard(trainData);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "sendAtcTrainStatusReportToAts() - ATC Train Status Report received for train %d",
					physicalTrainNumber);

			if (trainData.isValidForExport(m_locationKey, m_locationType))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "sendAtcTrainStatusReportToAts() - Train record for train %d is valid for this location",
					physicalTrainNumber);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "sendAtcTrainStatusReportToAts() - Train record for train %d is NOT VALID for this location",
					physicalTrainNumber);
			}

			trainData.updateAtcTrainStatusReport(atcTrainStatus);
		}
		catch (TA_Base_Core::AtsAgentException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", e.what());

			throw TA_Base_Core::NotAvailableException("ISCS ATC Data Table Record not available");
		}
	};

	/**
      * getTisInformationList
      *
      * This method is used to access the full list of TIS information
	  * and will typlically used to synchronise at startup of the client
	  * application
      *
	  * @return A sequence of AtsTisMessageCorbaDef for all platforms known
	  *			to the ATS agent. There will be two AtsTisMessageCorbaDef in
	  *			the sequence for each platform - one for platform information,
	  *			one for train information.
      */
	TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList* DataManager::getTisInformationList()
	{
		FUNCTION_ENTRY("getTisInformationList()");
	
		if (m_operationModeManager.inControlMode() == false)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "getTisInformationList() - in monitor mode");

			throw TA_Base_Core::OperationModeException();
		}

		std::vector<TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef> tisVector;
		TisDataMapIt it;

		for (it=m_tisDataMap.begin(); it != m_tisDataMap.end(); it++)
		{
			TA_Base_Core::ThreadGuard guard(*(*it).second);

			tisVector.push_back((*it).second->buildAtsTisMessage(true));
			tisVector.push_back((*it).second->buildAtsTisMessage(false));
		}

		TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList* tisInfoList = new TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList;
		tisInfoList->length(tisVector.size());

		std::vector<TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef>::iterator tvIt;
		unsigned int i = 0;

		for (tvIt = tisVector.begin(), i = 0; tvIt != tisVector.end(); tvIt++, i++)
		{
			(*tisInfoList)[i] = *tvIt;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "getTisInformationList() - %d AtsTisMessageCorbaDef records retrieved",
					tisVector.size());

		FUNCTION_EXIT;
		return tisInfoList;
	};

	/**
      * processOccTisUpdate
      *
      * This method is used to allow the OCC ATS Agent to pass on TIS
	  * information
      *
	  * @param	tisUpdate	A sequence of AtsTisMessageCorbaDef for updates  
	  *						to platforms for the station
      */
	void DataManager::processOccTisUpdate(const TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList& tisUpdate)
	{
		FUNCTION_ENTRY("processOccTisUpdate()");

		if (m_operationModeManager.inControlMode() == false)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 	"processOccTisUpdate() - in monitor mode");
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException();
		}

		if (ensureNormalMode())
		{
			localProcessOccTisUpdate(tisUpdate);
		}

		processUpdates();

		FUNCTION_EXIT;
	}

	void DataManager::localProcessOccTisUpdate(const TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList& tisUpdate)
	{
		unsigned int i;

		for (i = 0; i < tisUpdate.length(); i++)
		{
			try
			{
				AtsTisData& tisData = getTisData(tisUpdate[i].platformId);
				TA_Base_Core::ThreadGuard guard(tisData);

				tisData.importAtsTisMessage(tisUpdate[i]);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", e.what());
			}
		}
	}

	/**
      * getPasInformationList
      *
      * This method is used to access the full list of PAS information
	  * and will typlically used to synchronise at startup of the client
	  * application
      *
	  * @return A sequence of AtsPasMessageCorbaDef for all trains known
	  *			to the ATS agent.
      */
	TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* DataManager::getPasInformationList()
	{
		FUNCTION_ENTRY("getPasInformationList()");

		if (m_operationModeManager.inControlMode() == false)
		{
			std::string message = "Agent in Monitor Mode";
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"getPasInformationList() - %s",
				message.c_str());
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException(message.c_str());
		}

		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> pasVector;
		PasDataMapIt it;

		for (it=m_pasDataMap.begin(); it != m_pasDataMap.end(); it++)
		{
			TA_Base_Core::ThreadGuard guard(*(*it).second);

			std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> pasMessageVector = 
								(*(*it).second).buildAtsPasMessageState();

			std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>::iterator pasMessageIt;

			for (pasMessageIt = pasMessageVector.begin(); pasMessageIt != pasMessageVector.end(); pasMessageIt++)
			{
				pasVector.push_back(*pasMessageIt);
			}
		}

		TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* pasList = new TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList;
		pasList->length(pasVector.size());

		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>::iterator tvIt;
		unsigned int i = 0;

		for (tvIt = pasVector.begin(), i = 0; tvIt != pasVector.end(); tvIt++, i++)
		{
			(*pasList)[i] = *tvIt;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "getPasInformationList() - %d AtsPasMessageCorbaDef records retrieved",
					pasVector.size());

		FUNCTION_EXIT;
		return pasList;
	};

	/**
      * processOccPasUpdate
      *
      * This method is used to allow the OCC ATS Agent to pass on PAS
	  * updates
      *
	  * @param	pasUpdate	A sequence of AtsPasMessageCorbaDef for updates  
	  *						to platforms for the station
      */
	void DataManager::processOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate)
	{
		FUNCTION_ENTRY("processOccPasUpdate()");
		
		if (m_operationModeManager.inControlMode() == false)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"processOccPasUpdate() - in monitor mode");
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException();
		}

		//if (ensureNormalMode()) //limin--, CL-20680
		{
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "calling localProcessOccPasUpdate, will trigger location pa broadcast" );
			localProcessOccPasUpdate(pasUpdate);
		}

		processUpdates();

		FUNCTION_EXIT;
	}

	void DataManager::localProcessOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate)
	{
		unsigned int i;

		for (i = 0; i < pasUpdate.length(); i++)
		{
			try
			{
				AtsPasData& pasData = getPasData(pasUpdate[i].platformId);
				TA_Base_Core::ThreadGuard guard(pasData);

				pasData.importAtsPasMessage(pasUpdate[i]);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", e.what());
			}
		}
	}

	/**
      * getCctvInformationList
      *
      * This method is used to access the full list of CCTV information
	  * and will typlically used to synchronise at startup of the client
	  * application
      *
	  * @return A sequence of AtsCctvMessageCorbaDef for all trains known
	  *			to the ATS agent.
      */
	TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList* DataManager::getCctvInformationList()
	{
		FUNCTION_ENTRY("getCctvInformationList()");

		if (m_operationModeManager.inControlMode() == false)
		{
			std::string message = "Agent in Monitor Mode";
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"getCctvInformationList() - %s",
				message.c_str());
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException(message.c_str());
		}

		std::vector<TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef> cctvVector;
		CctvDataMapIt it;

		for (it=m_cctvDataMap.begin(); it != m_cctvDataMap.end(); it++)
		{
			TA_Base_Core::ThreadGuard guard(*(*it).second);

			cctvVector.push_back((*it).second->buildAtsCctvMessage());
		}

		TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList* cctvList = new TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList;
		cctvList->length(cctvVector.size());

		std::vector<TA_IRS_Bus::IAtsCctvCorbaDef::AtsCctvMessageCorbaDef>::iterator tvIt;
		unsigned int i = 0;

		for (tvIt = cctvVector.begin(), i = 0; tvIt != cctvVector.end(); tvIt++, i++)
		{
			(*cctvList)[i] = *tvIt;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "getCctvInformationList() - %d AtsCctvMessageCorbaDef records retrieved",
					cctvVector.size());

		FUNCTION_EXIT;
		return cctvList;
	};

	OperationModeManager& DataManager::getOperationModeManager()
	{
		return m_operationModeManager;
	}

	/**
      * processOccCctvUpdate
      *
      * This method is used to allow the OCC ATS Agent to pass on CCTV
	  * information
      *
	  * @param	cctvUpdate	A sequence of AtsCctvMessageCorbaDef for updates  
	  *						to trains for the sector
      */
	void DataManager::processOccCctvUpdate(const TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList& cctvUpdate)
	{
		FUNCTION_ENTRY("processOccCctvUpdate()");

		if (m_operationModeManager.inControlMode() == false)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 	"processOccCctvUpdate() - in monitor mode");
			FUNCTION_EXIT;

			throw TA_Base_Core::OperationModeException();
		}

		if (ensureNormalMode())
		{
			localProcessOccCctvUpdate(cctvUpdate);
		}

		processUpdates();

		FUNCTION_EXIT;
	}

	void DataManager::localProcessOccCctvUpdate(const TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList& cctvUpdate)
	{
		unsigned int i;

		for (i = 0; i < cctvUpdate.length(); i++)
		{
			try
			{
				AtsCctvData& cctvData = getCctvData(cctvUpdate[i].physicalTrainNumber);
				TA_Base_Core::ThreadGuard guard(cctvData);

				cctvData.importAtsCctvMessage(cctvUpdate[i]);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException",e.what());
			}
		}
	}

	//
	// Thread functions
	//

	void DataManager::run()
    {
        unsigned int flags = 0;
		int failCount = 0;

		// Initialise timer
		time(&m_occUpdateTime);

        while (m_keepRunning)
        {
			// Wait for something to process
			if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
				m_locationType == TA_Base_Core::AtsAgentEntityData::Depot)
			{
				m_semaphore.wait();
			}
			else
			{
				while (m_semaphore.tryWait() == false)
				{
					if (m_operationModeManager.inControlMode())
					{
						// Check to see if we have failed to receive an OCC Update for 5 cycles
						if (m_operationModeManager.getAgentState() != OperationModeManager::DoubleFailure )
						{
							LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"hongzhi1, received updates");
							if (time(NULL) > (m_occUpdateTime + (6 * 5 * 4)))
							{
								LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Not received info from OccAtsAgent for 5 cycles(20seconds), Enter DoubleFailure");
								m_operationModeManager.setOccAtsState(OperationModeManager::DoubleFailure);
							}
						}
						else
						{
							// Initialise timer
							time(&m_occUpdateTime);
						}
					}
					else
					{
						// Initialise timer
						time(&m_occUpdateTime);
					}

					Thread::sleep(400);
				}
			}

			// Watchdog
			{
				TA_ASSERT(m_watchDogData != NULL, "WatchDogData is NULL");

				try
				{
					m_watchDogData->sendUpdates();
				}
				catch (...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
										"run() - Exception caught while processing WatchDog updates");
				}
			}

			typedef std::map<TA_IRS_Bus::PhysicalTrainNumberType, AtsCctvData*> CctvDataMap;
		
			// CCTV
			{
				CctvDataMapIt cctvIt;

				for (cctvIt = m_cctvDataMap.begin(); cctvIt != m_cctvDataMap.end(); cctvIt++)
				{
					if ((*cctvIt).second != NULL)
					{
						TA_Base_Core::ThreadGuard guard(*(*cctvIt).second);
						
						if (m_occUpdateProcessor != NULL &&
							m_locationType == TA_Base_Core::AtsAgentEntityData::Occ &&
							(*cctvIt).second->isUpdated())
						{
							m_occUpdateProcessor->addCctvUpdate(
								(*cctvIt).second->buildAtsCctvMessage());
						}

						try
						{
							(*cctvIt).second->sendUpdates();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
												"run() - Exception caught while processing CCTV updates");
						}
					}
				}
			}

			// ECS
			{
				EcsDataMapIt ecsIt;

				for (ecsIt = m_ecsDataMap.begin(); ecsIt != m_ecsDataMap.end(); ecsIt++)
				{
					if ((*ecsIt).second != NULL)
					{
						TA_Base_Core::ThreadGuard guard(*(*ecsIt).second);

						if (m_occUpdateProcessor != NULL &&
							m_locationType == TA_Base_Core::AtsAgentEntityData::Occ &&
							(*ecsIt).second->isUpdated())
						{
							m_occUpdateProcessor->addEcsUpdate(
								(*ecsIt).second->buildAtsEcsMessage());
						}

						try
						{
							(*ecsIt).second->sendUpdates();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
												"run() - Exception caught while processing ECS updates");
						}
					}
				}
			}
		
			// Train
			{
				TrainDataMapIt trainIt;

				for (trainIt = m_trainDataMap.begin(); trainIt != m_trainDataMap.end(); trainIt++)
				{
					if ((*trainIt).second != NULL)
					{
						TA_Base_Core::ThreadGuard guard(*(*trainIt).second);

						if (m_occUpdateProcessor != NULL &&
							m_locationType == TA_Base_Core::AtsAgentEntityData::Occ &&
							(*trainIt).second->isUpdated())
						{
							m_occUpdateProcessor->addTrainUpdate(
								(*trainIt).second->buildAtsTrainMessage());
						}

						try
						{
							(*trainIt).second->sendUpdates();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
												"run() - Exception caught while processing Train updates");
						}
					}
				}
			}

			// PA
			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				PasDataMapIt pasIt;

				for (pasIt = m_pasDataMap.begin(); pasIt != m_pasDataMap.end(); pasIt++)
				{
					TA_Base_Core::ThreadGuard guard(*(*pasIt).second);

					if ((*pasIt).second != NULL)
					{
						TA_Base_Core::ThreadGuard guard(*(*pasIt).second);

						if (m_occUpdateProcessor != NULL &&
							m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
						{
							std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> pasMessageVector = 
								(*pasIt).second->buildAtsPasMessageVector();

							std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>::iterator pasMessageIt;

							for (pasMessageIt = pasMessageVector.begin(); pasMessageIt != pasMessageVector.end(); pasMessageIt++)
							{
								m_occUpdateProcessor->addPasUpdate(*pasMessageIt);
							}
						}

						try
						{
							(*pasIt).second->processUpdates();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
												"run() - Exception caught while processing PA updates");
						}
					}
				}
			}

			// TIS
			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Depot)
			{
				TisDataMapIt tisIt;

				for (tisIt = m_tisDataMap.begin(); tisIt != m_tisDataMap.end(); tisIt++)
				{
					if ((*tisIt).second != NULL)
					{
						TA_Base_Core::ThreadGuard guard(*(*tisIt).second);

						if (m_occUpdateProcessor != NULL &&
							m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
						{
							if ((*tisIt).second->isPlatformUpdated())
							{
								m_occUpdateProcessor->addTisUpdate(
									(*tisIt).second->buildAtsTisMessage(true));
							}

							if ((*tisIt).second->isTrainUpdated())
							{
								m_occUpdateProcessor->addTisUpdate(
									(*tisIt).second->buildAtsTisMessage(false));
							}
						}

						try
						{
							(*tisIt).second->processUpdates();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
												"run() - Exception caught while processing TIS updates");
						}
					}
				}
			}

			// PSD
			if (m_locationType != TA_Base_Core::AtsAgentEntityData::Depot && 
				m_locationType != TA_Base_Core::AtsAgentEntityData::Occ)
			{
				PsdDataMapIt psdIt;

				for (psdIt = m_psdDataMap.begin(); psdIt != m_psdDataMap.end(); psdIt++)
				{
					if ((*psdIt).second != NULL)
					{
						TA_Base_Core::ThreadGuard guard(*(*psdIt).second);

						try
						{
							(*psdIt).second->processUpdates();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
												"run() - Exception caught while processing PSD updates");
						}
					}
				}
			}
			// WILD
			if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
			{
				unsigned int i;

				for(i = 0; i < 5; i++)
				{
					if (m_wildDataArray[i] != NULL)
					{
						TA_Base_Core::ThreadGuard guard(*m_wildDataArray[i]);

						m_wildDataArray[i]->sendUpdates();
					}
				}
			}

			// DSS
			{
				TA_Base_Core::ThreadGuard guard(m_dssGuard);
				for(std::vector<AtsDssData>::iterator it = m_dssDataArray.begin(); it != m_dssDataArray.end(); ++it)
				{
					triggerDssPlan(it->dssEvent, it->dssInput);
				}
				m_dssDataArray.clear();			//TD18590, hongzhi
			}


            // TWP
            /*if (m_locationType == TA_Base_Core::AtsAgentEntityData::Depot)
			{
				m_twpData->sendUpdates();
			}
			*/

			//libo 
            if (m_locationType == TA_Base_Core::AtsAgentEntityData::Depot)// && m_operationModeManager.getAgentState() == OperationModeManager::AgentNormal)
			{
				bool bAtsLink = m_watchDogData->isAtsOk();
				if(!bAtsLink)
				{
					m_bAtsRecoverLink = false;
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD19853 Ats Link Failed");
				}
				else
				{

					if(!m_bAtsRecoverLink)
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD19853 Ats Link Recover,Resend all command");
						m_bAtsRecoverLink = true;
						m_twpData->ReSendUpdata();
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD19853 Ats Link Update Command");
						m_twpData->sendUpdates();
					}
				}
			}
			//libo 
			if (m_occUpdateProcessor != NULL &&
				m_locationType == TA_Base_Core::AtsAgentEntityData::Occ)
			{
				m_occUpdateProcessor->sendOccUpdates();
			}
        }

		m_keepRunning = true;
    }

	void DataManager::terminate()
    {
        m_keepRunning = false;
		m_semaphore.post();
    }

	void DataManager::processUpdates()
	{
        m_semaphore.post();
	}

	void DataManager::updatePSDWhenDoubleATSFail()
	{	//TD17760, when ATS Double Fail, now update PSD Datapoint State to Not connected
		if (m_locationType != TA_Base_Core::AtsAgentEntityData::Depot && 
			m_locationType != TA_Base_Core::AtsAgentEntityData::Occ)
		{
			PsdDataMapIt psdIt;
			
			for (psdIt = m_psdDataMap.begin(); psdIt != m_psdDataMap.end(); psdIt++)
			{
				if ((*psdIt).second != NULL)
				{
					TA_Base_Core::ThreadGuard guard(*(*psdIt).second);
					
					try
					{
						(*psdIt).second->processUpdatesWhenDoubleATSFail();
					}
					catch (...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
							"run() - Exception caught while processing PSD updates");
					}
				}
			}
		}
	}	
	//
	// Protected functions
	//

	void DataManager::triggerDssPlan(const unsigned long event, const std::string& input)
	{
		std::string planPath = "";
		bool planExecuted = false;
		
		try
		{
			planPath = m_dssPlanMap->getPlanPath(event);
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "sendUpdates() - error retrieving plan path - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "sendUpdates() - error retrieving plan path - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
		}
		
		if (!planPath.empty() && planPath.find_first_not_of(" \t") != std::string::npos)
		{
			bool retry = false;
			unsigned int attempt = 0;
			
			do
			{
				attempt++;
				
				try
				{
					std::string session = TA_Base_Core::SessionAccessFactory::getInstance().getSuperSessionId();
					
					m_planAgentLibrary.runPlanByName(session.c_str(), planPath.c_str(), input.c_str());
					
					planExecuted = true;
					retry = false;
				}
				catch (...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "sendUpdates() - Error: Plan execution failed");
					
					retry = (attempt < 2);
				}
			}
			while (retry);
		}
		
		if (!planExecuted)
		{
			// Raise an alarm
			TA_Base_Core::DescriptionParameters dp;
			std::stringstream dssEvent, dssInput;
			dssEvent << event;
			dssInput << input;
			dp.push_back(new TA_Base_Core::NameValuePair("AtsAgentName", m_entityData->getName()));
			dp.push_back(new TA_Base_Core::NameValuePair("DssEvent", dssEvent.str()));
			dp.push_back(new TA_Base_Core::NameValuePair("DssInput", dssInput.str()));
			
			
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::AtsAgentAlarms::AtsDssFailedToExecutePlan,
				m_entityData->getKey(),			// entity key
				m_entityData->getTypeKey(),		// entity type key
				dp,
				m_entityData->getName(),			// entity name
				m_entityData->getLocation(),		// location key
				m_entityData->getSubsystem(),	// subystem key
				m_entityData->getAssetName(),	// asset name
				TA_Base_Core::AlarmConstants::defaultTime);
			
		}
	}

	void DataManager::createOccPlatformData(unsigned long entityKey)
	{
		TA_IRS_Core::IAtsPlatforms platforms;

		try
		{
			platforms = TA_IRS_Core::AtsPlatformAccessFactory::getInstance().getAllAtsPlatforms();
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			std::string text = "createOccPlatformData() - error when retrieving platforms for all stations - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
		
			platforms.clear();
		}
		catch (TA_Base_Core::DataException& e)
		{
			std::string text = "createOccPlatformData() - error when retrieving platforms for all stations - " + std::string(e.what());
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
			
			platforms.clear();
		}

		if (platforms.size() == 0)
		{
			TA_THROW(TA_Base_Core::FatalConfigurationException(
						"Configuration Error: No Platforms Defined"));
		}

		for (TA_IRS_Core::IAtsPlatformsIt platformIt = platforms.begin(); platformIt != platforms.end(); platformIt++)
		{
			TA_IRS_Bus::PlatformIdType platformId
				= (*platformIt)->getPlatformId();
			unsigned long stationId 
				= (*platformIt)->getStationId();

            TA_IRS_Bus::TisPlatformIdType tisPlatformId
				= (*platformIt)->getTisPlatformId();


			TisDataMapIt tisIt
				= m_tisDataMap.find(platformId);
			PasDataMapIt pasIt
				= m_pasDataMap.find(platformId);
			PsdDataMapIt psdIt
				= m_psdDataMap.find(platformId);

			if (tisIt != m_tisDataMap.end() ||
				pasIt != m_pasDataMap.end() ||
				psdIt != m_psdDataMap.end())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException( 
							"Configuration Error: Duplicate Platform defined"));
			}

			AtsTisData* tisData = new AtsTisData (platformId, stationId, tisPlatformId, entityKey, m_locationKey);
			m_tisDataMap[platformId] = tisData;

			AtsPasData* pasData = new AtsPasData (platformId, stationId, entityKey, m_locationKey);
			m_pasDataMap[platformId] = pasData;
		}
	}

	void DataManager::createStationPlatformData(unsigned long entityKey)
	{		
		// Create all ATS Platform data	
		{		
			TA_IRS_Core::IAtsPSDDataVector atsPsdData;
			TA_IRS_Core::IAtsMssZones mssZones;

			try
			{
				// Get the PSD info for this MSS Zone
				mssZones = TA_IRS_Core::AtsMssZoneAccessFactory::getInstance().getAtsMssZonesWhere(false, m_locationKey);

				if (mssZones.size() == 1)
				{
					atsPsdData = TA_IRS_Core::AtsPSDDataFactory::getInstance().getAllPSDDataInMSSZone(mssZones[0]->getMssZone());
				}
				else
				{
					atsPsdData.clear();
				}
			}
			catch (TA_Base_Core::DatabaseException& e)
			{
				std::ostringstream text;
				text << "createStationPlatformData() - error when retrieving PSD configuration for station with location Id " << m_locationKey;
				text << " - " << e.what();
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text.str());

				atsPsdData.clear();
			}
			catch (TA_Base_Core::DataException& e)
			{
				std::ostringstream text;
				text << "createStationPlatformData() - error when retrieving PSD configuration for station with location Id " << m_locationKey;
				text << " - " << e.what();
				LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text.str());

				atsPsdData.clear();
			}

			for (TA_IRS_Core::IAtsMssZonesIt it = mssZones.begin();
				 it != mssZones.end();
				 it++)
			{
				if (*it != NULL)
				{
					delete (*it);
					(*it) = NULL;
				}
			}

			if (atsPsdData.size() == 0)
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
							"Configuration Error: No MSS PSD Data Defined"));
			}

			TA_IRS_Core::IAtsPSDDataVectorIt psdDataIt;

			for (psdDataIt = atsPsdData.begin(); psdDataIt != atsPsdData.end(); psdDataIt++)
			{
				TA_IRS_Bus::PlatformIdType platformId
					= (*psdDataIt)->getPlatformId();
				unsigned long stationId 
					= (*psdDataIt)->getStationId();
                TA_IRS_Bus::TisPlatformIdType tisPlatformId
					= (*psdDataIt)->getTisPlatformId();
                
				TisDataMapIt tisIt
					= m_tisDataMap.find(platformId);

				if (tisIt == m_tisDataMap.end())
				{
					AtsTisData* tisData = new AtsTisData (platformId, stationId, tisPlatformId, entityKey, m_locationKey);
					m_tisDataMap[platformId] = tisData;
				}

				PasDataMapIt pasIt
					= m_pasDataMap.find(platformId);
	
				if (pasIt == m_pasDataMap.end())
				{
					AtsPasData* pasData = new AtsPasData (platformId, stationId, entityKey, m_locationKey);
					m_pasDataMap[platformId] = pasData;
				}

				if (m_locationKey == stationId)
				{
					PsdDataMapIt psdIt
						= m_psdDataMap.find(platformId);

					AtsPsdData* psdData = NULL;
					if (psdIt == m_psdDataMap.end())
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
									"AtsPlatformRecord() - platform %d is at this location - create PsdData", 
									platformId);

						psdData = new AtsPsdData(platformId, stationId, entityKey, m_locationKey, m_locationType, *m_dataPointHelper);
						m_psdDataMap[platformId] = psdData;
					}
					else
					{
						psdData = (*psdIt).second;
					}

					if (psdData != NULL)
					{
						try
						{
							psdData->addPsdData(*psdDataIt);
						}
						catch(TA_Base_Core::FatalConfigurationException &e)
						{
							throw e;
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",  "createStationPlatformData() - error adding PSD data");
						}
					}
				}
			}
		}
		
		// Now need to check that each record has complete data
		{
			PsdDataMapIt psdIt;

			for (psdIt = m_psdDataMap.begin();
				 psdIt != m_psdDataMap.end();
				 psdIt++)
			{
				if ((*psdIt).second != NULL)
				{
					if(!(*psdIt).second->configurationComplete())
					{
						TA_THROW(TA_Base_Core::FatalConfigurationException(
									"Configuration Error: ATS Platform Data incomplete"));
					}
				}
			}
		}
	}

	WatchDogData& DataManager::getWatchDogData()
	{
		TA_ASSERT(m_watchDogData != NULL, "WatchDogData missing");

		return *m_watchDogData;
	}

	AtsEcsData& DataManager::getEcsData(const TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		EcsDataMapIt it = m_ecsDataMap.find(physicalTrainNumber);

		if (it == m_ecsDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No ECS data for train " << (unsigned long)physicalTrainNumber;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		return *((*it).second);
	}

	AtsTrainData& DataManager::getTrainData(const TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		TrainDataMapIt it = m_trainDataMap.find(physicalTrainNumber);

		if (it == m_trainDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No Train data for train " << (unsigned long)physicalTrainNumber;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		return *((*it).second);
	}

	DataManager::TrainDataMap& DataManager::getTrainDataMap()
	{
		return m_trainDataMap;
	}

	AtsCctvData& DataManager::getCctvData(const TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		CctvDataMapIt it = m_cctvDataMap.find(physicalTrainNumber);

		if (it == m_cctvDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No CCTV data for train " << (unsigned long)physicalTrainNumber;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		return *((*it).second);
	}

	AtsTisData& DataManager::getTisData(const TA_IRS_Bus::PlatformIdType platformId)
	{
		TisDataMapIt it = m_tisDataMap.find(platformId);

		if (it == m_tisDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No TIS data for platform " << (unsigned long)platformId;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		return *((*it).second);
	}

	AtsPasData& DataManager::getPasData(const TA_IRS_Bus::PlatformIdType platformId)
	{
		PasDataMapIt it = m_pasDataMap.find(platformId);

		if (it == m_pasDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No PAS data for platform " << (unsigned long)platformId;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		return *((*it).second);
	}

	AtsPsdData& DataManager::getPsdData(const TA_IRS_Bus::PlatformIdType platformId)
	{
		PsdDataMapIt it = m_psdDataMap.find(platformId);

		if (it == m_psdDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No PSD data for platform " << (unsigned long)platformId;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		return *((*it).second);
	}

	DataManager::PsdDataMap& DataManager::getPsdDataMap()
	{
		return m_psdDataMap;
	}

	void DataManager::clearTrainImportFlags()
	{
		TrainDataMapIt trainIt;

		for (trainIt = m_trainDataMap.begin(); 
			 trainIt != m_trainDataMap.end(); 
			 trainIt++)
		{
			if ((*trainIt).second != NULL)
			{
				(*trainIt).second->clearImportFlag();
			}
		}

		EcsDataMapIt ecsIt;

		for (ecsIt = m_ecsDataMap.begin(); 
			 ecsIt != m_ecsDataMap.end(); 
			 ecsIt++)
		{
			if ((*ecsIt).second != NULL)
			{
				(*ecsIt).second->clearImportFlag();
			}
		}

		CctvDataMapIt cctvIt;

		for (cctvIt = m_cctvDataMap.begin(); 
			 cctvIt != m_cctvDataMap.end(); 
			 cctvIt++)
		{
			if ((*cctvIt).second != NULL)
			{
				(*cctvIt).second->clearImportFlag();
			}
		}
	}

	void DataManager::checkIfTrainNotImported()
	{
		TrainDataMapIt trainIt;

		for (trainIt = m_trainDataMap.begin(); 
			 trainIt != m_trainDataMap.end(); 
			 trainIt++)
		{
			if ((*trainIt).second != NULL)
			{
				(*trainIt).second->checkIfTrainNotImported();
			}
		}

		EcsDataMapIt ecsIt;

		for (ecsIt = m_ecsDataMap.begin(); 
			 ecsIt != m_ecsDataMap.end(); 
			 ecsIt++)
		{
			if ((*ecsIt).second != NULL)
			{
				(*ecsIt).second->checkIfTrainNotImported();
			}
		}

		CctvDataMapIt cctvIt;

		for (cctvIt = m_cctvDataMap.begin(); 
			 cctvIt != m_cctvDataMap.end(); 
			 cctvIt++)
		{
			if ((*cctvIt).second != NULL)
			{
				(*cctvIt).second->checkIfTrainNotImported();
			}
		}
	}

	AtsTwpData& DataManager::getTwpData()
	{
		if (m_twpData == NULL)
		{
			TA_THROW(TA_Base_Core::AtsAgentException("TWP Data not available"));
		}

		return *m_twpData;
	}

	void DataManager::addNewDssEvent(std::vector< AtsDssData >& newData)
	{
		TA_Base_Core::ThreadGuard guard(m_dssGuard);
		for(std::vector<AtsDssData>::iterator it = newData.begin(); it != newData.end(); ++it)
		{
			m_dssDataArray.push_back(*it);
		}
	}

	AtsWildData& DataManager::getWildData(unsigned int index)
	{
		TA_ASSERT(index < 5, "WILD Data index out of range");

		if (m_wildDataArray[index] == NULL || m_locationType != TA_Base_Core::AtsAgentEntityData::Occ)
		{
			TA_THROW(TA_Base_Core::AtsAgentException("WILD Data not available"));
		}

		return *m_wildDataArray[index];
	}

	PowerData& DataManager::getPowerData(unsigned short subsectionId)
	{
		PowerDataMapIt it = m_powerDataMap.find(subsectionId);

		if (it == m_powerDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No Power data for subsection " << (unsigned short)subsectionId;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		return *((*it).second);
	}

	DataManager::PowerDataMap& DataManager::getPowerDataMap()
	{
		return m_powerDataMap;
	}

	bool DataManager::ensureNormalMode()
	{
		if (m_locationType == TA_Base_Core::AtsAgentEntityData::Occ || 
			m_locationType == TA_Base_Core::AtsAgentEntityData::Depot)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"DataManager::ensureNormalMode() - this is Occ or Depot, so we return");
			return false;
		}
		OperationModeManager::EAgentState agentState = m_operationModeManager.getAgentState();
		if (agentState != OperationModeManager::AgentNormal)
		{
                        time(&m_occUpdateTime); //bihui cl-21974
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"DataManager::ensureNormalMode()=%d - agent not in normal state, we return",agentState);
			// Retrieving data locally
			return false;
		}

		if (!m_operationModeManager.inOccAtsStartup())
		{
			// Already in Normal mode - just reset the timer
			time(&m_occUpdateTime);

			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"DataManager::ensureNormalMode() - in normal mode");
			// Return true to process the incoming update
			return true;
		}

		// Changing to Normal Mode
		m_operationModeManager.setOccAtsState(OperationModeManager::Normal);

		time(&m_occUpdateTime);

		try
		{
			// Now need to resynchronise with the OCC Agent
            TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList_var ecsInfoList;
            CORBA_CALL_RETURN( ecsInfoList, (*m_occAtsAgent), getEcsInformationList, () );
            localProcessOccEcsUpdate(ecsInfoList);

            TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList_var trainInfoList;
            CORBA_CALL_RETURN( trainInfoList, (*m_occAtsAgent), getTrainInformationList, () );
            localProcessOccTrainUpdate(trainInfoList);
            
            TA_IRS_Bus::IAtsTisCorbaDef::TisInfoList_var tisInfoList;
            CORBA_CALL_RETURN( tisInfoList, (*m_occAtsAgent), getTisInformationList, () );
            localProcessOccTisUpdate(tisInfoList);

            TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList_var pasInfoList;
            CORBA_CALL_RETURN( pasInfoList, (*m_occAtsAgent), getPasInformationList, () );
            localProcessOccPasUpdate(pasInfoList);

            TA_IRS_Bus::IAtsCctvCorbaDef::CctvInfoList_var cctvInfoList;
            CORBA_CALL_RETURN( cctvInfoList, (*m_occAtsAgent), getCctvInformationList, () );
            localProcessOccCctvUpdate(cctvInfoList);

			processUpdates();

			return false;
		}
		catch (...)
		{
			// Can't resynchronise - to degraded mode
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "Can't resynchronise - to DoubleFailure");
			m_operationModeManager.setOccAtsState(OperationModeManager::DoubleFailure);

			return false;
		}
	}

	CORBA::Boolean DataManager::inOccDoubleAtsFailure()
	{
		if (m_operationModeManager.inControlMode() == false)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "radioInFallback() - in monitor mode");

			throw TA_Base_Core::OperationModeException();
		}

		return CORBA::Boolean(m_operationModeManager.inDoubleAtsFailure());
	}

	void DataManager::radioInFallback(CORBA::Boolean radioFallback)
	{
		if (m_operationModeManager.inControlMode() == false)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "radioInFallback() - in monitor mode");

			throw TA_Base_Core::OperationModeException();
		}

		m_operationModeManager.inRadioFallback(radioFallback);
	}

	void DataManager::resetAts()
	{
		if (m_operationModeManager.inControlMode() == false)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "resetAts() - in monitor mode");

			throw TA_Base_Core::OperationModeException();
		}

		m_operationModeManager.resetAts();
	}

	//wenching++ (TD14526)
	void DataManager::updateControlModeStatus(bool inControl)
	{
		if ( m_watchDogData != NULL )
			m_watchDogData->updateIsInControlMode( inControl );	

		if ( m_dataPointHelper != NULL )
			m_dataPointHelper->updateIsInControlMode( inControl );
		
	}
	//++wenching (TD14526)


    void DataManager::atsServerStatusChanged(bool atsOk)
    {
        m_linkStatusThread.setLinkStatus(atsOk);

        if (false == atsOk)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "atsServerStatusChanged - invalidating all trains");

            // invalidate all valid trains
            TrainDataMapIt trainIt;

			for (trainIt = m_trainDataMap.begin(); trainIt != m_trainDataMap.end(); trainIt++)
			{
				if ( (*trainIt).second != NULL )
				{
					TA_Base_Core::ThreadGuard guard(*(*trainIt).second);

					try
					{
						(*trainIt).second->invalidateTrain();
						if (m_operationModeManager.inControlMode() == true)
						{
							(*trainIt).second->sendUpdates();
						}
					}
					catch (...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
							"atsServerStatusChanged() - Exception caught while invalidating Train");
					}
				}
			}

			TisDataMapIt tisIt;
			for( tisIt = m_tisDataMap.begin() ; tisIt != m_tisDataMap.end() ; ++tisIt )
			{
				if ( (*tisIt).second != NULL )
				{
					TA_Base_Core::ThreadGuard guard(*(*tisIt).second);
					
					try
					{
						(*tisIt).second->invalidPlatformData();
                        (*tisIt).second->processUpdates();
					}
					catch (...)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception",
							"atsServerStatusChanged() - Exception caught while invalidating platform data");
					}
				}
			}

        }
    }

	//TD15213
	void DataManager::setMAtsTwpEntityData(TA_Base_Core::AtsTwpEntityData* patstwpentitydata)
	{
		if ( NULL == m_dataPointHelper )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "setMAtsTwpEntityData fail due to m_dataPointHelper is NULL ");
			return;
		}
		
		m_dataPointHelper->setAtsTwpEntityData(patstwpentitydata);		
	}
	
	void DataManager::setMAtsPsdEntityData(TA_Base_Core::AtsPsdEntityData* patspsdentitydata)
	{
		if ( NULL == m_dataPointHelper )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "setMAtsPsdEntityData fail due to m_dataPointHelper is NULL ");
			return;
		}
		
		m_dataPointHelper->setAtsPsdEntityData(patspsdentitydata);		
	}

	//TD 16335
	//zhou yuan++
	unsigned long DataManager::getStationIdByPlatformId(unsigned long platformId)
	{
		return m_platformIdToStationMap[platformId];
	}
	//++zhou yuan

}
