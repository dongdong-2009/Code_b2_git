/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/scada/PMSAgent/src/RTUManager.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #4 $
  *
  * Last modification:	$DateTime: 2010/08/05 09:37:10 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	The RTUManager object is responsible for managing RTUs' running mode 
  * And processing RTUs' poll/write data
  *
  */

#include "app/scada/PMSAgent/src/RTUManager.h"
#include "app/scada/PMSAgent/src/RTUDpProcessor.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
    RTUManager::RTUManager()
        :
        m_isRTUSwitchingDisabled(false),
        m_agentAssetName(""),
        m_stationName(""),
        m_primaryRTU(NULL)
    {
        m_rtuDpProcessors.clear();
        m_rtus.clear();
		m_qualityStatus.clear();
        m_rtuHeartBeatWorker.start();//limin, failover issue
    }
    

    RTUManager::~RTUManager()
    {
    }
        

    TA_Base_Bus::IEntity* RTUManager::createRTUEntity ( TA_Base_Core::IEntityDataPtr EntityData )
    {
        // cast the EntityData to RTUEntityData
        TA_Base_Core::RTUEntityDataPtr rtuEntityData = boost::dynamic_pointer_cast<TA_Base_Core::RTUEntityData>(EntityData);
        
        TA_ASSERT( NULL != rtuEntityData.get(), "rtuEntityData is NULL in createRTUEntity" );
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating RTU entity for [%s]...", rtuEntityData->getName().c_str());
        // create RTU entity
        RTU* rtu = new RTU(*this, rtuEntityData);
        
        //TD10586
        m_rtuOutputWorker.addRTU(*rtu);
		m_rtuHeartBeatWorker.addRTU(*rtu);
        m_rtus.push_back(rtu);

        return rtu;
    }

    void RTUManager::createRTUWorkers()
    {
        // create station system worker for rtu internal datapoints processing
        for (unsigned long i = 0; i < m_rtus.size(); ++ i)
        {
            // create RTU other workers
            m_rtus[i]->createRTUWorkers();
        }
    }
    
    void RTUManager::setIsRTUSwitchingDisabled(bool isDisabled)
    {
        m_isRTUSwitchingDisabled = isDisabled;
    }

    void RTUManager::setAnalogueDpUpdateInterval(int analogueDpUpdateInterval)
    {
        m_analogueDpUpdateInterval = analogueDpUpdateInterval;

        unsigned long i;

        // update rtu internal dp processor
        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->updateAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
        }

        // update station system dp processors
        for (i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
            m_rtuDpProcessors[i]->updateAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
        }
    }

    void RTUManager::setAgentAssetName(std::string& assetName)
    {
        m_agentAssetName = assetName;
    }

    std::string& RTUManager::getAgentAssetName()
    {
        return m_agentAssetName;
    }

    void RTUManager::setStationName(std::string& stationName)
    {
        m_stationName = stationName;
    }

    std::string& RTUManager::getStationName()
    {
        return m_stationName;
    }
    
    bool RTUManager::updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
        for (unsigned long i = 0; i < m_rtus.size(); ++ i)
        {
            if (updateEvent.getKey() == m_rtus[i]->getPKey())
            {
                m_rtus[i]->updateRTUConfiguration(updateEvent);

                // return true to indicate no need to go further processing
                return true;
            }
        }

        // not rtu configuration update
        return false;
    }

    void RTUManager::addStationSystem(TA_Base_Core::IEntityData& EntityData)
    {
        // cast the IEntityData to StationSystemEntityData
        TA_Base_Core::StationSystemEntityData* stationSystemEntityData = NULL;
        stationSystemEntityData = dynamic_cast< TA_Base_Core::StationSystemEntityData * > (&EntityData);

        TA_ASSERT( NULL != stationSystemEntityData, "EntityData can't be cast to StationSystemEntityData" );

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Adding station system [%s] to RTU Manager", stationSystemEntityData->getName().c_str());
        
        // create station system worker
        RTUDpProcessor* dpProcessor = NULL;
        dpProcessor = new RTUDpProcessor(
                m_stationName,
                stationSystemEntityData->getName(),
                stationSystemEntityData->getPollingStartAddress(),
                stationSystemEntityData->getPollingEndAddress(),
                0, // offset address for station system is always equal to 0
                stationSystemEntityData->getBlockAddresses(),
                stationSystemEntityData->getSlaveID(),
                m_analogueDpUpdateInterval
            );
        
        if (dpProcessor != NULL)
        {
            m_rtuDpProcessors.push_back(dpProcessor);
			m_qualityStatus.push_back(TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED);
        }

        // create rtu output worker
        for (unsigned long i = 0; i < m_rtus.size(); i ++)
        {
            // for bit output worker
            m_rtus[i]->createOutputWriterForSystem (
                        stationSystemEntityData->getKey(),
                        stationSystemEntityData->getTypeKey(),
                        stationSystemEntityData->getName(),
                        stationSystemEntityData->getOutputServiceNumber(),
                        stationSystemEntityData->getOutputStartBitAddress(),
                        stationSystemEntityData->getOutputEndBitAddress(),
                        stationSystemEntityData->getSlaveID() );

            // for word output worker
            m_rtus[i]->createOutputWriterForSystem (
                        stationSystemEntityData->getKey(),
                        stationSystemEntityData->getTypeKey(),
                        stationSystemEntityData->getName(),
                        stationSystemEntityData->getOutputServiceNumber(),
                        stationSystemEntityData->getOutputStartWordAddress(),
                        stationSystemEntityData->getOutputEndWordAddress(),
                        stationSystemEntityData->getSlaveID() );
        }
    }

    bool RTUManager::addDataPoint(TA_Base_Bus::DataPoint* dp)
    {
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;
        bool isAdded = false;

        //Add output datapoint to RTU to be processed by StationSystemOutputWirter
        if (dp->isWriteable())
        {
            // output datapoints are in both rtus, so ingore the return value
            for (i = 0; i < m_rtus.size(); ++ i)
            {
                m_rtus[i]->addDataPoint(dp);
            }

            return true;
        }
        else
        {
            for (i = 0; isAdded == false && i < m_rtus.size(); ++ i)
            {
                isAdded = m_rtus[i]->addDataPoint(dp);
            }
            
            for (i = 0; isAdded == false && i < m_rtuDpProcessors.size(); ++ i)
            {
                isAdded = m_rtuDpProcessors[i]->addDataPoint(dp);
            }

            if (isAdded == false)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "DataPoint [name=%s, register address=%u] is not in any station system", 
                    dp->getDataPointName().c_str(), dp->getDataPointRegisterAddress());
            }

            return isAdded;
        }
    }
        
    bool RTUManager::removeDataPoint(TA_Base_Bus::DataPoint* dp)
    {
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;

        //remove datapoint to RTU to be processed by StationSystemOutputWirter
        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->removeDataPoint(dp);
        }

        //remove datapoint to station system worker, one datapoint is only processed by on worker
        for (i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
            m_rtuDpProcessors[i]->removeDataPoint(dp);
        }

        return true;
    }
        
    void RTUManager::setToControl()
    {
        unsigned long i;

        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->setToControlMode();
        }

        m_rtuOutputWorker.start();
		//m_rtuHeartBeatWorker.start();//limin, failover issue
        m_rtuHeartBeatWorker.setToControl();//limin, failover issue
    }

    void RTUManager::setToMonitor()
    {
        unsigned long i;
        
        for (i = 0; i < m_rtus.size(); ++ i)
        {
            m_rtus[i]->setToMonitorMode();
        }

        // before stop output thread, need to clear unsent write requests
        m_rtuOutputWorker.clearWriteQueue();
        m_rtuOutputWorker.terminateAndWait();

		//m_rtuHeartBeatWorker.terminateAndWait();//limin, failover issue
        m_rtuHeartBeatWorker.setToMonitor();//limin, failover issue
    }
    
    // constraint: there must be one and only one RTU is running in primary mode at a time
    // rule:
    //      1, if this is primary rtu and there is no other primary rtu, 
    //         then consider this rtu as primary rtu;
    //      2, if this is primary rtu and there is another primary rtu, 
    //         then compare rtu's weigtage value, consider the higher one as primary rtu, and switch others to standby rtu;
    //         if the weightage value is same, then consider this newer on as primary rtu, and switch others to standby rtu;
    //      3, if this is standby rtu and there is another primary rtu, then do nothing;
    //      4, if this is standby rtu and these is no other primary rtu, then compare rtu's weigtage value, switch the higher one to primary rtu

    void RTUManager::processRTUStatusChanged(RTU& rtu)
    {
        // to avoid multiple threads access at same time
        TA_Base_Core::ThreadGuard guard(m_primaryRTULock);
        
        // if this rtu communication failed
        if (rtu.getIsCommsOK() == false)
        {
            // if this rtu is orininal primary rtu, reset primary indicator
            if (m_primaryRTU == &rtu)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Primary RTU [%s] can't be connected", 
                    rtu.getName().c_str() );
                
                m_primaryRTU = NULL;
                
                // to check if there is another rtu can be connected
                // if yes, assume the connectable rtu as primary one, else, consider no primary rtu
                for (unsigned long i = 0; i < m_rtus.size(); ++ i)
                {
                    if (&rtu != m_rtus[i] && m_rtus[i]->getIsCommsOK())
                    {
                        m_primaryRTU = m_rtus[i];
                        break;
                    }
                }
            }

            // otherwise, ingore
            return;
        }

        // ingore switching rtu
        if (rtu.getIsRTUSwitching())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "RTU [%s] is switching, status change ingored", 
                rtu.getName().c_str() );

            return;
        }

        // if there is no primary rtu yet, assume this is primary one
        if (m_primaryRTU == NULL || m_primaryRTU->getIsCommsOK() == false)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No primary RTU yet, consider RTU [%s] as primary one", 
                rtu.getName().c_str() );
            
            m_primaryRTU = &rtu;
            return;
        }

        // both rtus are comm ok
        
        // if this is confirmed primary rtu
        if (rtu.getIsPrimaryRTU())
        {
            // if this is already consider as primary one, do nothing
            if (m_primaryRTU == &rtu)
            {
                return;
            }

            // if there is another primary rtu, compare rtu's weigtage value
            if (m_primaryRTU->getIsPrimaryRTU())
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Two RTUs are running in primary mode...");

                if (m_isRTUSwitchingDisabled)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "There are two primary RTUs, but RTU switching is disabled");
                    return;
                }
                
                int primaryWeightage = m_primaryRTU->getSWCWeightage();
                int newWeightage = rtu.getSWCWeightage();

                if (primaryWeightage > newWeightage)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Switch this RTU [%s] to standy", 
                        rtu.getName().c_str(), m_primaryRTU->getName().c_str() );

                    rtu.switchPrimaryToStandby();
                }
                // if weightage is equil, switch primary to a new one
                else
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Switch original primary RTU [%s] to standy", 
                        m_primaryRTU->getName().c_str(), rtu.getName().c_str() );

                    m_primaryRTU->switchPrimaryToStandby();

                    m_primaryRTU = &rtu;
                }

                return;
            }

            // if original primary rtu is not primary any more, consider this rtu as primary one
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Original primary RTU [%s] is not primary any more, consider this RTU [%s] as primary one", 
                m_primaryRTU->getName().c_str(), rtu.getName().c_str() );
            
            m_primaryRTU = &rtu;
            
            return;
        }

        // this rtu is confirmed standby rtu
        // if original primary rtu is still primary, do nothing
        if (m_primaryRTU->getIsPrimaryRTU())
        {
            return;
        }

        // need to switch rtu running mode, ingore if rtu switching is disabled
        if (m_isRTUSwitchingDisabled)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "There is no primary RTU, but RTU switching is disabled");
            return;
        }
        
        // if original primary rtu switched to standby, or just assumed one, need to switch it back to primary..?
		/*
		// Comment out this condition as next step will compare the weightage to decide primary RTU
        if (m_primaryRTU == &rtu)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "No primary RTU yet, switch RTU [%s] to primary mode", 
                rtu.getName().c_str() );
            
            rtu.switchStandbyToPrimary();
            return;
        }
		*/

        // if there is no real primary rtu, compare weightage
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Two RTUs are running in standby mode...");
        
        int primaryWeightage = m_primaryRTU->getSWCWeightage();
        int newWeightage = rtu.getSWCWeightage();
    
        if (primaryWeightage >= newWeightage)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Switch original assumed primary RTU [%s] to primary", 
                m_primaryRTU->getName().c_str() );
            
            m_primaryRTU->switchStandbyToPrimary();
        }
        // if this rtu has more weightage
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Switch this RTU [%s] to primary", 
                m_primaryRTU->getName().c_str() );
            
            rtu.switchStandbyToPrimary();
            m_primaryRTU = &rtu;
        }

        return;
    }

    //only process datapoint when the datablock is from primary rtu
    void RTUManager::processRTUDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock)
    {
        // if there is a primary rtu other than this, just ingore the data block
        if (m_primaryRTU != NULL && m_primaryRTU != &rtu)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTU [%s] is not primary rtu, datablock is ingored", 
                rtu.getName().c_str() );
            return;
        }

        // ingore the datablock is rtu is switching
        if (rtu.getIsRTUSwitching())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "RTU [%s] is switching, datablock is ingored", 
                rtu.getName().c_str() );
            return;
        }
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Dispatch RTU [%s] datablock to station systems...", 
            rtu.getName().c_str() );

        // to avoid multiple threads access at same time
        TA_Base_Core::ThreadGuard guard(m_dpProcessorLock);

        // if there is no primary rtu, or this is primary rtu
        for (unsigned long i = 0; i < m_rtuDpProcessors.size(); ++ i)
        {
            int slaveID = m_rtuDpProcessors[i]->getSlaveID();
              	
            // only process data as good quality when this rtu is primary one and station system is rtu internal system
            if (rtu.getIsCommsWithStationSystemsValid(slaveID))
            {
                m_qualityStatus[i] = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
            }
			else if (TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != m_qualityStatus[i])
			{			
				m_qualityStatus[i] = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;
			}

            m_rtuDpProcessors[i]->processRTUDataBlock(addressBlock, m_qualityStatus[i]);
        }
    }
        
}
