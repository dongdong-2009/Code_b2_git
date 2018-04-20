/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/OccUpdateProcessor.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This implements data relay for the OCC Agent
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/exceptions/src/FatalConfigurationException.h"

#include "app/ats/AtsAgent/src/StationOccUpdateProcessor.h"
#include "app/ats/AtsAgent/src/OccUpdateProcessor.h"

namespace TA_IRS_App
{
	OccUpdateProcessor::OccUpdateProcessor(unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
										   WatchDogData& watchDogData, SectorData& sectorData)
										   : m_sectorData(sectorData)
	{
		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ,
				  "OCC Update Processor only valid at OCC");

		// Need to find all the ATS agents and map them...
		TA_Base_Core::CorbaNameList atsAgents = 
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType(TA_Base_Core::AtsAgentEntityData::getStaticType(),true);

		TA_Base_Core::CorbaNameList::iterator corbaNameIt;

		for (corbaNameIt = atsAgents.begin(); corbaNameIt != atsAgents.end(); corbaNameIt++)
		{
			if (m_stationProcessorMap.find((*corbaNameIt).getLocationKey()) != m_stationProcessorMap.end())
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
							"Configuration Error: Duplicate ATS Agents for the same location"));
			}

			if ((*corbaNameIt).getLocationKey() != locationKey &&
				(*corbaNameIt).getObjectName().substr(0,3) != "Kcd")
			{
				StationOccUpdateProcessor* stationProcessor = NULL;

				stationProcessor = new StationOccUpdateProcessor(*corbaNameIt, watchDogData);

				if (stationProcessor != NULL)
				{
					m_stationProcessorMap[(*corbaNameIt).getLocationKey()] = stationProcessor;
				}
			}
		}
	}


	OccUpdateProcessor::~OccUpdateProcessor()
	{
		StationProcessorMap::iterator procIt;

		for (procIt = m_stationProcessorMap.begin(); procIt != m_stationProcessorMap.end(); procIt++)
		{
			delete (*procIt).second;
			(*procIt).second = NULL;
		}
	}

   	//
	// ECS
	//
	void OccUpdateProcessor::addEcsUpdate(const EcsMessage& ecsMessage)
	{
		std::vector<unsigned long> stations;

		if (ecsMessage.localisation.valid && ecsMessage.localisation.stationZoneId != 0)
		{
			// Know where this is - need to add it to the appropriate MSS stations
			stations = m_sectorData.getStationsInRelatedMssSector(ecsMessage.localisation.stationZoneId);

			m_sectorData.addLastSectorIfRequired(stations, ecsMessage.localisation.lastStationZoneId);
		}
		else
		{
			stations = m_sectorData.getAllStations();
		}

		std::vector<unsigned long>::iterator stationIt;

		for (stationIt = stations.begin(); stationIt != stations.end(); stationIt++)
		{
			StationProcessorMap::iterator procIt = m_stationProcessorMap.find((*stationIt));

			if (procIt != m_stationProcessorMap.end())
			{
				if ((*procIt).second != NULL)
				{
					(*procIt).second->addEcsUpdate(ecsMessage);
				}
			}
		}
	};

	//
	// Train
	//
	void OccUpdateProcessor::addTrainUpdate(const TrainMessage& trainMessage)
	{
		std::vector<unsigned long> stations;

		if (trainMessage.localisation.localisationValid && trainMessage.localisation.currentLocation != 0)
		{
			// Know where this is - need to add it to the appropriate MSS stations
			stations = m_sectorData.getStationsInRelatedMssSector(trainMessage.localisation.currentLocation);

			m_sectorData.addLastSectorIfRequired(stations, trainMessage.localisation.lastLocation);
		}
		else
		{
			stations = m_sectorData.getAllStations();
		}

		std::vector<unsigned long>::iterator stationIt;

		for (stationIt = stations.begin(); stationIt != stations.end(); stationIt++)
		{
			StationProcessorMap::iterator procIt = m_stationProcessorMap.find((*stationIt));

			if (procIt != m_stationProcessorMap.end())
			{
				if ((*procIt).second != NULL)
				{
					(*procIt).second->addTrainUpdate(trainMessage);
				}
			}
		}
	};


	//
	//	CCTV
	//
	void OccUpdateProcessor::addCctvUpdate(const CctvMessage& cctvMessage)
	{
		std::vector<unsigned long> stations;

		if (cctvMessage.localisation.valid && cctvMessage.localisation.stationId != 0)
		{
			// Know where this is - need to add it to the appropriate MSS stations
			stations = m_sectorData.getStationsInRelatedMssSector(cctvMessage.localisation.stationId);

			m_sectorData.addLastSectorIfRequired(stations, cctvMessage.localisation.lastStationId);
		}
		else
		{
			stations = m_sectorData.getAllStations();
		}

		std::vector<unsigned long>::iterator stationIt;

		for (stationIt = stations.begin(); stationIt != stations.end(); stationIt++)
		{
			StationProcessorMap::iterator procIt = m_stationProcessorMap.find((*stationIt));

			if (procIt != m_stationProcessorMap.end())
			{
				if ((*procIt).second != NULL)
				{
					(*procIt).second->addCctvUpdate(cctvMessage);
				}
			}
		}
	};

	//
	// TIS
	//
	void OccUpdateProcessor::addTisUpdate(const TisMessage& tisMessage)
	{
		std::vector<unsigned long> stations;

		// Know where this is - need to add it to the appropriate MSS stations
		stations = m_sectorData.getStationsInRelatedMssSector(tisMessage.stationId);

		std::vector<unsigned long>::iterator stationIt;

		for (stationIt = stations.begin(); stationIt != stations.end(); stationIt++)
		{
			StationProcessorMap::iterator procIt = m_stationProcessorMap.find((*stationIt));

			if (procIt != m_stationProcessorMap.end())
			{
				if ((*procIt).second != NULL)
				{
					(*procIt).second->addTisUpdate(tisMessage);
				}
			}
		}
	};
	
	//
	// PAS
	//
	void OccUpdateProcessor::addPasUpdate(const PasMessage& pasMessage)
	{
		std::vector<unsigned long> stations;

		// Know where this is - need to add it to the appropriate MSS stations
		stations = m_sectorData.getStationsInRelatedMssSector(pasMessage.stationId);

		std::vector<unsigned long>::iterator stationIt;

		for (stationIt = stations.begin(); stationIt != stations.end(); stationIt++)
		{
			StationProcessorMap::iterator procIt = m_stationProcessorMap.find((*stationIt));

			if (procIt != m_stationProcessorMap.end())
			{
				if ((*procIt).second != NULL)
				{
					(*procIt).second->addPasUpdate(pasMessage);
				}
			}
		}
	};

	void OccUpdateProcessor::sendOccUpdates()
	{
		StationProcessorMap::iterator procIt;

		for (procIt = m_stationProcessorMap.begin(); procIt != m_stationProcessorMap.end(); procIt++)
		{
			if ((*procIt).second != NULL)
			{
				(*procIt).second->processUpdates();
			}
		}
	}
}
