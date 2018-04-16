/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/ModbusAgentEntityData.cpp $
  * @author Dhanshri
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * ModbusAgentEntityData implements the interface IModbusAgentEntityData to provide read-only access
  * to ModbusAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "ModbusAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{

    const std::string ModbusAgentEntityData::ENTITY_TYPE                            = "ModbusAgent";
	const std::string ModbusAgentEntityData::OVERLAPREQUEST                         = "OverlapModbusRequest";
	const std::string ModbusAgentEntityData::INTERVAL_BETWN_MODBUSTRX               = "IntervalBetweenModbusTransaction";
    const std::string ModbusAgentEntityData::STATIONNAME                            = "StationName";
	const std::string ModbusAgentEntityData::MAX_MODBUS_LEN						 = "MaxModbusLength";
	const std::string ModbusAgentEntityData::ASSET_NAME                             = "AssetName";
	const std::string ModbusAgentEntityData::STD_MODBUS_LEN_LMT			         = "StandardModbusLengthLimit"; //TD16169
	const std::string ModbusAgentEntityData::POLL_TIMEOUT 							 = "PollTimeout";
	
	const std::string ModbusAgentEntityData::POLLING_DI_START_ADDRESS = "PollingDiStartAddress";
	const std::string ModbusAgentEntityData::POLLING_DI_END_ADDRESS = "PollingDiEndAddress";
	const std::string ModbusAgentEntityData::POLLING_DI_TIMEOUT = "PollingDiTimeout";
	const std::string ModbusAgentEntityData::POLLING_DI_TYPE = "PollingDiType";
	
	const std::string ModbusAgentEntityData::POLLING_AI_START_ADDRESS = "PollingAiStartAddress";
	const std::string ModbusAgentEntityData::POLLING_AI_END_ADDRESS = "PollingAiEndAddress";
	const std::string ModbusAgentEntityData::POLLING_AI_TIMEOUT = "PollingAiTimeout";
	const std::string ModbusAgentEntityData::POLLING_AI_TYPE = "PollingAiType";
	
	const std::string ModbusAgentEntityData::POLLING_MI_START_ADDRESS = "PollingMiStartAddress";
	const std::string ModbusAgentEntityData::POLLING_MI_END_ADDRESS = "PollingMiEndAddress";
	const std::string ModbusAgentEntityData::POLLING_MI_TIMEOUT = "PollingMiTimeout";
	const std::string ModbusAgentEntityData::POLLING_MI_TYPE = "PollingMiType";
	
	const std::string ModbusAgentEntityData::DP_PROCESSORS_START_ADDRESSES = "DpProcessorsStartAddresses";
	const std::string ModbusAgentEntityData::DP_PROCESSORS_END_ADDRESSES = "DpProcessorsEndAddresses";
	const std::string ModbusAgentEntityData::DP_PROCESSORS_ANA_DP_UPD_INTERVL = "AnalogueDpUpdateInterval";
	
	ModbusAgentEntityData::ModbusAgentEntityData()
    {
    }

	
    ModbusAgentEntityData::ModbusAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }

    ModbusAgentEntityData::~ModbusAgentEntityData()
    {
    }


    /**
     * getScanTimeMSecs
     *
     * Returns the scan time in msecs for ModbusAgent entity
     *
     * @return The integer value presenting the scan time in msecs
     */
    const int ModbusAgentEntityData::getIntervalBetweenModbusTransaction()
	{
		int iIntervalBetweenModbusTransaction = 1000;
		std::string value = getHelper()->getParameter(INTERVAL_BETWN_MODBUSTRX);
		if (!value.empty())
		{
			iIntervalBetweenModbusTransaction = EntityHelper::getIntegerData(value);
		}
		return iIntervalBetweenModbusTransaction;
	}


	const bool ModbusAgentEntityData::getOverlapModbusRequest()
	{
		bool isOverlapModbusRequest = false;
		std::string value = getHelper()->getParameter(OVERLAPREQUEST);
		if (!value.empty())
		{
			isOverlapModbusRequest = EntityHelper::getBooleanData(value);
		}
		return isOverlapModbusRequest;
	}

	const bool ModbusAgentEntityData::getStandardModbusLengthLimit()
	{
		bool isStandardModbusLengthLimit = false;
		std::string value = getHelper()->getParameter(STD_MODBUS_LEN_LMT);
		if (!value.empty())
		{
			isStandardModbusLengthLimit = EntityHelper::getBooleanData(value);
		}
		return isStandardModbusLengthLimit;
	}
	
	const int ModbusAgentEntityData::getPollTimeout()
	{
		int iPollTO = 2000;
		std::string value = getHelper()->getParameter(POLL_TIMEOUT);
		if (!value.empty())
		{
			iPollTO  = EntityHelper::getIntegerData(value);
		}
		return iPollTO;
	}
	
    const std::string ModbusAgentEntityData::getStationName()
	{
        return getHelper()->getParameter(STATIONNAME);
	}


    const std::string ModbusAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}


	const int ModbusAgentEntityData::getMaxModbusLength()
	{
		int iMaxModbusLength = 124;
		std::string value = getHelper()->getParameter(MAX_MODBUS_LEN);
		if(!value.empty())
		{
			iMaxModbusLength = EntityHelper::getIntegerData(value);
		}

        return iMaxModbusLength;
	}


	std::string ModbusAgentEntityData::getType()
	{
		return getStaticType();
	}

    const std::string ModbusAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


	std::string ModbusAgentEntityData::getPollingDiStartAddress()
	{
		return getHelper()->getParameter( POLLING_DI_START_ADDRESS );
	}

	std::string ModbusAgentEntityData::getPollingAiStartAddress()
	{
		return getHelper()->getParameter( POLLING_AI_START_ADDRESS );
	}
	
	std::string ModbusAgentEntityData::getPollingMiStartAddress()
	{
		return getHelper()->getParameter( POLLING_MI_START_ADDRESS );
	}

	std::string ModbusAgentEntityData::getPollingDiEndAddress()
	{
		return getHelper()->getParameter( POLLING_DI_END_ADDRESS );
	}

	std::string ModbusAgentEntityData::getPollingAiEndAddress()
	{
		return getHelper()->getParameter( POLLING_AI_END_ADDRESS );
	}

	std::string ModbusAgentEntityData::getPollingMiEndAddress()
	{
		return getHelper()->getParameter( POLLING_MI_END_ADDRESS );
	}


	int ModbusAgentEntityData::getPollingDiTimeOut()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_DI_TIMEOUT);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int ModbusAgentEntityData::getPollingAiTimeOut()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_AI_TIMEOUT);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int ModbusAgentEntityData::getPollingMiTimeOut()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_MI_TIMEOUT);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int ModbusAgentEntityData::getPollingDiType()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_DI_TYPE);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int ModbusAgentEntityData::getPollingAiType()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_AI_TYPE);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}

	int ModbusAgentEntityData::getPollingMiType()
	{
			int iParam = 1000;
		std::string value = getHelper()->getParameter(POLLING_MI_TYPE);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}
	
	std::string ModbusAgentEntityData::getDPProcessorsStartAddresses()
	{
		return getHelper()->getParameter( DP_PROCESSORS_START_ADDRESSES );			
	}
	
	std::string ModbusAgentEntityData::getDPProcessorsEndAddresses()
	{
		return getHelper()->getParameter( DP_PROCESSORS_END_ADDRESSES );			
	}
		
	int ModbusAgentEntityData::getAnalogueDpUpdateInterval()
	{
		int iParam = 1000;
		std::string value = getHelper()->getParameter(DP_PROCESSORS_ANA_DP_UPD_INTERVL);
		if(!value.empty())
		{
			iParam = EntityHelper::getIntegerData(value);
		}
        return iParam;
	}
	
    IEntityData* ModbusAgentEntityData::clone ( unsigned long key )
    {
        return new ModbusAgentEntityData ( key );
    }

    void ModbusAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
