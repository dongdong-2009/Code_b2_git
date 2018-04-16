#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
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
#include "core/threads/src/Thread.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"

#include "ace/OS.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{
const std::string  AbstractNonTrainDVABroadcast::EXPECTED_STATION_SYSTEM   = "TIS";
static const unsigned int NORMAL_TIS_PRIORITY       = 8;
static const unsigned int EMERGENCY_TIS_PRIORITY    = 3;

AbstractNonTrainDVABroadcast::AbstractNonTrainDVABroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                                            const std::string& sessionId)
 : AbstractStationBroadcast( broadcastConfigP, sessionId ),
   m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig() ),
   m_timerStarted(false)
// ++ Bug456
{
    FUNCTION_ENTRY( "AbstractNonTrainDVABroadcast" );

    m_lastZoneStatuses.clear();

    // Set the initial effective coverage to be the entire list of zones
    // The false flag prevents the sending of the synchronisation message
    setEffectiveCoverageIndices(
        calculateEffectiveCoverageIndices(m_theSpecifics.zones, m_theSpecifics.zones),
        false);

    // TES 735
    // No audit message needed for local playback
    m_progressReportSentry->setInhibitAuditMessages(m_theSpecifics.isLocalPlayback);
    // jeffrey++ Bug457
    m_zoneStatusTableIndex = 0;
    // ++jeffrey Bug457

    FUNCTION_EXIT;
}


AbstractNonTrainDVABroadcast::~AbstractNonTrainDVABroadcast()
{
    FUNCTION_ENTRY( "~AbstractNonTrainDVABroadcast" );
    FUNCTION_EXIT;
}


bool AbstractNonTrainDVABroadcast::isCyclic()
{
    FUNCTION_ENTRY( "isCyclic" );
    FUNCTION_EXIT;
    return m_theSpecifics.bCyclic;
}


void AbstractNonTrainDVABroadcast::validateBroadcastConfig() //throw(TA_Base_Core::BadParameterException)
{
    FUNCTION_ENTRY( "validateBroadcastConfig" );

    ThreadGuard guard( m_lock );
    int i = 0;

    // At least one zone must be specified
    if (m_theSpecifics.zones.length() == 0)
    {
        // log it and throw the exception
        // the calling method should catch this and log an audit message in one point
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                     "Station DVA broadcast failed - No zones requested" );

        throw BadParameterException("No zones requested");
    }

    // Test validity of zone keys in database
    for (int index = 0; index< m_theSpecifics.zones.length(); ++index)
    {
        try
        {
            unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.zones[index]);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Station DVA broadcast failed - A requested zone (%d) does not exist in the database",
                         m_theSpecifics.zones[index] );

            throw BadParameterException("A requested zone does not exist in the database");
        }
    }


    // At least one message must be specified
    if (m_theSpecifics.messages.length() == 0)
    {
        // log it and throw the exception
        // the calling method should catch this and log an audit message in one point
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                     "Station DVA broadcast failed - No messages requested" );

        throw BadParameterException("No messages requested");
    }

    // Test validity of message keys in database
    for (i=0; i<m_theSpecifics.messages.length(); ++i)
    {
        try
        {
            unsigned long messageId = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageIdFromKey(m_theSpecifics.messages[i]);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Station DVA broadcast failed - A requested message (%d) does not exist in the database",
                         m_theSpecifics.messages[i] );

            throw BadParameterException("A requested message does not exist in the database");
        }
    }

    if (m_theSpecifics.isSynchronisedWithTis)
    {
        if (isCyclic())
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Station DVA broadcast failed - STIS synchronisation is not available for cyclic broadcasts" );

            throw BadParameterException("STIS synchronisation is not available for cyclic broadcasts");
        }

        if (m_theSpecifics.messages.length() != 1)
        {
            // log it and throw the exception
            // the calling method should catch this and log an audit message in one point
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Station DVA broadcast failed - STIS synchronisation is not available for multi-broadcasts" );

            throw BadParameterException("STIS synchronisation is not available for multi-broadcasts");
        }
    }

    validateBroadcastConfigSpecifics();

    FUNCTION_EXIT;
}


void AbstractNonTrainDVABroadcast::execute()
{
    FUNCTION_ENTRY( "execute" );

    setWasExecutedSuccessfully( false );

    try
    {
        setAnnounceId( AbstractBroadcastCircularList::getInstance()->getNextAvailableStationAnnounceId() );
    }
    catch ( TA_Base_Core::NoFreeAnnounceIdException& )
    {
        // Serious error - terminate
        abnormalTermination( TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID );

        FUNCTION_EXIT;
        return;
    }

    // Flag to try to cleanup the announceId in terminate
    setWasExecutedSuccessfully( true );

    // All went well so we move to the runtime state
    setBroadcastState( TA_Base_Bus::IPAAgentCorbaDef::Runtime );

    // Clear any past errors only for the transition from Execution to Runtime
    // In the case of the transition from Execution to Termination to PendingRemoval,
    // we want to retain the GlobalState so that the PA Manager can see the
    // errors encountered.

    m_progressReportSentry->resetGlobalState();

    FUNCTION_EXIT;
}


void AbstractNonTrainDVABroadcast::runtimeSpecifics()
{
    FUNCTION_ENTRY( "runtimeSpecifics" );

    ThreadGuard guard( m_lock );

    // Check if it's time to terminate
    MessageSequenceStatus status;
    try
    {
        // TBD
        if (status.m_announceId != getAnnounceId())
        {
            // Unexpected serious error - terminate
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "abnormalTermination announce Id is %d, requested %d",
                status.m_announceId, getAnnounceId());
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
            // We set this flag so that the terminate method does not inadvertantly write over the
            // announceId of the existing broadacst and cause more problems.
            setWasExecutedSuccessfully(false);

            FUNCTION_EXIT;
            return;
        }
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
        determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);

        FUNCTION_EXIT;
        return;
    }


    switch (status.m_status)
    {
    case MessageSequenceStatus::CyclicInDwellPeriod:
        updateAllZones(TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS_DWELL_TIME);
        // Maochun Sun++
        // bug660 (TD12331)

        // m_lastZoneStatuses.clear();

        // ++Maochun Sun
        // bug660 (TD12331)
        break;
    case MessageSequenceStatus::NoDvaChannelAvailableAtStart:
        updateAllZones(TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CYCLE);
        m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_DVA_CHANNEL);
        break;
    case MessageSequenceStatus::DvaMessageCurrentlyRecording:
        updateAllZones(TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_CYCLE);
        m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_MESSAGE_BUSY_RECORDING);
        break;
    case MessageSequenceStatus::DvaChannelRequestedForHigherPriorityBroadcast:
        updateAllZones(TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN);
        break;
    case MessageSequenceStatus::NoFault:
        performVersionCheck();
        if (!isCyclic())
        {
            updateCurrentlyPlayingZones();
        }
        break;
    case MessageSequenceStatus::CyclicPlayingNow:
        performVersionCheck();
        updateCurrentlyPlayingZones();
        break;
    case MessageSequenceStatus::NotUsed2:
    case MessageSequenceStatus::NotUsed3:
        // Do nothing
        break;
    case MessageSequenceStatus::BroadcastFinishedAbnormally:
    case MessageSequenceStatus::BroadcastFinishedNormally:
        // Finished
        setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
        m_lastZoneStatuses.clear();

        FUNCTION_EXIT;
        return;
        break;
    default:
        // Unexpected serious error - terminate
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "PROTOCOL ERROR Table 302 Invalid Broadcast status %d", status.m_status);
        abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);

        FUNCTION_EXIT;
        return;
    }

    FUNCTION_EXIT;
}


void AbstractNonTrainDVABroadcast::updateCurrentlyPlayingZones()
{
    FUNCTION_ENTRY( "updateCurrentlyPlayingZones" );

    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices =
                                                getEffectiveCoverageIndices();
    // song toan++
    // TD10328
    // 
    // In order to know if this broadcast was overridden by another
    // one we have to cross check the announce id. A check for
    // source id is insufficient because this broadcast may be
    // overridden by another broadcast of SAME source id.

    // get announce id of given source which is being broadcast?
    unsigned int announceIdBroadcasting; // TBD = m_table202.getAnnounceId( m_theSpecifics.sourceId );

    // and the announce id was allocated for this broadcast
    MessageSequenceStatus msgSeqStatus; // TBD = m_table302.getMessageSequenceStatus( m_messageSeqId );
    unsigned int announceIdThis = msgSeqStatus.m_announceId;

    // jeffrey++ Bug457
    // bool bDifferentAnnounceId = (( announceIdBroadcasting != 0 ) &&
    // ( announceIdThis != announceIdBroadcasting ));
    bool bDifferentAnnounceId = (( announceIdBroadcasting != 0 ) && ((m_theSpecifics.sourceId != 0)) &&
                                    ( announceIdThis != announceIdBroadcasting ));
    // ++song toan
    // TD10328


    ZoneKeyToZoneStatusMap tmplastZoneStatuses = m_lastZoneStatuses;
    if(m_zoneStatusTableIndex == getZoneStatusTableIndex())
    {
        FUNCTION_EXIT;
        return;
    } else
    {
        m_zoneStatusTableIndex = getZoneStatusTableIndex();
    }

    m_lastZoneStatuses.clear();

    if(msgSeqStatus.m_sourceid != 0 && m_previousmsgSeqStatus.m_sourceid != 0)
    {
        // update the map
        try
        {

            updateCacheWithLatestZoneStatus();
        }
        catch(TA_Base_Core::PasConnectionException&)
        {
            determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);

            FUNCTION_EXIT;
            return;
        }

        // update progress report structure for each zone
        for (int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
        {
            unsigned long zoneKey = m_theSpecifics.zones[effectiveCoverageIndices[indexIndex]];

            ZoneKeyToZoneStatusMap::iterator it = m_lastZoneStatuses.find(zoneKey);
            ZoneKeyToZoneStatusMap::iterator it_prev = m_previouslastZoneStatuses.find(zoneKey);
            if (it!=m_lastZoneStatuses.end())
            {
                // We have found an update for this zoneId from the last iteration.  This
                // means that between the last iteration and this instant, the message was
                // actively being broadcast and the update for the ZoneStatus obtained in
                // the last iteration is valid.

                ZoneStatus& zoneStatus = it->second;

                if (zoneStatus.m_status)
                {
                    // Faulty zone
                    m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE);
                }
                else
                {
                    if(bDifferentAnnounceId)
                    {
                        // Overridden by another source
                        m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN);
                        m_progressReportSentry->setOverridingSourcePriority( effectiveCoverageIndices[indexIndex], zoneStatus.m_priority);
                    } else
                    {
                        ZoneStatus& zoneStatus_prev = it_prev->second;
                        if( zoneStatus.m_sourceId != 0 &&
                            msgSeqStatus.m_sourceid != zoneStatus.m_sourceId &&
                            msgSeqStatus.m_sourceid != zoneStatus_prev.m_sourceId )
                        {
                            // Overridden by another source
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN);
                            m_progressReportSentry->setOverridingSourcePriority( effectiveCoverageIndices[indexIndex], zoneStatus.m_priority);
                        } else
                        {
                            // All ok
                            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS );
                            m_progressReportSentry->setOverridingSourcePriority( effectiveCoverageIndices[indexIndex], 0 );
                        }
                    }
                }
            }
            else
            {
                // The ZoneStatus for this zoneId was not found.  This means that this is the first
                // iteration where the DVA message is actively being broadcast.  In this case
                // we cannot process the ZoneStatus' yet as we cannot be sure until we receive the
                // same state from the next iteration that the ZoneStatus' are valid.
                // 
                // Consider the example:
                // 302 set to indicate actively broadcasting
                // 200 switched to DVA sources
                // read302 - returns actively broadcasting
                // read200 - returns no overriden zones
                // read302 - returns actively broadcasting
                // read200 - returns no overriden zones
                // read302 - returns actively broadcasting
                // 302 set to indicate finished/dwell time       X These state changes sneak in
                // 200 switched to 0                             X to ruin everything
                // read200 - returns all zones overridden
                // 
                // This is the reason why we only process the ZoneStatus' in retrospect.

            }

        } // for loop

    }    // if
    m_previouslastZoneStatuses = tmplastZoneStatuses;
    m_previousmsgSeqStatus = msgSeqStatus;
    // ++jeffrey Bug457

    FUNCTION_EXIT;
}


void AbstractNonTrainDVABroadcast::updateAllZones(TA_Base_Bus::IPAAgentCorbaDef::ELocalState localState)
{
    FUNCTION_ENTRY( "updateAllZones" );

    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices =
        getEffectiveCoverageIndices();

    m_lastZoneStatuses.clear();

    try
    {

        updateCacheWithLatestZoneStatus();
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
        determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);

        FUNCTION_EXIT;
        return;
    }
    // check each zone status
    for (int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
    {
        unsigned long zoneKey = m_theSpecifics.zones[effectiveCoverageIndices[indexIndex]];

        ZoneKeyToZoneStatusMap::iterator it = m_lastZoneStatuses.find(zoneKey);
        ZoneKeyToZoneStatusMap::iterator it_prev = m_previouslastZoneStatuses.find(zoneKey);
        if (it!=m_lastZoneStatuses.end())
        {
            ZoneStatus& zoneStatus = it->second;

            if (zoneStatus.m_status)
            {
                // Faulty zone
                m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE);
            }
            // TD17344
            else if (localState == MessageSequenceStatus::DvaChannelRequestedForHigherPriorityBroadcast)
            {
                m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], localState );
                m_progressReportSentry->setOverridingSourcePriority( effectiveCoverageIndices[indexIndex], zoneStatus.m_priority );
            }
            // TD17344
            else
            {
                m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], localState );
                m_progressReportSentry->setOverridingSourcePriority( effectiveCoverageIndices[indexIndex], 0 );
            }
        }
    }


    FUNCTION_EXIT;
}


void AbstractNonTrainDVABroadcast::terminate()
{
    FUNCTION_ENTRY( "terminate" );

    ThreadGuard guard( m_lock );

    // ++ Bug456
    m_timerStarted = false;
    // ++ Bug456

    try
    {
        writeToMessagesSequenceBroadcastRequestTable(0);
    }
    catch(TA_Base_Core::PasWriteErrorException&)
    {
        // Not expected to get here
        m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
        // Carry on regardless
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
    }

    // Reset the announce id so that it can be used by new broadcasts
    setAnnounceId(0);

    setMessageSeqId(0);

    m_lastZoneStatuses.clear();

	try
	{
		updateFinalProgressReport();
	}
	catch(...)
	{
	   // Not expecting any failures...
       LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason For updateFinalProgressReport" );
	}

    // All went well so we move to the pending removal state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);
	
    FUNCTION_EXIT;
}

// Thread checked in parent
void AbstractNonTrainDVABroadcast::updateFinalProgressReport()
{
    FUNCTION_ENTRY( "updateFinalProgressReport" );
    // if it is a local playback - skip the audit message sending version
    if (m_theSpecifics.isLocalPlayback)
    {
        AbstractBroadcast::updateFinalProgressReport();
    }
    else
    {
        AbstractStationBroadcast::updateFinalProgressReport();
    }
    FUNCTION_EXIT;
}

int AbstractNonTrainDVABroadcast::getZoneStatusTableIndex()
{
    FUNCTION_ENTRY( "getZoneStatusTableIndex" );
    FUNCTION_EXIT;
    return 0;
}

} // namespace TA_IRS_App

