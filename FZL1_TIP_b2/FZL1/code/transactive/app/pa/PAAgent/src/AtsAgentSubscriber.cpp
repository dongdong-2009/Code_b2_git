/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/AtsAgentSubscriber.cpp $
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

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/TriggeredBroadcastWorker.h"
#include "app/pa/PAAgent/src/AtsAgentSubscriber.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

using namespace TA_Base_Core;
using namespace TA_IRS_Bus;


namespace TA_IRS_App
{

    // ExceptionChecked
    AtsAgentSubscriber::AtsAgentSubscriber()
    : m_pTriggeredBroadcastWorker(new TriggeredBroadcastWorker()),
      m_locationKey(0)
    {
        FUNCTION_ENTRY("Constructor");

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
		if ( NULL != m_pTriggeredBroadcastWorker )
		{
			delete m_pTriggeredBroadcastWorker;
			m_pTriggeredBroadcastWorker = NULL;
		}

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

        if (!m_bEnable)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s", "Disabled so we ignore it update.");
            FUNCTION_EXIT;
            return;
        }

        TA_Base_Core::ThreadGuard guard(m_lock);

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

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received ATS agent message ATSAgentComms::AtsPasUpdate" );

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
            << " LastTrain=" << messageData.trainInfo.lastTrain
			<< " PlatformDirection=" << messageData.platformDirection;

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
			FUNCTION_EXIT
            return;
        }

        if (NULL != m_pTriggeredBroadcastWorker)
        {
            m_pTriggeredBroadcastWorker->executeSigTriggeredBroadcast(messageData.atsEventType, messageData.platformDirection);
        }

        FUNCTION_EXIT;
    }

    void AtsAgentSubscriber::enable(const bool bEnable)
    {
        m_bEnable = bEnable;
    }

} // namespace TA_IRS_App

