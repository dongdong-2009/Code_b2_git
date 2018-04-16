/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/MasterDVABroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <set>
#include <time.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/MasterDVABroadcast.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{


MasterDVABroadcast::MasterDVABroadcast(
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
AbstractNonTrainDVABroadcast( broadcastConfigP, sessionId )
{
    TA_ASSERT(CachedConfig::getInstance()->getIsMaster(), "MasterDVABroadcast is not available for the Master PA Agent");
}

MasterDVABroadcast::~MasterDVABroadcast() 
{
}

void MasterDVABroadcast::validateBroadcastConfigSpecifics() //throw(TA_Base_Core::BadParameterException) 
{
    ThreadGuard guard( m_lock );
    int i = 0;

    // Test validity of zone keys in database (Global version)
    for (int index = 0; index< m_theSpecifics.zones.length(); ++index)
    {
        unsigned long zoneKey;
        unsigned int zoneId;
        unsigned long zoneLocationKey;
        unsigned int stationId;

        try
        {
            zoneKey = m_theSpecifics.zones[index];
            zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            throw BadParameterException("A requested zone does not exist in the database");
        }

        try
        {
            zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(zoneKey);
            stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            throw BadParameterException("An ISCS station location could not be resolved to a PAS location");
        }
    }
}

void MasterDVABroadcast::writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId)
    // throws (PasWriteErrorException, PasConnectionException)
{
}

void MasterDVABroadcast::performVersionCheck()
{
	unsigned long	zoneLocationKey;
    unsigned int	stationId;
	unsigned long	zoneKey;

	std::vector<unsigned int>	stationIdList;
	
	TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage effectiveZoneCoverage = getEffectiveZoneCoverage(m_theSpecifics.zones);
	for(int i=0; i<effectiveZoneCoverage.length(); i++)
	{
		zoneKey = effectiveZoneCoverage[i];
		zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(zoneKey);
        stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);	

		if (std::find(stationIdList.begin(), stationIdList.end(), stationId)==stationIdList.end())
		{
			stationIdList.push_back(stationId);
		}
	
	}

	// TBD
    {
        // Just a warning message
        m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_PUBLIC_DVA_SET_VERSION_CONFLICT);
    }
}

void MasterDVABroadcast::updateCacheWithLatestZoneStatus()
{
	TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();
	for (int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
	{
		unsigned long zoneKey = m_theSpecifics.zones[effectiveCoverageIndices[indexIndex]];
		
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);
		unsigned long zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(zoneKey);
		unsigned int stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);
		
        // TBD
	}
}

void MasterDVABroadcast::getEquipmentComponentAndVerify( const std::string& equipmentName, 
                                                         std::string& locationName, 
                                                         std::string& pidName)
    //throw(TA_Base_Core::InvalidPasConfigurationException)
{
    std::vector<std::string> tokens;

    PasHelpers::getInstance()->tokenise(equipmentName, tokens, ".");

    if (tokens.size()!=4)
    {
        TA_THROW(InvalidPasConfigurationException("Associated PID zone equipment is invalid") );
    }

    // Determine StationSystem
    if (tokens[1] != EXPECTED_STATION_SYSTEM)
    {
        TA_THROW(InvalidPasConfigurationException("Associated PID zone equipment is invalid") );
    }

    locationName = tokens[0];
    pidName = tokens[3];
}


void MasterDVABroadcast::disconnectSourceId()
{
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
    nullZones.length(0);

    // TBD
}

void MasterDVABroadcast::disconnectSequenceId()
{
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
    nullZones.length(0);

    // TBD
}

// TD17456
bool MasterDVABroadcast::isAllFailureZones()
{
	bool isNonValidZone = true;
	
    for (int i=0; i<m_theSpecifics.zones.length(); ++i)
    {
		// get the zone id and status for every selected zone
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.zones[i]);
		unsigned long zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(m_theSpecifics.zones[i]);
		unsigned int stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);
		
		ZoneStatus zoneSta;
        // TBD
		
		// for zone status, if in normal status, value is 0, and in failure status, value is 1
		isNonValidZone = isNonValidZone && zoneSta.m_status;
	}
	
	return isNonValidZone;

}

} // namespace TA_IRS_App
