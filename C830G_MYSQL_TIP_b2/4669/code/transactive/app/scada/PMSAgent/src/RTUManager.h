/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/4669/transactive/app/scada/PMSAgent/src/RTUManager.h $
  * @author:	HoaVu
  * @version:	$Revision: #3 $
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

#if !defined(AFX_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
#define AFX_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "app/scada/PMSAgent/src/RTUOutputWorker.h"
#include "app/scada/PMSAgent/src/HeartBeatWorker.h"
#include "app/scada/PMSAgent/src/RTU.h"
#include "bus/modbus_comms/src/DataBlock.h"


namespace TA_Base_Bus
{
    class IEntity;
    class DataPoint;
};

namespace TA_Base_Core
{
    class IEntityData;
    class ConfigUpdateDetails;
};

namespace TA_IRS_App
{
    struct RTUDataBlock;
    class RTU;
    class RTUDpProcessor;
    
    class RTUManager
    {
    public:
        RTUManager();
        virtual ~RTUManager();
        
        // for PMSAgent
        TA_Base_Bus::IEntity* createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData);
        bool updateRTUConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent);
        void createRTUWorkers();
        void addStationSystem(TA_Base_Core::IEntityData& EntityData);
        bool addDataPoint(TA_Base_Bus::DataPoint* dp);
        bool removeDataPoint(TA_Base_Bus::DataPoint* dp);

        // response to agent running mode
        void setToControl();
        void setToMonitor();

        // for RTU callback
        void processRTUStatusChanged(RTU& rtu);
        void processRTUDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock);

        // set running parameter
        void setIsRTUSwitchingDisabled(bool isDisabled);
        void setAnalogueDpUpdateInterval(int analogueDpUpdateInterval);

        void setAgentAssetName(std::string& assetName);
        std::string& getAgentAssetName();
        void setStationName(std::string& stationName);
        std::string& getStationName();
            
    private:
        // internal utility functions

    private:
        // global parameters
        bool						m_isRTUSwitchingDisabled;
        int							m_analogueDpUpdateInterval;
        std::string					m_agentAssetName;
        std::string					m_stationName;
		std::vector<TA_Base_Bus::EQualityStatus> m_qualityStatus;

        // an event processor for datapoint write request
        RTUOutputWorker     m_rtuOutputWorker;

		//heart beat processor
		HeartBeatWorker		m_rtuHeartBeatWorker;

        // a list of station system worker for input datapoints
        std::vector<RTUDpProcessor*> m_rtuDpProcessors;
        
        // enhence to unlimited number of rtus
        std::vector<RTU*>   m_rtus;
        RTU*                m_primaryRTU;
        TA_Base_Core::ReEntrantThreadLockable   m_primaryRTULock;
        TA_Base_Core::ReEntrantThreadLockable   m_dpProcessorLock;
    };
}

#endif // !defined(AFX_RTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
