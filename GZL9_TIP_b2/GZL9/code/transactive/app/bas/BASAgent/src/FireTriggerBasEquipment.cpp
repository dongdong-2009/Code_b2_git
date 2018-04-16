/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/FireTriggerBasEquipment.cpp $
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
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/bas/BASAgent/src/FireTriggerBasEquipment.h"
#include "app/bas/BASAgent/src/FireTriggerBasEquipmentManager.h"

namespace TA_IRS_App
{
	using namespace TA_Base_Core;
	FireTriggerBasEquipment::FireTriggerBasEquipment(const TA_IRS_Core::BasFireEquipmentPtr& basFireEquipmentPtr, FireTriggerBasEquipmentManager* fireTriggerManager)
		:m_fireTriggerBasEquipmentManager(fireTriggerManager)
	{
		m_basFireEquipmentPtr = basFireEquipmentPtr;
		try
		{
			initializeFireEquipment();
		}
		catch (TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when initializeFireEquipment");
		}
		
	}

	FireTriggerBasEquipment::~FireTriggerBasEquipment(void)
	{
	}

	//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
	void FireTriggerBasEquipment::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("processEntityUpdateEvent");

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"Receive datapoint update entitykey=%lu entityName=%s ScadaEntityUpdateType=%d", 
			entityKey, m_basFireEquipmentPtr->fireToken.c_str(), updateType);

		switch( updateType )
		{
		case TA_Base_Bus::ValueStateUpdate:
			break;
		default:
			return;
		}

		try
		{
#if !defined(BAS_TEST_MODE)
			if (TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON != m_dpDatapoint_FireTrigger->getStatus())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "This datapoint NOT good quality, so we ignore it update.");
				return;
			}
#endif

			TA_IRS_Core::BasStationFireStatus status = getCurrentDpValueToStationFireStatus();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"process Entity Update Event location=%d FireTriggerEquipment=%s datapointEntityKey=%d ScadaEntityUpdateType=%d Station fire stats:%d",
				m_basFireEquipmentPtr->locationKey, m_basFireEquipmentPtr->fireToken.c_str(), entityKey, updateType, status);

			if (NULL != m_fireTriggerBasEquipmentManager)
			{
				m_fireTriggerBasEquipmentManager->processFireTriggerBas(m_basFireEquipmentPtr->locationKey, m_basFireEquipmentPtr->modeValue, status);
			}			
		}
		catch (TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when processEntityUpdateEvent");
		}

		FUNCTION_EXIT;
	}

	void FireTriggerBasEquipment::notifyQuit()
	{
		if (NULL != m_proxyFactory)
		{
			m_proxyFactory->setProxiesToMonitorMode();
			m_fireTriggerBasEquipmentManager = NULL;
			TA_Base_Bus::ScadaProxyFactory::removeInstance();
		}
	}

	void FireTriggerBasEquipment::initializeFireEquipment()
	{
		FUNCTION_ENTRY("initializeFireEquipment");
		// get a handle to the ScadaProxyFactory
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
		if (NULL == m_proxyFactory)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "m_proxyFactory is NULL.");
			return;
		}
		m_proxyFactory->setProxiesToControlMode();

		TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_basFireEquipmentPtr->fireToken);
		TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData);
		if (NULL == dpEntityData)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Equipment is not defined as datapoint entity.");
			return;
		}

		TA_Base_Core::DataPointEntityDataPtr dp(dpEntityData);
		m_proxyFactory->createDataPointProxy(dp, *this, m_dpDatapoint_FireTrigger);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Fire trigger Equipment [%s] [%s] is created.", dp->getName().c_str(), dp->getDescription().c_str());
		FUNCTION_EXIT;
	}

	TA_IRS_Core::BasStationFireStatus FireTriggerBasEquipment::getCurrentDpValueToStationFireStatus()
	{
		TA_IRS_Core::BasStationFireStatus retValue;
		bool fireOrNormal = m_dpDatapoint_FireTrigger->getValueAsBool();
		if (!fireOrNormal)
		{
			retValue = TA_IRS_Core::FireStatus_Normal;
		}
		else
		{
			retValue = TA_IRS_Core::FireStatus_Fire;
		}
		return retValue;
	}
}
