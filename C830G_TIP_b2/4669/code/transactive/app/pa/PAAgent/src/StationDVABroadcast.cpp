#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/StationDVABroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2009/02/10 16:25:11 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <time.h>
#include <set>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/naming/src/NamedObject.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "app/pa/PAAgent/src/StationDVABroadcast.h"
#include "app/pa/PAAgent/src/Table200.h"
#include "app/pa/PAAgent/src/Table301.h"
#include "app/pa/PAAgent/src/Table302.h"
#include "app/pa/PAAgent/src/Table202.h"
#include "app/pa/PAAgent/src/Table350.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/PASConnection.h"

#include "ace/OS.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{

StationDVABroadcast::StationDVABroadcast( Table202& table202,
                                          Table201& table201,
                                          Table302& table302,
                                          Table350& table350,
                                          Table200& table200,
                                          Table301& table301,
                                          TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                          const std::string& sessionId )
 : AbstractStationBroadcast( broadcastConfigP, sessionId ),
   AbstractNonTrainDVABroadcast( table202, table302, table350, broadcastConfigP, sessionId ),
   m_table200(table200),
   m_table201(table201),
   m_table301(table301),
   m_locationName("")
{
    TA_ASSERT(!CachedConfig::getInstance()->getIsMaster(), "StationDVABroadcast is not available for the Master PA Agent");

    // Cache for frequent later use
    m_locationName = TA_IRS_Bus::CachedMaps::getInstance()->getLocationNameFromKey
        (CachedConfig::getInstance()->getAgentLocationKey());

    m_tisCoverage.station = m_locationName.c_str();
    m_tisCoverage.levels.length(0);
    m_tisCoverage.pids.length(0);
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

        TA_Base_Bus::IPAAgentCorbaDef::EDvaSet set = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageSetFromKey(m_theSpecifics.messages[i]);
    
        // Messages can only be from setB or setD
        if ( set != TA_Base_Bus::IPAAgentCorbaDef::SetB && set != TA_Base_Bus::IPAAgentCorbaDef::SetD )
        {
            if (CachedConfig::getInstance()->getIsDepot())
            {
                throw BadParameterException("A requested message Id is not from set D");
            }
            else
            {
                // Station
                throw BadParameterException("A requested message Id is not from set B");
            }
        }
    }

}

void StationDVABroadcast::writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId)
    // throws (PasWriteErrorException, PasConnectionException)
{
    m_table301.setTableDataAndWrite( getMessageSeqId(), 
                                      announceId,
                                      m_theSpecifics.periodInSecs,
                                      m_theSpecifics.startTime,
                                      m_theSpecifics.stopTime,
                                      m_broadcastConfigV->isEventTriggered,
                                      getEffectiveZoneCoverage(m_theSpecifics.zones) );
}

//Maochun Sun++
//TD13411
//void StationDVABroadcast::updateCacheWithLatestZoneStatus(unsigned long zoneKey)
void StationDVABroadcast::updateCacheWithLatestZoneStatus()
//++Maochun Sun
//TD13411
{
	//Maochun Sun++
	//TD13411
	/*
    // CachedMappingNotFoundException will never happen because it has been tested
    // in validateBroadcastConfig() 
    int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);

	// ++ Bug456
	m_table200.readTimedWait();
	// ++ Bug456
    m_lastZoneStatuses.insert(ZoneKeyToZoneStatusMap::value_type(zoneKey, m_table200.getZoneStatus(zoneId)));
	*/

	m_table200.readTimedWait();

	TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();
	for (int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
	{
		unsigned long zoneKey = m_theSpecifics.zones[effectiveCoverageIndices[indexIndex]];
		
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);
		
		m_lastZoneStatuses.insert(ZoneKeyToZoneStatusMap::value_type(zoneKey, m_table200.getZoneStatus(zoneId)));
	}
	//++Maochun Sun
	//TD13411
}

void StationDVABroadcast::setupAndValidateTisParameters(const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones)
    //throw(TA_Base_Core::BadParameterException) 
{
//    if (CachedConfig::getInstance()->getLocalTISAgentEntityName().empty())
//    {
//        PasHelpers::getInstance()->throwBadParameterException("Local TisAgent is not configured at this location.", getBroadcastId());
//    }
    try
    {
        // force the access factory to build an agent reference
        TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgent();
    }
    catch(...)
    {
        // can only be a DatabaseException or a DataException
        throw BadParameterException("Local TisAgent is not configured at this location.");
    }

    TA_IRS_Bus::StationDvaMessageRecord record;
    try
    {
        record = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(m_theSpecifics.messages[0]);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        throw BadParameterException("A requested message does not exist in the database");
    }

    // Verify if all TIS synchronisation parameters are set

    if ( record.m_tisMessageTag == -1 || 
         record.m_tisLibrarySection == -1 ||
         record.m_tisLibraryVersion == -1)
    {
        throw BadParameterException("PA-TIS synchronisation configuration is not complete");
    }


    // Determine PA Zone to PID Zone mapping

    std::set<std::string> uniquePidZones;

    for (int index=0; index<zones.length(); ++index)
    {
        TA_IRS_Bus::PaZoneRecord record = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey(zones[index]);

        std::list<std::string>::iterator it = record.m_equipmentEntities.begin();
        for (;it != record.m_equipmentEntities.end(); ++it)
        {
            uniquePidZones.insert(*it);
        }
    }

    // The max possible size.  We will resize later
    m_tisCoverage.pids.length(uniquePidZones.size());

    std::set<std::string>::iterator itForPidZones = uniquePidZones.begin();
    int count=0;
    for (; itForPidZones != uniquePidZones.end(); ++itForPidZones)
    {
        try
        {
            m_tisCoverage.pids[count] = getEquipmentComponentAndVerify(*itForPidZones).c_str();
            ++count;
        }
        catch (TA_Base_Core::InvalidPasConfigurationException& e)
        {
            PasHelpers::getInstance()->raiseInvalidPAConfigurationAlarm(e.what());
        }
    }

    // Resize in case some Pid Zones were invalid
    m_tisCoverage.pids.length(count);
}

void StationDVABroadcast::sendRequestToTisAgent(const TA_IRS_Bus::StationDvaMessageRecord& record, 
                                                const char* startTimeString,
                                                const char* stopTimeString,
                                                CORBA::UShort tisPriority)
{
        TA_Base_Core::STISDestinationList destinationList;
        destinationList.length(1);
        destinationList[0] = m_tisCoverage;

        CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getSTISAgent(), submitPredefinedDisplayRequest, ( 
            destinationList, 
            static_cast<TA_Base_Core::ELibrarySection>(record.m_tisLibrarySection),
            record.m_tisLibraryVersion,
            record.m_tisMessageTag,
            startTimeString,
            stopTimeString,
            tisPriority,
            getSessionId().c_str() ) );//limin

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

    m_table201.setTableDataAndWrite( m_table202.getSourceId(getAnnounceId()),
                                     getAnnounceId(),
                                     nullZones );
}

void StationDVABroadcast::disconnectSequenceId()
{
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage nullZones;
    nullZones.length(0);

    m_table301.setTableDataAndWrite( m_table302.getSequenceId(getAnnounceId()),
                                     0,
                                     0,
                                     0,
                                     0,
                                     false,
                                     nullZones );
}

// TD17456
bool StationDVABroadcast::isAllFailureZones()
{

	bool isNonValidZone = true;

	m_table200.readTimedWait();

    for (int i=0; i<m_theSpecifics.zones.length(); ++i)
    {
		// get the zone id and status for every selected zone
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.zones[i]);

		ZoneStatus zoneSta = m_table200.getZoneStatus(zoneId);

		// for zone status, if in normal status, value is 0, and in failure status, value is 1
		isNonValidZone = isNonValidZone && zoneSta.m_status;
	}
	
	return isNonValidZone;
}
// TD17456

} // namespace TA_IRS_App

