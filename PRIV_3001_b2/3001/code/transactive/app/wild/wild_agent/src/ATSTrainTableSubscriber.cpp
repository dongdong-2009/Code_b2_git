/* ATSTrainTableSubscriber
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/app/wild/wild_agent/src/ATSTrainTableSubscriber.cpp $
 * @author:  <Andy Siow>
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Implementation file
 */

#include "app/wild/wild_agent/src/ATSTrainTableSubscriber.h"
#include "app/wild/wild_agent/src/WildAgent.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "app/wild/wild_agent/src/SingletonTrainDataMap.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/WILDAgentExceptions.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
	ATSTrainTableSubscriber::ATSTrainTableSubscriber()
		: m_atsLocationKey(0)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "ATSTrainTableSubscriber");

		try
		{
			m_atsLocationKey = atol(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
			// Unsubscribe just in case.
			TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

			// Subscribe for Train updates from the local ATS Agent
			TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
				TA_Base_Core::ATSAgentComms::WILDTrainUpdate,	//context
				this,										//SpecialisedMessageSubscriber
				getATSAgentKey(),							//unsigned long entityKey,
 				0,		    								//unsigned long subsystemKey,
				m_atsLocationKey );							//unsigned long regionKey)
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", 
				"WILD Configuration error. Rethrow as Generic Agent exception");

			TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
			TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR));
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "ATSTrainTableSubscriber");
	}


	ATSTrainTableSubscriber::~ATSTrainTableSubscriber()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "~ATSTrainTableSubscriber");
		// Unsubscribe to ATSAgentComms::WILDTrainUpdate
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "~ATSTrainTableSubscriber");
	}


	unsigned long ATSTrainTableSubscriber::getATSAgentKey() const
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "getATSAgentKey");

		std::vector<TA_Base_Core::IEntityData*> ATSAgentData;
		
		try
		{
			ATSAgentData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::AtsAgentEntityData::getStaticType(), m_atsLocationKey );
			if(ATSAgentData.size() != 1)
			{
				for(int i=0; i<ATSAgentData.size(); i++)
				{
					delete ATSAgentData[i];
					ATSAgentData[i] = 0;
				}
				ATSAgentData.clear();

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
					"Multiple ATSAgent entities was retrieved for the given type and location");

				TA_THROW(TA_Base_Core::InvalidWILDConfigurationException("An error occured whilst retrieving ATS Agent Entity"));
			}

			unsigned long ATSAgengEntityKey = ATSAgentData[0]->getKey();
			
			delete ATSAgentData[0];
			ATSAgentData[0] = 0;
			ATSAgentData.clear();

			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "getATSAgentKey");
			return ATSAgengEntityKey;

		}
		catch(...)
		{
			for(int i=0; i<ATSAgentData.size(); i++)
			{
				delete ATSAgentData[i];
				ATSAgentData[i] = 0;
				
			}
			ATSAgentData.clear();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
				"Failed to retrieve ATS Agent entity information from the database.");

			TA_THROW(TA_Base_Core::InvalidWILDConfigurationException("An error occured whilst retrieving ATS Agent Entity"));
		}
	}


	void ATSTrainTableSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "ATSTrainTableSubscriber::receiveSpecialisedMessage");
		TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* ATSTrainMessage = 0;
		
		if(0 == TA_Base_Core::ATSAgentComms::WILDTrainUpdate.getTypeKey().compare(message.messageTypeKey))
		{
			if ( 0 != (message.messageState >>= ATSTrainMessage) )
			{
				SingletonTrainDataMap::getInstance()->extractAndInsertIntoMap(*ATSTrainMessage);
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
					"Failed to interpret WILDTrainUpdate message");

				LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "ATSTrainTableSubscriber::receiveSpecialisedMessage");
				return;
			}
		}

		ATSTrainMessage = 0;

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "ATSTrainTableSubscriber::receiveSpecialisedMessage");
	}

}; //TA_App
