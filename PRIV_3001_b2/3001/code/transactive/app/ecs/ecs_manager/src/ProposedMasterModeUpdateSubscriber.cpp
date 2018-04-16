/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/ProposedMasterModeUpdateSubscriber.cpp $
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

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ECSAgentAudit_MessageTypes.h"
#include "core/message/types/ECSAgentComms_MessageTypes.h"
#include "bus/ecs/ecs_agent/IDL/src/IMasterEcsAgentCorbaDef.h"
#include "app/ecs/ecs_manager/src/ProposedMasterModeUpdateSubscriber.h"
#include "app/ecs/ecs_manager/src/IProposedMasterModeObserver.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

	// ExceptionChecked
	ProposedMasterModeUpdateSubscriber::ProposedMasterModeUpdateSubscriber(TA_IRS_App::IProposedMasterModeObserver* observer)
	: m_auditSender(NULL), m_proposedStationModeObserver(observer)
	{
		FUNCTION_ENTRY("Constructor");
		std::vector<TA_Base_Core::IEntityData*> MasterECSAgentData;

		try
		{
			m_auditSender = 
				MessagePublicationManager::getInstance().getAuditMessageSender( ECSAgentAudit::Context );
		
			MasterECSAgentData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::MasterECSAgentEntityData::getStaticType(), 1 );

			if ( 1 != MasterECSAgentData.size() )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "There can only be one Master ECS Agent at a particular location" );
				TA_THROW( TA_Base_Core::DataException( "There can only be one Master ECS Agent at a particular location",TA_Base_Core::DataException::INVALID_VALUE,"Master ECS Agent" ) );
			}

			// Subscribe for Master Mode execution update.
			MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
				ECSAgentComms::EcsMasterModeExecutionUpdate,	        // context
				this,				                                    // SpecialisedMessageSubscriber
				MasterECSAgentData[0]->getKey(),						// unsigned long entityKey
				// 10745k-Start
				// Use IEntityData getSubsystem & getRegion instead of 0
				// 0
				MasterECSAgentData[0]->getSubsystem(),					// unsigned long subsystemKey
				// 1		    											
				MasterECSAgentData[0]->getRegion() );													// unsigned long regionKey
				// 10745k-End
			// Subscribe for Proposed Master mode updates.
			MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
				ECSAgentComms::EcsProposedModesUpdate,					// context
				this,				                                    // SpecialisedMessageSubscriber
				MasterECSAgentData[0]->getKey(),						// unsigned long entityKey
				// 10745l-Start
 				// Use IEntityData getSubsystem & getRegion instead of 0
				// 0
				MasterECSAgentData[0]->getSubsystem(),					// unsigned long subsystemKey
				// 1		    											
				MasterECSAgentData[0]->getRegion() );					// unsigned long regionKey
				// 10745l-End

			// We're done.. now clean up..
			for(unsigned int i=0; i<MasterECSAgentData.size(); i++)
			{
				if(MasterECSAgentData[i] != NULL)
				{
					delete MasterECSAgentData[i];
					MasterECSAgentData[i] = 0;
				}
			}
			MasterECSAgentData.clear();
		}
		catch(...)
		{
			for(unsigned int i=0; i<MasterECSAgentData.size(); i++)
			{
				if(MasterECSAgentData[i] != NULL)
				{
					delete MasterECSAgentData[i];
					MasterECSAgentData[i] = 0;
				}
			}
			MasterECSAgentData.clear();
			// Throw one level above so that it can be handled. 
			throw;
		}

		FUNCTION_EXIT;
	}

	// ExceptionChecked
	ProposedMasterModeUpdateSubscriber::~ProposedMasterModeUpdateSubscriber()
	{
		FUNCTION_ENTRY("Destructor");

		// Harmless to unsubscribe if not subscribed
		MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		if( m_auditSender )
		{
			delete m_auditSender;
			m_auditSender = NULL;
		}

		FUNCTION_EXIT;
	}


	// ExceptionChecked
	void ProposedMasterModeUpdateSubscriber::receiveSpecialisedMessage(const CommsMessageCorbaDef&  message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");

		try
		{
			const MessageType& MasterModeExecutionMessageType = TA_Base_Core::ECSAgentComms::EcsMasterModeExecutionUpdate;
			const MessageType& ProposedModesMessageType = TA_Base_Core::ECSAgentComms::EcsProposedModesUpdate;

			if(0 == MasterModeExecutionMessageType.getTypeKey().compare(message.messageTypeKey) )
			{
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsMasterModeExecutionUpdateData* EcsMasterModeUpdateMsg = 0;
				if ( (message.messageState >>= EcsMasterModeUpdateMsg) == 0 )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
						"Failed to interpret message");
					return;
				}
				// Implement master mode execution interface.
				m_proposedStationModeObserver->masterModeExecutionUpdated(true, EcsMasterModeUpdateMsg);
			}
			else if(0 == ProposedModesMessageType.getTypeKey().compare(message.messageTypeKey))
			{
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* masterModeUpdate = 0;
				if ( (message.messageState >>= masterModeUpdate) == 0 )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
						"Failed to interpret message");
					return;
				}
						
				m_proposedStationModeObserver->proposedMasterModesUpdated(true, masterModeUpdate);
			}
			
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
				"Failed to process data from ATS");
		}

		FUNCTION_EXIT;
	}

}

