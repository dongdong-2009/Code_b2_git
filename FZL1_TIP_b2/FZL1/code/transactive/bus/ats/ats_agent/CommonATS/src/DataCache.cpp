/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: g:\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\DataCache.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object maintains all data cache maps for ATS
  */

#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/ats/ats_agent/CommonATS/src/PowerData.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/data_access_interface/ats/src/AtsPowerDataFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "bus/ats/ats_agent/CommonATS/src/DataPointHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataLocation.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainPisData.h"
#include <string>


namespace TA_IRS_Bus
{
	TA_Base_Core::ReEntrantThreadLockable DataCache::m_singletonLock;
	DataCache* DataCache::m_pInstance = NULL;

	DataCache::DataCache()
		: m_dataPointHelper(NULL),
		  m_bPowerMapConfiged(false),
		  m_trainPisData(NULL),
		  m_trainLocationData(NULL)
	{
		FUNCTION_ENTRY("DataCache");
		FUNCTION_EXIT;
	}

	DataCache::~DataCache()
	{
		FUNCTION_ENTRY("~DataCache");

		if (NULL != m_dataPointHelper)
		{
			delete m_dataPointHelper;
			m_dataPointHelper = NULL;
		}

		if (NULL != m_trainLocationData)
		{
			delete m_trainLocationData;
			m_trainLocationData = NULL;
		}

		if (NULL != m_trainPisData)
		{
			delete m_trainPisData;
			m_trainPisData = NULL;
		}

		FUNCTION_EXIT;
	}

	DataCache* DataCache::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		
		if ( 0 == m_pInstance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			// Any lock is okay at this stage.
			TA_Base_Core::ThreadGuard guard( m_singletonLock );

			if ( 0 == m_pInstance )
			{
				m_pInstance = new DataCache();
			}
		}

		FUNCTION_EXIT;
		return m_pInstance;
	}

	void DataCache::initialize(TA_Base_Bus::ItaScada* itScada,TA_Base_Core::AtsAgentEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("initialize");

		m_dataPointHelper = new TA_IRS_Bus::DataPointHelper(entityData,this);
		m_trainLocationData = new TrainDataLocation(itScada,entityData);
		m_trainPisData      = new TrainPisData(itScada,entityData);
		initializePAData(entityData->getKey(),entityData->getLocation());
		initializePowerData(entityData);

		FUNCTION_EXIT;
	}

	void DataCache::releaseInstance()
	{
		FUNCTION_ENTRY("releaseInstance");

		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		if (0 != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance=0;
		}

		FUNCTION_EXIT;
	}

	void DataCache::initializePAData(unsigned entityKey, unsigned long locationKey)
	{
		FUNCTION_ENTRY("createPAData");

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
			TA_IRS_Bus::PlatformIdType platformId	= (*platformIt)->getPlatformId();
			unsigned long stationId		= (*platformIt)->getStationId();
			unsigned char  tisPlatformId = (*platformIt)->getTisPlatformId();
			PasDataMapIt pasIt	= m_pasDataMap.find(platformId);

			if (pasIt != m_pasDataMap.end())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException( 
					"Configuration Error: Duplicate Platform defined"));
			}

			TA_IRS_Bus::AtsPasData* pasData = new TA_IRS_Bus::AtsPasData (platformId, stationId, entityKey, locationKey,tisPlatformId);
			 
			m_pasDataMap[platformId] = pasData;

		}

		FUNCTION_EXIT;
	}

	void DataCache::initializePowerData(TA_Base_Core::AtsAgentEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("initializePowerData");

		std::map<unsigned long, std::vector<TA_IRS_Core::IAtsPowerData*> > atsPowerData;
		memset(m_powerStatus,0,sizeof(m_powerStatus));

		TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType = entityData->getAgentAtsLocationType();
		unsigned long locationKey = entityData->getLocation();

		if (locationType == TA_Base_Core::AtsAgentEntityData::Occ)
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
		else if (locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 || 
			     locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 ||
			     locationType == TA_Base_Core::AtsAgentEntityData::MainStation ||
			     locationType == TA_Base_Core::AtsAgentEntityData::Depot)
		{
			try
			{
				atsPowerData = TA_IRS_Core::AtsPowerDataFactory::getInstance().getAllPowerDataAtLocationBySubsection(locationKey);
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

		for (powerIt = atsPowerData.begin(); powerIt != atsPowerData.end(); powerIt++)
		{
			std::vector<TA_IRS_Core::IAtsPowerData*>  powerDatas=(*powerIt).second;
			if(powerDatas.size()>0)
			{
				TA_IRS_Core::IAtsPowerData* powerData = (TA_IRS_Core::IAtsPowerData*)(*powerDatas.begin());
				m_pwPKeytoSectionMap[powerData->getTractionPowerDataPointEntityKey()] = (unsigned short)(*powerIt).first;

			}
			std::map < unsigned short, TA_IRS_Bus::PowerData* >::iterator pow_it= m_powerDataMap.find((unsigned short)(*powerIt).first);

			if(pow_it != m_powerDataMap.end())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
					"Configuration Error: Duplicate Electrical Subsection defined"));
			}

			TA_IRS_Bus::PowerData* esRecord = NULL;

			try
			{
				esRecord = new TA_IRS_Bus::PowerData(locationType, 
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

		if (m_powerDataMap.size() != 0)
			m_bPowerMapConfiged = true;
		else
			LOG_EXCEPTION_CATCH(SourceInfo, "Configuration Exception",  "No valid electrical subsection records defined");

		FUNCTION_EXIT;
	}

	TA_IRS_Bus::AtsPasData& DataCache::getPasData(const TA_IRS_Bus::PlatformIdType platformId)
	{
		FUNCTION_ENTRY("getPaData");

		PasDataMapIt it = m_pasDataMap.find(platformId);

		if (it == m_pasDataMap.end() || (*it).second == NULL)
		{
			std::ostringstream message;
			message << "No PAS data for platform " << (unsigned long)platformId;

			TA_THROW(TA_Base_Core::AtsAgentException(message.str().c_str()));
		}

		FUNCTION_EXIT;
		return *((*it).second);
	}

	void DataCache::setPIDSData(const TA_Base_Bus::DataBlock<WORD>& data)
	{
		FUNCTION_ENTRY("setPIDSData");

		m_PIDSData = data;

		FUNCTION_EXIT;
	}

	TA_Base_Bus::DataBlock<WORD>& DataCache::getPIDSData()
	{
		FUNCTION_ENTRY("getPIDSData");
		FUNCTION_EXIT;
		return m_PIDSData;
	}

	void DataCache::updatePACache(const PAStationInfoVec& paStationInfoVec)
	{
		FUNCTION_ENTRY("updatePACache");

		PAStationInfoVec::const_iterator iterPAInfo = paStationInfoVec.begin();

		for(iterPAInfo; iterPAInfo != paStationInfoVec.end(); iterPAInfo++)
		{
			try
			{
				//LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Calling PA..");
				TA_IRS_Bus::AtsPasData& paData = getPasData((*iterPAInfo).PlatformId);
				TA_Base_Core::ThreadGuard guard(paData);

				//<DestinationStationID>
				paData.updateDestinationStationId((*iterPAInfo).DestinationStationId);
				//<Skip>
				paData.updateSkip((*iterPAInfo).Skip);
				//<InDepot
				paData.updateInDepot((*iterPAInfo).InDepot);
				//<Arrival>
				paData.updateArrival((*iterPAInfo).Arrival);
				//<Pre-Arrival>
				paData.updatePreArrival((*iterPAInfo).PreArrival);			 
                //there is no train validation in the ICD, so it is always true.
				paData.updateTrainInfoValidity(true);

			}
			catch(const TA_Base_Core::AtsAgentException& messge)
			{
				LOG_EXCEPTION_CATCH(SourceInfo,"Exception Occur: ",messge.what());
			}
			catch (...)
			{
				std::ostringstream text;
				text << "updatePACache() - Platform Record with Platform ID ";
				text << (unsigned long)(*iterPAInfo).PlatformId;
				text << " not found";

				LOG_EXCEPTION_CATCH(SourceInfo, "Exception Occur", text.str());
			}
		}
		
		FUNCTION_EXIT;
	}

	void DataCache::updateTrainCache(TrainInfoVec& trainInfo)
	{
		//FUNCTION_ENTRY("updateTrainCache");		 
		//update train record
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
			"Start to process train information number of train =%d",trainInfo.size() );
		TrainInfoVec::iterator iterTrain = trainInfo.begin();
		for (iterTrain; iterTrain != trainInfo.end(); iterTrain++ )
		{
			m_trainLocationData->updateTrainlocation((*iterTrain),trainInfo);
		}
        //remove non-existing record and clear data point based on oldtraininfo.
		m_trainLocationData->checkTrainInfoCache(trainInfo);

		//FUNCTION_EXIT;
	}

	PasDataMap& DataCache::getPasDataMap()
	{
	  FUNCTION_ENTRY("getPasDataMap");
	  FUNCTION_EXIT;
	  return m_pasDataMap;
	}

	void DataCache::powerStatusUpdated(unsigned long pkey,bool dpQuality,bool dpValue)
	{
		FUNCTION_ENTRY("powerStatusUpdated");

		if(m_pwPKeytoSectionMap.end() ==m_pwPKeytoSectionMap.find(	pkey))
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
				"No key[%ld] found in pKeyStationMap",pkey );
			return;
		}
        //SectionID starts from 1.
		unsigned short sectionID = m_pwPKeytoSectionMap[pkey]-1;
		if(sectionID<0 || sectionID>63)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
				"Sub-Section number %d ( DataPoint %ld) is not allowed ",sectionID,pkey );
			return;
		}
		unsigned short shiftBits = 2*(sectionID%4);
		unsigned char set=0;
		if(!dpQuality)
			set=0x3;
		else
		{
			if(true==dpValue)
				set=0x1;
			else
				set=0x2;
		}

		{
			TA_Base_Core::ThreadGuard guard(m_powerStatLock);
			setBits(&m_powerStatus[sectionID/4],set,shiftBits,2);
		}
		
		FUNCTION_EXIT;
	}

	void DataCache::setBits(unsigned char* data,unsigned char set,unsigned short start,unsigned short bits)
	{
		FUNCTION_ENTRY("setBits");

		if(start>7 || bits<0 ||bits>8)
			return;
		unsigned char a=*data;
		for(int i=0;i<bits;i++)
			a&=~(1<<(start+i));
		a|=set<<start;
		*data=a;

		FUNCTION_EXIT;
	}

	bool DataCache::IsPowerMapConfiged()
	{
		FUNCTION_ENTRY("IsPowerMapConfiged");
		FUNCTION_EXIT;
		return m_bPowerMapConfiged;
	}

	unsigned char* DataCache::getPowerStatus()
	{
		TA_Base_Core::ThreadGuard guard(m_powerStatLock);
		return m_powerStatus;
	}

	void DataCache::updateTrainPISCache(const PISInfoVec& pisInfo)
	{
		FUNCTION_ENTRY("updateTrainPISCache");

		m_trainPisData->updateTrainPisData(pisInfo);

		FUNCTION_EXIT;
	}

	void DataCache::setOperationMode(TA_Base_Core::EOperationMode eMode)
	{
		FUNCTION_ENTRY("setOperationMode");

		if (TA_Base_Core::Control == eMode )
		{
			m_trainLocationData->initializeDataPoint();
			m_trainPisData->initializeDatapoint();
		}

		FUNCTION_EXIT;
	}

	void DataCache::updateTrainInfoDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("updateTrainInfoDatapointQuality");

		m_trainLocationData->updateAllDataPointQuality(quality);

		FUNCTION_EXIT;
	}

}