/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/CtaEcsDatapoints.cpp $
  * @author:  seanl
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <time.h>
#include <iostream>

#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/CtaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/EmergencyProposalEngine.h"
#include "app/ecs/ECSAgent/src/CongestionProposalEngine.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentCompleteState.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/exceptions/src/ECSAgentExceptions.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


static std::string zoneDpDescription(const std::string& datapointName, unsigned long ecsZoneId)
{
	std::ostringstream label;
	label << datapointName << "(zone" << ecsZoneId << ")";

	return label.str();

}

CtaEcsDatapoints::CtaEcsDatapoints(ItaEcsCachedMaps& cachedMapsInstance, ItaEcsCachedConfig& cachedConfigInstance)
: m_theConfig(cachedConfigInstance),
  m_theMaps(cachedMapsInstance)
{
	FUNCTION_ENTRY("CtaEcsDatapoints");

	m_emergencyObserverData.clear();
	m_emergencyObservers.clear();
	m_congestionObserverData.clear();
	m_congestionObservers.clear();

	// get a handle to the ScadaProxyFactory
	m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
    TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
	m_proxyFactory->setProxiesToControlMode();

	FUNCTION_EXIT;
}

CtaEcsDatapoints::~CtaEcsDatapoints()
{
	FUNCTION_ENTRY("~CtaEcsDatapoints");

	m_emergencyObserverData.clear();
	m_emergencyObservers.clear();
	m_congestionObserverData.clear();
	m_congestionObservers.clear();

	// remove handle to ScadaProxyFactory
	m_proxyFactory->setProxiesToMonitorMode();
	TA_Base_Bus::ScadaProxyFactory::removeInstance();
	m_proxyFactory = NULL;

	FUNCTION_EXIT;
}

void CtaEcsDatapoints::initialise()
{
    m_allZoneIds = m_theMaps.getAllZoneIds();

    m_theMaps.attachObserver(this, ItaEcsCachedMaps::MAP_ZONE_RECORDS);

	TA_Base_Core::ThreadGuard guard( m_lockForObservers );
	std::vector<unsigned long>::iterator it = m_allZoneIds.begin();
    for (; it!=m_allZoneIds.end(); ++it)
    {
		// Sort out m_congestionObserverData mappings

		TA_Base_Bus::DataPoint* dp = m_theMaps.getMyZoneCongestionDataPoint(*it);
		if (NULL != dp)
		{
			unsigned long entityKey = dp->getEntityKey();
			TA_Base_Bus::DataPointProxySmartPtr * dpt = new TA_Base_Bus::DataPointProxySmartPtr();
			TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(entityKey, *this, *dpt);
			DataPointObserverData data((*it), dpt);
			m_congestionObserverData[entityKey] = data;
		}

		// Sort out m_emergencyObserverData mappings

		dp = m_theMaps.getMyZoneEmergencyDataPoint(*it);
		if (NULL != dp)
		{
			unsigned long entityKey = dp->getEntityKey();
			TA_Base_Bus::DataPointProxySmartPtr * dpt = new TA_Base_Bus::DataPointProxySmartPtr();
			TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(entityKey, *this, *dpt);

			DataPointObserverData data((*it), dpt);
			m_emergencyObserverData[entityKey] = data;
		}
	}
}

// AbstractFailoverable overrides 
void CtaEcsDatapoints::setControl(bool isOnStartup)
{   
	FUNCTION_ENTRY("setControl");

    // Important that this is called first the set the enabled flag.
    AbstractFailoverable::setControl(isOnStartup);

    if ( true == isOnStartup)
    {
        resetDataPoints();
    }

	FUNCTION_EXIT;
}


void CtaEcsDatapoints::setMonitor(bool isOnStartup)
{
	FUNCTION_ENTRY("setMonitor");

    AbstractFailoverable::setMonitor(isOnStartup);

	FUNCTION_EXIT;
}


void CtaEcsDatapoints::processItaEcsCachedMapsUpdate(ItaEcsCachedMaps::EItaEcsCachedMapsItemKey key)
{
	FUNCTION_ENTRY("processItaEcsCachedMapsUpdate");

    if (key == ItaEcsCachedMaps::MAP_ZONE_RECORDS)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdates );

        m_allZoneIds = m_theMaps.getAllZoneIds();
    }

	FUNCTION_EXIT;
}



void CtaEcsDatapoints::resetDataPoints()
{
	FUNCTION_ENTRY("resetDataPoints");

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdates );

    for (std::vector<unsigned long>::iterator it=m_allZoneIds.begin(); it!=m_allZoneIds.end(); ++it)
    {
        if (m_theConfig.getIsMaster() == true)
	    {
            // Unknown ecs zones will be ignored
            setCongestion(*it, TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL, true);
		    setEmergency(*it, false, true);

		    //setCongestionApplied(*it, false, true);
		    setEmergencyApplied(*it, false, true);
		    setEvacuationSelected(*it, 0, true);
		    setAirflowSelected(*it, 0, true);
		    setEvacuationApplied(*it, 0, true);
		    setAirflowApplied(*it, 0, true);
        }
        else
        {
            if(m_theMaps.isZoneIdInScope(*it) == true)
            {
                setEmergency(*it, false, true);
                setCongestion(*it, TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL, true);
            }
        }
    }

	if (m_theConfig.getIsMaster() == true)
	{
        // mastermode = mode status
        setCurrentMasterMode(TA_IRS_Bus::ECS_NONE_OR_INVALID, true);
        setMasterModeInProgress(0, true); // not in progress
	}
	else
	{
        setLastAppliedMasterMode(TA_IRS_Bus::ECS_NONE_OR_INVALID, true);
		setLastAppliedStationMode(TA_IRS_Bus::ECS_NONE_OR_INVALID, true);
		setSelectedMasterMode(TA_IRS_Bus::ECS_NONE_OR_INVALID, true);
		setSelectedOverridingStationMode(TA_IRS_Bus::ECS_NONE_OR_INVALID, true);
        setMasterModeLock(0,true);
        setMasterModeLockControl(0,true);
	}

	FUNCTION_EXIT;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helpers 
//
///////////////////////////////////////////////////////////////////////////////


void CtaEcsDatapoints::setIntegerDataPoint(TA_Base_Bus::DataPoint* dp, unsigned long value, const std::string& descriptionForDebug, bool fInit)
{
	FUNCTION_ENTRY("setIntegerDataPoint");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Setting %s datapoint to %ld", descriptionForDebug.c_str(), value );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return;
    }

    if (NULL == dp)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to find %s datapoint", descriptionForDebug.c_str(), value );
		FUNCTION_EXIT;
        return;
    }

    // for init force to set the datapoint value so that the quality of the data point is set
    switch( dp->getDataPointType() )
    {
    case TA_Base_Bus::DPT_ANALOGUE:
        if ( (unsigned long)dp->getCurrentValue().getFloat() != value || fInit == true )
        {
		    TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
		    dpValue.setFloat( value );

            TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
	        dp->updateFieldState( newState );
        }
        break;

    case TA_Base_Bus::DPT_ENUM:
        if ( dp->getCurrentValue().getEnum() != value || fInit == true )
        {
		    TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
            dpValue.setEnum( value );

            TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
	        dp->updateFieldState( newState );
        }
		break;

    default:
        std::ostringstream desc;
        desc << "Unexpected datapoint type " << dp->getDataPointName() << " Type: " << dp->getDataPointType();
        TA_ASSERT( true, desc.str().c_str() );
    }

	FUNCTION_EXIT;
}

void CtaEcsDatapoints::setIntegerDataPoint(TA_Base_Bus::DataPoint* dp, long value, const std::string& descriptionForDebug, bool fInit)
{
	FUNCTION_ENTRY("setIntegerDataPoint");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Setting %s datapoint to %ld", descriptionForDebug.c_str(), value );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return;
    }

    if (NULL == dp)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to find %s datapoint", descriptionForDebug.c_str(), value );
		FUNCTION_EXIT;
        return;
    }

    // for init force to set the datapoint value so that the quality of the data point is set
    switch( dp->getDataPointType() )
    {
    case TA_Base_Bus::DPT_ANALOGUE:
        if ( dp->getCurrentValue().getFloat() != value || fInit == true )
        {
		    TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
		    dpValue.setFloat( value );

            TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
	        dp->updateFieldState( newState );
        }
        break;

    case TA_Base_Bus::DPT_ENUM:
        if ( dp->getCurrentValue().getEnum() != value || fInit == true )
        {
		    TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
            dpValue.setEnum( value );

            TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
	        dp->updateFieldState( newState );
        }
		break;

    default:
        std::ostringstream desc;
        desc << "Unexpected datapoint type " << dp->getDataPointName() << " Type: " << dp->getDataPointType();
        TA_ASSERT( true, desc.str().c_str() );
    }

	FUNCTION_EXIT;
}

//zn++ - when station ecs agent startup, set forced value instead
void CtaEcsDatapoints::setForcedIntegerDataPoint(TA_Base_Bus::DataPoint* dp, long value, const std::string& descriptionForDebug)
{
	FUNCTION_ENTRY("setForcedIntegerDataPoint");
	
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Forced setting %s datapoint to %ld", descriptionForDebug.c_str(), value );
	
    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return;
    }
	
    if (NULL == dp)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to find %s datapoint", descriptionForDebug.c_str(), value );
		FUNCTION_EXIT;
        return;
    }
	
	std::ostringstream valueStream("");
	valueStream << value;

	TA_Base_Bus::DpValue dpValue(valueStream.str(), dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
	
	dp->setForcedStateWithValue(TA_Base_Bus::DPT_INTERNAL_SESSION_ID, dpValue.getDataPointValue());

	FUNCTION_EXIT;
}
//++zn

void CtaEcsDatapoints::setBooleanDataPoint(TA_Base_Bus::DataPoint* dp, bool value, const std::string& descriptionForDebug, bool fInit)
{
	FUNCTION_ENTRY("setBooleanDataPoint");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Setting %s datapoint to %s", descriptionForDebug.c_str(), value ? "True" : "False" );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return;
    }

    if (NULL == dp)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to find %s datapoint", descriptionForDebug.c_str() );

		FUNCTION_EXIT;
        return;
    }

    // for init force to set the datapoint value so that the 
    // quality of the data point is set
    if (dp->getCurrentValue().getBoolean() != value || fInit == true)
    {
		TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
		dpValue.setBoolean( value );

        TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
	    dp->updateFieldState( newState );
    }

	FUNCTION_EXIT;
}

void CtaEcsDatapoints::setBooleanProxy(TA_Base_Bus::DataPointProxySmartPtr& proxy, bool value, const std::string& descriptionForDebug, bool fInit)
{
	FUNCTION_ENTRY("setBooleanProxy");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Setting %s datapoint proxy to %s", descriptionForDebug.c_str(), value ? "True" : "False" );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return;
    }

	//16709++
    bool currentValue;

    try
    {
        currentValue = proxy->getValue().getBoolean();       
    }
    catch(TA_Base_Core::ScadaProxyException&)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed setting %s datapoint proxy", descriptionForDebug.c_str() );
		FUNCTION_EXIT;
        return;
    }

	if (currentValue == value)
    {
		return;
	}

	std::string booleanLabel;
	try
	{ 
		booleanLabel = proxy->getBooleanLabel(value);

		proxy->setValue(booleanLabel, TA_Base_Bus::DPT_INTERNAL_SESSION_ID);      
	}
	catch(TA_Base_Core::ScadaProxyException&)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Failed setting %s datapoint proxy", descriptionForDebug.c_str() );
	}
	//++16709

	FUNCTION_EXIT;
}

unsigned long CtaEcsDatapoints::getIntegerDataPoint(TA_Base_Bus::DataPoint* dp, const std::string& descriptionForDebug)
{
	FUNCTION_ENTRY("getIntegerDataPoint");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Getting %s datapoint value", descriptionForDebug.c_str() );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Skipping operation in operation mode" );
		TA_THROW( TA_Base_Core::EcsMonitorModeException() );
    }

    if ( NULL == dp)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s datapoint", descriptionForDebug.c_str() );

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Missing datapoint") );
    }

    unsigned long dpValue = 0;

    switch( dp->getDataPointType() )
    {
    case TA_Base_Bus::DPT_ANALOGUE:
        dpValue = (unsigned long)dp->getCurrentValue().getFloat();
		break;

    case TA_Base_Bus::DPT_ENUM:
        dpValue = dp->getCurrentValue().getEnum();
		break;

    default:
        std::ostringstream desc;
        desc << "Unexpected datapoint type " << dp->getDataPointName() << " Type: " << dp->getDataPointType();
        TA_ASSERT( true, desc.str().c_str() );
    }

	FUNCTION_EXIT;
    return dpValue;
}

bool CtaEcsDatapoints::getBooleanDataPoint(TA_Base_Bus::DataPoint* dp, const std::string& descriptionForDebug)
{
	FUNCTION_ENTRY("getBooleanDataPoint");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Getting %s datapoint value", descriptionForDebug.c_str() );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Skipping operation in operation mode" );
		TA_THROW( TA_Base_Core::EcsMonitorModeException() );
    }

    if (NULL == dp)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s datapoint", descriptionForDebug.c_str() );
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Missing datapoint") );
    }

	FUNCTION_EXIT;
    return dp->getCurrentValue().getBoolean();
}

bool CtaEcsDatapoints::getEmergency(unsigned long zoneId)
{	
	static const std::string descriptionForDebug("MyZoneEmergency");

	try
	{
		return getBooleanDataPoint(m_theMaps.getMyZoneEmergencyDataPoint(zoneId), zoneDpDescription(descriptionForDebug, zoneId));
	}
	catch(...) // EcsMonitorModeException, InvalidECSConfigurationException
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to get %s value.  Assuming False.",
			zoneDpDescription(descriptionForDebug, zoneId).c_str() );
		return false;
	}
}

void CtaEcsDatapoints::setIntegerProxy(TA_Base_Bus::DataPointProxySmartPtr& proxy, unsigned long value, const std::string& descriptionForDebug)
{ 
	FUNCTION_ENTRY("setIntegerProxy");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Setting %s datapoint proxy to %ld", descriptionForDebug.c_str(), value );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return;
    }


    try
    {
		std::ostringstream valueStream("");
		valueStream << value;

		//TD7858
		proxy->setManuallyOverrideValue(valueStream.str(), TA_Base_Bus::DPT_INTERNAL_SESSION_ID); 
    }
    catch(TA_Base_Core::ScadaProxyException&)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed setting %s datapoint proxy", descriptionForDebug.c_str() );
    }

	FUNCTION_EXIT;
}

void CtaEcsDatapoints::setIntegerProxy(TA_Base_Bus::DataPointProxySmartPtr& proxy, long value, const std::string& descriptionForDebug)
{ 
	FUNCTION_ENTRY("setIntegerProxy");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "Setting %s datapoint proxy to %ld", descriptionForDebug.c_str(), value );

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return;
    }


    try
    {
		std::ostringstream valueStream("");
		valueStream << value;

		//TD7858
		proxy->setManuallyOverrideValue(valueStream.str(), TA_Base_Bus::DPT_INTERNAL_SESSION_ID); 
    }
    catch(TA_Base_Core::ScadaProxyException&)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed setting %s datapoint proxy", descriptionForDebug.c_str() );
    }

	FUNCTION_EXIT;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Common 
//
///////////////////////////////////////////////////////////////////////////////



void CtaEcsDatapoints::attachCongestionObserver(ICongestionObserver* observer)
{
	FUNCTION_ENTRY("attachCongestionObserver");

	if (!observer)
	{
		FUNCTION_EXIT;
		return;
	}

    TA_Base_Core::ThreadGuard guard( m_lockForObservers  );
	m_congestionObservers.insert(observer);

	FUNCTION_EXIT;
}



void CtaEcsDatapoints::detachCongestionObserver(ICongestionObserver* observer)
{
	FUNCTION_ENTRY("detachCongestionObserver");

	if (!observer)
	{
		FUNCTION_EXIT;
		return;
	}

    TA_Base_Core::ThreadGuard guard( m_lockForObservers );

    CongestionObserversIt it = m_congestionObservers.find(observer);
	if (it!=m_congestionObservers.end())
	{
		m_congestionObservers.erase(it);
	}

	FUNCTION_EXIT;
}

void CtaEcsDatapoints::attachEmergencyObserver(IEmergencyObserver* observer)
{
	FUNCTION_ENTRY("attachEmergencyObserver");

	if (!observer)
	{
		FUNCTION_EXIT;
		return;
	}

    TA_Base_Core::ThreadGuard guard(m_lockForObservers);
	m_emergencyObservers.insert(observer);

	FUNCTION_EXIT;
}



void CtaEcsDatapoints::detachEmergencyObserver(IEmergencyObserver* observer)
{
	FUNCTION_ENTRY("detachEmergencyObserver");

	if (!observer)
	{
		FUNCTION_EXIT;
		return;
	}

    TA_Base_Core::ThreadGuard guard( m_lockForObservers);

    EmergencyObserversIt it = m_emergencyObservers.find(observer);
	if (it!=m_emergencyObservers.end())
	{
		m_emergencyObservers.erase(it);
	}

	FUNCTION_EXIT;
}

void CtaEcsDatapoints::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
{
	FUNCTION_ENTRY("processEntityUpdateEvent");

	if (false == m_isEnabled)
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
		FUNCTION_EXIT;
		return;
	}

    try
    {
		// Only proceed if it is a value update
        if ( updateType != TA_Base_Bus::ValueStateUpdate &&
             updateType != TA_Base_Bus::AlarmSummaryUpdate && //limin++, TD20740
             updateType != TA_Base_Bus::AckSummaryUpdate )    //limin++, TD20740
        {
			FUNCTION_EXIT;
            return;
        }

		TA_Base_Core::ThreadGuard guard( m_lockForObservers );

		// Search through list of emergency datapoints
		KeyToObserverDataMapIt koit = m_emergencyObserverData.find(entityKey);
		if (koit!=m_emergencyObserverData.end())
		{
			DataPointObserverData& data=koit->second;
			TA_Base_Bus::DataPointProxySmartPtr * dp = data.m_proxy;

			// Notify all observers
			EmergencyObserversIt eoit =  m_emergencyObservers.begin();

           	//perform initialization 
			for (;eoit!=m_emergencyObservers.end();++eoit)
			{
				// marvin++
				EmergencyProposalEngine* ptrProposalEngine = dynamic_cast<EmergencyProposalEngine*> (*eoit);
				if(ptrProposalEngine!=NULL)
				{					
					ptrProposalEngine->getCompleteState().clearProposedModes(TA_IRS_Bus::IMasterEcsAgentCorbaDef::MODE_OVERRIDING_STATION);
					break;
				}
				// ++marvin
			}

			for (eoit= m_emergencyObservers.begin();eoit!=m_emergencyObservers.end(); ++eoit)
			{
				(*eoit)->processEmergencyUpdate(data.m_ecsZoneId, (*dp)->getValue().getBoolean());	
			}

			FUNCTION_EXIT;
			return;
		}		

		// // Search through list of congestion datapoints
		koit = m_congestionObserverData.find(entityKey);
		if (koit!=m_congestionObserverData.end())
		{
			DataPointObserverData& data=koit->second;
			TA_Base_Bus::DataPointProxySmartPtr * dp = data.m_proxy;

			// Notify all observers
			CongestionObserversIt coit =  m_congestionObservers.begin();

			for (;coit!=m_congestionObservers.end(); ++coit)
			{
				(*coit)->processCongestionUpdate(data.m_ecsZoneId, static_cast<unsigned long>((*dp)->getValue().getEnum()) );
			}

			FUNCTION_EXIT;
			return;
		}		
    }
    catch(TA_Base_Core::ScadaProxyException&)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to process Congestion/Emergency DataPoint update for entity key = %d", entityKey );
    }

	FUNCTION_EXIT;
}



unsigned long CtaEcsDatapoints::getCongestion(unsigned long zoneId)
{
	static const std::string descriptionForDebug("MyZoneCongestion");

	try
	{
		return getIntegerDataPoint(m_theMaps.getMyZoneCongestionDataPoint(zoneId), zoneDpDescription(descriptionForDebug, zoneId));
	}
	catch(...) // EcsMonitorModeException, InvalidECSConfigurationException
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to get %s value.  Assuming %ld.",
			zoneDpDescription(descriptionForDebug, zoneId).c_str(),
			TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL );
		return TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL;
	}
}

void CtaEcsDatapoints::setCongestion(unsigned long zoneId, unsigned long value, bool fInit)
{
    setIntegerDataPoint(m_theMaps.getMyZoneCongestionDataPoint(zoneId), value, zoneDpDescription("MyZoneCongestion", zoneId), fInit);
}



void CtaEcsDatapoints::setEmergency(unsigned long zoneId, bool value, bool fInit)
{
    setBooleanDataPoint(m_theMaps.getMyZoneEmergencyDataPoint(zoneId), value, zoneDpDescription("MyZoneEmergency", zoneId), fInit);
}

//Datapoint is removed
/*
void CtaEcsDatapoints::setCongestionApplied(unsigned long zoneId, bool value, bool fInit)
{
    setBooleanDataPoint(m_theMaps.getMyZoneCongestionAppliedDataPoint(zoneId), value, zoneDpDescription("MyZoneCongestionApplied", zoneId), fInit);
}

void CtaEcsDatapoints::setCongestionAppliedGivenEntityKey( unsigned long entityKey, bool value)
{
    setBooleanDataPoint(m_theMaps.getMyZoneCongestionAppliedDataPointForCongestionEntityKey(entityKey), value, "MyZoneCongestionApplied");
}
*/


bool CtaEcsDatapoints::getEmergencyApplied(unsigned long zoneId)
{
	static const std::string descriptionForDebug("MyZoneEmergencyApplied");

	try
	{
		return getBooleanDataPoint(m_theMaps.getMyZoneEmergencyAppliedDataPoint(zoneId), zoneDpDescription(descriptionForDebug, zoneId));
	}
	catch(...) // EcsMonitorModeException, InvalidECSConfigurationException
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to get %s value.  Assuming False.",
			zoneDpDescription(descriptionForDebug, zoneId).c_str() );
		return false;
	}

}

void CtaEcsDatapoints::setEmergencyApplied(unsigned long zoneId, bool value, bool fInit)
{
    setBooleanDataPoint(m_theMaps.getMyZoneEmergencyAppliedDataPoint(zoneId), value, zoneDpDescription("MyZoneEmergencyApplied", zoneId), fInit);
}

void CtaEcsDatapoints::setEvacuationSelected(unsigned long zoneId, unsigned long value, bool fInit)
{
    setIntegerDataPoint(m_theMaps.getMyZoneEvacuationSelectedDataPoint(zoneId), value, zoneDpDescription("MyZoneEvacuationSelected", zoneId), fInit);
}

void CtaEcsDatapoints::setAirflowSelected(unsigned long zoneId, unsigned long value, bool fInit)
{
    setIntegerDataPoint(m_theMaps.getMyZoneAirflowSelectedDataPoint(zoneId), value, zoneDpDescription("MyZoneAirflowSelected", zoneId), fInit);
}


void CtaEcsDatapoints::setEvacuationApplied(unsigned long zoneId, unsigned long value, bool fInit)
{
    setIntegerDataPoint(m_theMaps.getMyZoneEvacuationAppliedDataPoint(zoneId), value, zoneDpDescription("MyZoneEvacuationApplied", zoneId), fInit);
}

void CtaEcsDatapoints::setAirflowApplied(unsigned long zoneId, unsigned long value, bool fInit)
{
    setIntegerDataPoint(m_theMaps.getMyZoneAirflowAppliedDataPoint(zoneId), value, zoneDpDescription("MyZoneAirflowApplied", zoneId), fInit);
}


std::set<unsigned long> CtaEcsDatapoints::getZonesInCongestion()
{
	FUNCTION_ENTRY("getZonesInCongestion");

    std::set<unsigned long> zonesInCongestion;

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );

		FUNCTION_EXIT;
        return zonesInCongestion;
    }

	TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdates );


    for (std::vector<unsigned long>::iterator it=m_allZoneIds.begin(); it!=m_allZoneIds.end(); ++it)
    {
	    // We deliberately reimplement getZoneCongestion() so that we do not need to throw 
        // and catch exceptions unecessarily 

        TA_Base_Bus::DataPoint* dp = m_theMaps.getMyZoneCongestionDataPoint(*it);

        if (NULL != dp)
        {
            if (dp->getCurrentValue().getEnum() != TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL)
            {
                zonesInCongestion.insert(*it);
            }
        }
        else
        {
            // This will also handle the case where the ecsZoneId may = TA_IRS_Bus::ECS_NONE_OR_INVALID
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugNorm, "Unknown ECS Zone %ld", (*it) );
        }
    }

	FUNCTION_EXIT;
    return zonesInCongestion;
}

std::set<unsigned long> CtaEcsDatapoints::getZonesInEmergency()
{
	FUNCTION_ENTRY("getZonesInEmergency");

    std::set<unsigned long> zonesInEmergency;

    if (false == m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode" );
		FUNCTION_EXIT;
        return zonesInEmergency;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdates );

    for (std::vector<unsigned long>::iterator it=m_allZoneIds.begin(); it!=m_allZoneIds.end(); ++it)
    {
	    // We dilberately reimplement getZoneEmergency() so that we do not need to throw 
        // and catch exceptions unecessarily 

        TA_Base_Bus::DataPoint* dp = m_theMaps.getMyZoneEmergencyDataPoint(*it);

        if (NULL != dp)
        {
            if (dp->getCurrentValue().getBoolean())
            {
                zonesInEmergency.insert(*it);
            }
        }
        else
        {
            // This will also handle the case where the ecsZoneId may = TA_IRS_Bus::ECS_NONE_OR_INVALID
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugNorm, "Ignoring unknown ECS Zone %ld", (*it) );
        }
    }

	FUNCTION_EXIT;
    return zonesInEmergency;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Station ECS Agent 
//
///////////////////////////////////////////////////////////////////////////////


void CtaEcsDatapoints::setLastAppliedMasterMode(long value, bool fInit)
{
    setIntegerDataPoint(m_theConfig.getMyLastAppliedMasterModeDataPoint(), value, "MyLastAppliedMasterMode");
}

void CtaEcsDatapoints::setLastAppliedStationMode(long value, bool fInit)
{
    setIntegerDataPoint(m_theConfig.getMyLastAppliedStationModeDataPoint(), value, "MyLastAppliedStationMode");
}

void CtaEcsDatapoints::setSelectedMasterMode(long value, bool fInit)
{
	//zn++
	if (fInit)
	{
		setForcedIntegerDataPoint(m_theConfig.getMySelectedMasterModeDataPoint(), value, "MySelectedMasterMode");
		return;
	}
	//++zn

    setIntegerDataPoint(m_theConfig.getMySelectedMasterModeDataPoint(), value, "MySelectedMasterMode");
}

void CtaEcsDatapoints::setSelectedOverridingStationMode(long value, bool fInit)
{
	//zn++
	if (fInit)
	{
		setForcedIntegerDataPoint(m_theConfig.getMySelectedOverridingStationModeDataPoint(), value, "MySelectedOverridingStationMode");
		return;
	}
	//++zn

    setIntegerDataPoint(m_theConfig.getMySelectedOverridingStationModeDataPoint(), value, "MySelectedOverridingStationMode");
}

void CtaEcsDatapoints::setIscsAlive(unsigned long value)
{
	FUNCTION_ENTRY("setIscsAlive");

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Setting MyIscsAlive datapoint to %ld", value );

    if (!m_isEnabled)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Skipping operation in operation mode");        
		FUNCTION_EXIT;
        return;
    }

    try
    {
        //TD11069, To prevent log event for alive control
        //for the value is label index, so need to convert to label before set to proxy
        std::string valueLabel;
        TA_Base_Bus::DpvEnumLabelsMap labelMap = m_theConfig.getIscsAliveProxy()->getDerivedStatesAsMap();
        TA_Base_Bus::DpvEnumLabelsMap::iterator itLabel = labelMap.find(value);

        if (itLabel != labelMap.end())
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "Set datapoint [%s] to [%s]", m_theConfig.getIscsAliveProxy()->getEntityName().c_str(), itLabel->second.c_str());
            m_theConfig.getIscsAliveProxy()->setValue(itLabel->second, TA_Base_Bus::DPT_INTERNAL_SESSION_ID, 0, NULL, false);
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "The value [%u] for iscsAlive datapoint is not valid", value);
        }
        
		// ToDo - closeIscsAliveSetFailureAlarm(); ???
	}
	catch(...) // TA_Base_Core::ScadaProxyException&
	{
		// It is important that no exceptions get through    
		// ToDo - raiseIscsAliveSetFailureAlarm(); ???

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed setting MyIscsAlive datapoint");
	}

	FUNCTION_EXIT;
}


void CtaEcsDatapoints::setMasterModeLock(bool value, bool fInit)
{
    setBooleanDataPoint(m_theConfig.getMyMasterModeLockDataPoint(), value, "MyMasterModeLock", fInit);
}

void CtaEcsDatapoints::setMasterModeLockControl(bool value, bool fInit)
{
    setBooleanDataPoint(m_theConfig.getMyMasterModeLockControlDataPoint(), value, "MyMasterModeLockControl", fInit);
}

bool CtaEcsDatapoints::getMasterModeLocked()
{
	TA_Base_Bus::DataPoint* dp = m_theConfig.getMyMasterModeLockDataPoint();

	if (NULL != dp)
	{
		return dp->getCurrentValue().getBoolean();
	}
	else
	{
        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, 
			"Cannot determine MyMasterModeLock state.  Assuming false.");
		return false;
	}
}

bool CtaEcsDatapoints::getLocalMode()
{
	FUNCTION_ENTRY("getLocalMode");

    bool state(true);

    try
    {
		state = m_theConfig.getLocalModeProxy()->getValue().getBoolean();

    }
    catch(TA_Base_Core::ScadaProxyException&)
    {
        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, 
            "CtaEcsDatapoints::getLocalMode() - Cannot determine LocalMode state.  Assuming true, not in local mode.");
        state = true;
    }

	FUNCTION_EXIT;

    return state;
}

unsigned long CtaEcsDatapoints::getCurrentStationMode()
{
	FUNCTION_ENTRY("getCurrentStationMode");

    unsigned long state(TA_IRS_Bus::ECS_NONE_OR_INVALID);

    try
    {
        state = (unsigned long)m_theConfig.getModeStatusProxy()->getValueAsDouble();
    }
    catch(TA_Base_Core::ScadaProxyException&)
    {
        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, 
            "Cannot determine CurrentStationMode state.  Assuming %ld.", TA_IRS_Bus::ECS_NONE_OR_INVALID);
        state = TA_IRS_Bus::ECS_NONE_OR_INVALID;
    }

	FUNCTION_EXIT;
    return state;
}




///////////////////////////////////////////////////////////////////////////////
//
//  Master ECS Agent 
//
///////////////////////////////////////////////////////////////////////////////



void CtaEcsDatapoints::setExternalLastAppliedMasterMode(unsigned long locationKey, long value)
{
	static const std::string descriptionForDebug("LastAppliedMasterMode");

    try
    {
		setIntegerProxy(m_theMaps.getLastAppliedMasterModeProxy(locationKey), value, descriptionForDebug);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Expecting ScadaProxyException, CachedMappingNotFoundException
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s proxy at %s", descriptionForDebug.c_str(), m_theMaps.getLocationName(locationKey).c_str());
    }
}

void CtaEcsDatapoints::setExternalLastAppliedStationMode(unsigned long locationKey, long value)
{
	static const std::string descriptionForDebug("LastAppliedStationMode");

    try
    {
		setIntegerProxy(m_theMaps.getLastAppliedStationModeProxy(locationKey), value, descriptionForDebug);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Expecting ScadaProxyException, CachedMappingNotFoundException
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s proxy at %s", descriptionForDebug.c_str(), m_theMaps.getLocationName(locationKey).c_str());
    }
}

void CtaEcsDatapoints::setExternalSelectedMasterMode(unsigned long locationKey, long value)
{
	static const std::string descriptionForDebug("SelectedMasterMode");

    try
    {
		setIntegerProxy(m_theMaps.getSelectedMasterModeProxy(locationKey), value, descriptionForDebug);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Expecting ScadaProxyException, CachedMappingNotFoundException
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s proxy at %s", descriptionForDebug.c_str(), m_theMaps.getLocationName(locationKey).c_str());
    }
}
/*
void CtaEcsDatapoints::setExternalSelectedMasterMode(unsigned long locationKey, long value)
{
	static const std::string descriptionForDebug("SelectedMasterMode");

    try
    {
		setIntegerProxy(m_theMaps.getSelectedMasterModeProxy(locationKey), value, descriptionForDebug);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Expecting ScadaProxyException, CachedMappingNotFoundException
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s proxy at %s", descriptionForDebug.c_str(), m_theMaps.getLocationName(locationKey).c_str());
    }
}

void CtaEcsDatapoints::setExternalSelectedOverridingStationMode(unsigned long locationKey, unsigned long value)
{
	static const std::string descriptionForDebug("SelectedOverridingStationMode");

    try
    {
		setIntegerProxy(m_theMaps.getSelectedOverridingStationModeProxy(locationKey), value, descriptionForDebug);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Expecting ScadaProxyException, CachedMappingNotFoundException
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s proxy at %s", descriptionForDebug.c_str(), m_theMaps.getLocationName(locationKey).c_str());
    }
}
*/
void CtaEcsDatapoints::setExternalSelectedOverridingStationMode(unsigned long locationKey, long value)
{
	static const std::string descriptionForDebug("SelectedOverridingStationMode");

    try
    {
		setIntegerProxy(m_theMaps.getSelectedOverridingStationModeProxy(locationKey), value, descriptionForDebug);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Expecting ScadaProxyException, CachedMappingNotFoundException
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s proxy at %s", descriptionForDebug.c_str(), m_theMaps.getLocationName(locationKey).c_str());
    }
}

void CtaEcsDatapoints::setExternalMasterModeLock(unsigned long locationKey, bool value)
{
	static const std::string descriptionForDebug("MasterModeLock");

    try
    {
		setBooleanProxy(m_theMaps.getMasterModeLockProxy(locationKey), value, descriptionForDebug);
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
        // Expecting ScadaProxyException, CachedMappingNotFoundException
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s proxy at %s", descriptionForDebug.c_str(), m_theMaps.getLocationName(locationKey).c_str() );
    }
}

unsigned long CtaEcsDatapoints::getCurrentMasterMode()
{
	FUNCTION_ENTRY("getCurrentMasterMode");

	static const std::string descriptionForDebug("MyModeStatus");
    TA_Base_Bus::DataPoint* dp = m_theConfig.getMyModeStatusDataPoint();
    
    if(NULL == dp) 
    {
        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s datapoint.  Assuming %ld.", descriptionForDebug.c_str(), TA_IRS_Bus::ECS_NONE_OR_INVALID );
		FUNCTION_EXIT;
        return TA_IRS_Bus::ECS_NONE_OR_INVALID;
    }

	FUNCTION_EXIT;
    return (unsigned long)m_theConfig.getMyModeStatusDataPoint()->getCurrentValue().getFloat();
}

void CtaEcsDatapoints::setCurrentMasterMode(unsigned long value, bool fInit)
{
    setIntegerDataPoint(m_theConfig.getMyModeStatusDataPoint(), value, "MyModeStatus", fInit);
}


bool CtaEcsDatapoints::getExternalStationInLocalMode(unsigned long locationKey)
{
	FUNCTION_ENTRY("getExternalStationInLocalMode");

    bool state(true);

    try
    {
	    TA_Base_Bus::DataPointProxySmartPtr &proxy = m_theMaps.getLocalModeProxy(locationKey);

		if (proxy->getStatus() == TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE ||
			proxy->getStatus() == TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON)
		{
			state = proxy->getValue().getBoolean();
		}
    }
    catch(TA_Base_Core::ScadaProxyException&)
    {
		state = true;
        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, 
            "Cannot determine LocalMode state at %s.  Assuming true.", m_theMaps.getLocationName(locationKey).c_str() );
    }
    catch(TA_Base_Core::CachedMappingNotFoundException&)
    {
		state = true;
        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find LocalMode proxy at %s.  Assuming true.", m_theMaps.getLocationName(locationKey).c_str() );
    }

	FUNCTION_EXIT;
    return !state;
}


void CtaEcsDatapoints::setMasterModeInProgress(unsigned long value, bool fInit)
{
    setIntegerDataPoint(m_theConfig.getMyModeInProgressDataPoint(), value, "MasterModeInProgress", fInit);
}

unsigned long CtaEcsDatapoints::getMasterModeInProgress()
{
    FUNCTION_ENTRY("getCurrentMasterMode");

	static const std::string descriptionForDebug("MasterModeInProgress");
    TA_Base_Bus::DataPoint* dp = m_theConfig.getMyModeInProgressDataPoint();

    if(NULL == dp) 
    {
        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugError, 
            "Failed to find %s datapoint.  Assuming %ld.", descriptionForDebug.c_str(), TA_IRS_Bus::ECS_NONE_OR_INVALID );

		FUNCTION_EXIT;
        return TA_IRS_Bus::ECS_NONE_OR_INVALID;
    }

	FUNCTION_EXIT;
    return m_theConfig.getMyModeInProgressDataPoint()->getCurrentValue().getEnum();
}
