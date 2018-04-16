
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/ModbusPacket.h"
#include "app/pa/PAAgent/src/TriggeredBroadcastWorker.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/FireAlarmSubscriber.h"

namespace TA_IRS_App
{
	using namespace TA_Base_Core;
	FireAlarmSubscriber::FireAlarmSubscriber() :
    m_pTriggeredBroadcastWorker(new TriggeredBroadcastWorker()),
    m_bInFire(false)
	{
		try
		{
			initializeFireTrigger();
		}
		catch (TA_Base_Core::TransactiveException& err)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", err.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Unknown exception when initializeFireTrigger");
		}
	}

	FireAlarmSubscriber::~FireAlarmSubscriber()
    {
        if (NULL != m_pTriggeredBroadcastWorker)
        {
            delete m_pTriggeredBroadcastWorker;
            m_pTriggeredBroadcastWorker = NULL;
        }
	}

	void FireAlarmSubscriber::initializeFireTrigger()
	{
		FUNCTION_ENTRY("initializeFireTrigger");
		// get a handle to the ScadaProxyFactory
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
		if(NULL == m_proxyFactory)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SCADA Proxy Factory not created");
			return;
		}
		m_proxyFactory->setProxiesToControlMode();

		TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(CachedConfig::getInstance()->getFasEntityName());
		TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData);
		if(NULL == dpEntityData)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Fire equipment of triggering broadcast is not defined as datapoint entity");
			return;
		}

		TA_Base_Core::DataPointEntityDataPtr dp(dpEntityData);

		m_proxyFactory->createDataPointProxy(dp, *this, m_dpDatapoint_FireTrigger);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Fire trigger Equipment [%s] [%s] is created.", dp->getName().c_str(), dp->getDescription().c_str());
		FUNCTION_EXIT;
	}

	//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
    void FireAlarmSubscriber::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("processEntityUpdateEvent");

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
			"Receive datapoint update entitykey=%lu entityName=%s ScadaEntityUpdateType=%s", 
			entityKey, CachedConfig::getInstance()->getFasEntityName().c_str(), scadaEntityUpdateTypeToString(updateType).c_str());

        if (updateType == TA_Base_Bus::AgentConnected)
        {
            try
            {
                m_bInFire = m_dpDatapoint_FireTrigger->isInAlarmState();
            }
            catch (std::exception& expWhat)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            }
            catch (...)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "...", "Unknown exception caught when call isInAlarmState" );
            }

            return;
        }
        else if (updateType != TA_Base_Bus::AlarmSummaryUpdate)
        {
            return;
        }

        if (!m_bEnable)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", "Disabled so we ignore it update.");
            return;
        }

		try
		{
			if (TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON != m_dpDatapoint_FireTrigger->getStatus())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", "This datapoint NOT good quality, so we ignore it update.");
				return;
			}

            bool bFire = m_dpDatapoint_FireTrigger->isInAlarmState();

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "New fire status [%s], Old fire status [%s]", fireStatueToString(bFire).c_str(), fireStatueToString(m_bInFire).c_str());

            if ((m_bInFire && bFire) || (!m_bInFire && !bFire))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", "Same fire status occur, ignore this update.");
                return;
            }

            m_bInFire = bFire;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
				"process Entity Update Event FireTriggerEquipment=%s datapointEntityKey=%d ScadaEntityUpdateType=%s Fire status:%s",
				 CachedConfig::getInstance()->getFasEntityName().c_str(), 
                 entityKey, scadaEntityUpdateTypeToString(updateType).c_str(), fireStatueToString(bFire).c_str());

            //Execute fire trigger
			if ( bFire )
			{
                executeFasTriggeredBroadcast( bFire );
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", "No alarm generated, ignore FAS trigger Broadcast");
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

	void FireAlarmSubscriber::notifyQuit()
    {
        m_dpDatapoint_FireTrigger.reset();
        m_proxyFactory->setProxiesToMonitorMode();
        TA_Base_Bus::ScadaProxyFactory::removeInstance();
    }

    void FireAlarmSubscriber::executeFasTriggeredBroadcast(bool bFire)
    {
        if (NULL != m_pTriggeredBroadcastWorker)
        {
            m_pTriggeredBroadcastWorker->executeFasTriggeredBroadcast(bFire);
        }
    }

    void FireAlarmSubscriber::enable(const bool bEnable)
    {
        m_bEnable = bEnable;
    }

    std::string FireAlarmSubscriber::fireStatueToString(bool bInFire)
    {
        return bInFire ? "Fire" : "Normal";
    }

	std::string FireAlarmSubscriber::scadaEntityUpdateTypeToString(TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("scadaEntityUpdateTypeToString");

		std::string strScadaEntityUpdateType = "";
		switch (updateType)
		{
		case TA_Base_Bus::NotApplicable:
			strScadaEntityUpdateType = "NotApplicable";
			break;
		case TA_Base_Bus::ValueStateUpdate:
			strScadaEntityUpdateType = "ValueStateUpdate";
			break;
		case TA_Base_Bus::ControlStateUpdate:
			strScadaEntityUpdateType = "ControlStateUpdate";
			break;
		case TA_Base_Bus::ConfigStateLastOperatorUpdate:
			strScadaEntityUpdateType = "ConfigStateLastOperatorUpdate";
			break;
		case TA_Base_Bus::ControlSummaryUpdate:
			strScadaEntityUpdateType = "ControlSummaryUpdate";
			break;
		case TA_Base_Bus::AlarmSummaryUpdate:
			strScadaEntityUpdateType = "AlarmSummaryUpdate";
			break;
		case TA_Base_Bus::OverrideSummaryUpdate:
			strScadaEntityUpdateType = "OverrideSummaryUpdate";
			break;
		case TA_Base_Bus::QualitySummaryUpdate:
			strScadaEntityUpdateType = "QualitySummaryUpdate";
			break;
		case TA_Base_Bus::ScanInhibitSummaryUpdate:
			strScadaEntityUpdateType = "ScanInhibitSummaryUpdate";
			break;
		case TA_Base_Bus::AlarmInhibitSummaryUpdate:
			strScadaEntityUpdateType = "AlarmInhibitSummaryUpdate";
			break;
		case TA_Base_Bus::MmsAlarmInhibitSummaryUpdate:
			strScadaEntityUpdateType = "MmsAlarmInhibitSummaryUpdate";
			break;
		case TA_Base_Bus::ControlInhibitSummaryUpdate:
			strScadaEntityUpdateType = "ControlInhibitSummaryUpdate";
			break;
		case TA_Base_Bus::AckSummaryUpdate:
			strScadaEntityUpdateType = "AckSummaryUpdate";
			break;
		case TA_Base_Bus::TagStateUpdate:
			strScadaEntityUpdateType = "TagStateUpdate";
			break;
		case TA_Base_Bus::NotesStateUpdate:
			strScadaEntityUpdateType = "NotesStateUpdate";
			break;
		case TA_Base_Bus::InhibitStateUpdate:
			strScadaEntityUpdateType = "InhibitStateUpdate";
			break;
		case TA_Base_Bus::ConfigStateUpdate:
			strScadaEntityUpdateType = "ConfigStateUpdate";
			break;
		case TA_Base_Bus::ConfigAvailable:
			strScadaEntityUpdateType = "ConfigAvailable";
			break;
		case TA_Base_Bus::AgentConnected:
			strScadaEntityUpdateType = "AgentConnected";
			break;
		default:
			strScadaEntityUpdateType = "[ERROR] Unknown ScadaEntityUpdateType";
			break;
		}
		FUNCTION_EXIT;
		return strScadaEntityUpdateType;
	}
}
