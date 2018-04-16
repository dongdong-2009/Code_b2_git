/**
*
* Source:   $File: //depot/GZL9_TIP/Base/transactive/app/scada/TisAgent/src/TrainData.h $
* @author:  Wang Wang
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2017/04/06 15:30:00 $
* Last modified by:  $Author: wang.wang $
*
*
* TrainData is a interface, and detailed train class should inherit from here.
* It's designed for update train data point based on dataBlock passed,
* but caller no needs to care what train class called.
*
*/

#pragma once

#ifndef TISAGENT_TRAINDATA_H_INCLUDED
#define TISAGENT_TRAINDATA_H_INCLUDED

#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include <string>
#include <vector>

namespace TA_IRS_App
{
    class TrainData
    {
    public: // method
        // constructor
        TrainData(std::string type, unsigned long num, unsigned long addr, unsigned long len);

        // destructor
        virtual ~TrainData();

        // create train object interface
        virtual void addTrainDps(std::vector<TA_Base_Bus::DataPoint*> & dps) = 0;

        // update train data point entry, used to common update, e.g. quality
        virtual int updateTrain(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);

        virtual std::string getTrainType() const;
        virtual unsigned long getTrainNum() const;
        virtual unsigned long getTrainAddr() const;
        virtual unsigned long getTrainInfoLen() const;
        virtual unsigned long getTrainEndAddr() const;        

        virtual bool isTrainDp(TA_Base_Bus::DataPoint * dp) = 0;
        
    private: // method        
        // update data points based on comparison of data block
        virtual int updateTrainDps(const TA_Base_Bus::ModbusDataBlock * pre_dataBlock, const TA_Base_Bus::ModbusDataBlock * new_dataBlock) = 0;

    protected: // member
        static const unsigned long WORD_LEN = 16; // dataBlock<WORD>, one WORD = 16 bits
        std::string m_trainType;
        unsigned long m_trainNumber;
        unsigned long m_trainAddr;
        unsigned long m_trainInfoLen;        
        TA_Base_Core::ReEntrantThreadLockable m_infoLock; // updating protection

    }; // class TrainData

} // namespace TA_IRS_App

#endif // !TISAGENT_TRAINDATA_H_INCLUDED
