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

#include "TrainInfoManager.h"
#include "CsrzzTrainData.h"

#include "bus/scada/rtu_library/src/ModbusDpProcessor.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/exceptions/src/GenericAgentException.h"

namespace TA_IRS_App
{
    // manager will create detailed train object based on these token
    // const std::string TrainInfoManager::SM_TOKEN = "SM";
    // const std::string TrainInfoManager::SF_TOKEN = "SF";
    const std::string TrainInfoManager::CSRZZ_TOKEN = "CSRZZ";

    TrainInfoManager* TrainInfoManager::m_instance = NULL;

    // copy from DebugUtil
    TrainInfoManager & TrainInfoManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard(getInstanceLock());
        if (m_instance == NULL)
        {
            m_instance = new TrainInfoManager;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create new TrainInfoManager instance.");
        }
        
        return *m_instance;
    }

    TrainInfoManager::~TrainInfoManager()                
    {
        for (unsigned int i = 0; i < m_trains.size(); i++)
        {
            delete m_trains[i];
            m_trains[i] = NULL;
        }
        m_trains.clear();
    }

    // copy from DebugUtil
    void TrainInfoManager::removeInstance()
    {
        {
            TA_Base_Core::ThreadGuard guard(getInstanceLock());
            if (NULL != m_instance)
            {
                delete m_instance;
                m_instance = NULL;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Remove TrainInfoManager instance.");
            }
        }
        
        TA_Base_Core::ReEntrantThreadLockable* lock = &(getInstanceLock());
        if (lock != NULL)
        {
            delete lock;
        }
    }

    // create train object based on database configuration
    void TrainInfoManager::addTrains(TA_Base_Core::TisTrainAlarmAgentEntityDataPtr tisTrainAlarmAgentEntityData, 
        std::map< unsigned long, TA_Base_Bus::DataPoint* > & dataPoints)
    {
        FUNCTION_ENTRY("TrainInfoManager::addTrains");
        if (!tisTrainAlarmAgentEntityData)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "TisTrainAlarmAgentEntityData pointer is NULL, so exit add trains...");
            return;
        }
        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "add trains according data points...");
            
        TA_Base_Core::ThreadGuard guard(m_trainLock);
        std::vector<std::string> trainTypes;
        std::vector<std::pair<std::string, unsigned long> > trainNumbers, trainInfoLens, startAddressOfTrains;
                
        try
        {
            trainTypes = tisTrainAlarmAgentEntityData->getTrainTypes();
            trainNumbers = tisTrainAlarmAgentEntityData->getTrainNumbers();
            trainInfoLens = tisTrainAlarmAgentEntityData->getTrainInfoLens();
            startAddressOfTrains = tisTrainAlarmAgentEntityData->getStartAddresses();            
        }
        // if throw this exception, the configuration should be incorrect, so we clear them,
        // and later it will not create train object.
        catch (const TA_Base_Core::GenericAgentException & gae)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "GenericAgentException", gae.what());
            trainTypes.clear();
            trainNumbers.clear();
            trainInfoLens.clear();
            startAddressOfTrains.clear();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Capture unknown exception, so clear the train data.");
            trainTypes.clear();
            trainNumbers.clear();
            trainInfoLens.clear();
            startAddressOfTrains.clear();
        }        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%d train types, %d train numbers, %d train info lens, %d start addressess of trains",
            trainTypes.size(), trainNumbers.size(), trainInfoLens.size(), startAddressOfTrains.size());
        // create train objectes
        constructTrains(trainTypes, startAddressOfTrains, trainNumbers, trainInfoLens);
        // after create objects, filter data point and add train data point to each object
        setDpForTrains(dataPoints);
        FUNCTION_EXIT;
    }

    // internal create train object
    void TrainInfoManager::constructTrains(std::vector<std::string> & trainTypes,
        std::vector<std::pair<std::string, unsigned long> > & startAddressOfTrains, 
        std::vector<std::pair<std::string, unsigned long> > & trainNumbers, 
        std::vector<std::pair<std::string, unsigned long> > & trainInfoLens)        
    {
        FUNCTION_ENTRY("TrainInfoManager::constructTrains");
        TA_Base_Core::ThreadGuard guard(m_trainLock);
        for (unsigned int i = 0; i < trainNumbers.size(); i++)
        { 
            std::string type = trainNumbers[i].first;
            unsigned long num = trainNumbers[i].second;
            unsigned long addr;
            
            if (startAddressOfTrains[i].first == trainNumbers[i].first)
            // address vector and number vector must have some order, and the values are corresponded
            {
                addr = startAddressOfTrains[i].second;
            }
            else // if values are not corresponding, we skip it. the object will not created
            {                
                TA_ASSERT(false, "train address and train number are not match.");
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Train type %s train number %d has not valid address. So skip this train.", type.c_str(), num);
                break;
            }

            unsigned long len = 0;
            for (unsigned int j = 0; j <= trainInfoLens.size(); j++)
            {
                if (trainNumbers[i].first == trainInfoLens[j].first)
                {
                    len = trainInfoLens[j].second;
                    break;
                }
            }

            if (len == 0) // skip it, and the object will not be created.
            {                
                TA_ASSERT(false, "Can't find info length of the train type");
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can't find info length of the train type %s. So skip this train.", type.c_str());
                break;
            }
            
            TrainData* trainPtr = NULL;
            // create detailed train object based on tokens
            if (type == CSRZZ_TOKEN)
            {
                trainPtr = new CsrzzTrainData(type, num, addr, len);
            }
            // else if (type == SM_TOKEN)
            // {
                // trainPtr = new SmTrainData(type, num, addr, len);
            // }
            else // invalid token appeared, skip it.
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid train type %s, so it will be created.", type.c_str());
                continue;
            }            
            
            m_trains.push_back(trainPtr);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Create a trian, the type is %s, number is %d, address is %d, length is %d", 
                type.c_str(), num, addr, len);
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "it has %d train data objects.", m_trains.size());
        FUNCTION_EXIT;
    }

    // copy from DebugUtil, maybe it has better way to sloved instance lock
    TA_Base_Core::ReEntrantThreadLockable & TrainInfoManager::getInstanceLock()
    {
        static TA_Base_Core::ReEntrantThreadLockable* lock = new TA_Base_Core::ReEntrantThreadLockable;
        return *lock;
    }

    // internal create train data point for each train object.
    void TrainInfoManager::setDpForTrains(std::map<unsigned long, TA_Base_Bus::DataPoint*> & dataPoints)
    {
        FUNCTION_ENTRY("TrainInfoManager::setDpForTrains");
        std::vector<TA_Base_Bus::DataPoint*> trainSpecialDps;

        // this loop is early filter, based on token, and collect them waiting for next filter.
        for (std::map<unsigned long, TA_Base_Bus::DataPoint*>::iterator itDps = dataPoints.begin(); 
            itDps != dataPoints.end(); 
            itDps++)
        {
            const std::string& address = itDps->second->getDataPointAddress();
            // it's pass if included any token
            if (std::string::npos != address.find(CSRZZ_TOKEN))
            {
                trainSpecialDps.push_back(itDps->second);
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Early filter get data point, entity key is %d", itDps->first);
            }
        }

        // train object will do more filter work to get exact target.
        for (unsigned int i = 0; i < m_trains.size(); i++)
        {
            m_trains[i]->addTrainDps(trainSpecialDps);
        }
        
        FUNCTION_EXIT;
    }

    // update train data point entry, call TrainData interface
    int TrainInfoManager::updateTrains(const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock)
    {
        FUNCTION_ENTRY("TrainInfoManager::updateTrains");
        TA_Base_Core::ThreadGuard guard(m_trainLock);
        int updateCount = 0;
        for (unsigned int i = 0; i < m_trains.size(); i++)
        {
            if (m_trains[i] != NULL)
            {                
                updateCount += m_trains[i]->updateTrain(dataBlock, pre_dataBlock);
            }
        }

        FUNCTION_EXIT;
        return updateCount;
    }

    int TrainInfoManager::updateTrain(unsigned long addr, const TA_Base_Bus::ModbusDataBlock * dataBlock, const TA_Base_Bus::ModbusDataBlock * pre_dataBlock)
    {
        FUNCTION_ENTRY("TrainInfoManager::updateTrain");
        int updateCount = 0;
        TrainData * train = getTrain(addr);
        updateCount = train->updateTrain(dataBlock, pre_dataBlock);
        FUNCTION_EXIT;
        return updateCount;
    }

    // return trains info
    std::vector<const TrainData *> TrainInfoManager::getTrains()
    {   
        FUNCTION_ENTRY("TrainInfoManager::getTrains");
        TA_Base_Core::ThreadGuard guard(m_trainLock);
        std::vector<const TA_IRS_App::TrainData *> trains;
        for (unsigned int i = 0; i < m_trains.size(); i++)
        {
            trains.push_back(m_trains[i]);
        }

        FUNCTION_EXIT;
        return trains;
    }

    // return trains count
    unsigned long TrainInfoManager::getTrainsCount()
    {
        FUNCTION_ENTRY("TrainInfoManager::getTrainsCount");
        TA_Base_Core::ThreadGuard guard(m_trainLock);
        FUNCTION_EXIT;
        return m_trains.size();
    }

    TrainData * TrainInfoManager::getTrain(unsigned long startAddr)
    {
        FUNCTION_ENTRY("TrainInfoManager::getTrain");
        TrainData * train = NULL;
        for (unsigned int i = 0; i < m_trains.size(); i++)
        {
            if (startAddr == m_trains[i]->getTrainAddr())
            {
                train = m_trains[i];
                break;
            }
        }
        
        FUNCTION_EXIT;
        return train;
    }

    bool TrainInfoManager::isTrainDp(TA_Base_Bus::DataPoint * dp)
    {
        FUNCTION_ENTRY("TrainInfoManager::isTrainDp");
        for (unsigned int i = 0; i < m_trains.size(); i++)
        {
            if (m_trains[i]->isTrainDp(dp))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "train %d include the data point %d", m_trains[i]->getTrainNum(),
                    dp->getParentEntityKey());
                return true;
            }
        }
        
        FUNCTION_EXIT;
        return false;
    }
    
} // namespace TA_IRS_App
