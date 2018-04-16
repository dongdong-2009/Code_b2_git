/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcastAccessFactory.h"

#include "bus/pa/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/ScheduleBroadcast.h"
#include "app/pa/PAAgent/src/ScheduleBroadcastWorker.h"

// Query database by OCL will cause memory leak. 

namespace TA_IRS_App
{

ScheduleBroadcastWorker::ScheduleBroadcastWorker() : 
m_mapScheduleBroadcasts(),
m_thdScheduleTimer(),
m_bIsControl( false )
{
    m_thdScheduleTimer.activate();

    updateScheduleBroadcastsFromDatabase();
}

ScheduleBroadcastWorker::~ScheduleBroadcastWorker()
{
    clearScheduleBroadcastInLocalMap();

    m_thdScheduleTimer.deactivate();
    m_thdScheduleTimer.wait();
}

void ScheduleBroadcastWorker::addNewScheduleBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& refConfig )
{
    // Firstly, save schedule config to database
    unsigned long ulNewSchedulekey = 0;

    if ( m_bIsControl )
    {
        TA_Base_Core::IConfigPaScheduleBroadcast* pNewSchedule = 0;
        try
        {
            std::vector<unsigned long> lstZoneKeys;
            PasHelpers::getInstance()->coverageToZoneKeys( refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().zones, lstZoneKeys );
            std::string strScheduleName = refConfig.broadcastId._ptr;

            pNewSchedule = TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().createPaScheduleBroadcast();
            pNewSchedule->setCyclicTimes( refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.usCyclicTimes );
            pNewSchedule->setDvaMsgKey( refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().messages[0] );
            pNewSchedule->setStartTime( refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.startTime );
            pNewSchedule->setPriorityGroup( refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().priorityGroup );
            pNewSchedule->setScheduleName( strScheduleName );
            pNewSchedule->setZoneKeys( lstZoneKeys );
            pNewSchedule->setIsEnable( true );
            pNewSchedule->setAutoDel( true );
            pNewSchedule->setIntervalType( TA_Base_Core::ONCE_SCHEDULE_INTERVAL_TYPE );
            pNewSchedule->setSourceType( TA_Base_Core::DVA_MSG_SOURCE_TYPE );
            pNewSchedule->setStatus( TA_Base_Core::SCHEDULE_STATUS_WAITING );
            pNewSchedule->setLocationKey( CachedConfig::getInstance()->getAgentLocationKey() );
            pNewSchedule->setEndTime( 0 );

            pNewSchedule->applyChanges();

            if ( 0 != pNewSchedule )
            {
                ulNewSchedulekey = pNewSchedule->getKey();
            }

            delete pNewSchedule;
            pNewSchedule = 0;
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
            if ( 0 != pNewSchedule )
            {
                delete pNewSchedule;
                pNewSchedule = 0;
            }
            throw TA_Base_Core::BadParameterException( "Bad parameter(s)" );
        }

        refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().pKey = ulNewSchedulekey;
    }
    else
    {
        ulNewSchedulekey = refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().pKey;
    }

    ScheduleKeyToTimerScheduleMap::iterator itFound = m_mapScheduleBroadcasts.find( ulNewSchedulekey );

    if ( m_mapScheduleBroadcasts.end() != itFound )
    {
        delScheduleBroadcast( ulNewSchedulekey, false );
    }

    // Secondly, store the new schedule broadcast to local map
    ScheduleBroadcast* pNewScheduleBroadcast = 0;
    try
    {
        pNewScheduleBroadcast = new ScheduleBroadcast( refConfig );
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
        if ( 0 != pNewScheduleBroadcast )
        {
            delete pNewScheduleBroadcast;
            pNewScheduleBroadcast = 0;
        }
    }
    if ( 0 == pNewScheduleBroadcast )
    {
        throw TA_Base_Core::BadParameterException( "Bad parameter(s)" );
    }

    m_mapScheduleBroadcasts[ulNewSchedulekey] = std::make_pair(INVALID_TIMER_ID, pNewScheduleBroadcast);

    // Thirdly, add the new schedule broadcast to scheduler
    if ( m_bIsControl )
    {
        ACE_Time_Value atmScheduleTime( pNewScheduleBroadcast->getStartTime(), 0 );
        ScheduleKeyToTimerScheduleMap::iterator itTheSchedule = m_mapScheduleBroadcasts.find( ulNewSchedulekey );
        long lNewEventId = m_thdScheduleTimer.schedule( this, static_cast<void*>(pNewScheduleBroadcast), atmScheduleTime );
        itTheSchedule->second.first = lNewEventId;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ace Timer scheduled broadcast: %d, timer id: %d", ulNewSchedulekey, lNewEventId );
    }

    // Fourthly, send message to monitor agent and manager
    if ( m_bIsControl )
    {
        PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate( refConfig );

        TA_Base_Bus::IPAAgentCorbaDef::ScheduleBroadcastsUpdateData updateData;
        updateData.ulScheduleKey = ulNewSchedulekey;
        updateData.broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
        updateData.eUpdateType =  TA_Base_Bus::IPAAgentCorbaDef::ScheduleCreate;
        updateData.strBroadcastId = refConfig.broadcastId;
        updateData.seqZoneKeys = refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().zones;
        updateData.tmStartTime = refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.startTime;
        updateData.ulCyclicTimes = refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.usCyclicTimes;
        updateData.ulDvaMsgKey = refConfig.theBroadcastConfigSpecifics.theStationDvaConfig().messages[0];
        updateData.strStatus = TA_Base_Core::SCHEDULE_STATUS_WAITING.c_str();
        PasHelpers::getInstance()->sendScheduleBroadcastsUpdate( updateData );
    }
}

void ScheduleBroadcastWorker::delScheduleBroadcast( unsigned long ulScheduleBroadcastKey, bool bJobDone, bool bForceDel )
{
    // Firstly, delete the schedule from database if auto delete is set
    if ( m_bIsControl )
    {
        TA_Base_Core::IConfigPaScheduleBroadcast* pExistSchedule = 0;
        try
        {
            pExistSchedule = dynamic_cast<TA_Base_Core::IConfigPaScheduleBroadcast*>(
                TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().getPaScheduleBroadcast( ulScheduleBroadcastKey, true ));
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
            if ( 0 != pExistSchedule )
            {
                delete pExistSchedule;
                pExistSchedule = 0;
            }
        }

        if ( 0 == pExistSchedule )
        {
            std::stringstream stmWhat;
            stmWhat << "Schedule broadcast key: " << ulScheduleBroadcastKey << " is not found in database";
            throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( TA_Base_Bus::IPAAgentCorbaDef::eBroadcastLimitReached, stmWhat.str().c_str() );
        }

        try
        {
            if ( pExistSchedule->getAutoDel() || bForceDel )
            {
                pExistSchedule->deleteThisObject();
            }
            else
            {
                pExistSchedule->setIsEnable( false );
                pExistSchedule->applyChanges();
            }
        }
        catch(...)
        {
            delete pExistSchedule;
            pExistSchedule = 0;
            throw TA_Base_Core::BadParameterException( "Bad parameter(s)" );
        }

        delete pExistSchedule;
        pExistSchedule = 0;
    }

    // Secondly, remove the schedule from local map and scheduler
    ScheduleKeyToTimerScheduleMap::iterator itFound = m_mapScheduleBroadcasts.find( ulScheduleBroadcastKey );

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage seqZoneKeys;
    CORBA::String_member orbBroadcastId;
    time_t tmStartTime = 0;
    CORBA::ULong ulCyclicTimes = 0;
    CORBA::ULong ulDvaMsgKey = 0;
    if ( m_mapScheduleBroadcasts.end() != itFound )
    {
        try
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Ace timer remove broadcast: %d, timer id: %d, jobdone: %d", itFound->first, itFound->second.first, bJobDone );
            if ( !bJobDone && m_bIsControl )
            {
                m_thdScheduleTimer.cancel( itFound->second.first );
                itFound->second.first = INVALID_TIMER_ID;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason for TA_IRS_Bus::Scheduler::remove" );
        }

        if ( 0 != itFound->second.second )
        {
            TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& refTheConfig = itFound->second.second->getConfig();
            orbBroadcastId = refTheConfig.broadcastId;
            seqZoneKeys = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().zones;
            tmStartTime = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.startTime;
            ulCyclicTimes = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.usCyclicTimes;
            ulDvaMsgKey = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().messages[0];

            delete itFound->second.second;
            itFound->second.second = 0;
        }

        m_mapScheduleBroadcasts.erase( itFound );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "The schedule broadcast was.removed already: %d", ulScheduleBroadcastKey );
    }

    // Thirdly, send a message to monitor agent and manager
    if ( m_bIsControl )
    {
        PasHelpers::getInstance()->sendPAAgentRemovedBroadcastUpdate( ulScheduleBroadcastKey );

        TA_Base_Bus::IPAAgentCorbaDef::ScheduleBroadcastsUpdateData updateData;
        updateData.ulScheduleKey = ulScheduleBroadcastKey;
        updateData.broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
        updateData.eUpdateType =  TA_Base_Bus::IPAAgentCorbaDef::ScheduleRemove;
        updateData.strBroadcastId = orbBroadcastId;
        updateData.seqZoneKeys = seqZoneKeys;
        updateData.tmStartTime = tmStartTime;
        updateData.ulCyclicTimes = ulCyclicTimes;
        updateData.ulDvaMsgKey = ulDvaMsgKey;
        updateData.strStatus = TA_Base_Core::SCHEDULE_STATUS_SUCCESSFUL.c_str();

        PasHelpers::getInstance()->sendScheduleBroadcastsUpdate( updateData );
    }
}

void ScheduleBroadcastWorker::updateScheduleBroadcastStatus( unsigned long ulScheduleBroadcastKey, const std::string& strStatus )
{
    TA_Base_Core::IConfigPaScheduleBroadcast* pExistSchedule = 0;
    try
    {
        pExistSchedule = dynamic_cast<TA_Base_Core::IConfigPaScheduleBroadcast*>(
            TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().getPaScheduleBroadcast( ulScheduleBroadcastKey, true ));
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
        if ( 0 != pExistSchedule )
        {
            delete pExistSchedule;
            pExistSchedule = 0;
        }
    }

    if ( 0 == pExistSchedule )
    {
        std::stringstream stmWhat;
        stmWhat << "Schedule broadcast key: " << ulScheduleBroadcastKey << " is not found in database";
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, stmWhat.str().c_str() );
        return;
    }

    try
    {
        pExistSchedule->setIsEnable( false );
        pExistSchedule->setStatus( strStatus );
        pExistSchedule->applyChanges();
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
        delete pExistSchedule;
        pExistSchedule = 0;
        return;
    }

    delete pExistSchedule;
    pExistSchedule = 0;

    ScheduleKeyToTimerScheduleMap::iterator itFound = m_mapScheduleBroadcasts.find( ulScheduleBroadcastKey );

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage seqZoneKeys;
    CORBA::String_member orbBroadcastId;
    time_t tmStartTime = 0;
    CORBA::ULong ulCyclicTimes = 0;
    CORBA::ULong ulDvaMsgKey = 0;
    if ( m_mapScheduleBroadcasts.end() != itFound )
    {
        if ( 0 != itFound->second.second )
        {
            TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& refTheConfig = itFound->second.second->getConfig();
            orbBroadcastId = refTheConfig.broadcastId;
            seqZoneKeys = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().zones;
            tmStartTime = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.startTime;
            ulCyclicTimes = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.usCyclicTimes;
            ulDvaMsgKey = refTheConfig.theBroadcastConfigSpecifics.theStationDvaConfig().messages[0];
            itFound->second.second->setToDisable();
        }
    }

    if ( m_bIsControl )
    {
        TA_Base_Bus::IPAAgentCorbaDef::ScheduleBroadcastsUpdateData updateData;
        updateData.ulScheduleKey = ulScheduleBroadcastKey;
        updateData.broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
        updateData.eUpdateType =  TA_Base_Bus::IPAAgentCorbaDef::ScheduleUpdate;
        updateData.strBroadcastId = orbBroadcastId;
        updateData.seqZoneKeys = seqZoneKeys;
        updateData.tmStartTime = tmStartTime;
        updateData.ulCyclicTimes = ulCyclicTimes;
        updateData.ulDvaMsgKey = ulDvaMsgKey;
        updateData.strStatus = strStatus.c_str();
        PasHelpers::getInstance()->sendScheduleBroadcastsUpdate( updateData );
    }
}

void ScheduleBroadcastWorker::setToControl()
{
    m_bIsControl = true;

    for ( ScheduleKeyToTimerScheduleMap::iterator itLoop = m_mapScheduleBroadcasts.begin();
            m_mapScheduleBroadcasts.end() != itLoop; ++itLoop )
    {
        if (( 0 != itLoop->second.second ) && itLoop->second.second->isEnable() )
        {
            ACE_Time_Value atmScheduleTime( itLoop->second.second->getStartTime(), 0 );
            long lNewEventId = m_thdScheduleTimer.schedule( this, static_cast<void*>(itLoop->second.second), atmScheduleTime );
            itLoop->second.first = lNewEventId;
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected NULL pointer" );
        }
    }
}

void ScheduleBroadcastWorker::setToMonitor()
{
    m_bIsControl = false;

    for ( ScheduleKeyToTimerScheduleMap::iterator itLoop = m_mapScheduleBroadcasts.begin();
            m_mapScheduleBroadcasts.end() != itLoop; ++itLoop )
    {
        if ( INVALID_TIMER_ID != itLoop->second.first )
        {
            m_thdScheduleTimer.cancel( itLoop->second.first );
            itLoop->second.first = INVALID_TIMER_ID;
        }
    }
}

int ScheduleBroadcastWorker::handle_timeout( const ACE_Time_Value& current_time, const void *act )
{
    const ScheduleBroadcast* pTimeoutSchedule = static_cast<const ScheduleBroadcast*>(act);
    if ( 0 == pTimeoutSchedule )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "NULL pointer schedule to be executed" );
        return 0;
    }

    // Parameter current_time doesn't mean real current time, need to get it by time(0)
    time_t tmPastTime = time( 0 ) - pTimeoutSchedule->getStartTime();
    if ( MAX_DELAY_BROADCAST_TIME < tmPastTime )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Ignore Schedule broadcast %d, because it is too late to broadcast it. Time elapse: %d seconds", pTimeoutSchedule->getPkey(), tmPastTime );
        updateScheduleBroadcastStatus( pTimeoutSchedule->getPkey(), TA_Base_Core::SCHEDULE_STATUS_FAILED );
        return 0;
    }

    if ( m_bIsControl )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Execute schedule broadcast : %d", pTimeoutSchedule->getPkey() );
        try
        {
            pTimeoutSchedule->executeBroadcast();
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Failed to execute broadcast %d because exception occur", pTimeoutSchedule->getPkey() );
            updateScheduleBroadcastStatus( pTimeoutSchedule->getPkey(), TA_Base_Core::SCHEDULE_STATUS_FAILED );
        	return 0;
        }
        delScheduleBroadcast( pTimeoutSchedule->getPkey(), true );
    }

    return 0;
}

void ScheduleBroadcastWorker::updateScheduleBroadcastsFromDatabase()
{
    TA_Base_Core::IPaScheduleBroadcasts vecSchedules = 
        TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().getPaScheduleBroadcastsByLocationKey( 
            CachedConfig::getInstance()->getAgentLocationKey(), true );

    for ( TA_Base_Core::IPaScheduleBroadcastsIt itLoop = vecSchedules.begin(); vecSchedules.end() != itLoop; ++itLoop )
    {
        TA_Base_Core::IPaScheduleBroadcast* pSchedule = *itLoop;
        if ( 0 == pSchedule )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer for TA_Base_Core::IPaBroadcastShortcut" );
            continue;
        }

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig newBroadcastConfigP;
        TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

        newBroadcastConfigP.broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
        newBroadcastConfigP.broadcastId = pSchedule->getScheduleName().c_str();
        newBroadcastConfigP.isEventTriggered = false;
        newBroadcastConfigP.submitTime = time( 0 );
        newBroadcastConfigP.isStop = false;

        theSpecifics.dwellInSecs = 0;
        theSpecifics.hasChime = false;
        theSpecifics.isSynchronisedWithTis = false;
        theSpecifics.stCyclicParams.period = 0;
        theSpecifics.sourceId = static_cast<CORBA::Octet>(0);
        theSpecifics.isLocalPlayback = false;

        theSpecifics.messages.length(1);
        theSpecifics.messages[0] = pSchedule->getDvaMsgKey();
        theSpecifics.stCyclicParams.startTime = pSchedule->getStartTime();
        theSpecifics.stCyclicParams.stopTime = pSchedule->getEndTime();
        PasHelpers::getInstance()->zoneKeysToCoverage( pSchedule->getZoneKeys(), theSpecifics.zones);
        theSpecifics.priorityGroup = pSchedule->getPriorityGroup();
        theSpecifics.stCyclicParams.usCyclicTimes = static_cast<CORBA::UShort>(pSchedule->getCyclicTimes());
        theSpecifics.pKey = pSchedule->getKey();
        newBroadcastConfigP.theBroadcastConfigSpecifics.theStationDvaConfig(theSpecifics);

        unsigned long ulScheduleKey = pSchedule->getKey();

        delete pSchedule;
        pSchedule = 0;
        *itLoop = 0;

        ScheduleBroadcast* pNewScheduleBroadcast = 0;
        try
        {
            pNewScheduleBroadcast = new ScheduleBroadcast( newBroadcastConfigP );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
            if ( 0 != pNewScheduleBroadcast )
            {
                delete pNewScheduleBroadcast;
                pNewScheduleBroadcast = 0;
            }
        }
        if ( 0 == pNewScheduleBroadcast )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unexpected NULL pointer for ScheduleBroadcast" );
            continue;
        }

        m_mapScheduleBroadcasts[ulScheduleKey] = std::make_pair(INVALID_TIMER_ID, pNewScheduleBroadcast);
    }

    vecSchedules.clear();
}

void ScheduleBroadcastWorker::clearScheduleBroadcastInLocalMap()
{
    for ( ScheduleKeyToTimerScheduleMap::iterator itLoop = m_mapScheduleBroadcasts.begin();
            m_mapScheduleBroadcasts.end() != itLoop; ++itLoop )
    {
        if ( INVALID_TIMER_ID != itLoop->second.first )
        {
            m_thdScheduleTimer.cancel( itLoop->second.first );
            itLoop->second.first = INVALID_TIMER_ID;
        }
        if ( 0 != itLoop->second.second )
        {
            delete itLoop->second.second;
            itLoop->second.second = 0;
        }
    }

    m_mapScheduleBroadcasts.clear();
}

}
