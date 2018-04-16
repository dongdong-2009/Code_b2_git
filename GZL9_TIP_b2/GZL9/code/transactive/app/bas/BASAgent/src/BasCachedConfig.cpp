/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasCachedConfig.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#include <utility>
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/bas/src/BasParameterValueType.h"
#include "app/bas/BASAgent/src/ModeExecutionException.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/bas/src/BASTimeTableMappingAccessFactory.h"

namespace TA_IRS_App
{
	TA_Base_Core::NonReEntrantThreadLockable BasCachedConfig::m_singletonLock;
	BasCachedConfig* BasCachedConfig::m_instance = NULL;

	BasCachedConfig::BasCachedConfig() : 
	m_stationName(""),
    m_locationDisplayName(""),
    m_stationFireMonitorTime( 0u ),
    m_entityKey( TA_Base_Core::DatabaseKey::getInvalidKey() ),
    m_ulLocationKey( TA_Base_Core::DatabaseKey::getInvalidKey() )
	{
		m_dataPoints.clear();
        m_basModeMap.clear();
        m_timeTableTypeMap.clear();
	}

	BasCachedConfig::~BasCachedConfig()
	{
	}

	BasCachedConfig& BasCachedConfig::getInstance()
	{
		FUNCTION_ENTRY("BasCachedConfig::getInstance");

		if (m_instance == NULL)
		{
			TA_THREADGUARD(m_singletonLock);

			if (m_instance == NULL)
			{
				m_instance = new BasCachedConfig();
			}
		}

		FUNCTION_EXIT;
		return *m_instance;
	}

	void BasCachedConfig::initCache(const TA_Base_Core::StationBASAgentEntityDataPtr& entityData, const std::map<unsigned long, TA_Base_Bus::DataPoint*>& datapoints)
	{
        FUNCTION_ENTRY("initCach");

        m_dataPoints = datapoints;

        if ( NULL != entityData.get() )
        {
            m_stationName = entityData->getStationName();
            m_modeControlToken	= entityData->getMyModeControlToken();
            m_modeAutoManualToken = entityData->getMyModeRightStatusToken();
            m_modeRightControlSqlToken = entityData->getMyModeRightsControlToken();
            m_modeInProgressToken = entityData->getMyModeExecutionStatusToken();
            m_modeValueToken = entityData->getMyModeValueToken();
            m_currentDayTTPt = getDatapointFromName(entityData->getCurrentDayTTPtBGN());
            m_nextDayTTPt = getDatapointFromName(entityData->getNextDayTTPtBGN());
            m_stationFireMonitorTime = entityData->getFireStationMonitorSecond();
            m_entityKey = entityData->getKey();
            m_TTScheduleTime = entityData->getTTScheduleTime();
            m_fireModeRecoverDpTag = entityData->getMyFireModeRecoverDpTag();
            m_ulLocationKey = entityData->getLocation();
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Bas Agent EntityData should not be NULL" );
        }

        if ( TA_Base_Core::DatabaseKey::isInvalidKey( m_ulLocationKey ))
        {
            m_ulLocationKey = TA_Base_Core::DataConversion::toUnsignedInteger( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ));
        }

        if ( TA_Base_Core::DatabaseKey::isValidKey( m_ulLocationKey ))
        {
            std::auto_ptr<TA_Base_Core::ILocation> location(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(m_ulLocationKey));
            m_locationDisplayName = location->getDisplayName();

            std::vector<TA_IRS_Core::Mode_SmartPtr> basModes;
            TA_IRS_Core::ModeAccessFactory::getInstance().getModes(m_ulLocationKey, basModes);
            for(std::vector<TA_IRS_Core::Mode_SmartPtr>::const_iterator modeIter = basModes.begin(); modeIter != basModes.end(); ++modeIter)
            {
                m_basModeMap.insert(std::make_pair((*modeIter)->getValue(), (*modeIter)));
            }

            TA_IRS_Core::BASTimeTableTimeTableAccessFactoryInstance::instance()->getLocationAndTimeTableTypeByLocation(m_timeTableTypeMap, m_ulLocationKey);
        }

		FUNCTION_EXIT;
	}

	const std::string& BasCachedConfig::getStationName() const
	{
		FUNCTION_ENTRY("getStationName");

		FUNCTION_EXIT;
		return m_stationName;
	}

	const std::string& BasCachedConfig::getModeControlToken() const
	{
		FUNCTION_ENTRY("getModeControlToken");

		FUNCTION_EXIT;
		return m_modeControlToken;
	}

	TA_Base_Bus::DataPoint* BasCachedConfig::getModeCtrlDPFromNode(const std::string& pnName)
	{
		return getDatapointFromName(pnName + m_modeControlToken);
	}

	TA_Base_Bus::DataPoint* BasCachedConfig::getModeAutoManualDPFromNode(const std::string& pnName)
	{
		return getDatapointFromName(pnName + m_modeAutoManualToken);
	}

	TA_Base_Bus::DataPoint* BasCachedConfig::getModeInProgressDPFromNode(const std::string& pnName)
	{
		return getDatapointFromName(pnName + m_modeInProgressToken);
	}

	TA_Base_Bus::DataPoint* BasCachedConfig::getModeValueDPFromNode(const std::string& pnName)
	{
		return getDatapointFromName(pnName + m_modeValueToken);
	}

	TA_Base_Bus::DataPoint* BasCachedConfig::getDatapointFromName(const std::string& ptName)
	{
		FUNCTION_ENTRY("getDatapointFromName");
		
		if(m_dataPoints.empty())
		{
			 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Points Map should not be empty");
			 return NULL;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get DataPoint %s", ptName.c_str());

		std::map<unsigned long, TA_Base_Bus::DataPoint*>::const_iterator iter = m_dataPoints.begin();
		while (iter != m_dataPoints.end())
		{
			if ((iter->second)->getDataPointName() == ptName)
			{
				FUNCTION_EXIT;
				return iter->second;
			}
			++iter;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "DataPoint [%s] are not found!!", ptName.c_str());
		return NULL;
	}

	unsigned long BasCachedConfig::getLocationKey()
	{
		FUNCTION_ENTRY("getLocationKey");
		FUNCTION_EXIT;
		return m_ulLocationKey;
	}

	unsigned long BasCachedConfig::getEntityKey()
	{
		return m_entityKey;
	}

	unsigned long BasCachedConfig::getStationFireMonitorTime()
	{
		return m_stationFireMonitorTime;
	}

	bool BasCachedConfig::isStationFireMonitorValid()
	{
		return (0 < m_stationFireMonitorTime) ? true : false; 
	}

	TA_Base_Bus::DataPoint* BasCachedConfig::getCurrentDayTTPt()
	{
		FUNCTION_ENTRY("getCurrentDayTTPt()");		
		FUNCTION_EXIT;
		return m_currentDayTTPt;
	}

	TA_Base_Bus::DataPoint* BasCachedConfig::getNextDayTTPt()
	{
		FUNCTION_ENTRY("getNextDayTTPt");
		FUNCTION_EXIT;
		return m_nextDayTTPt;		
	}

    TA_Base_Bus::DataPoint* BasCachedConfig::getModeRightControlDPFromNode( const std::string& pnName )
    {
        return getDatapointFromName(pnName + m_modeRightControlSqlToken);
    }

    void BasCachedConfig::getParameterValueType( TA_IRS_Core::ParameterValueTypeInfoVct& valueTypes )
    {
        FUNCTION_ENTRY("getParameterValueType");
        if (m_parameterValueTypeVct.empty())
        {
            try
            {
                TA_IRS_Core::BasParameterValueTypeAccessFactory::getInstance().getParameterValueTypeInfos(m_parameterValueTypeVct);
            }
            catch(std::exception& e)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Fail to get parameter value type" );
            }
        }        
        valueTypes = m_parameterValueTypeVct;
        FUNCTION_EXIT;
    }

    TA_Base_Bus::DataPoint& BasCachedConfig::getDatapointFromKey( const unsigned long dpKey )
    {
        std::map<unsigned long,TA_Base_Bus::DataPoint*>::iterator dpIter = m_dataPoints.find(dpKey);
        if (m_dataPoints.end() != dpIter && NULL != dpIter->second)
        {
            return *(dpIter->second);
        }

        std::stringstream errorMessage;
        errorMessage << "DataPoint is not cached: " << dpKey;
        TA_THROW(TA_Base_Core::DataException("Cannot find datapoint", TA_Base_Core::DataException::NO_VALUE, errorMessage.str()));
    }

    TA_IRS_Core::Mode_SmartPtr BasCachedConfig::getBasModeFromValue( const unsigned long modeValue )
    {
        std::map<unsigned long, TA_IRS_Core::Mode_SmartPtr>::iterator itr = m_basModeMap.find(modeValue);
        if (m_basModeMap.end() != itr)
        {
            return itr->second;
        }
        TA_THROW(ModeExecutionException(ModeExecutionException::UNDEFINE_FAILURE_TYPE, "Bas Mode cannot be found"));
    }

    std::string BasCachedConfig::getTTScheduleTime()
    {
        return m_TTScheduleTime;
    }

    std::string BasCachedConfig::getTimeTableTypeNameByKey( const unsigned long pkey )
    {
        TA_IRS_Core::LocationAndTimeTableTypeMap::iterator typeIter = m_timeTableTypeMap.find(pkey);
        if (m_timeTableTypeMap.end() != typeIter)
        {
            return typeIter->second.timetableTypeName;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find the timetable for key:%lu", pkey);
        return "";
    }

    std::string BasCachedConfig::getLocDisplayName()
    {
        return m_locationDisplayName;
    }

    TA_Base_Bus::DataPoint* BasCachedConfig::getFireModeRecoverDPFromNode( const std::string& pnName )
    {
        return getDatapointFromName(pnName + m_fireModeRecoverDpTag);
    }

    std::string BasCachedConfig::getTimetableDisplayNameByKey( const unsigned long pkey )
    {
        TA_IRS_Core::LocationAndTimeTableTypeMap::iterator typeIter = m_timeTableTypeMap.find(pkey);
        if (m_timeTableTypeMap.end() != typeIter)
        {
            return typeIter->second.timetableDisplayName;
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find the timetable for key:%lu", pkey);
        return "";
    }
}
