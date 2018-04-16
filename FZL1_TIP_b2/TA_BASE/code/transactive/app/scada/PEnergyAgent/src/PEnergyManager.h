/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/TA_BASE/transactive/app/scada/PEnergyAgent/src/PEnergyManager.h $
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

#if !defined(AFX_PENERGYMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
#define AFX_PENERGYMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "bus/scada/102_library/src/IEC102ObserverInterface.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "PEnergyPollingWorker.h"

namespace TA_Base_Bus
{
    class IEntity;
    class DataPoint;
	struct FrameEnergyData;
	class IEC102Library;
};

namespace TA_Base_Core
{
    class IEntityData;
    class ConfigUpdateDetails;
	
};

namespace TA_Base_App
{
    class PEnergyUnit;
	class PEnergyPollingWorker;
	class PEnergyDpProcessor;

	class PEnergyManager: public TA_Base_Bus::IEC102Observer
    {
    public:
        PEnergyManager();
        virtual ~PEnergyManager();
        
        // for PEnergyAgent
        TA_Base_Bus::IEntity* createWorkers(TA_Base_Core::IEntityDataPtr EntityData);
		void setupConfiguration(TA_Base_Core::IEntityDataPtr pEnergyEntityData);
        bool updatePEnergyUnitConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent);
        bool addDataPoint(TA_Base_Bus::DataPoint* dp);
        bool removeDataPoint(TA_Base_Bus::DataPoint* dp);

        // response to agent running mode
        void setToControl();
        void setToMonitor();
		void stopProcess();

        void setAgentAssetName(std::string& assetName);
        std::string& getAgentAssetName();
        void setStationName(std::string& stationName);
        std::string& getStationName();
        time_t calcStartTime(const time_t& endTime, const int& frequency); // this funciont for get start time based on their rad value.

		//For observer to get Channel connection status from IEC102 channel
		virtual void updateChannelStatus(const bool& status);
        void parseEPVList(std::string strList, int paramNumber);
        bool parseReceivedDataFrame(TA_Base_Bus::FrameEnergyData& objFrame, IEC102CommandType command);

    private:
		bool isEnergyDpNameMatched( const std::string & dpName );
		//std::vector<TA_Base_Core::IEC102PollingConfig> m_vIEC102PollingConfig;
        // global parameters
        std::string					m_agentAssetName;
        std::string					m_stationName;
		std::vector<TA_Base_Bus::EQualityStatus> m_qualityStatus;

		PEnergyDpProcessor	*m_pEnergyDpProcessor;

		// a polling data thread
		PEnergyPollingWorker	   *m_pEnergyDataPollingWorker;
		PEnergyPollingWorker	   *m_pMeasureDataPollingWorker;
		PEnergyPollingWorker	   *m_pCallLinkWorker;

		typedef std::map < unsigned long, TA_Base_Bus::DataPoint * > PEnergyDataPointMap;
		PEnergyDataPointMap						m_InputDataPoints;
		PEnergyDataPointMap						m_InputEnergyDataPoints;
		PEnergyDataPointMap						m_InputMessureDataPoints;

		TA_Base_Bus::IEC102Library* m_IEC102Library;
		bool		m_is102Started;
		bool		m_prevChannelStatus;
		int			m_pEnergyCommonAddr;
		int m_MeasureResponseTimeOutInMs;
		int m_EnergyDataPollInSec;
		int m_EnergyDataFrequency;
        int m_MeasureDataPollInSec;
		int m_MeasureDataFrequency;

   		//EnergyDataStartAddr - EnergyDataEndAddr : EPV# 0 - 1
		std::vector<int>	m_EnergyDataStartAddr;
		std::vector<int>	m_EnergyDataEndAddr;
		//MeasureDataStartAddr - MeasureDataEndAddr: EPV# 2 - 3
		std::vector<int>	m_MeasureDataStartAddr;
		std::vector<int>	m_MeasureDataEndAddr;

    };
}

#endif // !defined(AFX_PENERGYMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
