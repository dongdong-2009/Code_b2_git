/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/TA_BASE/transactive/app/scada/PEnergyAgent/src/PEnergyManager.cpp $
  * @author:	Liu Yong
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The EnergyManager object is responsible for managing 102_Library working mode  
  * And processing data between 102_Library and data points
  *
  */

#include "app/scada/PEnergyAgent/src/PEnergyManager.h"
#include "app/scada/PEnergyAgent/src/PEnergyPollingWorker.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/scada/PEnergyAgent/src/PEnergyDpProcessor.h"

#include "core/data_access_interface/entity_access/src/PEnergyUnitEntityData.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/102_library/src/IEC102ConfigData.h"
#include "bus/scada/102_library/src/IEC102Library.h"


using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_App
{
    PEnergyManager::PEnergyManager()
        :
        m_agentAssetName(""),
        m_stationName(""),
        m_pEnergyDataPollingWorker(NULL),
		m_pMeasureDataPollingWorker(NULL),
		m_pCallLinkWorker(NULL),
		m_IEC102Library(NULL),
		m_prevChannelStatus(false)
    {
		FUNCTION_ENTRY("PEnergyManager::PEnergyManager");
		m_qualityStatus.clear();
		m_EnergyDataStartAddr.clear();
		m_EnergyDataEndAddr.clear();
		m_MeasureDataStartAddr.clear();
		m_MeasureDataEndAddr.clear();
		m_InputDataPoints.clear();

		m_IEC102Library =  new TA_Base_Bus::IEC102Library();

		if (NULL == m_IEC102Library)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Failed to initialize 102Interface!!");
		}
		m_pEnergyDpProcessor = new PEnergyDpProcessor(m_InputDataPoints);
		FUNCTION_EXIT;//("PEnergyManager::PEnergyManager");
   }
    

    PEnergyManager::~PEnergyManager()
    {
		FUNCTION_ENTRY("PEnergyManager::~PEnergyManager");
		if (NULL != m_IEC102Library)
		{
			m_IEC102Library->removeIEC102Observer(this);
			m_IEC102Library->stopProcess();	
			delete m_IEC102Library;
			m_IEC102Library = NULL;
		}
		if (m_pEnergyDataPollingWorker != NULL)
		{
			delete m_pEnergyDataPollingWorker;
			m_pEnergyDataPollingWorker = NULL;
		}
		if (m_pMeasureDataPollingWorker != NULL)
		{
			delete m_pMeasureDataPollingWorker;
			m_pMeasureDataPollingWorker = NULL;
		}
		if (m_pCallLinkWorker != NULL)
		{
			delete m_pCallLinkWorker;
			m_pCallLinkWorker = NULL;
		}		
		if (m_pEnergyDpProcessor != NULL)
		{
			delete m_pEnergyDpProcessor;
			m_pEnergyDpProcessor = NULL;
		}
		FUNCTION_EXIT;//("PEnergyManager::~PEnergyManager");
   }
        

    TA_Base_Bus::IEntity* PEnergyManager::createWorkers( TA_Base_Core::IEntityDataPtr EntityData )
    {
		FUNCTION_ENTRY("createWorkers");
       // cast the EntityData to PEnergyUnitEntityDataPtr
        TA_Base_Core::PEnergyUnitEntityDataPtr pEnergyEntityData = boost::dynamic_pointer_cast<TA_Base_Core::PEnergyUnitEntityData>(EntityData);
        
        TA_ASSERT( NULL != pEnergyEntityData.get(), "pEnergyEntityData is NULL in createPEnergyUnitEntity" );
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating PEnergyUnit entity for [%s]...", pEnergyEntityData->getName().c_str());
        
        if (NULL != m_IEC102Library)
		{
			setupConfiguration(pEnergyEntityData);
			//if (vDBStartTimes[iCnt].TypeName == 1)
			{
				m_pEnergyDataPollingWorker = new PEnergyPollingWorker(*this, *m_IEC102Library, 
											GET_POWER_HISTORY_120,
											m_EnergyDataPollInSec,
											5,
											m_EnergyDataFrequency,
											m_EnergyDataStartAddr,
											m_EnergyDataEndAddr);
			}

			//if (vDBStartTimes[iCnt].TypeName == 1)
			{
				m_pMeasureDataPollingWorker = new PEnergyPollingWorker(*this, *m_IEC102Library, 
											GET_POWER_TELEMETRY_172,
											m_MeasureDataPollInSec,
											9,
											m_MeasureDataFrequency,
											m_MeasureDataStartAddr,
											m_MeasureDataEndAddr);
			}

			{
				m_pCallLinkWorker = new PEnergyPollingWorker(*this, *m_IEC102Library, 
											CALL_LINK_STATUS,
											30,
											15,
											m_MeasureDataFrequency,
											m_MeasureDataStartAddr,
											m_MeasureDataEndAddr);
			}			
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "IEC102Library object is NULL");
		}

		FUNCTION_EXIT;//("createWorkers");

        return 0;
    }

    void PEnergyManager::setAgentAssetName(std::string& assetName)
    {
		FUNCTION_ENTRY("setAgentAssetName");
        m_agentAssetName = assetName;
		FUNCTION_EXIT;//("setAgentAssetName");
   }

    std::string& PEnergyManager::getAgentAssetName()
    {
		FUNCTION_ENTRY("getAgentAssetName");
		FUNCTION_EXIT;//("getAgentAssetName");
       return m_agentAssetName;
    }

    void PEnergyManager::setStationName(std::string& stationName)
    {
		FUNCTION_ENTRY("setStationName");
		m_stationName = stationName;
		FUNCTION_EXIT;//("setStationName");
    }

    std::string& PEnergyManager::getStationName()
    {
		FUNCTION_ENTRY("getStationName");
		FUNCTION_EXIT;//("getStationName");
       return m_stationName;
    }
    
    bool PEnergyManager::updatePEnergyUnitConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
		FUNCTION_ENTRY("updatePEnergyConfiguration - EMPTY routine!!");

		FUNCTION_EXIT;//("updatePEnergyConfiguration");
       // not PEnergyUnit configuration update
        return false;
    }

    bool PEnergyManager::isEnergyDpNameMatched( const std::string & dpName )
    {
        // find whether the data point name have PENERGY.PW
         std::string::size_type pos = dpName.find ( "PENERGY.PW", 0 );
        if ( pos != std::string::npos )
        {
        	return true;
        }
        else
        {
            return false;
        }
        
    }

    bool PEnergyManager::addDataPoint(TA_Base_Bus::DataPoint* dp)
    {
		FUNCTION_ENTRY("addDataPoint");
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;
        bool isAdded = false;

        //Add output data point to RTU to be processed by StationSystemOutputWirter
        if (dp->isWriteable())
        {
			//Don't do anything
        }
        else
        {
			m_InputDataPoints[ dp->getEntityKey() ] = dp;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Add DataPoint %s with address %d and entitykey %d to m_InputDataPoints",dp->getDataPointName().c_str(),dp->getDataPointRegisterAddress(),dp->getEntityKey());

			if (isEnergyDpNameMatched(dp->getDataPointName()))
			{
				m_InputEnergyDataPoints[dp->getDataPointRegisterAddress()] = dp;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Add EnergyDataPoint %s with address %d to m_InputEnergyDataPoints",dp->getDataPointName().c_str(),dp->getDataPointRegisterAddress());
				
			}
			else
			{
				m_InputMessureDataPoints[dp->getDataPointRegisterAddress()] = dp;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Add MeasureDataPoint %s with address %d to m_InputMeasureDataPoints",dp->getDataPointName().c_str(),dp->getDataPointRegisterAddress());
			}
			
			isAdded = true;
        }
		FUNCTION_EXIT;//("addDataPoint");
        return isAdded;
    }
        
    bool PEnergyManager::removeDataPoint(TA_Base_Bus::DataPoint* dp)
    {
		FUNCTION_ENTRY("removeDataPoint");
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

		PEnergyDataPointMap::iterator itr = m_InputDataPoints.find( dp->getEntityKey() );

		if ( itr != m_InputDataPoints.end() )
		{
			m_InputDataPoints.erase( itr );
			return true;
		}

		if (isEnergyDpNameMatched(dp->getDataPointName()))
		{
			PEnergyDataPointMap::iterator itr = m_InputEnergyDataPoints.find( dp->getDataPointRegisterAddress() );

			if ( itr != m_InputDataPoints.end() )
			{
				m_InputEnergyDataPoints.erase( itr );
				return true;
			}

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Remove EnergyDataPoint %s with address %d from m_InputEnergyDataPoints",dp->getDataPointName().c_str(),dp->getDataPointRegisterAddress());
			
		}
		else
		{
			PEnergyDataPointMap::iterator itr = m_InputMessureDataPoints.find( dp->getDataPointRegisterAddress() );

			if ( itr !=  m_InputMessureDataPoints.end() )
			{
				 m_InputMessureDataPoints.erase( itr );
				return true;
			}
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Remove MeasureDataPoint %s with address %d from m_InputMeasureDataPoints",dp->getDataPointName().c_str(),dp->getDataPointRegisterAddress());
		}

		FUNCTION_EXIT;//("removeDataPoint");
        return true;
    }
        
    void PEnergyManager::setToControl()
    {
		FUNCTION_ENTRY("setToControl");
		if (NULL != m_IEC102Library)
		{
			m_IEC102Library->registerIEC102Observer(this);
			m_IEC102Library->startProcess();
		}

		if (m_pEnergyDataPollingWorker != NULL)
		{
			m_pEnergyDataPollingWorker->setInServiceState(true);
		}

		if (m_pMeasureDataPollingWorker != NULL)
		{
			m_pMeasureDataPollingWorker->setInServiceState(true);
		}

		if (m_pCallLinkWorker != NULL)
		{
			m_pCallLinkWorker->setInServiceState(true);
		}

		FUNCTION_EXIT;//("setToControl");
   }

    void PEnergyManager::setToMonitor()
    {
		FUNCTION_ENTRY("setToMonitor");
		if (m_pEnergyDataPollingWorker != NULL)
		{
			m_pEnergyDataPollingWorker->setInServiceState(false);
		}
		if (m_pMeasureDataPollingWorker != NULL)
		{
			m_pMeasureDataPollingWorker->setInServiceState(false);
		}
		if (m_pCallLinkWorker != NULL)
		{
			m_pCallLinkWorker->setInServiceState(false);
		}
		
		if (NULL != m_IEC102Library)
		{
			m_IEC102Library->removeIEC102Observer(this);
			m_IEC102Library->stopProcess();	
		}

		FUNCTION_EXIT;//("setToMonitor");

    }

	void PEnergyManager::stopProcess()
	{
		FUNCTION_ENTRY("stopProcess");
		if (m_pEnergyDataPollingWorker != NULL)
		{
			m_pEnergyDataPollingWorker->setInServiceState(false);
		}
		if (m_pMeasureDataPollingWorker != NULL)
		{
			m_pMeasureDataPollingWorker->setInServiceState(false);
		}
		if (m_pCallLinkWorker != NULL)
		{
			m_pCallLinkWorker->setInServiceState(false);
		}

		FUNCTION_EXIT;//("stopProcess");
	}  

	void PEnergyManager::setupConfiguration(TA_Base_Core::IEntityDataPtr pEntityData)
	{
		FUNCTION_ENTRY("setupConfiguration");
		// dynamic cast the entity data to PMSAgentEntityData
		TA_Base_Core::PEnergyUnitEntityDataPtr pEnergyEntityData;
		pEnergyEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::PEnergyUnitEntityData>(pEntityData);

		// invalidate existing data so that new data is loaded from database
		pEnergyEntityData->invalidate();

		if (m_IEC102Library != NULL)
		{
			std::stringstream ss;
			ss << pEnergyEntityData->getPort();
			std::string strPort = ss.str();
			std::string emptySTR="";

			TA_Base_Bus::IEC102ConfigData obj(pEnergyEntityData->getAddress(),
						strPort,
						(unsigned short)pEnergyEntityData->getRemoteAddress(),
						(unsigned long)pEnergyEntityData->getIEC102MessageResponseTimeOutInMs(),
						pEnergyEntityData->getIEC102SendRetryTimes());

			m_IEC102Library->setupConfiguration(obj);

			m_EnergyDataPollInSec = pEnergyEntityData->getEnergyDataPollInSec();
			m_EnergyDataFrequency = pEnergyEntityData->getEnergyDataFrequency();
			m_MeasureDataPollInSec = pEnergyEntityData->getMeasureDataPollInSec();
			m_MeasureDataFrequency = pEnergyEntityData->getMeasureDataFrequency();
			m_MeasureResponseTimeOutInMs = pEnergyEntityData->getIEC102MessageResponseTimeOutInMs();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PEnergyUnit Config:"
			"m_EnergyDataPollInSec %d, m_EnergyDataFrequency %d, "
			"\nm_MeasureDataPollInSec %d, m_MeasureDataFrequency %d", m_EnergyDataPollInSec, m_EnergyDataFrequency, 
																m_MeasureDataPollInSec, m_MeasureDataFrequency);

			//EnergyDataStartAddr - EnergyDataEndAddr : EPV# 0 - 1
			//MeasureDataStartAddr - MeasureDataEndAddr: EPV# 2 - 3
			parseEPVList(pEnergyEntityData->getEnergyDataStartAddr(), 0);
			parseEPVList(pEnergyEntityData->getEnergyDataEndAddr(), 1);
			parseEPVList(pEnergyEntityData->getMeasureDataStartAddr(), 2);
			parseEPVList(pEnergyEntityData->getMeasureDataEndAddr(), 3);
	
			int iCnt = 0;

			for(iCnt = 0; iCnt < m_EnergyDataStartAddr.size(); iCnt++)
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "\nPEnergyUnit EnergyDataStartAddr: Item#%d=%d", iCnt, m_EnergyDataStartAddr[iCnt]);

			for(iCnt = 0; iCnt < m_EnergyDataEndAddr.size(); iCnt++)
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "\nPEnergyUnit EnergyDataEndAddr: Item#%d=%d", iCnt, m_EnergyDataEndAddr[iCnt]);

			for(iCnt = 0; iCnt < m_MeasureDataStartAddr.size(); iCnt++)
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "\nPEnergyUnit MeasureDataStartAddr: Item#%d=%d", iCnt, m_MeasureDataStartAddr[iCnt]);

			for(iCnt = 0; iCnt < m_MeasureDataEndAddr.size(); iCnt++)
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "\nPEnergyUnit MeasureDataEndAddr: Item#%d=%d", iCnt, m_MeasureDataEndAddr[iCnt]);
		
			if(m_EnergyDataStartAddr.size() != m_EnergyDataEndAddr.size())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "\n EnergyDataStartAddr & EnergyDataEndAddr Mismatch! Check your Database configuration.");
			}
			if(m_MeasureDataStartAddr.size() != m_MeasureDataEndAddr.size())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "\n MeasureDataStartAddr & MeasureDataEndAddr Mismatch! Check your Database configuration.");
			}

		}
		FUNCTION_EXIT;//("setupConfiguration");
	}

	bool PEnergyManager::parseReceivedDataFrame(TA_Base_Bus::FrameEnergyData& objFrame, IEC102CommandType command)
	{
		//submit DP event of type UpdateFiledValue
		FUNCTION_ENTRY("parseReceivedDataFrame");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "command:%d has processed status as %d.", command, objFrame.bSuccess);
		if (objFrame.bSuccess)
        {
        	if (GET_POWER_HISTORY_120 == command)
        	{
	            for ( std::vector<TA_Base_Bus::PowerEnergyData>::iterator it = objFrame.PowerData.begin();
	                  it != objFrame.PowerData.end();
	                  it++)
	            {
					// set up DataPointState with the specified timestamp, quality and value
					PEnergyDataPointMap::iterator itr = m_InputEnergyDataPoints.find( it->address );
					TA_Base_Bus::DataPoint * dp;
					TA_Base_Bus::EQualityStatus quality;
					if (it->valid)
					{
						quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
					}
					else
					{
						quality = TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON;
					}
					
					if (itr != m_InputEnergyDataPoints.end() )
					{
						dp = itr->second;
						TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
						dpValue.setFloat( it->PowerResult );
						boost::shared_ptr<TA_Base_Bus::DataPointState> newState(new TA_Base_Bus::DataPointState( dpValue, it->dataTime, quality));
						m_pEnergyDpProcessor->submitUpdateDataPoint(dp,  newState);
					}
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DP with address %u does not exist", it->address);
					}
				}
        	}
			else if (GET_POWER_TELEMETRY_172 == command)
			{
	            for ( std::vector<TA_Base_Bus::PowerEnergyData>::iterator it = objFrame.PowerData.begin();
	                  it != objFrame.PowerData.end();
	                  it++)
	            {
					// set up DataPointState with the specified timestamp, quality and value
					PEnergyDataPointMap::iterator itr = m_InputMessureDataPoints.find( it->address );
					TA_Base_Bus::DataPoint * dp;
					TA_Base_Bus::EQualityStatus quality;
					if (it->valid)
					{
						quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
					}
					else
					{
						quality = TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON;
					}
					
					if (itr != m_InputMessureDataPoints.end() )
					{
						dp = itr->second;
						TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
						dpValue.setFloat( it->PowerResult );
						boost::shared_ptr<TA_Base_Bus::DataPointState> newState(new TA_Base_Bus::DataPointState( dpValue, it->dataTime, quality));
						m_pEnergyDpProcessor->submitUpdateDataPoint(dp,  newState);
					}
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "DP with address %u does not exist", it->address);
					}
				}			
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Unknown Command Type %d", command);
				FUNCTION_EXIT;
				return false;
			}
			//TA_Base_Core::IEC102PollingConfigAccessFactory::getInstance().setStartTime(saveEndTime, pKey);
			return true;
        }
		FUNCTION_EXIT;
		return false;
	}

	//For observer to get Channel connection status from IEC102 channel
	void PEnergyManager::updateChannelStatus(const bool& status)
	{
		FUNCTION_ENTRY("updateChannelStatus");
		//submit DP event of type UpdateAllQualityToBad
		if( status != m_prevChannelStatus)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ChannelStatus = BAD");
			m_pEnergyDpProcessor->submitUpdateAllQualityToBad();
			m_prevChannelStatus = status;
		}
		FUNCTION_EXIT;
	}

	//paramNumber = 0, for EnergyDataStartAddr
	//paramNumber = 1, for EnergyDataEndAddr
	//paramNumber = 2, for MeasureDataStartAddr
	//paramNumber = 3, for MeasureDataEndAddr
	void PEnergyManager::parseEPVList(std::string strList, int paramNumber)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Current List %s", strList.c_str());
		int pos = 0, index = 0;
		int value = 0;
		std::string valueSTR = "";

		while("" != strList )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Current strList %s", strList.c_str());
			pos = strList.find ( ",", 0 );
			if ( 0 < pos )
			{
				valueSTR = (strList.substr( 0, pos )).c_str();
				value = atoi(valueSTR.c_str());
				if( value >= 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Member added in strList %s", valueSTR.c_str());
					if(paramNumber == 0)
					{
						m_EnergyDataStartAddr.push_back(value);
					}
					else if(paramNumber == 1)
					{
						m_EnergyDataEndAddr.push_back(value);
					}
					else if(paramNumber == 2)
					{
						m_MeasureDataStartAddr.push_back(value);
					}
					else if(paramNumber == 3)
					{
						m_MeasureDataEndAddr.push_back(value);
					}
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Value configured is less than 0 so default = 0, strList %s", strList.c_str());
				}
				strList = (strList.substr ( pos + 1, strList.length() )).c_str();
			}
			else if ( strList != "")
			{
				valueSTR = strList.c_str();
				value = atoi(valueSTR.c_str());
				if( value >= 0)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Member added in List %s", valueSTR.c_str());
					if(paramNumber == 0)
					{
						m_EnergyDataStartAddr.push_back(value);
					}
					else if(paramNumber == 1)
					{
						m_EnergyDataEndAddr.push_back(value);
					}
					else if(paramNumber == 2)
					{
						m_MeasureDataStartAddr.push_back(value);
					}
					else if(paramNumber == 3)
					{
						m_MeasureDataEndAddr.push_back(value);
					}	
					break;
				}
				else 
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Value configured is less than 0 so default = 0, strList %s", strList.c_str());
				}
				strList = "";
			}
		} //while end

		return ;
	
	}

	// this funciont for get start time based on their rad value.
	time_t PEnergyManager::calcStartTime(const time_t& endTime, const int& frequency)
	{
		time_t startTime = endTime - 3600; // defalut value, just preparing for default in switch
		switch (frequency) 
		{
		case 0x00:
		case 0x0E:
		case 0x0B:
			startTime = endTime - 300; // 5 min, because 0x0E represents 1 min to poll, may it's too frequently to server, so process it equals to 5min
			break;
		case 0x0C:
			startTime = endTime - 900; // 15 min
			break;
		case 0x0D:
			startTime = endTime - 1800; // 30 min
			break;
		case 0x0F:
			startTime = endTime - 3600; // 60 min
			break;
		default:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid RAD");
			break;
		}
		
		return startTime;
	}

}
