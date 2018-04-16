/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/PMSAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PMSAgentEntityData implements the interface IPMSAgentEntityData to provide read-only access
  * to PMSAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{

    const std::string PMSAgentEntityData::ENTITY_TYPE                            = "PMSAgent";
	const std::string PMSAgentEntityData::SCANTIME                               = "ScanTimeMSecs";
	const std::string PMSAgentEntityData::RTU_SWITCHING_DISABLED                 = "RTUSwitchingDisabled";
    const std::string PMSAgentEntityData::STATIONNAME                            = "StationName";
	const std::string PMSAgentEntityData::PROXY_DP_HEALTH_MONITORING_PERIOD_SECS = "ProxyDpMonitoringPeriodSecs";
	const std::string PMSAgentEntityData::ASSET_NAME                             = "AssetName";
	const std::string PMSAgentEntityData::ANALOGUE_DP_UPDATE_INTERVAL            = "AnalogueDataPointUpdateInterval"; //TD16169

	PMSAgentEntityData::PMSAgentEntityData()
    {
    }

	
    PMSAgentEntityData::PMSAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }

    PMSAgentEntityData::~PMSAgentEntityData()
    {
    }


    /**
     * getScanTimeMSecs
     *
     * Returns the scan time in msecs for PMSAgent entity
     *
     * @return The integer value presenting the scan time in msecs
     */
    int PMSAgentEntityData::getScanTimeMSecs()
	{
		int scanTimeMSecs = 250;
		std::string value = getHelper()->getParameter(SCANTIME);
		if (!value.empty())
		{
			scanTimeMSecs = EntityHelper::getIntegerData(value);
		}
		return scanTimeMSecs;
	}


	bool PMSAgentEntityData::getIsRTUSwitchingDisabled()
	{
		bool isRTUSwitchingDisabled = false;
		std::string value = getHelper()->getParameter(RTU_SWITCHING_DISABLED);
		if (!value.empty())
		{
			isRTUSwitchingDisabled = EntityHelper::getBooleanData(value);
		}
		return isRTUSwitchingDisabled;
	}


    const std::string PMSAgentEntityData::getStationName()
	{
        return getHelper()->getParameter(STATIONNAME);
	}


    std::string PMSAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}


	int PMSAgentEntityData::getProxyDpHealthMonitoringPeriodSecs()
	{
		int proxyDpMonitoringPeriodSecs = 10;
		std::string value = getHelper()->getParameter(PROXY_DP_HEALTH_MONITORING_PERIOD_SECS);
		if(!value.empty())
		{
			proxyDpMonitoringPeriodSecs = EntityHelper::getIntegerData(value);
		}

        return proxyDpMonitoringPeriodSecs;
	}


	std::string PMSAgentEntityData::getType()
	{
		return getStaticType();
	}

    std::string PMSAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

     //TD16169
    int PMSAgentEntityData::getAnalogueDataPointUpdateInterval()
	{
		int analogueDataPointUpdateInterval = 0;  //TD16169
		std::string value = getHelper()->getParameter(ANALOGUE_DP_UPDATE_INTERVAL);
		if (!value.empty())
		{
			analogueDataPointUpdateInterval = EntityHelper::getIntegerData(value);
		}

        return analogueDataPointUpdateInterval;
	}

          
    IEntityData* PMSAgentEntityData::clone ( unsigned long key )
    {
        return new PMSAgentEntityData ( key );
    }

    void PMSAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
