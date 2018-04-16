/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StationBroadcastProxy.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <ace/OS.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/DVARecordingBroadcast.h"
#include "app/pa/PAAgent/src/MasterDVABroadcast.h"
#include "app/pa/PAAgent/src/MasterLiveBroadcast.h"
#include "app/pa/PAAgent/src/StationDVABroadcast.h"
#include "app/pa/PAAgent/src/StationLiveBroadcast.h"
#include "app/pa/PAAgent/src/StationMusicBroadcast.h"
#include "app/pa/PAAgent/src/UpdateDvaMessage.h"
#include "app/pa/PAAgent/src/PasTransactionManager.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/StationBroadcastProxy.h"

namespace TA_IRS_App
{

const std::string StationBroadcastProxy::ATS_AGENT_SESSION_ID = "AtsAgentSessionId";

StationBroadcastProxy::StationBroadcastProxy( BroadcastManager& refManager ) :
m_refManager( refManager )
{
    FUNCTION_ENTRY( "StationBroadcastProxy" );

    std::string strStationPaName = CachedConfig::getInstance()->getAgentName();
    strStationPaName += TA_Base_Bus::IPAAgentCorbaDef::STATION_PA_CORBA_NAME;
    activateServantWithName( strStationPaName );

    FUNCTION_EXIT;
}


StationBroadcastProxy::~StationBroadcastProxy()
{
    FUNCTION_ENTRY( "~StationBroadcastProxy" );

    try
    {
        deactivateServant();
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Exception caught in destructor");
    }

    FUNCTION_EXIT;
}

// IDL Interface functions
// exception checked
std::string StationBroadcastProxy::getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType,
                                                const std::string& extraInformation /* = "" */)
    // TA_THROW CorruptInternalStateDataException
{
    FUNCTION_ENTRY( "getNewBroadcastId" );

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string broadcastTypeString;

    switch (broadcastType)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::StationMusic :
        broadcastTypeString = "StationMusic @ ";
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::StationLive :
        broadcastTypeString = "StationLive @ ";
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::StationDva :
        broadcastTypeString = "Station @ ";
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::StationRecording :
        broadcastTypeString = "StationRecording @ ";
        break;
    default :
        TA_THROW(TA_Base_Core::CorruptInternalStateDataException("Unexpected EBroadcastType"));
    }

    // Build time stamp
    time_t now = time( NULL );
    tm nowTm;
    ACE_OS::localtime_r(&now, &nowTm);

    char nowString[80] = {0};
    if ( 12 > nowTm.tm_hour )
    {
        sprintf(nowString,
            "%02d:%02dam",
            (nowTm.tm_hour),
            (nowTm.tm_min));
    }
    else
    {
        sprintf(nowString,
            "%02d:%02dpm",
            (nowTm.tm_hour - 12),
            (nowTm.tm_min));
    }

    // Since we only have a resolution of 1 sec, we need to check if the request
    // was done within 1 second of the previous broadcast by comparing the new nowString
    // with the previous.  We just need to make sure that the broadcastIds are all
    // unique.

    unsigned long extraInfoForUniqueness = 0;
    std::string broadcastId = broadcastTypeString + nowString;

    while ( m_refManager.isBroadcastIdExisted( broadcastId ))
    {
        // Ensure broadcastIds are different by updating the suffix
        ++extraInfoForUniqueness;

        char suffix[10] = {0};
        sprintf(suffix, " (%d)", extraInfoForUniqueness);

        broadcastId = broadcastTypeString + nowString + suffix;
    }

    FUNCTION_EXIT;
    return broadcastId;
}

std::string StationBroadcastProxy::getNewDvaBroadcastId( const unsigned long ulMsgKey, const size_t szMsgCount, const time_t tmStartTime, const time_t tmEndTime, const time_t tmInterval )
{
    TA_IRS_Bus::StationDvaMessageRecord stDvaMsg = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( ulMsgKey );

    bool bIsEmergency = (( stDvaMsg.usMsgType == TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_EMERGENCY ) || ( stDvaMsg.usMsgType == TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY ));
    const char* const Comma = ", ";
    std::stringstream stmFormat;
    stmFormat << stDvaMsg.strMsgName << (( szMsgCount > 1u ) ? "(MultiMessage), " : Comma ) << ( bIsEmergency ? "Emergency" : "Normal" ) << Comma << "Start Time: ";

    bool bCyclic = ( tmStartTime != 0u ) && ( tmEndTime != 0u );

    time_t tmActualStartTime =  bCyclic ? tmStartTime : time( NULL );

    tm tmStart;
    ACE_OS::localtime_r(&tmActualStartTime, &tmStart);

    stmFormat << std::setfill('0') << std::setw(2) << tmStart.tm_hour << ":" << std::setfill('0') << std::setw(2) << tmStart.tm_min;

    if ( bCyclic )
    {
        stmFormat << Comma << "End Time: ";

        tm tmEnd;
        ACE_OS::localtime_r(&tmEndTime, &tmEnd);

        stmFormat << std::setfill('0') << std::setw(2) << tmEnd.tm_hour << ":" << std::setfill('0') << std::setw(2) << tmEnd.tm_min;

        stmFormat << Comma << "Interval: ";

        tm tmIntv;
        ACE_OS::localtime_r(&tmInterval, &tmIntv);

        stmFormat << std::setfill('0') << std::setw(2) << tmIntv.tm_min;
    }

    unsigned long extraInfoForUniqueness = 1;
    std::string broadcastId = stmFormat.str().c_str();

    while ( m_refManager.isBroadcastIdExisted( broadcastId ))
    {
        // Ensure broadcastIds are different by updating the suffix
        ++extraInfoForUniqueness;

        char suffix[10] = {0};
        sprintf(suffix, " (%d)", extraInfoForUniqueness);

        broadcastId = stmFormat.str() + suffix;
    }

    return broadcastId;
}

char* StationBroadcastProxy::broadcastStationScheduleDva( 
    const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, 
    const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, CORBA::UShort dwellInSecs, 
    CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, const char* groupName, 
    const TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams& stCyclicParams, const char* sessionId )
{
    FUNCTION_ENTRY( "broadcastStationScheduleDva" );

    LOGOUT("broadcastStationScheduleDva");

    TA_THREADGUARD( m_lock );

    TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string broadcastId = getNewDvaBroadcastId( messages[0u], messages.length(), stCyclicParams.startTime, stCyclicParams.stopTime, stCyclicParams.period );

    // this will validate the parameters like the zones and messages
    // as part of the audit message submission
    PasHelpers::getInstance()->submitPaBroadcastStationDvaRequestAuditMessage( hasChime, &stCyclicParams, zones, messages, sessionId );

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
    newBroadcastConfigP->broadcastId = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered = false;
    newBroadcastConfigP->submitTime = time( NULL );

    theSpecifics.dwellInSecs = dwellInSecs;
    theSpecifics.hasChime = hasChime;
    theSpecifics.isSynchronisedWithTis = isSynchronisedWithTis;
    theSpecifics.messages = messages;
    theSpecifics.sourceId = 0u;
    theSpecifics.zones = zones;
    theSpecifics.bCyclic = true;
    theSpecifics.isLocalPlayback = false;
    theSpecifics.stCyclicParams = stCyclicParams;
    theSpecifics.stCyclicParams.usScheduleId = INVALID_SCHEDULE_EVENT_ID;

    // Initialize the union inside BroadcastConfig
    newBroadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig(theSpecifics);

    AbstractBroadcast* newBroadcast = NULL;
    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

        if (CachedConfig::getInstance()->getIsMaster())
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterDVABroadcast.");
            newBroadcast = new MasterDVABroadcast( newBroadcastConfigP, sessionId );
        }
        else
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationDVABroadcast.");
            newBroadcast = new StationDVABroadcast( newBroadcastConfigP, sessionId );
        }

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );

        // Pass on to the client
        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // Pass on to the client
        FUNCTION_EXIT;
        throw;
    }

    // No worker for cyclic broadcast
    m_refManager.addCyclicBroadcast( newBroadcast );

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId = newBroadcastConfigP->broadcastId;
    updateData.broadcastType = newBroadcastConfigP->broadcastType;
    updateData.sessionId = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
    return CORBA::string_dup(newBroadcastConfigP->broadcastId);
}


// exception checked
char* StationBroadcastProxy::broadcastStationDva( 
    const TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
    CORBA::UShort dwellInSecs, CORBA::Boolean hasChime, CORBA::Boolean isSynchronisedWithTis, const char* groupName, const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastStationDva" );

    LOGOUT("broadcastStationDva");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    // Check the broadcast limit after sending the audit messages for traceability
    if (m_refManager.getStationBroadcastCount() >= CachedConfig::getInstance()->getMaxBroadcasts())
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Request failed due to broadcast limit (%lu)",
            CachedConfig::getInstance()->getMaxBroadcasts() );

        PasHelpers::getInstance()->throwBroadcastLimitReachedException();
    }

    std::string broadcastId = getNewDvaBroadcastId( messages[0u], messages.length() );

    // this will validate the parameters like the zones and messages
    // as part of the audit message submission
    PasHelpers::getInstance()->submitPaBroadcastStationDvaRequestAuditMessage( hasChime, NULL, zones, messages, sessionId );

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
    newBroadcastConfigP->broadcastId        = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered   = false;
    newBroadcastConfigP->submitTime         = time( NULL );

    theSpecifics.dwellInSecs = dwellInSecs;
    theSpecifics.hasChime = hasChime;
    theSpecifics.isSynchronisedWithTis = isSynchronisedWithTis;
    theSpecifics.messages = messages;
    theSpecifics.sourceId = 0u;
    theSpecifics.zones = zones;
    theSpecifics.bCyclic = false;
    theSpecifics.isLocalPlayback = false; // function playbackStationDva handle local playback
    theSpecifics.stCyclicParams.period = 0;
    theSpecifics.stCyclicParams.startTime = 0;
    theSpecifics.stCyclicParams.stopTime = 0;
    theSpecifics.stCyclicParams.usScheduleId = INVALID_SCHEDULE_EVENT_ID;
    theSpecifics.stCyclicParams.bCycSun = false;
    theSpecifics.stCyclicParams.bCycMon = false;
    theSpecifics.stCyclicParams.bCycTue = false;
    theSpecifics.stCyclicParams.bCycWed = false;
    theSpecifics.stCyclicParams.bCycThu = false;
    theSpecifics.stCyclicParams.bCycFri = false;
    theSpecifics.stCyclicParams.bCycSat = false;

    // Initialize the union inside BroadcastConfig
    newBroadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig(theSpecifics);

    AbstractBroadcast* newBroadcast = NULL;
    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

        if (CachedConfig::getInstance()->getIsMaster())
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterDVABroadcast.");
            newBroadcast = new MasterDVABroadcast( newBroadcastConfigP, sessionId );
        }
        else
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationDVABroadcast.");
            newBroadcast = new StationDVABroadcast( newBroadcastConfigP, sessionId );
        }

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }

    m_refManager.addStationBroadcast( newBroadcast );

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId = newBroadcastConfigP->broadcastId;
    updateData.broadcastType = newBroadcastConfigP->broadcastType;
    updateData.sessionId = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
    return CORBA::string_dup(newBroadcastConfigP->broadcastId);
}


void StationBroadcastProxy::agentLinkTest()
{
    FUNCTION_ENTRY( "agentLinkTest" );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    // empty method for PA Manager link test only

    FUNCTION_EXIT;
}


bool StationBroadcastProxy::hardwareLinkTest(const char* broadcastId, const char* sessionId)
{
    FUNCTION_ENTRY( "hardwareLinkTest" );

    // test hardware connection availability
    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string strBroadcastId = broadcastId;

    if( strBroadcastId.empty() )
    {
        FUNCTION_EXIT;
        return m_refManager.isConnectionAvaliable();
    }
    else
    {
        AbstractBroadcast* broadcast = m_refManager.getBroadcastById(strBroadcastId);
        if ( NULL != broadcast )
        {
            broadcast->setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Execution);
            broadcast->operatorAbort();
        }

        FUNCTION_EXIT;
        return false;
    }
}

// exception checked
char* StationBroadcastProxy::broadcastStationLive
    ( CORBA::Boolean hasChime,
	  CORBA::UShort sourceId,
      const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
      const char* groupName,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastStationLive" );

    LOGOUT("broadcastStationLive");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    // Check the broadcast limit after sending the audit messages for tracability
    if (m_refManager.getStationBroadcastCount() >= CachedConfig::getInstance()->getMaxBroadcasts())
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Request failed due to broadcast limit (%lu)",
            CachedConfig::getInstance()->getMaxBroadcasts() );

        PasHelpers::getInstance()->throwBroadcastLimitReachedException();
    }

    // this is to build a bvroadcast ID with more information
    std::string extraInformation(groupName);
    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationLive, extraInformation);

    // this will validate the parameters like the zones and messages
    // as part of the audit message submission
    PasHelpers::getInstance()->submitPaBroadcastStationLiveRequestAuditMessage( zones, sessionId );

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::StationLiveConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationLive;
    newBroadcastConfigP->broadcastId        = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered   = false;
    newBroadcastConfigP->submitTime         = time( NULL );
    theSpecifics.zones                      = zones;
	theSpecifics.hasChime                   = hasChime;
    theSpecifics.sourceId                   = sourceId;
    
    newBroadcastConfigP->theBroadcastConfigSpecifics.theStationLiveConfig(theSpecifics);

    AbstractBroadcast* newBroadcast = NULL;

    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException
        if (CachedConfig::getInstance()->getIsMaster())
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterLiveBroadcast.");
            newBroadcast = new MasterLiveBroadcast( newBroadcastConfigP, sessionId );
        }
        else
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationLiveBroadcast.");
            newBroadcast = new StationLiveBroadcast( newBroadcastConfigP, sessionId );
        }

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& ex)
    {
		LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugDebug, ex.strWhat.in());
        delete newBroadcast;
        newBroadcast = NULL;

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }

    m_refManager.addStationBroadcast( newBroadcast );

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId   = newBroadcastConfigP->broadcastId;
    updateData.broadcastType   = newBroadcastConfigP->broadcastType;
    updateData.sessionId       = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
    return CORBA::string_dup(newBroadcastConfigP->broadcastId);
}


TA_Base_Bus::IPAAgentCorbaDef::EMusicType StationBroadcastProxy::getMusicStatus(const char* sessionId)
{
    FUNCTION_ENTRY( "getMusicStatus" );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    FUNCTION_EXIT;
    return static_cast<TA_Base_Bus::IPAAgentCorbaDef::EMusicType>(m_refManager.getMusicStatus());
}


void StationBroadcastProxy::broadcastStationMusic
    ( TA_Base_Bus::IPAAgentCorbaDef::EMusicType musicType,
      const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastStationMusic" );

    LOGOUT("broadcastStationMusic");

    TA_THREADGUARD( m_lock );

    TA_Base_Bus::GenericAgent::ensureControlMode();

    const int eCurType = m_refManager.getMusicStatus();

    // [Work around start] For music status can not be retrieved from PAS

    //if ( eCurType == musicType )
    //{
    //    PasHelpers::getInstance()->throwPaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "Can't set to same music type" );
    //}

    m_refManager.setMusicStatus( musicType );

    // [Work around end] For music status can not be retrieved from PAS

    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationMusic);

    PasHelpers::getInstance()->submitPaBroadcastStationMusicRequestAuditMessage( musicType, sessionId );

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::StationMusicConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationMusic;
    newBroadcastConfigP->broadcastId        = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered   = false;
    newBroadcastConfigP->submitTime         = time( NULL );

    theSpecifics.musicType = musicType;
    theSpecifics.zones = zones;

    if (CachedConfig::getInstance()->getIsMaster())
    {
        // Only OCC music in OCC
        theSpecifics.sourceId = PAS_MUSIC_SOURCE_FROM_OCC;
    }
    else
    {
        switch (musicType)
        {
        case TA_Base_Bus::IPAAgentCorbaDef::OccMusic:
            theSpecifics.sourceId = PAS_MUSIC_SOURCE_FROM_OCC;
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::LocalMusic:
            theSpecifics.sourceId = PAS_MUSIC_SOURCE_FROM_STN;
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::NoMusic:
            {
                if ( TA_Base_Bus::IPAAgentCorbaDef::OccMusic == eCurType )
                {
                    theSpecifics.sourceId = PAS_MUSIC_SOURCE_FROM_OCC;
                }
                else if ( TA_Base_Bus::IPAAgentCorbaDef::LocalMusic == eCurType )
                {
                    theSpecifics.sourceId = PAS_MUSIC_SOURCE_FROM_STN;
                }
                else
                {
                    // Invalid request
                }
                break;
            }
        default:
            // impossible to reach here thanks to previous switch statement
            // which thrown an exception
            break;
        }
    }

    newBroadcastConfigP->theBroadcastConfigSpecifics.theStationMusicConfig(theSpecifics);

    StationMusicBroadcast oStationMusicBroadcast( newBroadcastConfigP, sessionId );
    try
    {
        oStationMusicBroadcast.validateBroadcastConfig();

        oStationMusicBroadcast.execute();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
    {
        if ( expWhat.eErrorId == TA_Base_Bus::IPAAgentCorbaDef::eFepcCommsFailure )
        {
            m_refManager.setMusicStatus( TA_Base_Bus::IPAAgentCorbaDef::NoMusic );
        }

        // Pass on to the client
        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // Pass on to the client
        FUNCTION_EXIT;
        throw;
    }

    m_refManager.setMusicStatus( musicType );

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId = newBroadcastConfigP->broadcastId;
    updateData.broadcastType = newBroadcastConfigP->broadcastType;
    updateData.sessionId = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
}


char* StationBroadcastProxy::recordAdhocMessage(CORBA::UShort usSrcId, 
    CORBA::ULong ulMsgKey, CORBA::UShort usMsgType, const char* strSession)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "recordAdhocMessage" );

    LOGOUT("recordAdhocMessage");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationRecording);

    PasHelpers::getInstance()->submitPaRecordAdhocRequestAuditMessage( usSrcId, ulMsgKey, usMsgType, strSession );

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::StationRecordingConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationRecording;
    newBroadcastConfigP->broadcastId = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered = false;
    newBroadcastConfigP->submitTime = time( NULL );

    theSpecifics.messageKey = ulMsgKey;
    theSpecifics.sourceId = usSrcId;
    theSpecifics.usMsgType = usMsgType;

    newBroadcastConfigP->theBroadcastConfigSpecifics.theStationRecordingConfig(theSpecifics);


    AbstractBroadcast* newBroadcast = NULL;
    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

        newBroadcast = new DVARecordingBroadcast( newBroadcastConfigP, strSession );

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
    {
        delete newBroadcast;
        newBroadcast = NULL;
        // Pass on to the client

        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );

        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }

    m_refManager.addStationBroadcast( newBroadcast );

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId = newBroadcastConfigP->broadcastId;
    updateData.broadcastType = newBroadcastConfigP->broadcastType;
    updateData.sessionId = strSession;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
    return CORBA::string_dup(newBroadcastConfigP->broadcastId);
}


void StationBroadcastProxy::retryStationBroadcast
    ( const char* broadcastId,
      const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "retryStationBroadcast" );

    LOGOUT("retryStationBroadcast");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(broadcastId);
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Request failed due to invalid BroadcastId",
            broadcastId );
    }

    AbstractStationBroadcast* stationBroadcast = dynamic_cast<AbstractStationBroadcast*>(broadcast);
    if (stationBroadcast == NULL)
    {
        std::ostringstream msg;
        msg << "The specified broadcastId " << broadcastId << " is not a station broadcast";

        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, msg.str() );
    }

    PasHelpers::getInstance()->submitPaRetryStationBroadcastRequestAuditMessage(
        stationBroadcast->getBroadcastType(),
        zones,
        sessionId );

    // Queue actual execution in BroadcastWorker thread
    stationBroadcast->retry(zones, sessionId);

    FUNCTION_EXIT;
}


void StationBroadcastProxy::terminateBroadcast ( const char* broadcastId, const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "terminateBroadcast" );

    LOGOUT("terminateBroadcast");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(broadcastId);

    if ( NULL == broadcast)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to terminate a non-existent broadcast ",
            broadcastId );
    }

    if ( broadcast->getBroadcastType() == TA_Base_Bus::IPAAgentCorbaDef::StationDva )
    {
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* config = broadcast->getBroadcastConfigCopy();

        if ( NULL != config )
        {
            {
                // Destroy the reference before delete BroadcastConfig
                TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig& refConfig = 
                    config->theBroadcastConfigSpecifics.theStationDvaConfig();

                if ( !refConfig.isLocalPlayback)
                {
                    PasHelpers::getInstance()->submitPaTerminateBroadcastAuditMessage( refConfig.messages, sessionId );
                }
            }

            delete config;
            config = NULL;
        }
    }
    else if ( broadcast->getBroadcastType() == TA_Base_Bus::IPAAgentCorbaDef::TrainDva )
    {
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* config = broadcast->getBroadcastConfigCopy();

        if ( NULL != config )
        {
            if ( config->theBroadcastConfigSpecifics.theTrainDvaConfig().isEmergency )
            {
                PasHelpers::getInstance()->submitTrainPAPrerecordedAbortAuditMessage( broadcastId, config->theBroadcastConfigSpecifics.theTrainDvaConfig().trains, sessionId );
            }

            delete config;
        }
    }

    if (broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::Termination &&
        broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::Aborting &&
        broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval)
    {
        // Queue actual termination in BroadcastWorker thread
        broadcast->operatorAbort();
    }

    FUNCTION_EXIT;
}


void StationBroadcastProxy::removeBroadcast ( const char* broadcastId, const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "removeBroadcast" );

    LOGOUT("removeBroadcast");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(broadcastId);

    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException( "Failed to remove a non-existent broadcast", broadcastId );
    }

    // Flag the table for deletion when the BroadcastWorker is ready
    broadcast->setIsTaggedForRemoval(true);

    FUNCTION_EXIT;
}


void StationBroadcastProxy::restoreBroadcast ( const char* broadcastId, const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "restoreBroadcast" );

    LOGOUT("restoreBroadcast");

    TA_THREADGUARD( m_lock );

    TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(broadcastId);

    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException( "Failed to remove a non-existent broadcast", broadcastId );
    }

    // Flag the table for deletion when the BroadcastWorker is ready
    broadcast->setIsTaggedForRemoval(false);

    FUNCTION_EXIT;
}


bool StationBroadcastProxy::setAdhocType
    ( CORBA::ULong messageKey,
      CORBA::UShort messageType,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY("setAdhocType");
    LOGOUT("setAdhocType");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    // Check the validity of the new messageType
    if ( !PasHelpers::getInstance()->isAdHocDvaMsg( messageType ))
    {
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified messageType is not recognized" );
    }

    // Check the validity of the messageKey

    TA_IRS_Bus::StationDvaMessageRecord record;

    try
    {
        record = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified message does not exist in the database" );
    }

    // Check if the messageKey is from this location
    if (CachedConfig::getInstance()->getAgentLocationKey() != record.ulLocationKey)
    {
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified message does not belong to this location" );
    }

    // Check if the messageKey is an adhoc message
    if ( !PasHelpers::getInstance()->isAdHocDvaMsg( record.usMsgType ))
    {
        // out of range
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified message is not an ad-hoc message" );
    }

    // dont set the type if it is being set to the same thing
    if (record.usMsgType == messageType)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Received a request to set Adhoc Type to the current Adhoc type - Ignoring." );

        return false;
    }

	/*
    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == record.usMsgType )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Received a request to set Adhoc Type of a blank ad-hoc message - Ignoring." );

        return false;
    }
	*/

    UpdateDvaMessage oUpdateDvaMessage( CachedConfig::getInstance()->getIsMaster(), NULL );

    oUpdateDvaMessage.setToBeDeleted( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == messageType );
    oUpdateDvaMessage.setDvaMsgId(record.usMsgId);
    oUpdateDvaMessage.setOriginalMessageType(record.usMsgType);
    oUpdateDvaMessage.setUpdateMessageType(messageType);

    int nExecutionResult = PasTransactionManager::getInstance().executeTransaction( oUpdateDvaMessage, false );

    switch ( nExecutionResult )
    {
    case TRANSACTION_FAILED:
        PasHelpers::getInstance()->throwPaErrorException(TA_Base_Bus::IPAAgentCorbaDef::eSetAdhocTypeFailure, "Unexpected response from the PAS" );
        break;
    case TRANSACTION_CONNECTION_FAILURE:
        PasHelpers::getInstance()->throwFepcCommsFailureException( "Failed to connect to PAS" );
        break;
    case TRANSACTION_IN_MONITOR_MODE:
        // Must be Control mode but we still check it
        return false;
        break;
    case TRANSACTION_SUCCESS:
        break;
    default:
        break;
    }

    // No return, Command sent to PAS successfully

    // No event for delete Ad-Hoc message
    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK != messageType)
    {
        // audit the change
        PasHelpers::getInstance()->submitPaSetAdhocTypeAuditMessage(
            record.usMsgType, messageType, messageKey, sessionId );
    }

    // Update CachedMaps and the database

    try
    {
        record.usMsgType = messageType;
        if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == messageType)
        {
            record.strMsgName = "";
        }
        TA_IRS_Bus::CachedMaps::getInstance()->setStationDvaMessageRecord(record, true);
    }
    catch(...)
    {
        // Expecting either :
        // TA_Base_Core::DataException
        // TA_Base_Core::DatabaseException
        // TA_Base_Core::DataConfigurationException

        FUNCTION_EXIT
        throw TA_Base_Core::DatabaseErrorException();
    }

    FUNCTION_EXIT;
    return true;
}


bool StationBroadcastProxy::setAdhocLabel
    ( CORBA::ULong messageKey,
      const char* label,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "setAdhocLabel" );

    LOGOUT("setAdhocLabel");

    TA_THREADGUARD( m_lock );

 	TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string strNewLabel = label;

    // Check the validity of the label
    if ( strNewLabel.length() > TA_Base_Bus::IPAAgentCorbaDef::MAX_ADHOC_LABEL_LENGTH )
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified label is greater than 120 characters" );
    }

    // Check the validity of the messageKey
    TA_IRS_Bus::StationDvaMessageRecord record;
    try
    {
        record = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageRecordFromKey(messageKey);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified message does not exist in the database" );
    }

    // Check if the messageKey is from this location
    if (CachedConfig::getInstance()->getAgentLocationKey() != record.ulLocationKey)
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified message does not belong to this location" );
    }

    // Check if the messageKey is an adhoc message
    if ( !PasHelpers::getInstance()->isAdHocDvaMsg( record.usMsgType ))
    {
        // out of range
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified message is not an ad-hoc message" );
    }

    if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == record.usMsgType )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Received a request to set Adhoc label of a blank ad-hoc message - Ignoring." );

        return false;
    }

    // dont set the label if it is being set to the same thing
    if ( record.strMsgName.compare( strNewLabel ) == 0 )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Received a request to set Adhoc Label to the current Adhoc Label - Ignoring." );

        FUNCTION_EXIT;
        return false;
    }

    // Good to go
    try
    {
        std::string oldLabel = record.strMsgName;

        record.strMsgName = strNewLabel.c_str();
        TA_IRS_Bus::CachedMaps::getInstance()->setStationDvaMessageRecord(record, true);

        PasHelpers::getInstance()->submitPaSetAdhocLabelAuditMessage(
            oldLabel, strNewLabel, messageKey, sessionId );
    }
    catch(...)
    {
        // Expecting either :
        // TA_Base_Core::DataException
        // TA_Base_Core::DatabaseException
        // TA_Base_Core::DataConfigurationException

        FUNCTION_EXIT;
        throw TA_Base_Core::DatabaseErrorException();
    }

    FUNCTION_EXIT;
    return true;
}


TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* StationBroadcastProxy::getBroadcastIdTypes()
{
    FUNCTION_ENTRY( "getBroadcastIdTypes" );

    LOGOUT("getBroadcastIdTypes");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    FUNCTION_EXIT;
    return m_refManager.getStationBroadcastIdTypes();
}


void StationBroadcastProxy::changeBroadcastId
    ( const char* fromBroadcastId,
      const char* toBroadcastId,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "changeBroadcastId" );

    LOGOUT("changeBroadcastId");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(fromBroadcastId);
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to change a non-existent broadcast", fromBroadcastId );
    }

    if ( !m_refManager.updateBroadcastId( fromBroadcastId, toBroadcastId ))
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "NewBroadcastId is used by another broadcast" );
    }

    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    updateData.fromBroadcastId = fromBroadcastId;
    updateData.toBroadcastId   = toBroadcastId;
    updateData.broadcastType   = broadcast->getBroadcastType();
    updateData.sessionId       = broadcast->getSessionId().c_str();

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentRenamedBroadcastUpdate(updateData);

    PasHelpers::getInstance()->submitPaChangeBroadcastIdAuditMessage(
        fromBroadcastId,
        toBroadcastId,
        sessionId );

    FUNCTION_EXIT;
}


TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* StationBroadcastProxy::getBroadcastConfig ( const char* broadcastId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "getBroadcastConfig" );

    LOGOUT("getBroadcastConfig");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(broadcastId);
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException("Failed to return the broadcast config for a non-existent broadcast", broadcastId);
    }

    // Return the deep copy
    FUNCTION_EXIT;
    return broadcast->getBroadcastConfigCopy();
}


TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* StationBroadcastProxy::getProgressReport ( const char* broadcastId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "getProgressReport" );

    LOGOUT("getProgressReport");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(broadcastId);
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException("Failed to return the broadcast config for a non-existent broadcast", broadcastId);
    }

    // Return the deep copy
    FUNCTION_EXIT;
    return broadcast->getProgressReportCopy();
}


TA_Base_Bus::IPAAgentCorbaDef::StationDvaMessageRecords* StationBroadcastProxy::getStationDvaMessageRecords()
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "getStationDvaMessageRecords" );

    LOGOUT("getStationDvaMessageRecords");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    TA_Base_Bus::IPAAgentCorbaDef::StationDvaMessageRecords* newData = NULL;

    TA_IRS_Bus::PaStationDvaMessageCachedMap& theMap = TA_IRS_Bus::CachedMaps::getInstance()->getPaStationDvaMessageCachedMap();
    std::list<TA_IRS_Bus::StationDvaMessageRecord> allRecords = theMap.getAllRecords();
    std::list<TA_IRS_Bus::StationDvaMessageRecord>::iterator it = allRecords.begin();

    newData = new TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData();
    newData->length(allRecords.size());
    for (int index=0;    it!=allRecords.end();    ++it,++index)
    {
        (*newData)[index].m_pKey                = (*it).ulKey;
        (*newData)[index].m_id                  = (*it).usMsgId;
        (*newData)[index].m_locationKey         = (*it).ulLocationKey;
        (*newData)[index].m_label               = (*it).strMsgName.c_str();
        (*newData)[index].m_type                = (*it).usMsgType;
    }

    // There should be no exceptions thrown above
    FUNCTION_EXIT;
    return newData;
}


TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords* StationBroadcastProxy::getDvaVersionRecords()
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "getDvaVersionRecords" );

    LOGOUT("getDvaVersionRecords");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords* newData = NULL;

    TA_IRS_Bus::PaDvaVersionCachedMap& theMap = TA_IRS_Bus::CachedMaps::getInstance()->getPaDvaVersionCachedMap();
    std::list<TA_IRS_Bus::DvaVersionRecord> allRecords = theMap.getAllRecords();
    std::list<TA_IRS_Bus::DvaVersionRecord>::iterator it = allRecords.begin();

    newData = new TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords();
    newData->length(allRecords.size());
    for (int index=0;    it!=allRecords.end();    ++it,++index)
    {
        (*newData)[index].ulKey                = (*it).ulKey;
        (*newData)[index].ulLocationKey         = (*it).ulLocationKey;
        (*newData)[index].strVersionNumber   = (*it).strVersionNumber.c_str();
    }

    // There should be no exceptions thrown above
    FUNCTION_EXIT;
    return newData;
}


void StationBroadcastProxy::broadcastEventTriggeredStationDva( CORBA::ULong messageKey,
                                                          const char* messageVersionNumber,
                                                          CORBA::Boolean hasChime,
                                                          CORBA::Boolean isSynchronisedWithTis,
                                                          CORBA::ULong durationInSecs,
                                                          CORBA::UShort periodInSecs,
                                                          const char* sessionId,
                                                          const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredStationDva" );

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

    // Check station DVA message version
    std::string currentVersionNumber =
         TA_IRS_Bus::CachedMaps::getInstance()->getDvaMessageVersionFromStationDvaMessageKey(messageKey);
    if (currentVersionNumber != messageVersionNumber)
    {
        // Only log the error as an audit message

        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "Event triggered station DVA message broadcast failed - version inconsistency" );
    }


    PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaRequestAuditMessage(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        zones,
        sessionId );

    broadcastEventTriggeredStationDvaInternal(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        zones,
        sessionId );

    FUNCTION_EXIT;
}


void StationBroadcastProxy::broadcastEventTriggeredStationDvaInternal( const std::string& broadcastId,
                                                                  const CORBA::ULong& messageKey,
                                                                  const char* messageVersionNumber,
                                                                  const CORBA::Boolean& hasChime,
                                                                  const CORBA::Boolean& isSynchronisedWithTis,
                                                                  const CORBA::ULong& durationInSecs,
                                                                  const CORBA::UShort& periodInSecs,
                                                                  const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                                                                  const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaInternal" );

    LOGOUT("broadcastEventTriggeredStationDva");

	TA_Base_Bus::GenericAgent::ensureControlMode();

    CORBA::UShort sourceId = 0;

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

    // Setup the structure for only one message as event triggered broadcasts are
    // never multi-broadcasts
    TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq messages;
    messages.length(1);
    messages[0] = messageKey;

    time_t startTime;
    time_t stopTime;

    if (durationInSecs == 0)
    {
        // Oneshot starting from now
        startTime = 0;
        stopTime  = 0;
    }
    else
    {
        startTime = time(NULL);
        stopTime  = startTime + durationInSecs;
    }

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
    newBroadcastConfigP->broadcastId        = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered   = true;
    newBroadcastConfigP->submitTime         = time( NULL );

    theSpecifics.dwellInSecs                = 0;
    theSpecifics.hasChime                   = hasChime;
    theSpecifics.isSynchronisedWithTis      = isSynchronisedWithTis;
    theSpecifics.messages                   = messages;
    theSpecifics.stCyclicParams.period               = periodInSecs;
    theSpecifics.sourceId                   = sourceId;
    theSpecifics.stCyclicParams.startTime                  = startTime;
    theSpecifics.stCyclicParams.stopTime                   = stopTime;
    theSpecifics.zones                      = zones;
    theSpecifics.isLocalPlayback            = false;
    // [TBD] Does event triggered broadcast support cyclic?
    theSpecifics.bCyclic = false;

    // Initialise the union inside BroadcastConfig
    newBroadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig(theSpecifics);

    AbstractBroadcast* newBroadcast = NULL;

    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

        if (CachedConfig::getInstance()->getIsMaster())
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterDVABroadcast.");
            newBroadcast = new MasterDVABroadcast( newBroadcastConfigP, sessionId );
        }
        else
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationLiveBroadcast.");
            newBroadcast = new StationDVABroadcast( newBroadcastConfigP, sessionId );
        }

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException&)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // Pass on to the client
        FUNCTION_EXIT;
        throw;
    }

    m_refManager.addStationBroadcast( newBroadcast );

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId = newBroadcastConfigP->broadcastId;
    updateData.broadcastType = newBroadcastConfigP->broadcastType;
    updateData.sessionId = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
}


void StationBroadcastProxy::broadcastEventTriggeredStationDvaForStation( CORBA::ULong messageKey,
                                                                    const char* messageVersionNumber,
                                                                    CORBA::Boolean hasChime,
                                                                    CORBA::Boolean isSynchronisedWithTis,
                                                                    CORBA::ULong durationInSecs,
                                                                    CORBA::UShort periodInSecs,
                                                                    const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaForStation" );

    LOGOUT("broadcastEventTriggeredStationDvaForStation");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

    // Check station DVA message version
    std::string currentVersionNumber =
         TA_IRS_Bus::CachedMaps::getInstance()->getDvaMessageVersionFromStationDvaMessageKey(messageKey);
    if (currentVersionNumber != messageVersionNumber)
    {
        // Only log the error as an audit message

        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "Event triggered station DVA message broadcast failed - version inconsistency" );
    }

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;

    // Find all valid PA Zones for this location
    TA_IRS_Bus::PaZoneRecords records =
        TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordsFromLocationKey(CachedConfig::getInstance()->getAgentLocationKey());

    zones.length(records.size());

    TA_IRS_Bus::PaZoneRecordsIt it = records.begin();
    int index=0;
    int removedVirtualZones=0;
    for (; it != records.end(); ++it, ++index)
    {
        if ( (*it).m_stnEquipmentEntityName.empty() &&
             (*it).m_occEquipmentEntityName.empty() )
        {
            // These zones are ignored because they are for paging console outputs.
            ++removedVirtualZones;
            continue;
        }
        zones[index] = (*it).m_pKey;
    }

    // Resize if zones removed;
    if (removedVirtualZones)
    {
        zones.length(records.size()-removedVirtualZones);
    }

    PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForStationRequestAuditMessage(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        CachedConfig::getInstance()->getAgentLocationKey(),
        sessionId );

    broadcastEventTriggeredStationDvaInternal(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        zones,
        sessionId );

    FUNCTION_EXIT;
}


void StationBroadcastProxy::broadcastEventTriggeredStationDvaForArea( CORBA::ULong messageKey,
                                                                 const char* messageVersionNumber,
                                                                 CORBA::Boolean hasChime,
                                                                 CORBA::Boolean isSynchronisedWithTis,
                                                                 CORBA::ULong durationInSecs,
                                                                 CORBA::UShort periodInSecs,
                                                                 const char* sessionId,
                                                                 const char* zoneArea )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaForArea" );

    LOGOUT("broadcastEventTriggeredStationDvaForArea");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

    // Check station DVA message version
    std::string currentVersionNumber =
         TA_IRS_Bus::CachedMaps::getInstance()->getDvaMessageVersionFromStationDvaMessageKey(messageKey);
    if (currentVersionNumber != messageVersionNumber)
    {
        // Only log the error as an audit message

        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "Event triggered station DVA message broadcast failed - version inconsistency" );
    }


    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;

    // Find all valid PA Zones for the given area
    TA_IRS_Bus::PaZoneRecords records = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordsFromArea(zoneArea);

    zones.length(records.size());

    TA_IRS_Bus::PaZoneRecordsIt it = records.begin();
    int index=0;
    for (; it != records.end(); ++it, ++index)
    {
        zones[index] = (*it).m_pKey;
    }

    PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForAreaRequestAuditMessage(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        zoneArea,
        sessionId );

    broadcastEventTriggeredStationDvaInternal(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        zones,
        sessionId );

    FUNCTION_EXIT;
}


void StationBroadcastProxy::broadcastEventTriggeredStationDvaForGroup( CORBA::ULong messageKey,
                                                                  const char* messageVersionNumber,
                                                                  CORBA::Boolean hasChime,
                                                                  CORBA::Boolean isSynchronisedWithTis,
                                                                  CORBA::ULong durationInSecs,
                                                                  CORBA::UShort periodInSecs,
                                                                  const char* sessionId,
                                                                  const char* zoneGroupLabel )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaForGroup" );

    LOGOUT("broadcastEventTriggeredStationDvaForGroup");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();
    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;

    // Find all PA Zones for the given group

    try
    {
        std::list<unsigned long> zoneKeys =
            TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupCachedMap().getZonesAssociatedWithGroup(
                CachedConfig::getInstance()->getAgentLocationKey(),
                zoneGroupLabel );

        zones.length(zoneKeys.size());

        std::list<unsigned long>::iterator it = zoneKeys.begin();
        int index=0;
        for (; it != zoneKeys.end(); ++it, ++index)
        {
            zones[index] = *it;
        }
    }
    catch (...)
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "Event triggered Station broadcast failed - zone group details could not be retrieved" );
    }

    PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        zoneGroupLabel,
        sessionId );

    broadcastEventTriggeredStationDvaInternal(
        broadcastId,
        messageKey,
        messageVersionNumber,
        hasChime,
        isSynchronisedWithTis,
        durationInSecs,
        periodInSecs,
        zones,
        sessionId );

    FUNCTION_EXIT;
}

void StationBroadcastProxy::forceTerminateAllExistPA()
{
	FUNCTION_ENTRY("forceTerminateAllExistPA");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

	try
	{
        // [TBD] No need to implement this function in C955
	}
	catch(TA_Base_Core::PasWriteErrorException& err)
	{
		TA_THROW(err);
	}
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception when forceTerminateAllExistPA()");			
		throw;
	}
	
	FUNCTION_EXIT;
}

char* StationBroadcastProxy::playbackStationDva(CORBA::ULong messageKey, CORBA::UShort sourceId, const char* sessionId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
	FUNCTION_ENTRY( "playbackStationDva" );

	LOGOUT("playbackStationDva");

	TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

	// Check the broadcast limit after sending the audit messages for tracability
	if (m_refManager.getStationBroadcastCount() >= CachedConfig::getInstance()->getMaxBroadcasts())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Request failed due to broadcast limit (%lu)",
			CachedConfig::getInstance()->getMaxBroadcasts() );

		PasHelpers::getInstance()->throwBroadcastLimitReachedException();
    }

    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

	TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
	TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

	// Note: string members will make a deep copy from a const char*

	newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
	newBroadcastConfigP->broadcastId        = broadcastId.c_str();
	newBroadcastConfigP->isEventTriggered   = false;
	newBroadcastConfigP->submitTime         = time( NULL );

    theSpecifics.dwellInSecs = 0u;
    theSpecifics.hasChime = false;
	theSpecifics.messages.length(1);
	theSpecifics.messages[0] = messageKey;
	theSpecifics.sourceId = sourceId;
    theSpecifics.zones.length(1);
    theSpecifics.zones[0] = 0u;
    theSpecifics.isSynchronisedWithTis = false;
	theSpecifics.isLocalPlayback = true;
    theSpecifics.bCyclic = false;

	// Initialise the union inside BroadcastConfig
	newBroadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig(theSpecifics);

	AbstractBroadcast* newBroadcast = NULL;
	try
	{
		// Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

		if (CachedConfig::getInstance()->getIsMaster())
		{
			LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterDVABroadcast.");
			newBroadcast = new MasterDVABroadcast( newBroadcastConfigP, sessionId );
		}
		else
		{
			LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationDVABroadcast.");
			newBroadcast = new StationDVABroadcast( newBroadcastConfigP, sessionId );
		}

		newBroadcast->validateBroadcastConfig();
	}
	catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat)
	{
		delete newBroadcast;
		newBroadcast = NULL;

		LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );

		// Pass on to the client

		FUNCTION_EXIT;
		throw;
	}
	catch(...)
	{
		delete newBroadcast;
		newBroadcast = NULL;

		LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

		// Pass on to the client

		FUNCTION_EXIT;
		throw;
	}

    m_refManager.addStationBroadcast( newBroadcast );

	// Notify all PA Managers
	TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

	// Note: string members will make a deep copy from a const char*
	updateData.fromBroadcastId = EMPTY_STRING;
	updateData.toBroadcastId   = newBroadcastConfigP->broadcastId;
	updateData.broadcastType   = newBroadcastConfigP->broadcastType;
	updateData.sessionId       = sessionId;

	PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
	PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

	FUNCTION_EXIT;
	return CORBA::string_dup(newBroadcastConfigP->broadcastId);
}

} // namespace TA_IRS_App

