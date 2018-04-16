/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/FireTriggerBasEquipmentManager.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#include <sstream>
#include "app/bas/BASAgent/src/FireTriggerBasEquipmentManager.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ECSAgentComms_MessageTypes.h"
#include "core/message/types/ControlStationComms_MessageTypes.h"
#include "app/bas/BASAgent/src/StationModeManager.h"
#include "app/bas/BASAgent/src/StationFireMonitorThread.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "bus/application_types/src/apptypes.h"
#include "core/message/src/MessageConstants.h"
#include "core/utilities/src/TA_String.h"

namespace TA_IRS_App
{
	using namespace TA_Base_Core;
	FireTriggerBasEquipmentManager::FireTriggerBasEquipmentManager(StationModeManager* basAgent)
		:m_basAgentPtr(basAgent),m_fireMonitorThread(NULL)
	{
	}

	FireTriggerBasEquipmentManager::~FireTriggerBasEquipmentManager(void)
	{
		for (std::vector<FireTriggerBasEquipment*>::iterator it = m_allFireTriggerEquipments.begin();
				it != m_allFireTriggerEquipments.end(); it++)
		{
			(*it)->notifyQuit();
			delete (*it);
			(*it) = NULL;
		}
		m_allFireTriggerEquipments.clear();
		m_fireMonitorThread->terminate();
	}

	void FireTriggerBasEquipmentManager::initializeManager()
	{
		// create the monitor thread
		m_fireMonitorThread = new StationFireMonitorThread(this);
		if (NULL == m_fireMonitorThread)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "can not start the fire monitor thread");
		}
		else
		{
			m_fireMonitorThread->start();
		}

		unsigned long locationKey = BasCachedConfig::getInstance().getLocationKey();
		TA_IRS_Core::ModeAccessFactory::getInstance().getModes(locationKey, m_allModes);

		// create the fire equipment
		createAllFireTriggerEquipment();


		MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( 
			TA_Base_Core::ECSAgentComms::EcsStationFireACK, this, NULL);
		MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( 
			TA_Base_Core::ControlStationComms::LaunchApplicationACK, this, NULL);
	}

	void FireTriggerBasEquipmentManager::uninitializeManager()
	{
		MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		if (NULL != m_fireMonitorThread)
		{
			m_fireMonitorThread->notifyToQuit();
			m_fireMonitorThread->notifyStationFire();
		}
	}

	void FireTriggerBasEquipmentManager::processFireTriggerBas(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus fireOrNormal)
	{
		if (!BasCachedConfig::getInstance().isStationFireMonitorValid())
		{
			std::string strEntityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Current FAS trigger BAS switch is OFF, so we will ignore the event.");
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "If you want open the switch, please let the entity [%s] parameter [StationFireMonitorSecond] value > 0 ", strEntityName.c_str());
			return;
		}

		bool bNeedUpdate = false;
		{
			TA_THREADGUARD(m_fireModeLock);		

			AllFireModes::iterator itLocationFire = m_allfireModes.find(locationKey);
			if (itLocationFire == m_allfireModes.end()) // first add the fire status in this location
			{
				FireInfo fireInfo;
				fireInfo.modeValue = modeValue;
				fireInfo.fireTime = getCurrentTime();
				if (TA_IRS_Core::FireStatus_Fire == fireOrNormal)
				{
					fireInfo.whetherProcessed = false;
					fireInfo.status = TA_IRS_Core::FireStatus_Fire;					
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Record the station Fire, LocationKey[%u] ModeValue[%u]", locationKey, modeValue);
					bNeedUpdate = true;
				}
				else
				{
					fireInfo.whetherProcessed = true;
					fireInfo.status = TA_IRS_Core::FireStatus_Normal;
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Record the station Normal, LocationKey[%u] ModeValue[%u]", locationKey, modeValue);
				}
				m_allfireModes[locationKey].push_back(fireInfo);
			}
			else // have add the location fire vector
			{
				LocationFires::iterator itFire;
				for (itFire = itLocationFire->second.begin(); itFire != itLocationFire->second.end(); itFire++)
				{
					if ((*itFire).modeValue == modeValue)
					{
						break;
					}
				}

				if (itFire == itLocationFire->second.end()) //NOT find this fire mode
				{
					FireInfo fireInfo;
					fireInfo.modeValue = modeValue;
					fireInfo.fireTime = getCurrentTime();
					if (TA_IRS_Core::FireStatus_Fire == fireOrNormal)
					{
						fireInfo.whetherProcessed = false;
						fireInfo.status = TA_IRS_Core::FireStatus_Fire;
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Record the station Fire, LocationKey[%u] ModeValue[%u]", locationKey, modeValue);
						bNeedUpdate = true;
					} 
					else
					{
						fireInfo.whetherProcessed = true;
						fireInfo.status = TA_IRS_Core::FireStatus_Normal;
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Record the station Normal, LocationKey[%u] ModeValue[%u]", locationKey, modeValue);
					}
					m_allfireModes[locationKey].push_back(fireInfo);
				}
				else
				{
					if (fireOrNormal == itFire->status)
					{
						// there have record it so do NOTHING
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This datapoint value same as last time, so ignore.");
					} 
					else
					{
						itFire->status = fireOrNormal;
						itFire->whetherProcessed = false;
						itFire->fireTime = getCurrentTime();
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This Station Fire mode status change, LocationKey[%u] ModeValue[%u]", locationKey, modeValue);
						bNeedUpdate = true;
					}
				}
			}
		}
		
		if (bNeedUpdate)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
				"Now notify to process this fire event LocationKey[%u] ModeValue[%u] FireStatus[%s]",
				locationKey, modeValue, TA_IRS_Core::FireStatus_Normal == fireOrNormal ? "Normal" : "Fire");
			if (NULL != m_fireMonitorThread)
			{
				m_fireMonitorThread->notifyStationFire();
			} 
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "The NULL==m_fireMonitorThread, application error, please notify programmer. ");
			}
		}
	}

	void FireTriggerBasEquipmentManager::createAllFireTriggerEquipment()
	{
		using namespace TA_Base_Core;
		using namespace TA_IRS_Core;

		unsigned long locationKey = BasCachedConfig::getInstance().getLocationKey();
		BasFireEquipmentVct basLocationFireEquipment;
		BasFireAccessFactory::getInstance().getAllBasFireEquipmentByLocation(locationKey, basLocationFireEquipment);

		for (BasFireEquipmentVct::iterator itEquipment = basLocationFireEquipment.begin(); itEquipment != basLocationFireEquipment.end(); itEquipment++)
		{
			m_allFireTriggerEquipments.push_back(new FireTriggerBasEquipment(*itEquipment, this));
		}
	}

	time_t FireTriggerBasEquipmentManager::getCurrentTime()
	{
		return time(0);
	}

	bool FireTriggerBasEquipmentManager::isTimeExpired(time_t fireTime)
	{
		// in seconds
		static unsigned long monitorTime = BasCachedConfig::getInstance().getStationFireMonitorTime();

		time_t currentTime = getCurrentTime();
		return currentTime - fireTime > monitorTime ? true : false;
	}

	unsigned long FireTriggerBasEquipmentManager::getSubsystemKeyByModeValue(unsigned long modeValue)
	{
		for (std::vector<TA_IRS_Core::Mode_SmartPtr>::iterator it = m_allModes.begin();
				it != m_allModes.end(); it++)
		{
			if ((*it)->getValue() == modeValue)
			{
				return (*it)->getSubsystemKey();
			}
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "can not find the mode in database ModeValue=%u", modeValue);
		return 0;
	}

	bool FireTriggerBasEquipmentManager::isSubsystemModeMatched(unsigned long subsystemKey, unsigned long modeValue)
	{
		if (NULL == m_basAgentPtr)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "m_basAgentPtr is NULL.");
			return false;
		}

        try
        {
		    return (m_basAgentPtr->getCurrentModeValue(subsystemKey) == modeValue);
		}
		catch (const TA_Base_Core::TransactiveException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		}
		catch(const std::exception& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(e).name(), e.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "UnknownException when get mode Value.");
		}
        
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Get mode value failed.");
		return false;
	}

	bool FireTriggerBasEquipmentManager::needProcessUnprocessedFireMode()
	{
		AllFireModes fireModeInfo;
		getExpiredStationFires(fireModeInfo);

		unsigned long locationKey = BasCachedConfig::getInstance().getLocationKey();
		AllFireModes::iterator it = fireModeInfo.find(locationKey);
		if (it != fireModeInfo.end())
		{
			return it->second.empty() ? false : true;
		}
		return false;
	}

	void FireTriggerBasEquipmentManager::getExpiredStationFires(AllFireModes& fireModeInfo)
	{
		TA_THREADGUARD(m_fireModeLock);	
		fireModeInfo.clear();
		unsigned long locationKey = BasCachedConfig::getInstance().getLocationKey();

		AllFireModes::iterator it = m_allfireModes.find(locationKey);
		if (it != m_allfireModes.end())
		{
			for (LocationFires::iterator itFire = it->second.begin(); itFire != it->second.end(); itFire++)
			{
				if (false == itFire->whetherProcessed)
				{
					fireModeInfo[locationKey].push_back(*itFire);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "This fire mode expired, ModeRunningTime[%u] CurrentTime[%u] ModeValue[%u]", itFire->fireTime, getCurrentTime(), itFire->modeValue);
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
						"This fire mode not expired, ModeRunningTime[%u] CurrentTime[%u] ModeValue[%u] Status[%s]", 
						itFire->fireTime, getCurrentTime(), itFire->modeValue, TA_IRS_Core::FireStatus_Fire==itFire->status ? "Fire" : "Normal");
				}
			}			
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "can not find the station fire trigger in location %u", locationKey);
		}
	}

	void FireTriggerBasEquipmentManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
	{
		if (0 == std::string(message.messageTypeKey.in()).compare(TA_Base_Core::ControlStationComms::LaunchApplicationACK.getTypeKey()))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Receive a LaunchApplicationACK message");
			TA_Base_Core::IControlStationCorbaDef::LaunchApplicationACKEvent* event;
			if((message.messageState >>= event) != 0)
			{
				if (event->locationKey == BasCachedConfig::getInstance().getLocationKey())
				{
					receiveLaunchApplicationAckMessage(*event);
				} 
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Receive this LaunchApplicationACK message NOT match this location");
				}				
			}
		}
		else if (0 == std::string(message.messageTypeKey.in()).compare(TA_Base_Core::ECSAgentComms::EcsStationFireACK.getTypeKey()))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Receive a EcsStationFireACK message");
			TA_IRS_Bus::IStationBasAgentCorbaDef::StationFireNotifyACKEvent* event;
			if((message.messageState >>= event) != 0)
			{
				if (event->locationKey == BasCachedConfig::getInstance().getLocationKey())
				{
					receiveStationFireACKMessage(*event);
				} 
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Receive this EcsStationFireACK message NOT match this location");
				}				
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Receive a unknown corba message");
		}
	}

	void FireTriggerBasEquipmentManager::receiveLaunchApplicationAckMessage(TA_Base_Core::IControlStationCorbaDef::LaunchApplicationACKEvent event)
	{
		TA_THREADGUARD(m_launchAppLock);

		TA_IRS_App::LaunchApplicationACKEvent temp;
		temp.locationKey = event.locationKey;
		temp.applicationType = event.applicationType;
		temp.bSuccess = event.bSuccess;
		m_receivedLaunchAppEvent.push_back(temp);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "receiveLaunchApplicationAckMessage locationKey[%u] application[%u] Success[%s]",
			temp.locationKey, temp.applicationType, temp.bSuccess ? "true" : "false");
	}

	void FireTriggerBasEquipmentManager::receiveStationFireACKMessage(TA_IRS_Bus::IStationBasAgentCorbaDef::StationFireNotifyACKEvent event)
	{
		TA_THREADGUARD(m_StationFireLock);

		TA_IRS_App::StationFireNotifyACKEvent temp;
		temp.locationKey = event.locationKey;
		temp.modeValue = event.modeValue;
		m_receivedStationFireEvent.push_back(temp);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "receiveStationFireACKMessage locationKey[%u] modeValue[%u]",
			temp.locationKey, temp.modeValue);
	}

	bool FireTriggerBasEquipmentManager::isReceiveLaunchAppAckMessage(unsigned long locationKey, unsigned long appType)
	{
		TA_THREADGUARD(m_launchAppLock);
		bool bFound = false;
		for (std::vector<LaunchApplicationACKEvent>::iterator it = m_receivedLaunchAppEvent.begin();
			it != m_receivedLaunchAppEvent.end(); )
		{
			if (it->locationKey == locationKey && it->applicationType == appType)
			{
				bFound = bFound || it->bSuccess;
				it = m_receivedLaunchAppEvent.erase(it);
			}
			else
			{
				it++;
			}
		}
		return bFound;
	}

	bool FireTriggerBasEquipmentManager::isReceiveStationFireAckMessage(unsigned long locationKey, unsigned long modeValue)
	{
		TA_THREADGUARD(m_StationFireLock);
		
		bool bFound = false;
		for (std::vector<StationFireNotifyACKEvent>::iterator it = m_receivedStationFireEvent.begin();
			it != m_receivedStationFireEvent.end(); )
		{
			if (it->locationKey == locationKey && it->modeValue == modeValue)
			{
				bFound = true;
				it = m_receivedStationFireEvent.erase(it);
			}
			else
			{
				it++;
			}
		}
		return bFound;
	}

	void FireTriggerBasEquipmentManager::sendLaunchApplicaitonMessage(unsigned long locationKey, unsigned long appType, std::string runParameter)
	{
		if ((NULL == m_basAgentPtr) || (!m_basAgentPtr->isAgentRunningControlMode()))
		{
			return;
		}

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sendLaunchApplicaitonMessage locationKey = %lu, appType = %lu, runParameter = %s", locationKey, appType, runParameter.c_str());

		CORBA::Any data;
		TA_Base_Core::IControlStationCorbaDef::LaunchApplicationEvent event;
		event.runParameter = CORBA::string_dup(runParameter.c_str());
		event.locationKey = locationKey;
		event.applicationType = appType;
		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::ControlStationComms::LaunchApplicationACK);
		if (NULL != commsMessageSender)
		{
			unsigned long locationKey = BasCachedConfig::getInstance().getLocationKey();
			unsigned long entityKey = BasCachedConfig::getInstance().getEntityKey();
			
			commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::ControlStationComms::LaunchApplication, data, NULL);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sendLaunchApplicaitonMessage success locationKey = %lu, appType = %lu, runParameter = %s", locationKey, appType, runParameter.c_str());
		} 
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "can not get the TA_Base_Core::ECSAgentComms::EcsLaunchApplication comms sender");
		}
	}

	void FireTriggerBasEquipmentManager::sendStationFireMessage(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status)
	{
		if ((NULL == m_basAgentPtr) || (!m_basAgentPtr->isAgentRunningControlMode()))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Agent is work in monitor mode, so will NOT sendStationFireMessage");
			return;
		}

		CORBA::Any data;
		TA_IRS_Bus::IStationBasAgentCorbaDef::StationFireNotifyEvent event;
		event.locationKey = locationKey;
		event.modeValue = modeValue;		
		event.status = TA_IRS_Core::FireStatus_Normal == status ? TA_IRS_Bus::IStationBasAgentCorbaDef::StationFire_Normal : TA_IRS_Bus::IStationBasAgentCorbaDef::StationFire_Fire;
		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::ECSAgentComms::EcsStationFireNotification);
		if (NULL != commsMessageSender)
		{
			unsigned long locationKey = BasCachedConfig::getInstance().getLocationKey();
			unsigned long entityKey = BasCachedConfig::getInstance().getEntityKey();

            NameValuePair regionFilter( TA_Base_Core::REGION_FILTER_NAME, TA_Base_Core::toString(locationKey) );
            FilterData filterData;
            filterData.push_back(&regionFilter);
			commsMessageSender->sendBroadcastCommsMessage(TA_Base_Core::ECSAgentComms::EcsStationFireNotification , data, &filterData);

		} 
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "can not get the TA_Base_Core::ECSAgentComms::EcsLaunchApplication comms sender");
		}
	}
	
	void FireTriggerBasEquipmentManager::processStationFireMode()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Now process the Station fire trigger event.");

		TA_THREADGUARD(m_fireModeLock);	
		if (m_allfireModes.size() > 0)
		{
			unsigned long locationKey = BasCachedConfig::getInstance().getLocationKey();
			AllFireModes::iterator itFireVect = m_allfireModes.find(locationKey);
			for (LocationFires::iterator it = itFireVect->second.begin(); it != itFireVect->second.end(); it++)
			{
#if !defined (BAS_TEST_MODE)
				bool whetherExpired = isTimeExpired(it->fireTime);
				bool whetherModeMatched = isSubsystemModeMatched(getSubsystemKeyByModeValue(it->modeValue), it->modeValue);

				if ((TA_IRS_Core::FireStatus_Normal == it->status && true == it->whetherProcessed ) ||
					(TA_IRS_Core::FireStatus_Fire == it->status && (it->whetherProcessed == true || false == whetherExpired || true == whetherModeMatched)))
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
						"NO need process this time Status[%s] whetherProcessed[%s] whetherExpired[%s] whetherModeMatched[%s]",
						TA_IRS_Core::FireStatus_Fire==it->status ? "Fire" : "Normal", 
						it->whetherProcessed ? "true" : "false", 
						whetherExpired ? "true" : "false", whetherModeMatched ? "true" : "false");
					continue;
				}
#endif

				try
				{
					processStationFireMode(locationKey, it->modeValue, it->status);
				}
				catch (TA_Base_Core::TransactiveException& err)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
				}
				catch (...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown Exception when processStationFireMode.");
				}
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The expired fire mode map empty");
		}
	}

#ifdef BAS_TEST_MODE
 	void FireTriggerBasEquipmentManager::processStationFireMode(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status)
 	{
        if (TA_IRS_Core::FireStatus_Fire == status)
        {
            launchApplication(locationKey, modeValue, BAS_EMERGENCY_MODE_MANAGER);
        }
 		sendStationFireNotifyCation(locationKey, modeValue, status);
 	}
#else
	void FireTriggerBasEquipmentManager::processStationFireMode(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "processStationFireMode locationKey = %lu, modeValue = %lu, status = %lu", locationKey, modeValue, status);
        if (TA_IRS_Core::FireStatus_Fire == status)
        {
            if ( launchApplication(locationKey, modeValue, BAS_EMERGENCY_MODE_MANAGER) )
            {
                setStationFireProcessed(locationKey, modeValue);
            }
        }
        else if(sendStationFireNotifyCation(locationKey, modeValue, status))
		{
            setStationFireProcessed(locationKey, modeValue);
		}

	}
#endif

	bool FireTriggerBasEquipmentManager::launchApplication(unsigned long locationKey, unsigned long modeValue, unsigned long ApplicationType)
	{
		bool bSuccess = false;
		try
		{
            std::stringstream launchParameter;
            launchParameter << " --ManagerMode=StationFire" << " --FireLocationKey=" << locationKey << " --ModeValue=" << modeValue;

			sendLaunchApplicaitonMessage(locationKey, ApplicationType, launchParameter.str());//this runparameter used in control station class sessionmanager

			int tryTimes = 10;
			do 
			{
				TA_Base_Core::Thread::sleep(500);
				bSuccess = isReceiveLaunchAppAckMessage(locationKey, ApplicationType);
			} 
			while ( tryTimes-- > 0 && false == bSuccess);

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "launchApplication Location[%u] application[%u] %s",
				locationKey, ApplicationType, bSuccess ? "true" : "false");
		}
		catch(TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when launchApplication.");
		}
		return bSuccess;
	}

	bool FireTriggerBasEquipmentManager::sendStationFireNotifyCation(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status)
	{
		bool bSuccess = false;
		try
		{
			sendStationFireMessage(locationKey, modeValue, status);

			int tryTimes = 10;
			do 
			{
				TA_Base_Core::Thread::sleep(500);
				bSuccess = isReceiveStationFireAckMessage(locationKey, modeValue);
			} 
			while ( tryTimes-- > 0 && false == bSuccess);

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "send Station Fire NotifyCation Location[%u] modeValue[%u] Success[%s]",
				locationKey, modeValue, bSuccess ? "true" : "false");
		}
		catch(TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when sendStationFireNotifyCation.");
		}
		return bSuccess;
	}

	void FireTriggerBasEquipmentManager::setStationFireProcessed(unsigned long locationKey, unsigned long modeValue)
	{
		TA_THREADGUARD(m_StationFireLock);
		AllFireModes::iterator itFireVect = m_allfireModes.find(locationKey);
		if (itFireVect != m_allfireModes.end())
		{
			for (LocationFires::iterator it = itFireVect->second.begin(); it != itFireVect->second.end(); it++)
			{
				if (it->modeValue == modeValue)
				{
					it->whetherProcessed = true;
					break;
				}
			}
		}		
	}
}
