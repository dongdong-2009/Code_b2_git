/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ProposedStationModeUpdateSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/09/27 14:47:02 $
  * Last modified by:  $Author: huangjian $
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
//#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ECSAgentAudit_MessageTypes.h"
#include "core/message/types/ECSAgentComms_MessageTypes.h"
#include "app/ecs/ecs_manager/src/ProposedStationModeUpdateSubscriber.h"
#include "app/ecs/ecs_manager/src/IProposedStationModeObserver.h"
#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"

#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/exceptions/src/DataException.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

	// ExceptionChecked
	ProposedStationModeUpdateSubscriber::ProposedStationModeUpdateSubscriber(TA_IRS_App::IProposedStationModeObserver* observer)
	: m_auditSender(NULL), m_proposedStationModeObserver(observer)
	{
		FUNCTION_ENTRY("Constructor");

		std::vector<TA_Base_Core::IEntityData*> MasterECSAgentData;
		try
		{
			// marvin++ Implementation change. Listen on OCC channel not on station channel for proposals
			m_auditSender = 
				MessagePublicationManager::getInstance().getAuditMessageSender( ECSAgentAudit::Context );

			MasterECSAgentData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::MasterECSAgentEntityData::getStaticType(), 1 );

			if ( 1 != MasterECSAgentData.size() )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "There can only be one Master ECS Agent at a particular location" );
				TA_THROW( TA_Base_Core::DataException( "There can only be one Master ECS Agent at a particular location",TA_Base_Core::DataException::INVALID_VALUE,"Master ECS Agent" ) );
			}

//TD13744, need not subscribe EcsMasterModeExecutionUpdate
//			// Subscribe for Master Mode execution update.
//			MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
//				ECSAgentComms::EcsMasterModeExecutionUpdate,	        // context
//				this,				                                    // SpecialisedMessageSubscriber
//				MasterECSAgentData[0]->getKey(),						// unsigned long entityKey
//				// 10745k-Start
//				// Use IEntityData getSubsystem & getRegion instead of 0
//				// 0
//				MasterECSAgentData[0]->getSubsystem(),					// unsigned long subsystemKey
//				// 1		    											
//				MasterECSAgentData[0]->getRegion() );													// unsigned long regionKey
//				// 10745k-End
			
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

			//TD13744, for this subscriber also response to EcsStationModeExecutionUpdate event,
            //so need to subscribe to EcsStationModeExecutionUpdate too
            TA_Base_Core::CorbaNameList ecsStationAgentCorbaNames;
            ecsStationAgentCorbaNames = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType(TA_Base_Core::StationECSAgentEntityData::getStaticType(),true);

            for (TA_Base_Core::CorbaNameList::iterator itCorbaName = ecsStationAgentCorbaNames.begin();
                    itCorbaName != ecsStationAgentCorbaNames.end(); ++ itCorbaName)
            {
                MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
                    ECSAgentComms::EcsStationModeExecutionUpdate,	        // context
                    this,				                                    // SpecialisedMessageSubscriber
                    itCorbaName->getEntityKey(),						// unsigned long entityKey
                    MasterECSAgentData[0]->getSubsystem(),					// unsigned long subsystemKey
                    itCorbaName->getLocationKey()                   // unsigned long locationKey
                    );													
            }
            
            
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
			// The level above handles any exceptions..
			throw;
		}

		FUNCTION_EXIT;
	}

	// ExceptionChecked
	ProposedStationModeUpdateSubscriber::~ProposedStationModeUpdateSubscriber()
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
	void ProposedStationModeUpdateSubscriber::receiveSpecialisedMessage(const CommsMessageCorbaDef&  message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* proposedStationMode = NULL;
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* overridingStationModes = NULL;

		try
		{
			const MessageType& StationModeExecutionMessageType = TA_Base_Core::ECSAgentComms::EcsStationModeExecutionUpdate;          
			//proposed station mode message type handles both overriding and proposal message
			const MessageType& ProposedStationModeMessageType = TA_Base_Core::ECSAgentComms::EcsProposedModesUpdate;
			const MessageType& OverridingStationModeMessageType = TA_Base_Core::ECSAgentComms::EcsProposedModesUpdate;

			if(0 == StationModeExecutionMessageType.getTypeKey().compare(message.messageTypeKey) )
			{
				TA_IRS_Bus::IStationEcsAgentCorbaDef::EcsStationModeExecutionUpdateData* EcsStationUpdateMsg = NULL;
				if ( (message.messageState >>= EcsStationUpdateMsg) == 0 )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
						"Failed to interpret message");
					return;
				}	
                else if (0 != TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).compare(EcsStationUpdateMsg->sessionId))
                {
                    //TD13744, not the information triggered by me, ingore it
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
                        "Received EcsStationModeExecutionUpdate message is not triggered by this ECS Manager, so ingore it");
                    return;
                }

                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, 
                    "Received EcsStationModeExecutionUpdate message, notifying observer...");
                
				m_proposedStationModeObserver->stationModeExecutionUpdated(true, EcsStationUpdateMsg);
			}
			else if(0 == ProposedStationModeMessageType.getTypeKey().compare(message.messageTypeKey))
			{
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData* proposedModes = NULL;
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData proposedStationMode;
				TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData overridingStationModes;

				if( (message.messageState >>= proposedModes) == 0 ) 
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
						"Failed to interpret message");
					return;
				}
				
				filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION,*proposedModes,proposedStationMode);
				filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION,*proposedModes,overridingStationModes);

				// Process overriding proposed station modes first
				m_proposedStationModeObserver->overridingProposedStationModesUpdated(true, &overridingStationModes);
				m_proposedStationModeObserver->proposedStationModeUpdated( true, &proposedStationMode);	

				//zn++ for logging received data
				logReceivedProposedMode(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION, overridingStationModes);
				logReceivedProposedMode(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_STATION, proposedStationMode);
				//++zn
			}
		}
		catch (...)
		{

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
				"Failed to process data from ");
		}

		FUNCTION_EXIT;
	}


	// marvin++
	void ProposedStationModeUpdateSubscriber::filterProposedMessageByType(TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type,
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes,
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& filteredModes)
	{
		if ( 0 != filteredModes.length() )
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Corba sequence not initialized" );
			return;
		}

		int length = 0;
		for(int ctr=0; ctr < proposedModes.length(); ctr++)
		{
			int mode = proposedModes[ctr].modeType;
			if(proposedModes[ctr].modeType == type)
			{
				length++;
			}
		}

		filteredModes.length(length);

		int toCtr=0;
		for(int frmCtr=0; frmCtr < proposedModes.length(); frmCtr++)
		{
			int mode = proposedModes[frmCtr].modeType;
			if(proposedModes[frmCtr].modeType == type)
			{
				filteredModes[toCtr] = proposedModes[frmCtr];
				toCtr++;
			}
		}
	}
	// ++marvin

	//zn++ for logging received data
	void ProposedStationModeUpdateSubscriber::logReceivedProposedMode(
		TA_IRS_Bus::IMasterEcsAgentCorbaDef::EProposedModeType type, 
		const TA_IRS_Bus::IMasterEcsAgentCorbaDef::EcsProposedModesUpdateData& proposedModes)
	{
		FUNCTION_ENTRY("logReceivedProposedMode");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Proposed mode type: %d received", type);

		if (proposedModes.length() == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "No records received!");
			FUNCTION_EXIT;
			return;
		}

		long mode;
		unsigned long location, locationKeyOfCongestionPair;
		int modeType;

		for (unsigned int i = 0; i < proposedModes.length(); ++i)
		{
			mode = proposedModes[i].mode;
			location = proposedModes[i].locationKey;
			locationKeyOfCongestionPair = proposedModes[i].locationKeyOfCongestionPair;
			modeType = proposedModes[i].modeType;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[Location: %lu] [Congestion Pair: %lu] [Proposed Mode: %li] [Type: %d]", 
				location, locationKeyOfCongestionPair, mode, modeType);
		}

		FUNCTION_EXIT;
	}
	//++zn

}

