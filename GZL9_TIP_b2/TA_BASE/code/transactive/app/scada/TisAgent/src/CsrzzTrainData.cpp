#include "CsrzzTrainData.h"

#include "core/synchronisation/src/ThreadGuard.h"

#include <bitset>

namespace TA_IRS_App
{
    const std::string CsrzzTrainData::CSRZZ_CODE_TOKEN 	= "CODE";
    const std::string CsrzzTrainData::CSRZZ_HB_TOKEN 	= "HB";
    const std::string CsrzzTrainData::CSRZZ_TIME_TOKEN 	= "TIME";

    CsrzzTrainData::CsrzzTrainData(std::string type, unsigned long num, unsigned long addr, unsigned long len)
    :
    TrainData(type, num, addr, len)
    {
        FUNCTION_ENTRY("CsrzzTrainData::CsrzzTrainData");
        FUNCTION_EXIT;
    }

    CsrzzTrainData::~CsrzzTrainData()
    {        
        m_specialDps.clear();
    }

    // find data point meets this train, and add data point to vector
    void CsrzzTrainData::addTrainDps(std::vector<TA_Base_Bus::DataPoint*> & dps)
    {
        FUNCTION_ENTRY("CsrzzTrainData::addTrainDps");
        TA_Base_Core::ThreadGuard gurad(m_dpsVecLock);
        for (unsigned int i = 0; i < dps.size(); i++)
        {
            std::stringstream ss;
            ss << m_trainType << ":" << m_trainNumber << ":"; // construct to "train_type:train_number:" format
            CSRZZ_TRAIN_DATAPOINT_TYPE dpType = getDpType(dps[i]->getDataPointAddress(), ss.str());
            if (dpType != UNKNOWN_TRAIN_DATAPOINT_TYPE)
            {                
                m_specialDps[dpType] = dps[i];
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add data point address %s associated with train type %s, train number %d.",
                    dps[i]->getDataPointAddress().c_str(), 
					m_trainType.c_str(), 
					m_trainNumber);
            }
        }

        FUNCTION_EXIT
    }

    bool CsrzzTrainData::isTrainDp(TA_Base_Bus::DataPoint * dp)
    {
        FUNCTION_ENTRY("CsrzzTrainData::isTrainDp");
        std::map<CSRZZ_TRAIN_DATAPOINT_TYPE, TA_Base_Bus::DataPoint*>::iterator it;
        for (it = m_specialDps.begin(); it != m_specialDps.end(); it++)
        {
            if (dp->getEntityKey() == it->second->getEntityKey())
			{
                return true;
			}
        }
        
        FUNCTION_EXIT
        return false;
    }

    CsrzzTrainData::CSRZZ_TRAIN_DATAPOINT_TYPE CsrzzTrainData::getDpType(std::string dpAddr, std::string token)
    {   
        FUNCTION_ENTRY("CsrzzTrainData::getDpType");
        
		// match "train_type:train_number:CODE"
		if (token + CSRZZ_CODE_TOKEN == dpAddr) 
        {
            FUNCTION_EXIT;
            return CSRZZ_CODE;
        }

		// match "train_type:train_number:HB"
        if (token + CSRZZ_HB_TOKEN == dpAddr) 
        {
            FUNCTION_EXIT;
            return CSRZZ_HB;
        }

		// match "train_type:train_number:TIME"
        if (token + CSRZZ_TIME_TOKEN == dpAddr) 
        {
            FUNCTION_EXIT;
            return CSRZZ_TIME;
        }

        FUNCTION_EXIT;
        return UNKNOWN_TRAIN_DATAPOINT_TYPE;
    }

    // update train data point, implement TrainData interface
    int CsrzzTrainData::updateTrainDps(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock)
    {
        FUNCTION_ENTRY("CsrzzTrainData::updateTrainDps");
        TA_Base_Core::ThreadGuard gurad(m_dpsVecLock);
        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "update data point for train type %s, train number %d", 
            m_trainType.c_str(), m_trainNumber);

        // update train data point based on different data point type
        int updateCount = 0;
        for (std::map<CSRZZ_TRAIN_DATAPOINT_TYPE, TA_Base_Bus::DataPoint*>::iterator it = m_specialDps.begin(); it != m_specialDps.end(); it++)
        {
            switch (it->first)
            {
            case CSRZZ_CODE:
                updateCount += (pre_dataBlock == NULL) ? updateCodeType(dataBlock) : updateCodeType(dataBlock, pre_dataBlock);
                break;
            case CSRZZ_HB:
                updateCount += (pre_dataBlock == NULL) ? updateHbType(dataBlock) : updateHbType(dataBlock, pre_dataBlock);
                break;
            case CSRZZ_TIME:
                updateCount += (pre_dataBlock == NULL) ? updateTimeType(dataBlock) : updateTimeType(dataBlock, pre_dataBlock);
                break;
            default:
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Get unhandle special data point type %d in CSRZZ train", it->first );
                break;
            }
        }
        
        FUNCTION_EXIT;
        return updateCount;
    }

    
    // update CODE data point type
    int CsrzzTrainData::updateCodeType(const TA_Base_Bus::ModbusDataBlock * dataBlock)
    {   
        FUNCTION_ENTRY("CsrzzTrainData::updateCodeType");
        unsigned int newCode = getValueOfCode(dataBlock);
        TA_Base_Bus::DataPoint * dp = m_specialDps[CSRZZ_CODE];
        TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
        dpValue.setFloat(newCode, dp->getPrecision());
        TA_Base_Bus::DataPointState newState(dpValue, dataBlock->timestamp, dataBlock->quality);
        dp->updateFieldState(newState);
        FUNCTION_EXIT;
        return 1;
    }

    int CsrzzTrainData::updateCodeType(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock)
    {
        FUNCTION_ENTRY("CsrzzTrainData::updateCodeType");
        TA_ASSERT(NULL != pre_dataBlock, "previous data block pointer can't be null.");
        TA_Base_Bus::DataPoint * dp = m_specialDps[CSRZZ_CODE];
        unsigned int newCode = getValueOfCode(dataBlock);
        unsigned int preCode = getValueOfCode(pre_dataBlock);
        if (dataBlock->quality != pre_dataBlock->quality || newCode != preCode)
        {
            TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
            dpValue.setFloat(newCode, dp->getPrecision());
            TA_Base_Bus::DataPointState newState(dpValue, dataBlock->timestamp, dataBlock->quality);
            dp->updateFieldState(newState);
            FUNCTION_EXIT;
            return 1;
        }

        FUNCTION_EXIT;
        return 0;
    }

    // update HB data point type
    int CsrzzTrainData::updateHbType(const TA_Base_Bus::ModbusDataBlock * dataBlock)
    {   
        FUNCTION_ENTRY("CsrzzTrainData::updateHbType");
        TA_Base_Bus::DataPoint * dp = m_specialDps[CSRZZ_HB];
        unsigned int value = getValueOfHb(dataBlock);

        TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
        dpValue.setFloat(value, dp->getPrecision());
        TA_Base_Bus::DataPointState newState(dpValue, dataBlock->timestamp, dataBlock->quality);
        dp->updateFieldState(newState);
        FUNCTION_EXIT;
        return 1;
    }

    int CsrzzTrainData::updateHbType(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock)
    {
        FUNCTION_ENTRY("CsrzzTrainData::updateHbType");
        TA_ASSERT(NULL != pre_dataBlock, "previous data block pointer can't be null.");
        TA_Base_Bus::DataPoint * dp = m_specialDps[CSRZZ_HB];
        unsigned int newHb = getValueOfHb(dataBlock);
        unsigned int preHb = getValueOfHb(pre_dataBlock);
        if (dataBlock->quality != pre_dataBlock->quality || newHb != preHb)
        {
            TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
            dpValue.setFloat(newHb, dp->getPrecision());
            TA_Base_Bus::DataPointState newState(dpValue, dataBlock->timestamp, dataBlock->quality);
            dp->updateFieldState(newState);
            return 1;
        }

        FUNCTION_EXIT;
        return 0;
    }

    // update TIME data point type
    int CsrzzTrainData::updateTimeType(const TA_Base_Bus::ModbusDataBlock * dataBlock)
    {   
        FUNCTION_ENTRY("CsrzzTrainData::updateTimeType");
        TA_Base_Bus::DataPoint * dp = m_specialDps[CSRZZ_TIME];
        std::string s = getValueOfTime(dataBlock);
        TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());                
        dpValue.setText(s);
        TA_Base_Bus::DataPointState newState(dpValue, dataBlock->timestamp, dataBlock->quality);
        dp->updateFieldState(newState);
        FUNCTION_EXIT;
        return 1;
    }

    int CsrzzTrainData::updateTimeType(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock)
    {
        FUNCTION_ENTRY("CsrzzTrainData::updateTimeType");
        TA_Base_Bus::DataPoint * dp = m_specialDps[CSRZZ_TIME];
        std::string newTime = getValueOfTime(dataBlock);
        std::string preTime = getValueOfTime(pre_dataBlock);
        if (dataBlock->quality != pre_dataBlock->quality || newTime != preTime)
        {
            TA_Base_Bus::DpValue dpValue(dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels());
            dpValue.setText(newTime);
            TA_Base_Bus::DataPointState newState(dpValue, dataBlock->timestamp, dataBlock->quality);
            dp->updateFieldState(newState);
            FUNCTION_EXIT;
            return 1;
        }

        FUNCTION_EXIT;
        return 0;
    }

    // get TRAINID data point type
    unsigned int CsrzzTrainData::getValueOfCode(const TA_Base_Bus::ModbusDataBlock * dataBlock)
    {   
        FUNCTION_ENTRY("CsrzzTrainData::getValueOfCode");
        
        unsigned long offsetAddr = m_trainAddr + CSRZZ_CODE_OFFSET;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz CODE offset is %d", offsetAddr);

        unsigned int value = dataBlock->data[offsetAddr];
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz CODE orgial valuse %d, bit %s",
            value, std::bitset<WORD_LEN>(value).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        // here bit postion is from high to low
        value = value & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz CODE processed valuse %d, bit %s",
            value, std::bitset<CSRZZ_CODE_LEN>(value).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        FUNCTION_EXIT;
        return value;
    }

    unsigned int CsrzzTrainData::getValueOfHb(const TA_Base_Bus::ModbusDataBlock * dataBlock)
    {   
        FUNCTION_ENTRY("CsrzzTrainData::getValueOfHb");
        
        unsigned long offsetAddr = m_trainAddr + CSRZZ_HB_OFFSET;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz HB offset is %d", offsetAddr);

        unsigned int value = dataBlock->data[offsetAddr];
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz HB orgial valuse %d, bit %s",
            value, std::bitset<WORD_LEN>(value).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        // here bit postion is from high to low
        value = value >> 8;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz HB processed valuse %d, bit %s",
            value, std::bitset<CSRZZ_HB_LEN>(value).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());
        
        FUNCTION_EXIT;
        return value;
    }

    std::string CsrzzTrainData::getValueOfTime(const TA_Base_Bus::ModbusDataBlock * dataBlock)
    {   
        FUNCTION_ENTRY("CsrzzTrainData::getValueOfTime");
        unsigned long offsetAddr = m_trainAddr + CSRZZ_TIME_OFFSET;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME offset is %d", offsetAddr);

        unsigned int year = dataBlock->data[offsetAddr];
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME year origianl value %d, bit %s",
            year, std::bitset<WORD_LEN>(year).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());
        year = year & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME year processed value %d, bit %s ",
            year, std::bitset<8>(year).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        unsigned int month = dataBlock->data[offsetAddr];
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME month origianl value %d, bit %s",
            month, std::bitset<WORD_LEN>(month).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());
        month = (month >> 8) & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME month processed value %d, bit %s ",
            month, std::bitset<8>(month).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        unsigned int day = dataBlock->data[offsetAddr + 1];
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME day origianl value %d, bit %s",
            day, std::bitset<WORD_LEN>(day).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());
        day = day & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME day processed value %d, bit %s",
            day, std::bitset<8>(day).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        unsigned int hour = dataBlock->data[offsetAddr + 1];
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME hour origianl value %d, bit %s",
            hour, std::bitset<WORD_LEN>(hour).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());
        hour = (hour >> 8) & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME hour processed value %d, bit %s",
            hour, std::bitset<8>(hour).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        unsigned int minutes = dataBlock->data[offsetAddr + 2];
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME minutes origianl value %d, bit %s",
            minutes, std::bitset<WORD_LEN>(minutes).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());
        minutes = minutes & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME hour processed value %d, bit %s",
            minutes, std::bitset<8>(minutes).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        unsigned int seconds = (dataBlock->data[offsetAddr + 2] >> 8) & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME minutes origianl value %d, bit %s",
            seconds, std::bitset<WORD_LEN>(seconds).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());
        seconds = (seconds >> 8) & 0xFF;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Csrzz TIME hour processed value %d, bit %s",
            seconds, std::bitset<8>(seconds).to_string<char, std::string::traits_type, std::string::allocator_type>().c_str());

        std::stringstream ss;
        ss << year << "-" << month << "-" << day << " "
            << hour << ":" << minutes << ":" << seconds;

        FUNCTION_EXIT;
        return ss.str();
    }
} // namespace TA_IRS_App
