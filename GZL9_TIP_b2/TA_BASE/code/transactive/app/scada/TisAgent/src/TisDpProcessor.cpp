/**
*
* Source:   $File: //depot/GZL9_TIP/Base/transactive/app/scada/TrainAlarmAgent/src/TisDpProcessor.h $
* @author:  Wang Wang
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2017/05/18 15:30:00 $
* Last modified by:  $Author: wang.wang $
*
*
* TisDpProcessor is inheriting from ModbusDpProcessor (it's in rtu_lib in bus layer).
* its major function is implementing some special data point processing of TisAgent,
* but other nomal working still flow ModbusDpProcessor which is in rtu_lib laryer.
*
*/

#include "TisDpProcessor.h"
#include "TrainInfoManager.h"

namespace TA_IRS_App
{
    TisDpProcessor::TisDpProcessor( const std::string & name, 
									unsigned long 		pollingStartAddress,
									unsigned long 		pollingEndAddress,
									unsigned long 		offsetAddress,
									bool 		  		reverseDiBitOrder, 
									int 				analogueDpUpdateInterval)
	:
	CommonDpProcessor(name, pollingStartAddress, pollingEndAddress, offsetAddress, reverseDiBitOrder, analogueDpUpdateInterval),        
	m_pollingStartAddr(pollingStartAddress),
	m_pollingEndAddr(pollingEndAddress)
    {
        FUNCTION_ENTRY("TisDpProcessor::TisDpProcessor");
        FUNCTION_EXIT;
    }

    TisDpProcessor::~TisDpProcessor()
    {
        m_specialDps.clear();
    }

    unsigned long TisDpProcessor::getPollingStartAddr()
    {
        FUNCTION_ENTRY("TisDpProcessor::getPollingStartAddr");
        FUNCTION_EXIT;
        return m_pollingStartAddr;
    }

    unsigned long TisDpProcessor::getPollingEndAddr()
    {
        FUNCTION_ENTRY("TisDpProcessor::getPollingEndAddr");
        FUNCTION_EXIT;
        return m_pollingEndAddr;
    }

    
    bool TisDpProcessor::addDataPoint(TA_Base_Bus::DataPoint * dp)
    {
        FUNCTION_ENTRY("TisDpProcessor::addDataPoint");
        TA_ASSERT(NULL != dp, "dp is NULL in RTUModbusTable::addDataPoint");
        
        TrainData * train = TrainInfoManager::getInstance().getTrain(m_pollingStartAddr);
        TA_ASSERT(NULL != train, "train is NULL in TisDpProcessor::addDataPoint");
        
        if (train->isTrainDp(dp))
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "data point %d belongs to train special data point, so adding is ignored.", dp->getEntityKey());
            m_specialDps.push_back(dp);
            FUNCTION_EXIT;
            return false;
        }
        
        if (TrainInfoManager::getInstance().isTrainDp(dp))
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "data point %d belongs to other train special data point, so adding is ignored.", dp->getEntityKey());
            FUNCTION_EXIT;
            return false;
        }

        // if dp is not special data point, still use parent funciton to do it
        FUNCTION_EXIT;
        return CommonDpProcessor::addDataPoint(dp);
    }


    // updata all special data points without condition
    int TisDpProcessor::populateSpecialDataPoints()
    {    
        FUNCTION_ENTRY("TisDpProcessor::populateSpecialDataPoints");
        // return TrainInfoManager::getInstance().updateTrains(m_dataBlock, NULL);
        int updateCount = TrainInfoManager::getInstance().updateTrain(m_pollingStartAddr, m_dataBlock, NULL);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "all update %d train special data point", updateCount);
        FUNCTION_EXIT;
        return updateCount;
    }


    // updata special data points used to compare previous dataBlock and new dataBlock
    int TisDpProcessor::processSpecialDataPoint()
    {
        FUNCTION_ENTRY("TisDpProcessor::processSpecialDataPoint");
        // return TrainInfoManager::getInstance().updateTrains(m_dataBlock, m_prevDataBlock);
        int updateCount = TrainInfoManager::getInstance().updateTrain(m_pollingStartAddr, m_dataBlock, m_prevDataBlock);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "update %d train special data point", updateCount);
        FUNCTION_EXIT;
        return updateCount;
    }

} // namespace TA_IRS_App
