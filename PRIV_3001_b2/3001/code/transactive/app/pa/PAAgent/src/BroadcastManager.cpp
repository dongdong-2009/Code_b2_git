/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/BroadcastManager.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */

#include <iostream>
#include <sstream>
#include <time.h>

#include "ace/OS.h"
#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/CyclicBroadcastWorker.h"
#include "app/pa/PAAgent/src/BroadcastWorker.h"
#include "app/pa/PAAgent/src/ScheduledDvaHistory.h"
#include "app/pa/PAAgent/src/ScheduleDvaIdSupplier.h"
#include "app/pa/PAAgent/src/TrainAgentSubscriber.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/TrainDVABroadcast.h"
#include "app/pa/PAAgent/src/TrainLiveBroadcast.h"
#include "app/pa/PAAgent/src/DVARecordingBroadcast.h"
#include "app/pa/PAAgent/src/MasterDVABroadcast.h"
#include "app/pa/PAAgent/src/MasterLiveBroadcast.h"
#include "app/pa/PAAgent/src/StationDVABroadcast.h"
#include "app/pa/PAAgent/src/StationLiveBroadcast.h"
#include "app/pa/PAAgent/src/StationMusicBroadcast.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"

namespace TA_IRS_App
{

BroadcastManager::BroadcastManager() : 
m_mtxBroadcastsLock(),
m_mapIdToBroadcasts(),
m_pStationBroadcasts( new AbstractBroadcastCircularList() ),
m_pCyclicDvaBroadcasts( new AbstractBroadcastCircularList() ),
m_pTrainBroadcasts( new AbstractBroadcastCircularList() ),
m_pStationBroadcastWorker( NULL ),
m_pTrainBroadcastWorker( NULL ),
m_pCyclicDvaWorker( NULL ),
m_trainAgentSubscriber( NULL ),
m_pScheduledDvaHistory( new ScheduledDvaHistory() ),
m_MusicType(TA_Base_Bus::IPAAgentCorbaDef::NoMusic),
m_bConnectionAvailable( false )
{
    FUNCTION_ENTRY( "BroadcastManager" );

    m_pStationBroadcastWorker = new BroadcastWorker( *m_pStationBroadcasts, *this );
    m_pTrainBroadcastWorker = new BroadcastWorker( *m_pTrainBroadcasts, *this );
    m_pCyclicDvaWorker = new CyclicBroadcastWorker( *m_pCyclicDvaBroadcasts, *m_pScheduledDvaHistory, *this );
    m_trainAgentSubscriber = new TrainAgentSubscriber( *m_pTrainBroadcasts );

    if ( NULL != m_pStationBroadcastWorker )
    {
        m_pStationBroadcastWorker->start();
    }

    if ( NULL != m_pTrainBroadcastWorker )
    {
        m_pTrainBroadcastWorker->start();
    }

    if ( NULL != m_pCyclicDvaWorker )
    {
        m_pCyclicDvaWorker->start();
    }

    FUNCTION_EXIT;
}


BroadcastManager::~BroadcastManager()
{
    FUNCTION_ENTRY( "~BroadcastManager" );

    try
    {
        if ( NULL != m_trainAgentSubscriber )
        {
            delete m_trainAgentSubscriber;
            m_trainAgentSubscriber = NULL;
        }

        if ( NULL != m_pCyclicDvaWorker )
        {
            if ( m_pCyclicDvaWorker->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
            {
                m_pCyclicDvaWorker->terminateAndWait();
            }

            delete m_pCyclicDvaWorker;
            m_pCyclicDvaWorker = NULL;
        }

        if ( NULL != m_pTrainBroadcastWorker )
        {
            if ( m_pTrainBroadcastWorker->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
            {
                m_pTrainBroadcastWorker->terminateAndWait();
            }

            delete m_pTrainBroadcastWorker;
            m_pTrainBroadcastWorker = NULL;
        }

        if ( NULL != m_pStationBroadcastWorker )
        {
            if ( m_pStationBroadcastWorker->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
            {
                m_pStationBroadcastWorker->terminateAndWait();
            }

            delete m_pStationBroadcastWorker;
            m_pStationBroadcastWorker = NULL;
        }

        if ( NULL != m_pScheduledDvaHistory )
        {
            delete m_pScheduledDvaHistory;
            m_pScheduledDvaHistory = NULL;
        }

        if ( NULL != m_pCyclicDvaBroadcasts )
        {
            // Cleanup time for all broadcasts
            m_pCyclicDvaBroadcasts->clearAllBroadcasts();

            delete m_pCyclicDvaBroadcasts;
            m_pCyclicDvaBroadcasts = NULL;
        }

        if ( NULL != m_pTrainBroadcasts )
        {
            // Cleanup time for all broadcasts
            m_pTrainBroadcasts->clearAllBroadcasts();

            delete m_pTrainBroadcasts;
            m_pTrainBroadcasts = NULL;
        }

        if ( NULL != m_pStationBroadcasts )
        {
            // Cleanup time for all broadcasts
            m_pStationBroadcasts->clearAllBroadcasts();

            delete m_pStationBroadcasts;
            m_pStationBroadcasts = NULL;
        }
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Exception caught in destructor");
    }

    FUNCTION_EXIT;
}

void BroadcastManager::enable( const bool bEnable )
{
    if ( NULL != m_pStationBroadcastWorker )
    {
        m_pStationBroadcastWorker->enable( bEnable );
    }
    if ( NULL != m_pTrainBroadcastWorker )
    {
        m_pTrainBroadcastWorker->enable( bEnable );
    }
    if ( NULL != m_pCyclicDvaWorker )
    {
        m_pCyclicDvaWorker->enable( bEnable );
    }
}

bool BroadcastManager::isBroadcastIdExisted( const std::string& strId )
{
    ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);

    IdToBroadcastMapIt itFound = m_mapIdToBroadcasts.find( strId );
    if ( m_mapIdToBroadcasts.end() != itFound )
    {
        return true;
    }

    return false;
}

AbstractBroadcast* BroadcastManager::getBroadcastById( const std::string& strId )
{
    AbstractBroadcast* pBroadcast = NULL;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);

        IdToBroadcastMapIt itFound = m_mapIdToBroadcasts.find( strId );
        if ( m_mapIdToBroadcasts.end() != itFound )
        {
            pBroadcast = itFound->second;
        }
    }

    return pBroadcast;
}

void BroadcastManager::addTrainBroadcast( AbstractBroadcast* pBroadcast )
{
    if ( NULL == pBroadcast )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Broadcast list is NULL" );
        return;
    }

    if ( NULL == m_pTrainBroadcasts )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Train broadcast list is NULL" );
        return;
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);
        m_pTrainBroadcasts->insert( pBroadcast );

        m_mapIdToBroadcasts.insert( IdToBroadcastMap::value_type( pBroadcast->getBroadcastId(), pBroadcast ));
    }
}

void BroadcastManager::addStationBroadcast( AbstractBroadcast* pBroadcast )
{
    if ( NULL == pBroadcast )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Broadcast list is NULL" );
        return;
    }

    if ( NULL == m_pStationBroadcasts )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station broadcast list is NULL" );
        return;
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);
        m_pStationBroadcasts->insert( pBroadcast );

        m_mapIdToBroadcasts.insert( IdToBroadcastMap::value_type( pBroadcast->getBroadcastId(), pBroadcast ));
    }
}

void BroadcastManager::addCyclicBroadcast( AbstractBroadcast* pBroadcast )
{
    if ( NULL == pBroadcast )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Broadcast list is NULL" );
        return;
    }

    if ( NULL == m_pCyclicDvaBroadcasts )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cyclic broadcast list is NULL" );
        return;
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);
        m_pCyclicDvaBroadcasts->insert( pBroadcast );

        m_mapIdToBroadcasts.insert( IdToBroadcastMap::value_type( pBroadcast->getBroadcastId(), pBroadcast ));
    }
}

void BroadcastManager::delBroadcast( const std::string& strId )
{
    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType eType = TA_Base_Bus::IPAAgentCorbaDef::StationMusic;
    std::string stdSession = "";

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);

        IdToBroadcastMapIt itFound = m_mapIdToBroadcasts.find( strId );
        if ( m_mapIdToBroadcasts.end() == itFound )
        {
            return;
        }

        m_pStationBroadcasts->remove( itFound->second );
        m_pCyclicDvaBroadcasts->remove( itFound->second );
        m_pTrainBroadcasts->remove( itFound->second );

        eType = itFound->second->getBroadcastType();
        stdSession = itFound->second->getSessionId().c_str();

        delete itFound->second;
        itFound->second = NULL;

        m_mapIdToBroadcasts.erase( itFound );
    }

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = strId.c_str();
    updateData.toBroadcastId = CORBA::string_dup("");
    updateData.broadcastType = eType;
    updateData.sessionId = stdSession.c_str();

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentRemovedBroadcastUpdate( strId.c_str() );
}


void BroadcastManager::getCompleteBroadcastStateSeqCopy( TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq& refState )
{
    FUNCTION_ENTRY( "getCompleteBroadcastStateSeqCopy" );

    TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq broadcastIdTypeSeq;
    broadcastIdTypeSeq.length( 0 );
    m_pStationBroadcasts->appendAllBroadcastIdTypes(broadcastIdTypeSeq);
    m_pCyclicDvaBroadcasts->appendAllBroadcastIdTypes(broadcastIdTypeSeq);
    m_pTrainBroadcasts->appendAllBroadcastIdTypes(broadcastIdTypeSeq);

    refState.length(broadcastIdTypeSeq.length());
    int numberOfsomehowRemovedBroadcasts = 0;
    for (unsigned int index = 0; index < broadcastIdTypeSeq.length(); ++index)
    {
        AbstractBroadcast* broadcast = getBroadcastById(broadcastIdTypeSeq[index].broadcastId.in());

        if ( broadcast == NULL)
        {
            ++numberOfsomehowRemovedBroadcasts;
            continue;
        }

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfig =
            broadcast->getBroadcastConfigCopy();

        TA_Base_Bus::IPAAgentCorbaDef::ProgressReport*  progressReport =
            broadcast->getProgressReportCopy();
		if (NULL == progressReport)
		{
			LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugError, "getProgressReportCopy() is NULL");
			TA_THROW(TA_Base_Core::TransactiveException("getProgressReportCopy() is NULL"));
		}

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq*  broadcastParameters =
            broadcast->getBroadcastParametersCopy();

        refState[index].broadcastId =
            broadcast->getBroadcastId().c_str();
        refState[index].theBroadcastConfig = *broadcastConfig;
        refState[index].theProgressReport = *progressReport;
        refState[index].broadcastParameters = *broadcastParameters;

        delete broadcastConfig;
        delete progressReport;
        delete broadcastParameters;
    }

    // resize for lost broadcasts
    refState.length(broadcastIdTypeSeq.length() - numberOfsomehowRemovedBroadcasts);

    FUNCTION_EXIT;
}


void BroadcastManager::processPAAgentNewBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig& updateData )
{
    FUNCTION_ENTRY( "processPAAgentNewBroadcastUpdate" );

    AbstractBroadcast* broadcast = NULL;
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newConfig = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();

    // Solid copy
    *newConfig = updateData;

    if ( updateData.broadcastType == TA_Base_Bus::IPAAgentCorbaDef::StationMusic)
    {
        // copy the music status from the "control" agent
        m_MusicType = updateData.theBroadcastConfigSpecifics.theStationMusicConfig().musicType;

        // Music broadcast is a synchronized call
        //broadcast = new StationMusicBroadcast( newConfig, "" );

        //addStationBroadcast( broadcast );

        delete newConfig;
        newConfig = NULL;

        return;
    }
    else if ( updateData.broadcastType == TA_Base_Bus::IPAAgentCorbaDef::StationLive)
    {
        if (CachedConfig::getInstance()->getIsMaster())
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterLiveBroadcast.");
            broadcast = new MasterLiveBroadcast( newConfig, "" );
        }
        else
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationLiveBroadcast.");
            broadcast = new StationLiveBroadcast( newConfig, "" );
        }

        addStationBroadcast( broadcast );
    }
    else if ( updateData.broadcastType == TA_Base_Bus::IPAAgentCorbaDef::StationDva)
    {
        if (CachedConfig::getInstance()->getIsMaster())
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterDVABroadcast.");
            broadcast = new MasterDVABroadcast( newConfig, "" );
        }
        else
        {
            LOG_GENERIC(SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationDVABroadcast.");
            broadcast = new StationDVABroadcast( newConfig, "" );
        }

        if ( updateData.theBroadcastConfigSpecifics.theStationDvaConfig().bCyclic )
        {
            addCyclicBroadcast( broadcast );
        }
        else
        {
            addStationBroadcast( broadcast );
        }
    }
    else if ( updateData.broadcastType == TA_Base_Bus::IPAAgentCorbaDef::StationRecording)
    {
        broadcast = new DVARecordingBroadcast( newConfig, "" );

        addStationBroadcast( broadcast );
    }
    else if ( updateData.broadcastType == TA_Base_Bus::IPAAgentCorbaDef::TrainLive)
    {
        broadcast = new TrainLiveBroadcast(newConfig, "");

        addTrainBroadcast( broadcast );
    }
    else if ( updateData.broadcastType == TA_Base_Bus::IPAAgentCorbaDef::TrainDva)
    {
        broadcast = new TrainDVABroadcast(newConfig, "");

        addTrainBroadcast( broadcast );
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "NewBroadcastUpdate message processing failed: Unknown broadcastType" );

        FUNCTION_EXIT;
        return;
    }

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "NewBroadcastUpdate message processed: BroadcastId=%s", broadcast->getBroadcastId().c_str() );

    FUNCTION_EXIT;
}


void BroadcastManager::processPAAgentRemovedBroadcastUpdate( const std::string& updateData )
{
    FUNCTION_ENTRY( "processPAAgentRemovedBroadcastUpdate" );

    if ( updateData.empty() )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
            "Invalid input parameter [Empty String] while calling StationBroadcastProxy::processPAAgentRemovedBroadcastUpdate" );
        return;
    }

    delBroadcast( updateData );

    FUNCTION_EXIT;
}


void BroadcastManager::processPAAgentRenamedBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData& updateData )
{
    FUNCTION_ENTRY( "processPAAgentRenamedBroadcastUpdate" );

    updateBroadcastId( updateData.fromBroadcastId.in(), updateData.toBroadcastId.in() );

    FUNCTION_EXIT;
}


void BroadcastManager::processPAAgentBroadcastParameterUpdate( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter& updateData )
{
    FUNCTION_ENTRY( "processPAAgentBroadcastParameterUpdate" );

    AbstractBroadcast* broadcast = getBroadcastById(updateData.broadcastId.in());

    if ( NULL != broadcast )
    {
        // Logging included
        broadcast->setBroadcastParameter(updateData);
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "BroadcastParameterUpdate message processing: BroadcastId=%s unknown", updateData.broadcastId.in() );
    }

    FUNCTION_EXIT;
}


void BroadcastManager::processPAAgentBroadcastProgressUpdate( const TA_Base_Bus::IPAAgentCorbaDef::ProgressReport& updateData )
{
    FUNCTION_ENTRY( "processPAAgentBroadcastProgressUpdate" );

    AbstractBroadcast* broadcast = getBroadcastById(updateData.broadcastId.in());

    if ( NULL != broadcast )
    {
        broadcast->setProgressReport(updateData);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "BroadcastProgressUpdate message processed: BroadcastId=%s", updateData.broadcastId.in() );

    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
            "BroadcastProgressUpdate message processing: BroadcastId=%s unknown", updateData.broadcastId.in() );
    }

    FUNCTION_EXIT;
}


void BroadcastManager::processPAAgentEntireAgentStateUpdate( const TA_Base_Bus::IPAAgentCorbaDef::CompleteAgentState& updateData )
{
    FUNCTION_ENTRY( "processPAAgentEntireAgentStateUpdate" );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Processing EntireAgentStateUpdate message" );

    for (unsigned int index = 0; index < updateData.broadcastStates.length(); ++index)
    {
        processPAAgentNewBroadcastUpdate( updateData.broadcastStates[index].theBroadcastConfig );
        processPAAgentBroadcastProgressUpdate( updateData.broadcastStates[index].theProgressReport );

        for (unsigned int parameterIndex = 0; parameterIndex < updateData.broadcastStates[index].broadcastParameters.length(); ++parameterIndex)
        {
            processPAAgentBroadcastParameterUpdate( updateData.broadcastStates[index].broadcastParameters[parameterIndex] );
        }
    }

    if ( updateData.bAvailableIdInitialized )
    {
        setCompleteAvaliableEventIds( updateData.availableIds );
    }

    FUNCTION_EXIT;
}

void BroadcastManager::setModeChangingFlag(bool flag)
{
    if ( NULL != m_pStationBroadcastWorker )
    {
        m_pStationBroadcastWorker->setModeChangingFlag( flag );
    }
    if ( NULL != m_pTrainBroadcastWorker )
    {
        m_pTrainBroadcastWorker->setModeChangingFlag( flag );
    }
    if ( NULL != m_pCyclicDvaWorker )
    {
        m_pCyclicDvaWorker->setModeChangingFlag( flag );
    }
}

const int BroadcastManager::getMusicStatus()
{
    return m_MusicType;
}

void BroadcastManager::setMusicStatus( const int eType )
{
    m_MusicType = eType;
}

unsigned long BroadcastManager::getTrainBroadcastCount()
{
    ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);
    return m_pTrainBroadcasts->getSize();
}

unsigned long BroadcastManager::getStationBroadcastCount()
{
    ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);
    return m_pStationBroadcasts->getSize() + m_pCyclicDvaBroadcasts->getSize();
}

TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* BroadcastManager::getTrainBroadcastIdTypes()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* broadcastIdTypes = 
        new TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq();
    broadcastIdTypes->length( 0 );

    if ( NULL == m_pTrainBroadcasts )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Train broadcast list is NULL" );
        return broadcastIdTypes;
    }

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);
        m_pTrainBroadcasts->appendAllBroadcastIdTypes(*broadcastIdTypes);
    }

    return broadcastIdTypes;
}

TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* BroadcastManager::getStationBroadcastIdTypes()
{
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* broadcastIdTypes = 
        new TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq();
    broadcastIdTypes->length( 0 );

    if (( NULL == m_pStationBroadcasts ) || ( NULL == m_pCyclicDvaBroadcasts ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Station broadcast list is NULL" );
        return broadcastIdTypes;
    }

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);
        m_pStationBroadcasts->appendAllBroadcastIdTypes(*broadcastIdTypes);
        m_pCyclicDvaBroadcasts->appendAllBroadcastIdTypes(*broadcastIdTypes);
    }

    return broadcastIdTypes;
}

bool BroadcastManager::updateBroadcastId( const std::string& strSrcId, const std::string& strObjId )
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard(m_mtxBroadcastsLock);

        IdToBroadcastMapIt itFound = m_mapIdToBroadcasts.find( strObjId );
        if ( m_mapIdToBroadcasts.end() != itFound )
        {
            return false;
        }

        itFound = m_mapIdToBroadcasts.find( strSrcId );
        if ( m_mapIdToBroadcasts.end() == itFound )
        {
            return false;
        }

        AbstractBroadcast* pTemp = itFound->second;
        m_mapIdToBroadcasts.erase( itFound );
        m_mapIdToBroadcasts.insert( IdToBroadcastMap::value_type(strObjId, pTemp) );
		try
		{
			pTemp->setBroadcastId( strObjId );
		}
		catch ( ... )
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception when call setBroadcastId");
			return false;			
		}
        
    }

    return true;
}

bool BroadcastManager::getCompleteAvaliableEventIds( TA_Base_Bus::IPAAgentCorbaDef::ScheduleDvaIds& refEventIds )
{
    std::set<unsigned short> setAllIds;
    if ( !ScheduleDvaIdSupplier::getInstance().getAllAvailableEventIds( setAllIds ))
    {
        return false;
    }

    refEventIds.length( setAllIds.size() );

    int nIndex = 0;
    for ( std::set<unsigned short>::iterator itLoop = setAllIds.begin(); setAllIds.end() != itLoop; ++itLoop )
    {
        refEventIds[nIndex++] = *itLoop;
    }

    return true;
}

void BroadcastManager::setCompleteAvaliableEventIds( const TA_Base_Bus::IPAAgentCorbaDef::ScheduleDvaIds& refEventIds )
{
    std::set<unsigned short> setAllIds;
    for ( unsigned int unLoop = 0u; unLoop < refEventIds.length(); ++unLoop )
    {
        setAllIds.insert( refEventIds[unLoop] );
    }

    ScheduleDvaIdSupplier::getInstance().setAllAvailableEventIds( setAllIds );
}

void BroadcastManager::connectionUpdated( bool bConnectionAvailable )
{
    m_bConnectionAvailable = bConnectionAvailable;
}

bool BroadcastManager::isConnectionAvaliable()
{
    return m_bConnectionAvailable;
}

void BroadcastManager::restoreScheduledDva()
{
    std::vector<ScheduleDvaConfig> vecScheduledDva = m_pScheduledDvaHistory->getAllScheduledDva();
    for( std::vector<ScheduleDvaConfig>::const_iterator it = vecScheduledDva.begin(); it != vecScheduledDva.end(); ++it )
    {
        const ScheduleDvaConfig& refScheduledDva = *it;

        try
        {
            TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
            TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

            newBroadcastConfigP->broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
            newBroadcastConfigP->broadcastId = refScheduledDva.strScheuleName.c_str();
            newBroadcastConfigP->isEventTriggered = false;
            newBroadcastConfigP->submitTime = time( NULL );

            PasHelpers::getInstance()->stdConfigToCorbaConfig( refScheduledDva, theSpecifics );

            newBroadcastConfigP->theBroadcastConfigSpecifics.theStationDvaConfig( theSpecifics );

            AbstractBroadcast* newBroadcast = NULL;
            if( CachedConfig::getInstance()->getIsMaster() )
            {
                LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating MasterDVABroadcast." );
                newBroadcast = new MasterDVABroadcast( newBroadcastConfigP, "" );
            }
            else
            {
                LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugInfo, "Creating StationDVABroadcast." );
                newBroadcast = new StationDVABroadcast( newBroadcastConfigP, "" );
            }

            newBroadcast->setWasExecutedSuccessfully( true );
            newBroadcast->setBroadcastState( TA_Base_Bus::IPAAgentCorbaDef::Runtime );
			ScheduleDvaIdSupplier::getInstance().reserveEventId(refScheduledDva.usScheduleId);
            addCyclicBroadcast( newBroadcast );
        }
        catch( TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        }
		catch( std::exception& expWhat)
		{
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
		}
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown" );
        }
    }
}

} // namespace TA_IRS_App

