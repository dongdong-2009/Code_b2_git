#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/StationDVABroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/naming/src/NamedObject.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/StationDVABroadcast.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{
StationDVABroadcast::StationDVABroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
AbstractNonTrainDVABroadcast( broadcastConfigP, sessionId ),
m_locationName("")
{
    TA_ASSERT(!CachedConfig::getInstance()->getIsMaster(), "StationDVABroadcast is not available for the Master PA Agent");

    // Cache for frequent later use
    m_locationName = TA_IRS_Bus::CachedMaps::getInstance()->getLocationNameFromKey
        (CachedConfig::getInstance()->getAgentLocationKey());
}

StationDVABroadcast::~StationDVABroadcast() 
{
}

void StationDVABroadcast::validateBroadcastConfigSpecifics() //throw(TA_Base_Core::BadParameterException) 
{
    ThreadGuard guard( m_lock );

    int i;

    // Zones can only be from this location
    unsigned long myLocationKey = CachedConfig::getInstance()->getAgentLocationKey();
    for (i=0; i<m_theSpecifics.zones.length(); ++i)
    {
        unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(m_theSpecifics.zones[i]);
        if (locationKey != myLocationKey)
        {
            throw BadParameterException("A requested zone is not within this station's scope");
        }
    }

    for (i=0; i<m_theSpecifics.messages.length(); ++i)
    {
        // Messages can only be from this location 
        unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageLocationKeyFromKey(m_theSpecifics.messages[i]);
        if (locationKey != myLocationKey)
        {
            throw BadParameterException("A requested message is not within this station's scope");
        }
    }

}

void StationDVABroadcast::writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId)
    // throws (PasWriteErrorException, PasConnectionException)
{
    // TBD
}

void StationDVABroadcast::updateCacheWithLatestZoneStatus()
{
	TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();
	for (int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
	{
		unsigned long zoneKey = m_theSpecifics.zones[effectiveCoverageIndices[indexIndex]];
		
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);
		
        // TBD
	}
}

std::string StationDVABroadcast::getEquipmentComponentAndVerify(const std::string& equipmentName)
    //throw(TA_Base_Core::InvalidPasConfigurationException)
{
    std::vector<std::string> tokens;

    PasHelpers::getInstance()->tokenise(equipmentName, tokens, ".");

    if (tokens.size()!=4)
    {
        TA_THROW(InvalidPasConfigurationException("Associated PID zone equipment is invalid") );
    }
    // Determine Station
    if (tokens[0] != m_locationName)
    {
        TA_THROW(InvalidPasConfigurationException("Associated PID zone equipment is invalid") );
    }

    // Determine StationSystem
    if (tokens[1] != EXPECTED_STATION_SYSTEM)
    {
        TA_THROW(InvalidPasConfigurationException("Associated PID zone equipment is invalid") );
    }

    // Determine StationSubsystem
    // Don't care ...

    // Determine Equipment
    return tokens[3];
}

void StationDVABroadcast::disconnectSourceId()
{
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
    nullZones.length(0);

    // TBD
}

void StationDVABroadcast::disconnectSequenceId()
{
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
    nullZones.length(0);

    // TBD
}

bool StationDVABroadcast::isAllFailureZones()
{

	bool isNonValidZone = true;

    for (int i=0; i<m_theSpecifics.zones.length(); ++i)
    {
		// get the zone id and status for every selected zone
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.zones[i]);

		ZoneStatus zoneSta;
        // TBD

		// for zone status, if in normal status, value is 0, and in failure status, value is 1
		isNonValidZone = isNonValidZone && zoneSta.m_status;
	}
	
	return isNonValidZone;
}

} // namespace TA_IRS_App

