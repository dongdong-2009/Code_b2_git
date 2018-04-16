#include "TrainData.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

namespace TA_IRS_App
{
    TrainData::TrainData(std::string type, unsigned long num, unsigned long addr, unsigned long len)
        :
        m_trainType(type),
        m_trainNumber(num),
        m_trainAddr(addr),
        m_trainInfoLen(len)
        // m_trainQuality(TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON)
    {
        FUNCTION_ENTRY("TrainData::TrainData");
        FUNCTION_EXIT;
    }
    
    TrainData::~TrainData()
    {
        
    }

    // update train data point entry, used to common update, e.g. quality
    int TrainData::updateTrain(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock)
    {
        FUNCTION_ENTRY("TrainData::updateTrain");
        TA_Base_Core::ThreadGuard guard(m_infoLock);

        int updateCount = updateTrainDps(dataBlock, pre_dataBlock);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "updateTrainDps %d", updateCount);
        FUNCTION_EXIT;
        return updateCount;
    }

    std::string TrainData::getTrainType() const
    {
        FUNCTION_ENTRY("TrainData::getTrainType");
        FUNCTION_EXIT;
        return m_trainType;        
    }

    unsigned long TrainData::getTrainNum() const
    {
        FUNCTION_ENTRY("TrainData::getTrainNum");
        FUNCTION_EXIT;
        return m_trainNumber;
    }

    unsigned long TrainData::getTrainAddr() const
    {   
        FUNCTION_ENTRY("TrainData::getTrainAddr");
        FUNCTION_EXIT;
        return m_trainAddr;
    }

    unsigned long TrainData::getTrainInfoLen() const
    {   
        FUNCTION_ENTRY("TrainData::getTrainInfoLen");
        FUNCTION_EXIT;
        return m_trainInfoLen;
    }

    unsigned long TrainData::getTrainEndAddr() const
    {
        FUNCTION_ENTRY("TrainData::getTrainEndAddr");
        if (0 == m_trainAddr + m_trainInfoLen)
        {
            return 0;
        }

        FUNCTION_EXIT;
        return m_trainAddr + m_trainInfoLen - 1;        
    }

} // namespace TA_IRS_App
