#include "StdAfx.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "app/bas/BASModeManager/src/Equipment.h"
#include "app/bas/BASModeManager/src/BASAgentHelper.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"
#include "app/bas/BASModeManager/src/ModeRunningStatus.h"
#include "app/bas/BASModeManager/src/SubSystemStatus.h"
#include "app/bas/BASModeManager/src/CommonDef.h"

namespace TA_IRS_App
{

	Equipment::Equipment(unsigned long locationKey, 
						BasManagerSubscriber* basSubscriberPtr, 
						TA_IRS_Core::SubSystem_SmartPtr subsystem, 
						std::string equipmentName)
	:
	m_basSubscriberPtr(basSubscriberPtr),
	m_equipmentName(equipmentName),
	m_subsystem(subsystem), 
	m_locationKey(locationKey) 
	{
		FUNCTION_ENTRY("Equipment");
		FUNCTION_EXIT;
	}
	void Equipment::initializeEquipment()
	{
		FUNCTION_ENTRY("initializeEquipment");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Equipment [%s] create beginning.", m_equipmentName.c_str());
		// get a handle to the ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory& refScadaFactory = TA_Base_Bus::ScadaProxyFactory::getInstance();

		refScadaFactory.setProxiesToControlMode();

	    try
	    {
		    TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_equipmentName);
		    TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData);

			if (NULL != dnEntityData)
			{
				TA_Base_Core::SharedIEntityDataList myDatapoints = dnEntityData->getChildDataPoints();
				TA_Base_Core::SharedIEntityDataList::const_iterator iterPt;

				unsigned int count = 0;
				const std::string strCurrentModeDataPointName = BASAgentHelperInstance::getInstance().getCurrentExcuteModeDataPointName(m_locationKey, m_subsystem);
				const std::string strCurrentModeStatusDataPointName = BASAgentHelperInstance::getInstance().getCurrentExcuteModeStatusDataPointName(m_locationKey, m_subsystem);
				const std::string strCurrentModeBitDataPointName = BASAgentHelperInstance::getInstance().getCurrentExcuteModeBitDataPointName(m_locationKey, m_subsystem);
				const std::string strCurrentModeBitSetDataPointName = BASAgentHelperInstance::getInstance().getCurrentExcuteModeBitSetDataPointName(m_locationKey, m_subsystem);
				for ( iterPt = myDatapoints.begin(); iterPt != myDatapoints.end(); ++iterPt)
				{
					TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>((*iterPt).get());
					//TA_ASSERT(dpEntityData != NULL, "Equipment is not defined as datapoint entity");
					if (NULL == dpEntityData)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Equipment is not defined as datapoint entity,EquipmentName=%s", m_equipmentName.c_str());
						continue;
					}
					TA_Base_Core::DataPointEntityDataPtr dp(dpEntityData);
					std::string strDataPointName = dp->getName();

					if (strCurrentModeDataPointName == strDataPointName)
					{
						refScadaFactory.createDataPointProxy(dp, *this, m_dpCurrentExcuteMode);
					} 
					else if(strCurrentModeBitDataPointName == strDataPointName)
					{
						refScadaFactory.createDataPointProxy(dp, *this, m_dpCurrentExcuteModeBit);
					}
					else if (strCurrentModeStatusDataPointName == strDataPointName)
					{
						refScadaFactory.createDataPointProxy(dp, *this, m_dpCurrentExcuteModeStatus);
					}
					else if (strCurrentModeBitSetDataPointName == strDataPointName)
					{
						refScadaFactory.createDataPointProxy(dp, *this, m_dpCurrentExcuteModeSetBit);
					}
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Equipment is not defined as datanode entity,EquipmentName=%s", m_equipmentName.c_str());
			}
	    }
		catch ( const std::exception& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught unknown exception when initializeEquipment");
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Equipment [%s] is created.", m_equipmentName.c_str());
	    TA_Base_Bus::ScadaProxyFactory::removeInstance();

		FUNCTION_EXIT;
	}

	void Equipment::prepareToExit()
	{
		FUNCTION_ENTRY("prepareToExit");
	    m_dpCurrentExcuteMode.reset();
	    m_dpCurrentExcuteModeBit.reset();
	    m_dpCurrentExcuteModeStatus.reset();
	    m_dpCurrentExcuteModeSetBit.reset();
		FUNCTION_EXIT;
	}
	Equipment::Equipment(void)
	{
		FUNCTION_ENTRY("Equipment");
		FUNCTION_EXIT;
	}

	Equipment::~Equipment(void)
	{
		FUNCTION_ENTRY("~Equipment");
		FUNCTION_EXIT;
	}

	void Equipment::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("processEntityUpdateEvent");
		using namespace TA_Base_Core;

		// filter the call back updated data point 
		switch(updateType)
		{
		case TA_Base_Bus::ValueStateUpdate:
			break;
		case TA_Base_Bus::ControlStateUpdate:
			break;
		case TA_Base_Bus::ConfigStateLastOperatorUpdate:
			break;
		default:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
				"process Entity Update Event, but we no focus on this update type. locationKey=%u subsystemKey=%d subsystemName=%s, datapointEntityKey=%d ScadaEntityUpdateType=%d",
				m_locationKey, m_subsystem->getKey(), m_subsystem->getName().c_str(), entityKey, updateType);
			FUNCTION_EXIT;
			return;
		}
		// get the value and value type
		EquipmentValueType valueType;
		double newValue = 0.0;
		if( false == getDatapointNewValue(entityKey, newValue, valueType))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "This data point [%lu:%s] value is not good quality or it's a out put datapoint, so ignore it", entityKey, getDatapointStringByEntityKey(entityKey).c_str());
			return;
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
			"process Entity Update Event location=%u subsystemKey=%u subsystemName=%s datapointEntityKey=%d datapointValue=%f EquipmentValueType=%d", 
			m_locationKey, m_subsystem->getKey(), m_subsystem->getName().c_str(), entityKey, newValue, valueType);

		// deal with the datapoint update
		BAS_TRY
		{
			if ( EVT_CurrentExcuteMode == valueType )
			{
	            if ( false == isModeValid( static_cast<unsigned long>(newValue) ) )
	            {
	                ModeStatusManagerInstance::instance()->updateModeStatusForClean(m_locationKey, m_subsystem->getKey());
	            }
	            else
	            {
	                // get the mode key by value
	                unsigned long modeKey = getModeKeyByValue(newValue);
	                int modeStatus = static_cast<int>(getEquipmentValue(EVT_CurrentExcuteModeStatus));
	                ModeStatusManagerInstance::instance()->updateModeStatus(modeKey, modeStatus);
	            }
			}
			else if (EVT_CurrentExcuteModeBit == valueType)
			{			
	            SubSystemStatusInstance::instance()->updateSubsystemMode(m_locationKey, m_subsystem->getKey(), newValue, ModeSet::modeType_AutoManul);
			}
			else if (EVT_CurrentExcuteModeStatus == valueType)
			{
				unsigned long modeKey = TA_Base_Core::DatabaseKey::getInvalidKey();
				double modeValue = 0.0;
				EquipmentValueType valueType;
				// get current mode key and type
				getDatapointNewValue(m_dpCurrentExcuteMode->getEntityKey(), modeValue, valueType);
	            if (true == isModeValid( static_cast<unsigned long>(modeValue) ) )
	            {
	                modeKey = getModeKeyByValue(modeValue);
	                int modeStatus = static_cast<int>(newValue);
	                ModeStatusManagerInstance::instance()->updateModeStatus(modeKey, modeStatus);
	            }
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Receive a unknown datapoint update!!!");
			}
		}
		BAS_CATCH;
		if (BAS_WhetherHaveError)
		{
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, BAS_GetExceptionString);
			FUNCTION_EXIT
			return;
		}
	
		FUNCTION_EXIT;
	}

	unsigned long Equipment::getEquipmentValue(EquipmentValueType type)
	{
		FUNCTION_ENTRY("getEquipmentValue");
		unsigned long retValue = 0;

		switch(type)
		{
	    case EVT_CurrentExcuteMode:
	        if ( TA_Base_Bus::DPT_ANALOGUE == m_dpCurrentExcuteMode->getType() )
	        {
	            retValue = m_dpCurrentExcuteMode->getValueAsDouble();
	        }
	        else if ( TA_Base_Bus::DPT_DERIVED == m_dpCurrentExcuteMode->getType() )
	        {
	            retValue = m_dpCurrentExcuteMode->getDerivedStateAsUnsignedLong( m_dpCurrentExcuteMode->getFieldValue() );
	        }
			break;
	    case EVT_CurrentExcuteModeBit:
	        if ( TA_Base_Bus::DPT_ANALOGUE == m_dpCurrentExcuteModeBit->getType() )
	        {
	            retValue = m_dpCurrentExcuteModeBit->getValueAsDouble();
	        }
	        else if ( (TA_Base_Bus::DPT_DERIVED == m_dpCurrentExcuteModeBit->getType() ) 
				   || (TA_Base_Bus::DPT_ENUM == m_dpCurrentExcuteModeBit->getType()) )
	        {
	            retValue = m_dpCurrentExcuteModeBit->getDerivedStateAsUnsignedLong( m_dpCurrentExcuteModeBit->getFieldValue() );
	        }
	        else if ( TA_Base_Bus::DPT_BOOLEAN == m_dpCurrentExcuteModeBit->getType() )
	        {
	            retValue = m_dpCurrentExcuteModeBit->getValueAsBool() ? eControlManual : eControlAuto;
	        }
			break;
	    case EVT_CurrentExcuteModeStatus:
	        if ( TA_Base_Bus::DPT_ANALOGUE == m_dpCurrentExcuteModeStatus->getType() )
	        {
	            retValue = m_dpCurrentExcuteModeStatus->getValueAsDouble();
	        }
	        else if ((TA_Base_Bus::DPT_DERIVED == m_dpCurrentExcuteModeStatus->getType()) 
				  || (TA_Base_Bus::DPT_ENUM == m_dpCurrentExcuteModeStatus->getType())  )
	        {
	            retValue = m_dpCurrentExcuteModeStatus->getDerivedStateAsUnsignedLong( m_dpCurrentExcuteModeStatus->getFieldValue() );
	        }
			break;
		}

		FUNCTION_EXIT;
		return retValue;
	}

	TA_IRS_Core::SubSystem_SmartPtr Equipment::getSubsystem()
	{
		FUNCTION_ENTRY("getSubsystem");
		FUNCTION_EXIT;
		return m_subsystem;
	}
	bool Equipment::getDatapointNewValue(unsigned long datapointKey, double& value, EquipmentValueType& type)
	{
		FUNCTION_ENTRY("getDatapointNewValue");
		bool bGoodQuality = false ;

		if(datapointKey == m_dpCurrentExcuteMode->getEntityKey())
		{
			type = EVT_CurrentExcuteMode;
			if(TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == m_dpCurrentExcuteMode->getStatus())
			{
				value = getEquipmentValue( EVT_CurrentExcuteMode );
				bGoodQuality = true;
			}		
		}
		else if(datapointKey == m_dpCurrentExcuteModeBit->getEntityKey())
		{
			type = EVT_CurrentExcuteModeBit;
			if(TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == m_dpCurrentExcuteModeBit->getStatus())
			{
				value = getEquipmentValue( EVT_CurrentExcuteModeBit );
				bGoodQuality = true;
			}	
		}
		else if(datapointKey == m_dpCurrentExcuteModeStatus->getEntityKey())
		{
			type = EVT_CurrentExcuteModeStatus;
			if(TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == m_dpCurrentExcuteModeStatus->getStatus())
	        {
	            value = getEquipmentValue( EVT_CurrentExcuteModeStatus );
				bGoodQuality = true;
			}	
		}
		FUNCTION_EXIT;
		return bGoodQuality;
	}

	bool Equipment::isModeValid(unsigned long modeValue)
	{
		using namespace TA_Base_Core;
		bool isValid = false;
		Mode_SmartPtr modePtr = BasManagerDataHelperInstance::instance()->getModeByValue(m_locationKey,  modeValue);
		if (NULL != modePtr.get())
		{
			if (modePtr->getSubsystemKey() == m_subsystem->getKey())
			{
				isValid = true;
			}
		}

		if (false == isValid)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "This mode value [%d] do not match the subsystem:%s", modeValue, m_subsystem->getName().c_str());
		}
		return isValid;
	}

	unsigned long Equipment::getModeKeyByValue(double value)
	{
		FUNCTION_ENTRY("getModeKeyByValue");
		Mode_SmartPtr mode = BasManagerDataHelperInstance::instance()->getModeByValue(m_locationKey, value);
		if (NULL == mode.get())
		{
			std::stringstream ss;
			ss << "invalid data point value , mode value = " << value;
			TA_THROW(BasUnknownMode(ss.str()));
		}
		unsigned long modeKey = mode->getKey();
		FUNCTION_EXIT;
		return modeKey;
	}

	void Equipment::setAutoCfgValue(const int nValue)
	{
	    std::string strValue = "";
	    TA_Base_Bus::EDataPointType eDpType = m_dpCurrentExcuteModeSetBit->getType();

	    std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

	    if ( TA_Base_Bus::DPT_BOOLEAN == eDpType )
	    {
	        strValue = m_dpCurrentExcuteModeSetBit->getBooleanLabel( eControlManual == nValue );
	    }
	    else if ( TA_Base_Bus::DPT_ANALOGUE == eDpType )
	    {
	        strValue = BASManagerUtility::unsignedlongToString( nValue );
	    }
	    else if ( (TA_Base_Bus::DPT_COMBINED_OUTPUT == eDpType) || (TA_Base_Bus::DPT_ENUM == eDpType) )
	    {
	        strValue = m_dpCurrentExcuteModeSetBit->getDerivedStateAsString( nValue );
	    }
	    else
	    {
	        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
	            "Such DataPoint type %s is not support", m_dpCurrentExcuteModeSetBit->getTypeAsString().c_str() );
	        return;
	    }

	    m_dpCurrentExcuteModeSetBit->setValue( strValue, session );
	}

	std::string Equipment::getDatapointStringByEntityKey(unsigned long entitykey)
	{
		FUNCTION_ENTRY("getDatapointStringByEntityKey");
		std::string strReturnString = "Unknown entity.";
		if (entitykey == m_dpCurrentExcuteMode->getEntityKey())
		{
			strReturnString = m_dpCurrentExcuteMode->getEntityName();
		}
		else if(entitykey == m_dpCurrentExcuteModeBit->getEntityKey())
		{
			strReturnString = m_dpCurrentExcuteModeBit->getEntityName();
		}
		else if (entitykey == m_dpCurrentExcuteModeStatus->getEntityKey())
		{
			strReturnString = m_dpCurrentExcuteModeStatus->getEntityName();
		}
		else if (entitykey == m_dpCurrentExcuteModeSetBit->getEntityKey())
		{
			strReturnString = m_dpCurrentExcuteModeSetBit->getEntityName();
		}

		FUNCTION_EXIT;
		return strReturnString;
	}
	
}

