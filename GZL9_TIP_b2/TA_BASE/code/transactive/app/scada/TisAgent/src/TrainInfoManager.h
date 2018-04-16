/**
*
* Source:   $File: //depot/GZL9_TIP/Base/transactive/app/scada/TisAgent/src/TrainInfoManager.h $
* @author:  Wang Wang
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2017/04/06 15:30:00 $
* Last modified by:  $Author: wang.wang $
*
*
* TrainInfoManager is a singleton class.
* It's used to manage TrainData object, includes create TrainData object based on database configuratoin,
* update train data point, get train object info etc.
*
*/

#pragma once

#ifndef TISAGENT_TRAININFOMANAGER_H_INCLUDED
#define TISAGENT_TRAININFOMANAGER_H_INCLUDED

#include "TrainData.h"

#include "bus/scada/rtu_library/src/CommonDpProcessor.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/data_access_interface/entity_access/src/TisTrainAlarmAgentEntityData.h"

#include <map>
#include <vector>

namespace TA_IRS_App
{    
    class TrainInfoManager
    {
    public: // method
        // singleton entry
        static TrainInfoManager& getInstance();

        // singleton destroyer
        static void removeInstance();
        
        // create train object based on database configuration
        void addTrains(TA_Base_Core::TisTrainAlarmAgentEntityDataPtr tisTrainAlarmAgentEntityData, std::map< unsigned long, TA_Base_Bus::DataPoint * > & dataPoints);

        // update all train data point entry
        int updateTrains(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);        

        // int updateTrain(const TrainData* train, const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);
        int updateTrain(unsigned long addr, const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock);

        // return trains
        std::vector<const TrainData *> getTrains();

        // return trains count
        unsigned long getTrainsCount();

        TrainData * getTrain(unsigned long startAddr);

        bool isTrainDp(TA_Base_Bus::DataPoint * dp);
    
    private: // method
        TrainInfoManager() { };
        ~TrainInfoManager();
        TrainInfoManager(const TrainInfoManager &); 			// disable copy
        TrainInfoManager & operator=(const TrainInfoManager &); // disable assignment        

        // internal create train object
        void constructTrains(std::vector<std::string>& trainTypes,
            std::vector<std::pair<std::string, unsigned long> >& startAddressOfTrains,
            std::vector<std::pair<std::string, unsigned long> >& trainNumbers,
            std::vector<std::pair<std::string, unsigned long> >& trainInfoLens);

        // internal create train data point for each train object.
        void setDpForTrains(std::map<unsigned long, TA_Base_Bus::DataPoint*> & dataPoints);

        // singleton lock for getInstance(), copy from DebugUtil
        static TA_Base_Core::ReEntrantThreadLockable& getInstanceLock();

    private: // member
        static TrainInfoManager* m_instance;

        // locking for access m_trains
        TA_Base_Core::ReEntrantThreadLockable m_trainLock;
        std::vector<TA_IRS_App::TrainData*> m_trains;

    private: // member
        // manager will create detailed train object based on these token
        // const static std::string SF_TOKEN;
        // const static std::string SM_TOKEN;
        const static std::string CSRZZ_TOKEN;
        
    }; // class TrainInfoManager

} // namespace TA_IRS_App

#endif // !TISAGENT_TRAININFOMANAGER_H_INCLUDED
