/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\OccUpdateProcessor.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling the data synchronization from OCC to Stations
  */

#include "bus/ats/ats_agent/CommonATS/src/OccUpdateProcessor.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/ats/ats_agent/CommonATS/src/StationOccUpdateProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Bus
{
	OccUpdateProcessor::OccUpdateProcessor(unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType)
		: m_locationKey(locationKey),
		  m_locationType(locationType)
	{
		FUNCTION_ENTRY("OccUpdateProcessor");

		TA_ASSERT(locationType == TA_Base_Core::AtsAgentEntityData::Occ,
			"OCC Update Processor only valid at OCC");

		initialize();

		FUNCTION_EXIT;
	}

	OccUpdateProcessor::~OccUpdateProcessor()
	{
		FUNCTION_ENTRY("~OccUpdateProcessor");

		deInitialize();

		FUNCTION_EXIT;
	}

	void OccUpdateProcessor::initialize()
	{
		FUNCTION_ENTRY("initialize");

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

			if ((*corbaNameIt).getLocationKey() != m_locationKey &&
				(*corbaNameIt).getObjectName().substr(0,3) != "Kcd")
			{
				StationOccUpdateProcessor* stationProcessor = NULL;

				stationProcessor = new StationOccUpdateProcessor(*corbaNameIt, (*corbaNameIt).getLocationKey());

				if (stationProcessor != NULL)
				{
					m_stationProcessorMap[(*corbaNameIt).getLocationKey()] = stationProcessor;
				}
			}
		}

		FUNCTION_EXIT;
	}

	void OccUpdateProcessor::deInitialize()
	{
		FUNCTION_ENTRY("deInitialize");

		StationProcessorMap::iterator procIt;

		for (procIt = m_stationProcessorMap.begin(); procIt != m_stationProcessorMap.end(); procIt++)
		{
			delete (*procIt).second;
			(*procIt).second = NULL;
		}

		FUNCTION_EXIT;
	}

	void OccUpdateProcessor::addPasUpdate(const PasMessage& pasMessage)
	{
		FUNCTION_ENTRY("addPasUpdate");

		StationProcessorMap::iterator stationIt;	
        //send message only if station id of stationProcessor matches with station id in message
		stationIt=m_stationProcessorMap.find(pasMessage.stationId);
		if (stationIt!= m_stationProcessorMap.end())
		{
			(*stationIt).second->addPasUpdate(pasMessage);
		}         
		//for (stationIt = m_stationProcessorMap.begin(); stationIt != m_stationProcessorMap.end(); stationIt++)
		//{
		//	if ((*stationIt).second != NULL)
		//	{	
		//		//send message only if station id of stationProcessor matches with station id in message
		//		if(pasMessage.stationId == (*stationIt).second->getStationId())
		//		{
		//			(*stationIt).second->addPasUpdate(pasMessage);
		//		}
		//	}
		//}

		FUNCTION_EXIT;
	}

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