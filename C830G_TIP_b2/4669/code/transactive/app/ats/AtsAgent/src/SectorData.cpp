/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/SectorData.cpp $
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

#include "app/ats/AtsAgent/src/SectorData.h"

namespace TA_IRS_App
{
	SectorData::SectorData()
	{
		// Need to construct a MSS map to enable finding of agents for data
		TA_IRS_Core::IAtsMssZones mssZones = 
			TA_IRS_Core::AtsMssZoneAccessFactory::getInstance().getAllAtsMssZones();

		TA_IRS_Core::IAtsMssZonesIt mssZonesIt;

		for (mssZonesIt = mssZones.begin(); mssZonesIt != mssZones.end(); mssZonesIt++)
		{
			if ( (*mssZonesIt) != NULL)
			{
				TA_Base_Core::ILocation* loc = 
					TA_Base_Core::LocationAccessFactory::getInstance().getLocationByName((*mssZonesIt)->getMssZone());

				if (loc != NULL)
				{
					m_stationToMssMap[(*mssZonesIt)->getStationId()] = loc->getKey();

					MssToStationMap::iterator it = m_mssToStationMap.find(loc->getKey());

					if (it != m_mssToStationMap.end())
					{
						(*it).second.push_back((*mssZonesIt)->getStationId());
					}
					else
					{
						std::vector<unsigned long> stationList;
						stationList.push_back((*mssZonesIt)->getStationId());
						m_mssToStationMap[loc->getKey()] = stationList;
					}

					delete loc;
					loc = NULL;
				}

				delete (*mssZonesIt);
				(*mssZonesIt) = NULL;
			}
		}
	}

	SectorData::~SectorData()
	{
	}

	std::vector<unsigned long> SectorData::getStationsInRelatedMssSector(unsigned long station)
	{
		// First find the MSS
		StationToMssMap::iterator mssIt = m_stationToMssMap.find(station);

		if (mssIt != m_stationToMssMap.end())
		{
			// Now find stations for this MSS
			MssToStationMap::iterator stationIt = m_mssToStationMap.find((*mssIt).second);

			if (stationIt != m_mssToStationMap.end())
			{	
				// Return vector of stations
				return (*stationIt).second;
			}
		}

		// Can't find any stations - have to spray this everywhere...
		return getAllStations();
	}

	std::vector<unsigned long> SectorData::getAllStations()
	{
		std::vector<unsigned long> allStations;
		StationToMssMap::iterator mssIt;

		for (mssIt = m_stationToMssMap.begin(); mssIt != m_stationToMssMap.end(); mssIt++)
		{
			allStations.push_back((*mssIt).first);
		}

		return allStations;
	}

	void SectorData::addLastSectorIfRequired(std::vector<unsigned long>& stations, unsigned long lastLocation)
	{
		// No need to spray this everywhere if no-one had it.
		if (lastLocation == 0)
		{
			return;
		}

		std::vector<unsigned long>::iterator it;

		for(it = stations.begin(); it != stations.end(); it++)
		{
			if ((*it) == lastLocation)
			{
				// Already in the list
				return;
			}
		}

		std::vector<unsigned long> lastSector = getStationsInRelatedMssSector(lastLocation);

		for(it = lastSector.begin(); it != lastSector.end(); it++)
		{
			stations.push_back(*it);
		}
	}
}
