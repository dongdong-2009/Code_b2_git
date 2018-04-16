/**
*
* Source:   $File: //depot/GZL9_TIP/Base/transactive/app/scada/TisAgent/src/CsrzzTrainData.h $
* @author:  Wang Wang
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2017/04/06 15:30:00 $
* Last modified by:  $Author: wang.wang $
*
*
* CsrzzTrainData is implmentation of TrainData.
* It's used to update SF train type data point.
* 
*/

#pragma once

#ifndef TISAGENT_CSRZZ_TRAINDATA_H_INCLUDED
#define TISAGENT_CSRZZ_TRAINDATA_H_INCLUDED

#include "TrainData.h"

namespace TA_IRS_App
{
	//CsrzzTrainData is only for the train information of the train from CSR ZhuZhou Electric Locomotive Co Ltd
    class CsrzzTrainData : public virtual TrainData
    {
    public: // data struct
        enum CSRZZ_TRAIN_DATAPOINT_TYPE
        {
            UNKNOWN_TRAIN_DATAPOINT_TYPE,	// Unknown Train DataPoint Type
            CSRZZ_CODE, 					// "CSRZZ:train_number:CODE" will be diveided into this type.
            CSRZZ_HB,   					// "CSRZZ:train_number:HB" ...
            CSRZZ_TIME						// "CSRZZ:train_number:TIME" ...
        };

    public: // method
        // constructor
        CsrzzTrainData(std::string type, unsigned long num, unsigned long addr, unsigned long len);

        // destructor
        virtual ~CsrzzTrainData();

        // find data point meets this train, and add data point to vector
        virtual void addTrainDps(std::vector<TA_Base_Bus::DataPoint*> & dps);

        virtual bool isTrainDp(TA_Base_Bus::DataPoint * dp);

    private: // method
        // get data point type based on string format
        CSRZZ_TRAIN_DATAPOINT_TYPE getDpType(std::string dpAddr, std::string token);
        
        // update train data point, implement TrainData interface        
        virtual int updateTrainDps(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);

        // update data point based on data point type, these methods represent updaing without condition
        int updateCodeType(const TA_Base_Bus::ModbusDataBlock * dataBlock);
        int updateHbType(const TA_Base_Bus::ModbusDataBlock * dataBlock);
        int updateTimeType(const TA_Base_Bus::ModbusDataBlock * dataBlock);

        // update data point based on data point type, these methods represent updaing with comparison of data block, 
        // if value or qulity has been changed, then update value, else do nothing
        int updateCodeType(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);
        int updateHbType(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);
        int updateTimeType(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);

        // get value from data block for each data point type
        unsigned int getValueOfCode(const TA_Base_Bus::ModbusDataBlock * dataBlock);
        unsigned int getValueOfHb(const TA_Base_Bus::ModbusDataBlock * dataBlock);
        std::string getValueOfTime(const TA_Base_Bus::ModbusDataBlock * dataBlock);
        
    private: // member
        TA_Base_Core::ReEntrantThreadLockable m_dpsVecLock; // data point vector protection
        std::map<CSRZZ_TRAIN_DATAPOINT_TYPE, TA_Base_Bus::DataPoint*> m_specialDps;
        
        static const std::string CSRZZ_CODE_TOKEN;
        static const std::string CSRZZ_HB_TOKEN;
        static const std::string CSRZZ_TIME_TOKEN;

        // offset is data point address in one train data,
        // start bit is start postion of calculate data point value in WORD (16 bits),
        // len is one train full data length, how may WORD for this train data.
        const static unsigned int CSRZZ_CODE_OFFSET = 0, CSRZZ_CODE_START_BIT = 0, CSRZZ_CODE_LEN = 8;
        const static unsigned int CSRZZ_HB_OFFSET = 0, CSRZZ_HB_START_BIT = 8, CSRZZ_HB_LEN = 8;
        const static unsigned int CSRZZ_TIME_OFFSET = 2, CSRZZ_TRAINID_START_BIT = 0, CSRZZ_TIME_LEN = 48;
    }; // class CsrzzTrainData

} // namespace TA_IRS_App


#endif // !TISAGENT_CSRZZ_TRAINDATA_H_INCLUDED
