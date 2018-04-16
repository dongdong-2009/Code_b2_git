#include <boost/regex.hpp> 
#include "TVSSAlarmProcessor.h"
#include "TVSSServant.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#define MAX_DP_COUNT 4
#define MAX_STATION_COUNT 80
#define PREFIX "OCC_"
namespace TA_IRS_App
{

TVSSAlarmProcessor::TVSSAlarmProcessor(std::string locationName,TVSSServant* servant,unsigned long checkAlarmInterval)
:m_locationName(locationName),m_servant(servant),m_terminate(true),m_equipmentAlarmCheckInterval(checkAlarmInterval>0?checkAlarmInterval:2000)
{
	ClearDpList();
	if(locationName == "OCC")
	{
		// Get location information
		try
		{
			stationIdToLocationKeyMap =
				TA_IRS_Core::TVSSStationIdAccessFactory::getInstance().getStationIdToLocationKeyMap();

			m_dpCount = 1 + stationIdToLocationKeyMap.size() * 3;		//according to the db config
            m_dpNames = new std::string[m_dpCount];
			m_dpNames[0] = ".+" + m_gOccSvrName;

			for(TA_IRS_Core::TVSSStationIdAccessFactory::StationIdToLocationKeyMap::iterator it = stationIdToLocationKeyMap.begin();
				it != stationIdToLocationKeyMap.end();
				++it)
			{
				TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(it->second);
				std::string locationName = location->getName();
				//m_locationNameStationIdMap.insert(std::pair<std::string, unsigned long>(location->getName(), it->first));
				delete location;
				unsigned long stationId = it->first;

				m_dpNames[(stationId-1)*3+1] = PREFIX + locationName + ".+" + m_gGWName;
				m_dpNames[(stationId-1)*3+2] = PREFIX + locationName + ".+" + m_gAPName;
				m_dpNames[(stationId-1)*3+3] = PREFIX + locationName + ".+" + m_gSBName;
			}
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Failed init station id location key map");
		}
	}
	else
	{
		m_dpCount = 4;
        m_dpNames = new std::string[m_dpCount];
		m_dpNames[0] = ".+" + m_gGWName;
		m_dpNames[1] = ".+" + m_gAPName;
		m_dpNames[2] = ".+" + m_gSBName;
		m_dpNames[3] = ".+" + m_gLTMName;
	}

}


TVSSAlarmProcessor::~TVSSAlarmProcessor()
{
	if(!m_terminate)
	{
		m_terminate = true;
		TA_Base_Core::ThreadGuard guard(m_lock);
	}
	terminateAndWait();
	ClearDpList();
	delete []m_dpNames;
}

bool TVSSAlarmProcessor::AddDataPoint(std::string strName,TA_Base_Bus::DataPoint* dataPoint)
{
	FUNCTION_ENTRY("AddDataPoint");
	
	bool shouldInsert = false;
	for(int i = 0; i<m_dpCount;i++)
	{
		const char *szStr = strName.c_str();
		const char *szReg = m_dpNames[i].c_str();
		boost::regex reg( szReg );
		bool bMatch = boost::regex_match( szStr , reg);
		if(bMatch)
		{
			shouldInsert = true;
			m_dpNames[i] = strName;
			break;
		}
	}
	if(!shouldInsert)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"DataPoint[%s] can not be added to AlarmProcessor",strName.c_str());
		return false;
	}
	m_dpList[strName] = dataPoint;
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"DataPoint[%s] has been added to AlarmProcessor",strName.c_str());
	//	dpValue.setBoolean(false);
//	TA_Base_Bus::DataPointState dpStatus(dpValue,time(NULL),TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
//	dataPoint->updateFieldState(dpStatus);
	FUNCTION_EXIT;
	return true;
}

bool TVSSAlarmProcessor::SetDataPointValue(int alarmIndex,bool alarmValue)
{
	FUNCTION_ENTRY("SetDataPointValue");
	if(alarmIndex >= m_dpCount )
	{
		return true;
	}
	if(m_dpList.end() == m_dpList.find(m_dpNames[alarmIndex]))
	{
		return true;
	}
	TA_Base_Bus::DataPoint* dataPoint = m_dpList[m_dpNames[alarmIndex]];
	if(dataPoint==NULL)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"Try to change a DataPoint but can not find its name %s",m_dpNames[alarmIndex].c_str());
		return false;
	}
	TA_Base_Bus::DpValue dpValue = dataPoint->getCurrentValue();
	dpValue.setBoolean(alarmValue);
	TA_Base_Bus::DataPointState dpStatus(dpValue,time(NULL),TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
    if(isDataPointNeedUpdate(dataPoint, dpStatus))
    {
    	dataPoint->updateFieldState(dpStatus);
    	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Change value of DataPoint[%s] to %d",m_dpNames[alarmIndex].c_str(),alarmValue);
    }
	FUNCTION_EXIT;
	return true;
}

void TVSSAlarmProcessor::setDataPointsAsBadValue()
{
	FUNCTION_ENTRY("setDataPointsAsBadValue");
	for(int i=0;i<m_dpCount;i++)
	{
		if(m_dpList.find(m_dpNames[i]) != m_dpList.end())
		{
			TA_Base_Bus::DataPoint* dataPoint = m_dpList[m_dpNames[i]];
			if(NULL != dataPoint)
			{
				TA_Base_Bus::DpValue dpValue = dataPoint->getCurrentValue();
				TA_Base_Bus::DataPointState badState(dpValue,time(NULL),TA_Base_Bus::QUALITY_BAD_COMM_FAILURE);
                if(isDataPointNeedUpdate(dataPoint, badState))
                {
                	dataPoint->updateFieldState(badState);
    				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Change status of DataPoint[%s] to quality bad",m_dpNames[i].c_str());
                }
			}
		}
	}
	FUNCTION_EXIT;
}

void TVSSAlarmProcessor::SetControlMode()
{
	FUNCTION_ENTRY("SetControlMode");
	/*
	if(m_dpList.empty())
	{
		return;
	}
	*/
	m_terminate = false;
	start();
	FUNCTION_EXIT;
}

void TVSSAlarmProcessor::SetMonitorMode()
{
	FUNCTION_ENTRY("SetMonitorMode");
	/*
	if(m_dpList.empty())
	{
		return;
	}
	*/
	m_terminate = true;
	terminateAndWait();
	FUNCTION_EXIT;
}

void TVSSAlarmProcessor::ClearDpList()
{
	if(m_dpList.empty())
	{
		return;
	}
	m_dpList.clear();

}

void TVSSAlarmProcessor::run()
{
	while(!m_terminate)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		//servant process alarm status
		m_servant->tvssEquipmentAlarmQuery();
		sleep(m_equipmentAlarmCheckInterval);
	}
}

void TVSSAlarmProcessor::terminate()
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	m_terminate = true;
}

int TVSSAlarmProcessor::GetDPCount()
{
    return m_dpCount;
}

bool TVSSAlarmProcessor::isDataPointNeedUpdate( TA_Base_Bus::DataPoint* dataPoint, const TA_Base_Bus::DataPointState& newState )
{
    return dataPoint->getCurrentFieldValue() != newState.getValue() || dataPoint->getFieldValueQualityStatus() != newState.getStatus();
}

}