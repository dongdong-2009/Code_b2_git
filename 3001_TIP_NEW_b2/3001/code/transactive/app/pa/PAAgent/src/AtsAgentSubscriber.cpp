/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/AtsAgentSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/09/30 11:36:59 $
  * Last modified by:  $Author: huangjian $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/StationBroadcastProxy.h"
#include "app/pa/PAAgent/src/AtsAgentSubscriber.h"

using namespace TA_Base_Core;
using namespace TA_IRS_Bus;


namespace TA_IRS_App
{

    // ExceptionChecked
    AtsAgentSubscriber::AtsAgentSubscriber(StationBroadcastProxy& theBroadcastManager)
    : m_broadcastManager(theBroadcastManager),
      m_locationKey( CachedConfig::getInstance()->getAgentLocationKey() )
    {
        FUNCTION_ENTRY("Constructor");

        FUNCTION_EXIT;
    }

    // ExceptionChecked
    AtsAgentSubscriber::~AtsAgentSubscriber()
    {
        FUNCTION_ENTRY("Destructor");

        TA_Base_Core::ThreadGuard guard(m_lock);

        // Just to ensure that we don't start deleting until the messaging has finished 
        // processing the last message.

        FUNCTION_EXIT;
    }


    void AtsAgentSubscriber::subscribe()
    {
        FUNCTION_ENTRY("subscribe");

        if ( !CachedConfig::getInstance()->getIsLocalATSAgentConfigured() )
        {
            // No local Ats Agent was found on startup.  
            return;
        }

        // Subscribe for updates from the local Ats Agent
        MessageSubscriptionManager::getInstance().subscribeToCommsMessage( ATSAgentComms::AtsPasUpdate,                            //context
            this,                                                   //SpecialisedMessageSubscriber
            CachedConfig::getInstance()->getLocalATSAgentKey(),     //unsigned long entityKey
            0,                                                      //unsigned long subsystemKey
            CachedConfig::getInstance()->getAgentLocationKey() );   //unsigned long regionKey

        FUNCTION_EXIT;
    }


    void AtsAgentSubscriber::unsubscribe()
    {
        FUNCTION_ENTRY("unsubscribe");

        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        
        FUNCTION_EXIT;
    }
        

    // ExceptionChecked
    void AtsAgentSubscriber::receiveSpecialisedMessage(const CommsMessageCorbaDef&  message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
        TA_Base_Core::ThreadGuard guard(m_lock);

        try
        {
            const TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef* messageData = 0;
            if ( (message.messageState >>= messageData) == 0 )
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "Failed to interpret message");
                return;
            }

            processMessage(*messageData);
        }
        catch(...)
        {
        }

        FUNCTION_EXIT;
    }


    // ExceptionChecked
    void AtsAgentSubscriber::processMessage( const TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef& messageData )
    {
        FUNCTION_ENTRY("processMessage");

        unsigned long effectiveDestinationStationId = messageData.trainInfo.valid ? messageData.trainInfo.destinationStationId : 0u; // Unallocated if invalid
        bool effectiveLastTrain = messageData.trainInfo.valid ? messageData.trainInfo.lastTrain : false;       // Not the last train if invalid

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Processing Ats info: EventType=%d StationId=%lu PlatformId=%lu Valid=%d DestinationStationId=%lu LastTrain=%d", 
            messageData.atsEventType, messageData.stationId, messageData.platformId, messageData.trainInfo.valid,
            messageData.trainInfo.destinationStationId, messageData.trainInfo.lastTrain);

        if (!messageData.trainInfo.valid)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                "Ats info is not valid StationId=%lu, IsLastTrain=%d", effectiveDestinationStationId, effectiveLastTrain);
        }

        if (messageData.stationId != m_locationKey)
        {
            return;
        }

        PaAtsTriggeredBroadcastRecord record = CachedMaps::getInstance()->getAtsTriggeredBroadcastRecordFromDetail(
            messageData.atsEventType, messageData.stationId, effectiveDestinationStationId, messageData.platformId, effectiveLastTrain);
         
        if (record.m_pKey == TA_Base_Core::DatabaseKey::getInvalidKey())
        {
            // Not found - ignore
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Event triggered broadcast not configured - ignored");
            return;
        }

        if (!record.m_isEnabled)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Event triggered broadcast has been disabled - ignored");
            return;
        }

        PaZoneGroupRecord groupRecord;

        try
        {
            groupRecord = CachedMaps::getInstance()->getPaZoneGroupRecordFromKey(record.m_paZoneGroupKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException& )
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Event triggered Station broadcast failed - invalid zone group key %lu", record.m_paZoneGroupKey );
            return;
        }

        // Verify validity of zone group
        if (!groupRecord.m_isEventGroup || (groupRecord.m_locationKey != m_locationKey))
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Event triggered Station broadcast failed - invalid zone group key %lu", record.m_paZoneGroupKey );
            return;
        }

        // Create request:
        m_broadcastManager.broadcastEventTriggeredStationDvaForGroup( record.m_paDvaMessageKey,
            record.m_dvaMessageVersion.c_str(), false, false, 0, 0, 
            StationBroadcastProxy::ATS_AGENT_SESSION_ID.c_str(), groupRecord.m_label.c_str());
        FUNCTION_EXIT;
    }


} // namespace TA_IRS_App

