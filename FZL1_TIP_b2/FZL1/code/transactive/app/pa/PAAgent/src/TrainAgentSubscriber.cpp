/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/TrainAgentSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/AbstractTrainBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/TrainAgentSubscriber.h"
//#include "app/pa/PAAgent/src/TrainLiveBroadcast.h"
//#include "app/pa/PAAgent/src/TrainDVABroadcast.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h"
#include "core/message/types/TrainAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_Base_Core;


namespace TA_IRS_App
{

        TrainAgentSubscriber::TrainAgentSubscriber()
        : m_allBroadcasts(NULL),
          m_iscsLibraryVersion(0)
        {
            FUNCTION_ENTRY( "TrainAgentSubscriber" );

            m_allBroadcasts = AbstractBroadcastCircularList::getInstance();

            // close all library version alarms on startup
            PasHelpers::getInstance()->closeTrainLibraryVersionAlarms();

            m_trainPaCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainPaCorbaProxy();
            m_trainPaCorbaProxy->addObserver(this);

            FUNCTION_EXIT;
        }


        void TrainAgentSubscriber::subscribe()
        {
            FUNCTION_ENTRY( "subscribe" );

            TA_Base_Core::ThreadGuard guard(m_lock);

            if (CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty())
            {
                // No local Train Agent was found on startup.
                FUNCTION_EXIT;
                return;
            }

    // subscribe to PA library version updates from train agent
    // only at the OCC or the depot
    if (CachedConfig::getInstance()->getIsMaster() || CachedConfig::getInstance()->getIsDepot())
    {
        // also need to know the global iscs version
        TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().registerGlobalVersionObserver(this);
        onGlobalTrainVersionChanged();
    }
    
    FUNCTION_EXIT;
}


        void TrainAgentSubscriber::unsubscribe()
        {
            FUNCTION_ENTRY( "unsubscribe" );

            TA_Base_Core::ThreadGuard guard(m_lock);

            if (CachedConfig::getInstance()->getIsMaster() || CachedConfig::getInstance()->getIsDepot())
            {
                TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap().deRegisterGlobalVersionObserver(this);
            }

            FUNCTION_EXIT;
        }


        TrainAgentSubscriber::~TrainAgentSubscriber()
        {
            FUNCTION_ENTRY( "~TrainAgentSubscriber" );

            // Will assert if not unsubscribed by client code.

            TA_Base_Core::ThreadGuard guard(m_lock);

            // Just to ensure that we don't start deleting until the messaging has finished
            // processing the last message.

            m_allBroadcasts = NULL;

            FUNCTION_EXIT;
        }


        void TrainAgentSubscriber::onGlobalTrainVersionChanged()
        {
            FUNCTION_ENTRY( "onGlobalTrainVersionChanged" );

            m_iscsLibraryVersion = TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getLibraryVersion();

            PasHelpers::getInstance()->closeTrainLibraryVersionAlarmOnVersionChange(m_iscsLibraryVersion);

            FUNCTION_EXIT;
        }


        void TrainAgentSubscriber::processPaAnnouncementUpdate( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& event )
        {
            FUNCTION_ENTRY( "processPaAnnouncementUpdate" );

            std::stringstream msg;
            msg << "Processing Live Announcement info"
                << " AnnouncementId=" << static_cast<int>(event.announcementId)
                << " UpdateType=" << TA_IRS_Bus::PaTypes::paAnnouncementUpdateTypeToString( event.updateType );

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());

            //AbstractTrainBroadcast* trainBroadcast =
            //    m_allBroadcasts->getTrainBroadcast(event.announcementId);

            //TrainLiveBroadcast* liveBroadcast = dynamic_cast<TrainLiveBroadcast*>(trainBroadcast);

            //if (liveBroadcast)
            //{
            //    msg.str() = "";
            //    msg << "Located Train broadcast BroadcastId=" << liveBroadcast->getBroadcastId();
            //    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());

            //    liveBroadcast->processRuntimeNotification( event );
            //}

            FUNCTION_EXIT;
        }


        void TrainAgentSubscriber::processPaUpdateEvent( const TA_IRS_Bus::PaTypes::PaUpdateEvent& event )
        {
            FUNCTION_ENTRY( "processPaUpdateEvent" );

            std::stringstream msg;
            msg << "Processing PA update event"
                << " TrainId=" << static_cast<int>(event.trainId)
                << " AnnouncementId=" << static_cast<int>(event.announcementId)
                << " SessionId=" << event.sessionId
                << " UpdateType=" << TA_IRS_Bus::PaTypes::paUpdateTypeToString( event.updateType );

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog,
                 TA_Base_Core::DebugUtil::DebugInfo,
                 msg.str().c_str() );

            // if the announcement ID is set, get the live broadcast for this announcement
            if ( 0 != event.announcementId )
            {
                AbstractTrainBroadcast* trainBroadcast =
                    m_allBroadcasts->getTrainBroadcast(event.trainId, event.announcementId);

                if ( NULL != trainBroadcast )
                {
                    msg.str() = "";
                    msg << "Located Train broadcast BroadcastId=" << trainBroadcast->getBroadcastId();
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());

                    trainBroadcast->processRuntimeNotification( event );
                }
            }

            
            //Enable this if MPU changeover should terminate DVA broadcasts
            //Also see processChangeoverStatusEvent in train agent TimsPaManager.cpp

            // if this is a reset notification
            //if ( TA_IRS_Bus::PaTypes::AnnouncementReset == event.updateType )
            //{
            //    // all train DVA broadcasts with this train in them need to be set to reset

            //    std::list<TrainDVABroadcast*> trainDvaList = m_allBroadcasts->getAllTrainDvaBroadcasts();
            //
            //    for ( std::list<TrainDVABroadcast*>::iterator iter = trainDvaList.begin();
            //          trainDvaList.end() != iter; ++iter )
            //    {
            //        (*iter)->processRuntimeNotification( event );
            //    }
            //}

            

            FUNCTION_EXIT;
        }


        void TrainAgentSubscriber::processTrainLibraryInfo( const TA_IRS_Bus::PaTypes::TrainLibraryInfo& event )
        {
            FUNCTION_ENTRY( "processTrainLibraryInfo" );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Processing Live Announcement info Train=%d Version=%d",
                         static_cast<int>(event.trainId),
                         static_cast<int>(event.libraryVersion) );

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

