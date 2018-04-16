/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/OccUpdateProcessor.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
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
		

		StationProcessorMap::iterator stationIt;

		for (stationIt = m_stationProcessorMap.begin(); stationIt != m_stationProcessorMap.end(); stationIt++)
		{


			if ((*stationIt).second != NULL)
			{
				(*stationIt).second->addEcsUpdate(ecsMessage);
			}

		}
	};

	//
	// Train
	//
	void OccUpdateProcessor::addTrainUpdate(const TrainMessage& trainMessage)
	{
		

		StationProcessorMap::iterator stationIt;

		for (stationIt = m_stationProcessorMap.begin(); stationIt != m_stationProcessorMap.end(); stationIt++)
		{
			
		
				if ((*stationIt).second != NULL)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"AddTrainUpdate to Stations...");
					(*stationIt).second->addTrainUpdate(trainMessage);
				}
		
		}
	};


	//
	//	CCTV
	//
	void OccUpdateProcessor::addCctvUpdate(const CctvMessage& cctvMessage)
	{	

		StationProcessorMap::iterator stationIt;

		for (stationIt = m_stationProcessorMap.begin(); stationIt != m_stationProcessorMap.end(); stationIt++)
		{


			if ((*stationIt).second != NULL)
			{
				(*stationIt).second->addCctvUpdate(cctvMessage);
			}

		}
	};

	//
	// TIS
	//
	void OccUpdateProcessor::addTisUpdate(const TisMessage& tisMessage)
	{
		
		StationProcessorMap::iterator stationIt;

		for (stationIt = m_stationProcessorMap.begin(); stationIt != m_stationProcessorMap.end(); stationIt++)
		{


			if ((*stationIt).second != NULL)
			{
				(*stationIt).second->addTisUpdate(tisMessage);
			}

		}
	};
	
	//
	// PAS
	//
	void OccUpdateProcessor::addPasUpdate(const PasMessage& pasMessage)
	{
	
		

		StationProcessorMap::iterator stationIt;

		for (stationIt = m_stationProcessorMap.begin(); stationIt != m_stationProcessorMap.end(); stationIt++)
		{


			if ((*stationIt).second != NULL)
			{
				(*stationIt).second->addPasUpdate(pasMessage);
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
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"processUpdates to stations...");
				(*procIt).second->processUpdates();
			}
		}
	}
}
