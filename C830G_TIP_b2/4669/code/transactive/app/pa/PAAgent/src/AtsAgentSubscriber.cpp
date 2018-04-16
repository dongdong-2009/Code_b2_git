/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/AtsAgentSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/AtsAgentSubscriber.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

using namespace TA_Base_Core;
using namespace TA_IRS_Bus;


namespace TA_IRS_App
{

    // ExceptionChecked
    AtsAgentSubscriber::AtsAgentSubscriber(BroadcastManager& theBroadcastManager)
    : m_broadcastManager(theBroadcastManager),
      m_locationKey(0)
    {
        FUNCTION_ENTRY("Constructor");
        TA_ASSERT(!CachedConfig::getInstance()->getIsMaster(), "AtsAgentSubscriber created for master");

        m_locationKey = CachedConfig::getInstance()->getAgentLocationKey();
        
        subscribe();

        FUNCTION_EXIT;
    }

    // ExceptionChecked
    AtsAgentSubscriber::~AtsAgentSubscriber()
    {
        FUNCTION_ENTRY("Destructor");

        unsubscribe();
        
        TA_Base_Core::ThreadGuard guard(m_lock);

        // Just to ensure that we don't start deleting until the messaging has finished 
        // processing the last message.

        FUNCTION_EXIT;
    }

    
void AtsAgentSubscriber::subscribe()
{
    FUNCTION_ENTRY("subscribe");
        
        if ( false == CachedConfig::getInstance()->getIsLocalATSAgentConfigured() )
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
	    
        // TODO:: check operation mode, only Control Mode will process this msg
        try
        {
            TA_Base_Bus::GenericAgent::ensureControlMode();
        }
        catch (TA_Base_Core::OperationModeException &)
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            const TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef* messageData = 0;
            if ( (message.messageState >>= messageData) == 0 )
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to interpret message");
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

        int effectiveDestinationStationId(0); // Unallocated if invalid
        bool effectiveLastTrain(false);       // Not the last train if invalid

        if (messageData.trainInfo.valid)
        {
            effectiveDestinationStationId = messageData.trainInfo.destinationStationId;
            effectiveLastTrain = messageData.trainInfo.lastTrain; 
        }


        std::stringstream msg;
        msg << "Processing Ats info"
            << " EventType=" << messageData.atsEventType
            << " StationId=" << messageData.stationId
            << " PlatformId=" << messageData.platformId
            << " Valid=" << messageData.trainInfo.valid
            << " DestinationStationId=" << messageData.trainInfo.destinationStationId
            << " LastTrain=" << messageData.trainInfo.lastTrain;

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());

        if (!messageData.trainInfo.valid)
        {
            msg << "Ats info is not valid"
                << " EffectiveDestinationStationId=" << effectiveDestinationStationId
                << " EffectiveLastTrain=" << effectiveLastTrain;

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, msg.str().c_str());
        }

        if (messageData.stationId != m_locationKey)
        {
            return;
        }

        PaAtsTriggeredBroadcastRecord record = CachedMaps::getInstance()->getAtsTriggeredBroadcastRecordFromDetail(
            messageData.atsEventType,
            messageData.stationId,
            effectiveDestinationStationId,
            messageData.platformId,
            effectiveLastTrain);
         
        if (record.m_pKey == TA_Base_Core::DatabaseKey::getInvalidKey())
        {
            // Not found - ignore
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
                "Event triggered broadcast not configured - ignored");
            return;
        }

        if (!record.m_isEnabled)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
                "Event triggered broadcast has been disabled - ignored");
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
                         "Event triggered Station broadcast failed - invalid zone group key %d",
                         record.m_paZoneGroupKey );
            return;
        }

        // Verify validity of zone group
        if (!groupRecord.m_isEventGroup || (groupRecord.m_locationKey != m_locationKey))
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Event triggered Station broadcast failed - invalid zone group key %d",
                         record.m_paZoneGroupKey );
            return;
        }

        // Create request:
        m_broadcastManager.broadcastEventTriggeredStationDvaForGroup( record.m_paDvaMessageKey,
                                                                      record.m_dvaMessageVersion.c_str(), 
                                                                      false,
                                                                      false,
                                                                      0,
                                                                      0,
                                                                      BroadcastManager::ATS_AGENT_SESSION_ID.c_str(),
                                                                      groupRecord.m_label.c_str());
        FUNCTION_EXIT;
    }


} // namespace TA_IRS_App

