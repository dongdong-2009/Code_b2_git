/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/TrainAgentSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/TrainAgentSubscriber.h"
#include "app/pa/PAAgent/src/TrainLiveBroadcast.h"
#include "app/pa/PAAgent/src/TrainDVABroadcast.h"

#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

TrainAgentSubscriber::TrainAgentSubscriber( AbstractBroadcastCircularList& refObject ) : 
m_allBroadcasts( refObject ),
m_iscsLibraryVersion(0)
{
    FUNCTION_ENTRY( "TrainAgentSubscriber" );

    subscribe();

    FUNCTION_EXIT;
}


TrainAgentSubscriber::~TrainAgentSubscriber()
{
    FUNCTION_ENTRY( "~TrainAgentSubscriber" );

    unsubscribe();

    TA_Base_Core::ThreadGuard guard(m_lock);

    FUNCTION_EXIT;
}


void TrainAgentSubscriber::subscribe()
{
    FUNCTION_ENTRY( "subscribe" );

    TA_Base_Core::ThreadGuard guard(m_lock);

    if ( !CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty() )
    {
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainPaCorbaProxy()->addObserver(this);
    }

    // subscribe to PA library version updates from train agent only at the OCC or the depot
    if (CachedConfig::getInstance()->getIsMaster() || CachedConfig::getInstance()->getIsDepot())
    {
        // also need to know the global ISCS version
        TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().registerGlobalVersionObserver(this);
        onGlobalTrainVersionChanged();
    }

    FUNCTION_EXIT;
}


void TrainAgentSubscriber::unsubscribe()
{
    FUNCTION_ENTRY( "unsubscribe" );

    TA_Base_Core::ThreadGuard guard(m_lock);

    if ( !CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty() )
    {
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainPaCorbaProxy()->removeObserver(this);
    }

    if (CachedConfig::getInstance()->getIsMaster() || CachedConfig::getInstance()->getIsDepot())
    {
        TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().deRegisterGlobalVersionObserver(this);
    }

    FUNCTION_EXIT;
}


void TrainAgentSubscriber::onGlobalTrainVersionChanged()
{
    FUNCTION_ENTRY( "onGlobalTrainVersionChanged" );

    try
    {
        m_iscsLibraryVersion = TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getLibraryVersion();

        PasHelpers::getInstance()->closeTrainLibraryVersionAlarmOnVersionChange(m_iscsLibraryVersion);
    }
    catch(TA_Base_Core::DataException& exp )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", exp.what() );
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Catch unknown exception");
    }

    FUNCTION_EXIT;
}


void TrainAgentSubscriber::processPaAnnouncementUpdate( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& event )
{
    FUNCTION_ENTRY( "processPaAnnouncementUpdate" );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Processing Live Announcement info AnnouncementId=%d UpdateType=%s", 
        static_cast<int>(event.announcementId), TA_IRS_Bus::PaTypes::paAnnouncementUpdateTypeToString( event.updateType ).c_str());

    AbstractTrainBroadcast* trainBroadcast = m_allBroadcasts.getTrainBroadcast(event.announcementId);

    TrainLiveBroadcast* liveBroadcast = dynamic_cast<TrainLiveBroadcast*>(trainBroadcast);

    if (liveBroadcast)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Located Train broadcast BroadcastId=%s", liveBroadcast->getBroadcastId().c_str());

        liveBroadcast->processRuntimeNotification( event );
    }

    FUNCTION_EXIT;
}


void TrainAgentSubscriber::processPaUpdateEvent( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event )
{
    FUNCTION_ENTRY( "processPaUpdateEvent" );

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Processing PA update event TrainId=%d AnnouncementId=%d SessionId=%s UpdateType=%s",
        static_cast<int>(event.trainId), static_cast<int>(event.announcementId), event.sessionId.c_str(), TA_IRS_Bus::PaTypes::paUpdateTypeToString( event.updateType ).c_str());

    // if the announcement ID is set, get the live broadcast for this announcement
    if ( 0 != event.announcementId )
    {
        AbstractTrainBroadcast* trainBroadcast = m_allBroadcasts.getTrainBroadcast(event.trainId, event.announcementId);

        if ( NULL != trainBroadcast )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "Located Train broadcast BroadcastId=%s", trainBroadcast->getBroadcastId().c_str());

            trainBroadcast->processRuntimeNotification( event );
        }
    }

    //Enable this if MPU changeover should terminate DVA broadcasts
    //Also see processChangeoverStatusEvent in train agent TimsPaManager.cpp

    // if this is a reset notification
    if ( TA_IRS_Bus::PaTypes::AnnouncementReset == event.updateType )
    {
        // all train DVA broadcasts with this train in them need to be set to reset

        std::list<TrainDVABroadcast*> trainDvaList = m_allBroadcasts.getAllTrainDvaBroadcasts();
    
        for ( std::list<TrainDVABroadcast*>::iterator iter = trainDvaList.begin(); trainDvaList.end() != iter; ++iter )
        {
            (*iter)->processRuntimeNotification( event );
        }
    }

    FUNCTION_EXIT;
}


void TrainAgentSubscriber::processTrainLibraryInfo( const TA_IRS_Bus::PaTypes::TrainLibraryInfo& event )
{
    FUNCTION_ENTRY( "processTrainLibraryInfo" );

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Processing Live Announcement info Train=%d Version=%lu", static_cast<int>(event.trainId), event.libraryVersion );

    // compare the versions
    if( event.libraryVersion != m_iscsLibraryVersion)
    {
        // raise an alarm
        PasHelpers::getInstance()->raiseTrainLibraryVersionAlarm(event.trainId, event.libraryVersion, m_iscsLibraryVersion);
    }
    else
    {
        PasHelpers::getInstance()->closeTrainLibraryVersionAlarm(event.trainId);
    }

    FUNCTION_EXIT;
}


} // namespace TA_IRS_App

