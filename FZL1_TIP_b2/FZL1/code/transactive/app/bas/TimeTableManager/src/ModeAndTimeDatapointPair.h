#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_ModeAndTimeDatapointPair)
#define Transactive_3002_GF_App_Bas_TimeTableManager_ModeAndTimeDatapointPair
#pragma once

#include "boost/smart_ptr.hpp"
#include "core/synchronisation/src/Semaphore.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"

#include "app/bas/TimeTableManager/src/TimeTableManagerUtility.h"

namespace TA_IRS_App
{
	class ModeAndTimeDatapointPair : public TA_Base_Bus::IEntityUpdateEventProcessor
    {
	public:
		ModeAndTimeDatapointPair(unsigned long locationKey, const std::string& strModeCodeName, const std::string& strBeginTime);
		~ModeAndTimeDatapointPair(void);
	public:

		bool getModeCodeAndBeginTime(unsigned long& modeCode, time_t& beginTime);

        void initializeDatapoint(const std::vector<TA_Base_Core::DataPointEntityDataPtr>& myDatapoints);
        void waitInitialization();
        void cancelWaitInitialization();
		void uninit();

		std::string getModeDataPointName();

		std::string getBeginTimeDataPointName();

        const unsigned long getDpKey();

	public:
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

	protected:
		bool getDpValueByDataPointKey(unsigned long dpEntityKey, unsigned long& dpValue, bool& isModeCode);
		std::string regulateFeedbackBeginTime(const std::string& strBeginTime);

		// modbusValue value should be 0x1925, mean is 19:25
		// the return value is 
		time_t translateDisplayBeginTime(unsigned long modbusValue);
        const unsigned long getModeDpKey();
        const unsigned long getTimeDpKey();

	private:
		ModeAndTimeDatapointPair(void);
	private:
		TA_Base_Bus::DataPointProxySmartPtr m_dpModeCode;
		TA_Base_Bus::DataPointProxySmartPtr m_dpBeginTime;
        TA_Base_Core::Semaphore m_smaInitialization;

		std::string m_strModeCodeName;
		std::string m_strBeginTime;
		unsigned long m_locationKey;
        unsigned long m_ulValidProxyNum;
        unsigned long m_ulInitedProxyNum;
	};

	typedef boost::shared_ptr<ModeAndTimeDatapointPair> ModeAndTimeDatapointPair_SmartPtr;
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_ModeAndTimeDatapointPair
