/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/StatusMonitor.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4503 )
#endif // _MSC_VER

#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "app/signs/tis_agent/src/TTISManager.h"//limin
#include "bus/signs_4669/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "app/signs/tis_agent/src/GlobalConstant.h"

namespace TA_IRS_App
{
    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DataPointState;
    using TA_Base_Bus::DpValue;
    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::PredefinedMessageLibraryAccessFactory;
    using TA_Base_Core::IPredefinedMessageLibrary;

	//
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable StatusMonitor::m_singletonLock;
	StatusMonitor* StatusMonitor::m_theClassInstance = NULL;

	StatusMonitor::StatusMonitor()
      : m_shouldBeRunning( true ),
		m_isTimeScheduleDownloadInProgress(false)
	{
		// Note - the library versions above are initialised for testing purposes only
	}

	StatusMonitor::~StatusMonitor()
	{
	}

    //
    // getInstance
    //
    StatusMonitor* StatusMonitor::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.
			m_theClassInstance = new StatusMonitor();
		}

		return m_theClassInstance;
    }

	void StatusMonitor::run()
	{
		FUNCTION_ENTRY("run()");

		while( m_shouldBeRunning )
		{
			if( AgentModeMonitor::getInstance().isInControlMode() )
			{
				if( m_locationType == TA_Base_Core::ILocation::OCC )
				{
					STISManager::getInstance()->submitOCCStatusRequestMessage();
					STISManager::getInstance()->submitCCTStatusRequest();
                    STISManager::getInstance()->submitAllStationsStatusRequestMessage();
				}
				else if( m_locationType != TA_Base_Core::ILocation::DPT ) // Do not want to poll the STIS server at the depot
				{
					STISManager::getInstance()->submitStationStatusRequestMessage();
				}				
			}

			this->sleep(m_statusPollInterval);
		}

		// Set this again, so that the next call to 'run()' will work
		m_shouldBeRunning = true;

		FUNCTION_EXIT;
	}

	void StatusMonitor::terminate()
	{
		FUNCTION_ENTRY("terminate");
			
		m_shouldBeRunning = false;

		FUNCTION_EXIT;
	}

	void StatusMonitor::initialise( TA_Base_Core::TISAgentEntityDataPtr agentData,
                                    TA_Base_Core::ILocation::ELocationType locationType )
	{
		m_location = agentData->getLocationName();
        m_locationType = locationType;
		m_statusPollInterval = agentData->getStatusPollInterval();
		m_agentAssetName = agentData->getAssetName();
	}

    unsigned long StatusMonitor::getNextSTISStationLibraryVersion()
    {
		DataPoint* dataPoint = m_dataPointCahe.getStisNextStisLibraryVersionDataPoint(m_location);
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis next version datapoint at:%s", m_location.c_str());
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}
    
	unsigned long StatusMonitor::getCurrentSTISStationLibraryVersion()
	{
		DataPoint* dataPoint = m_dataPointCahe.getStisCurrentStisLibraryVersionDataPoint(m_location);
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis current version datapoint at:%s", m_location.c_str());
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}

    unsigned long StatusMonitor::getNextISCSStationLibraryVersion()
    {
		DataPoint* dataPoint = m_dataPointCahe.getIscsNextStisLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs next version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
    } 
    
	unsigned long StatusMonitor::getCurrentISCSStationLibraryVersion()
	{
		DataPoint* dataPoint = m_dataPointCahe.getIscsCurrentStisLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs current version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}

	bool StatusMonitor::getStationLibrariesSynchronised()
	{
		DataPoint* dataPoint = m_dataPointCahe.getStisLibrarySynchronisedDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find library synchronised datapoint");
			return false;
		}
		return dataPoint->getCurrentValue().getBoolean();
	}
	
	void StatusMonitor::setNextISCSStationLibraryVersion(unsigned long nextLibraryVersion )
    {
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsNextStisLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
			    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs next version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, nextLibraryVersion);
		}

    }

	void StatusMonitor::setNextSTISStationLibraryVersion(unsigned long nextLibraryVersion, const std::string& dataPointFirstLevelName)
    {

		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			std::string firstLevelName = (dataPointFirstLevelName.empty() ? m_location : dataPointFirstLevelName);
			DataPoint* dataPoint = m_dataPointCahe.getStisNextStisLibraryVersionDataPoint(firstLevelName);
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis next version datapoint at:%s", firstLevelName.c_str());
				return;
			}
			setDataPointIntValue(dataPoint, nextLibraryVersion);
		}
    }

	void StatusMonitor::setStationLibrariesSynchronised( bool synchronised )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getStisLibrarySynchronisedDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find library synchronised datapoint");
				return;
			}
			setDataPointBooleanValue(dataPoint, synchronised);
		}
	}

	void StatusMonitor::setCurrentISCSStationLibraryVersion( unsigned long libraryVersion )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsCurrentStisLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs current version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, libraryVersion);
		}
	}

	void StatusMonitor::setCurrentSTISStationLibraryVersion( unsigned long libraryVersion, const std::string& dataPointFirstLevelName)
	{

		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			std::string firstLevelName = (dataPointFirstLevelName.empty() ? m_location : dataPointFirstLevelName);
			DataPoint* dataPoint = m_dataPointCahe.getStisCurrentStisLibraryVersionDataPoint(firstLevelName);
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis current version datapoint at:%s", firstLevelName.c_str());
				return;
			}
			setDataPointIntValue(dataPoint, libraryVersion);
		}
	}

	void StatusMonitor::addDataPoint( DataPoint* theDataPoint )
	{
		m_dataPointCahe.addDataPoint(theDataPoint);
	}

	void StatusMonitor::setDataPointsToControlMode()
	{
		m_dataPointCahe.setDataPointsToControlMode();
	}

	void StatusMonitor::setDataPointsToMonitorMode()
	{
		m_dataPointCahe.setDataPointsToMonitorMode();
	}
		
	void StatusMonitor::setBadDataPointQuality()
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Status Monitor setting quality of datapoints to bad");
		m_dataPointCahe.setDataPointsToBadQuality();
	}

	void StatusMonitor::setNextISCSStationLibraryVersion_central( unsigned long nextVersionNumber_central )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsCdbNextStisLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs next version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, nextVersionNumber_central);
		}
	}

	void StatusMonitor::setCurrentISCSStationLibraryVersion_central( unsigned long libraryVersion_central )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsCdbCurrentStisLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs current version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, libraryVersion_central);
		}
	}

    unsigned long StatusMonitor::getNextISCSStationLibraryVersion_central()
	{
		DataPoint* dataPoint = m_dataPointCahe.getIscsCdbNextStisLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs next version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}

	unsigned long StatusMonitor::getCurrentISCSStationLibraryVersion_central()
	{
		DataPoint* dataPoint = m_dataPointCahe.getIscsCdbCurrentStisLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs current version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}

	void StatusMonitor::setDataPointIntValue( DataPoint* dataPoint, unsigned long value, bool forceState /*= true*/ )
	{
		DpValue dpValue = dataPoint->getCurrentValue();
		dpValue.setFloat(value);
		TA_Base_Bus::EQualityStatus quality =
			(forceState ? TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE : TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
		DataPointState newState ( dpValue, time( NULL ), quality);
		try
		{
			if(forceState)
			{
				dataPoint->updateForcedState(newState);
			}
			else 
			{
				dataPoint->updateFieldState(newState);
			}
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Update datapoint:%s value failed", dataPoint->getDataPointName().c_str());
		}
	}

	void StatusMonitor::setDataPointBooleanValue( DataPoint* dataPoint, bool value, bool forceState /*= true*/ )
	{
		DpValue dpValue = dataPoint->getCurrentValue();
		dpValue.setBoolean(value);
		TA_Base_Bus::EQualityStatus quality =
			(forceState ? TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE : TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
		DataPointState newState ( dpValue, time( NULL ), quality);
		try
		{
			if(forceState)
			{
			    dataPoint->updateForcedState(newState);
			}
			else 
			{
				dataPoint->updateFieldState(newState);
			}
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Update datapoint:%s value failed", dataPoint->getDataPointName().c_str());
		}
	}

	void StatusMonitor::setDataPointEnumValue(TA_Base_Bus::DataPoint* dataPoint, int value, bool forceState /*= true*/)
	{
        DpValue dpValue(dataPoint->getDataPointDataType(),
                        dataPoint->getEnumLabelsMap(),
                        dataPoint->getBooleanLabels());
		TA_Base_Bus::EQualityStatus quality =
			(forceState ? TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE : TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
		dpValue.setEnum(value);
		DataPointState newState (dpValue, time( NULL ), quality);
		try
		{
			if(forceState)
			{
			    dataPoint->updateForcedState(newState);
			}
			else 
			{
				dataPoint->updateFieldState(newState);
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Update datapoint:%s value failed", dataPoint->getDataPointName().c_str());
		}
	}

	void StatusMonitor::setTimeScheduleDownloadStatus(bool status)
	{
		m_isTimeScheduleDownloadInProgress = status;
	}

	bool StatusMonitor::isTimeScheduleInDownload()
	{
		return m_isTimeScheduleDownloadInProgress;
	}

	std::string StatusMonitor::getAgentAssetName()
	{
		return m_agentAssetName;
	};

	void StatusMonitor::updatePidStatus( const int address, const int status, const std::string& dataPointFirstLevelName /*= ""*/ )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			std::string firstLevelName = (dataPointFirstLevelName.empty() ? m_location : dataPointFirstLevelName);
			DataPoint* dataPoint = m_dataPointCahe.getPidDataPoint(firstLevelName, address, PID_STATUS_TYPE);
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "Can not find pid status datapoint, address: %d, station: %s",
					address, firstLevelName.c_str());
				return;
			}
			setDataPointEnumValue(dataPoint, status, false);
		}
	}

	void StatusMonitor::updateConnectionLinkAlarmDataPointState(bool status, const std::string& dataPointFirstLevelName /*= ""*/)
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			std::string firstLevelName = (dataPointFirstLevelName.empty() ? m_location : dataPointFirstLevelName);
			DataPoint* dataPoint = m_dataPointCahe.getLinkNormalAlarmDataPoint(firstLevelName);
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find connection link alarm datapoint at:%s", firstLevelName.c_str());
				return;
			}
			setDataPointBooleanValue(dataPoint, status, false);
		}
	}

	void StatusMonitor::updateOccServerStatusAlarmDataPointState(bool status)
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getOccServerNormalAlarmDataPoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find occ server status alarm datapoint");
				return;
			}
			setDataPointBooleanValue(dataPoint, status, false);
		}
	}

    unsigned long StatusMonitor::getNextSTISTrainLibraryVersion()
    {
		DataPoint* dataPoint = m_dataPointCahe.getStisNextTrainLibraryVersionDataPoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis next train version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
    }

	/*
	unsigned long StatusMonitor::getCurrentSTISTrainLibraryVersion()
	{
		DataPoint* dataPoint = m_dataPointCahe.getStisCurrentTrainLibraryVersionDataPoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis current train version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}
	*/

    unsigned long StatusMonitor::getNextISCSTrainLibraryVersion()
    {
		DataPoint* dataPoint = m_dataPointCahe.getIscsNextTrainLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs next train version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
    }

	unsigned long StatusMonitor::getCurrentISCSTrainLibraryVersion()
	{
		DataPoint* dataPoint = m_dataPointCahe.getIscsCurrentTrainLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs current train version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}

	bool StatusMonitor::getTrainLibrariesSynchronised()
	{
		DataPoint* dataPoint = m_dataPointCahe.getTrainLibrarySynchronisedDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find train library synchronised datapoint");
			return false;
		}
		return dataPoint->getCurrentValue().getBoolean();
	}

	void StatusMonitor::setNextISCSTrainLibraryVersion( unsigned long nextLibraryVersion )
    {

		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsNextTrainLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs next train version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, nextLibraryVersion);
		}
    }

	void StatusMonitor::setNextSTISTrainLibraryVersion( unsigned long nextLibraryVersion )
    {

		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getStisNextTrainLibraryVersionDataPoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis next train version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, nextLibraryVersion);
		}
    }

	void StatusMonitor::setTrainLibrariesSynchronised( bool synchronised )
	{

		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getTrainLibrarySynchronisedDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find train library synchronised datapoint");
				return;
			}
			setDataPointBooleanValue(dataPoint, synchronised);
		}
	}

	void StatusMonitor::setCurrentISCSTrainLibraryVersion(unsigned long libraryVersion )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsCurrentTrainLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find iscs current train version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, libraryVersion);
		}
	}

	/*
	void StatusMonitor::setCurrentSTISTrainLibraryVersion( unsigned long libraryVersion )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getStisCurrentTrainLibraryVersionDataPoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find stis current train version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, libraryVersion);
		}
	}
	*/

	void StatusMonitor::setNextISCSTrainLibraryVersion_central( unsigned long nextVersionNumber_central )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsCdbNextTrainLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs next train version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, nextVersionNumber_central);
		}
	}

	void StatusMonitor::setCurrentISCSTrainLibraryVersion_central( unsigned long libraryVersion_central )
	{
		if( AgentModeMonitor::getInstance().isInControlMode() )
		{ 
			DataPoint* dataPoint = m_dataPointCahe.getIscsCdbCurrentTrainLibraryVersionDatapoint();
			if(NULL == dataPoint)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs current train version datapoint");
				return;
			}
			setDataPointIntValue(dataPoint, libraryVersion_central);
		}
	}

    unsigned long StatusMonitor::getNextISCSTrainLibraryVersion_central()
	{
		DataPoint* dataPoint = m_dataPointCahe.getIscsCdbNextTrainLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs current train version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}

	unsigned long StatusMonitor::getCurrentISCSTrainLibraryVersion_central()
	{
		DataPoint* dataPoint = m_dataPointCahe.getIscsCdbCurrentTrainLibraryVersionDatapoint();
		if(dataPoint == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can not find cdb iscs current train version datapoint");
			return 0;
		}
		return dataPoint->getCurrentValue().getFloat();
	}
}  // namespace TA_IRS_App
