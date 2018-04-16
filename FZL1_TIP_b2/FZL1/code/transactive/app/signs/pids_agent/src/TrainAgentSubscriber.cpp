/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/StringHelper.h $
* @author:  Albertus Zakaria
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/10/01 16:36:38 $
* Last modified by:  $Author: grace.koh $
*
* A place holder for generic string functions
*
*/

#include "app/signs/pids_agent/src/TrainAgentSubscriber.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "bus/ats/ats_agent/IDL/src/IAtsPidsCorbaDef.h"
#include "app/signs/pids_agent/src/PIDSManager.h"
#include "app/signs/pids_agent/src/PIDSCommunicationsHandler.h"
#include "app/signs/pids_agent/src/PIDSSigDisplayRequest.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	const int TrainAgentSubscriber::SIGINFO_FEP_REGISTER_ADDRESS = 512;
	TrainAgentSubscriber::~TrainAgentSubscriber()
	{
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
	}

	void TrainAgentSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& commsm)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		if(TA_Base_Core::ATSAgentComms::AtsPidsUpdate.getTypeKey() == commsm.messageTypeKey.in())
		{
			TA_IRS_Bus::IAtsPidsCorbaDef::PidsInfoList *pList = NULL;
			if(commsm.messageState >>= pList)
			{
				PIDSSigDisplayRequest pidsSigRequest;
				pidsSigRequest.setData(*pList);
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending sig display request message" );
				PIDSCommunicationsHandler::getInstance()->sendMessage(&pidsSigRequest, true, SIGINFO_FEP_REGISTER_ADDRESS);
			}
		}
		FUNCTION_EXIT;	
	}

	void TrainAgentSubscriber::subscribeToMessages(unsigned long locationKey)
	{
		FUNCTION_ENTRY( "subscribeToMessages" );
		try
		{
			TA_Base_Core::IEntityDataList entityData =
				TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
				TA_Base_Core::AtsAgentEntityData::getStaticType(), locationKey );


			if( entityData.size() != 1)
			{
				const char* msg = "There is no ATS agent at this location.";
				const char* fld = "ATS Agent";
				TA_THROW(TA_Base_Core::DataException(msg, TA_Base_Core::DataException::MISSING_MANDATORY, fld));
			}

			unsigned long entityKey    = entityData[0]->getKey();
			unsigned long subsystemKey = entityData[0]->getSubsystem();

			delete entityData[0];

			TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
				TA_Base_Core::ATSAgentComms::AtsPidsUpdate, 
				this,
				entityKey,
				0,
				locationKey
				);
		}
		catch ( const TA_Base_Core::DatabaseException& dbe )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
		}
		catch ( const TA_Base_Core::DataException& de )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
		}
		FUNCTION_EXIT;
	}
}