#ifndef FZ_POLLINGSCHEDULER102_H
#define FZ_POLLINGSCHEDULER102_H
#include <vector>

#include "PEnergyTimerWorker.h"
#include "bus/scada/102_library/src/IEC102Library.h"

namespace TA_Base_App
{
	class PEnergyManager;

	enum IEC102CommandType
    {
        GET_POWER_HISTORY_120,
        GET_POWER_TELEMETRY_171,
        GET_POWER_TELEMETRY_172,
        CALL_LINK_STATUS
    };

	class PEnergyPollingWorker : public PEnergyTimerWorker
	{
	public:
		PEnergyPollingWorker( PEnergyManager& pEnergyManager,
			TA_Base_Bus::IEC102Library& pIEC102Library, 
                             IEC102CommandType command, 
                             int timeout,
                             int startDelaySecond,
                             unsigned char rad,
                             std::vector<int>& vStartEqAddress, 
                             std::vector<int>& vEndEqAddress );
		virtual ~PEnergyPollingWorker();
		virtual void process();
		//void saveStartEndTimesinDB(IEC102CommandType , time_t );
		//void getStartEndTimesFromDB(TA_Base_Core::IEC102PollingConfig objConfig);

	private:
		TA_Base_Bus::IEC102Library&           m_pIEC102Library;
		PEnergyManager&	m_pEnergyManager;
        IEC102CommandType        m_commandType;
        unsigned char            m_rad;
        int                      m_startEqAddress;
        int                      m_endEqAddress;
		std::vector<int>		m_DataStartAddr;
		std::vector<int>		m_DataEndAddr;
        time_t                   m_queryStartTime;
        time_t                   m_queryEndTime;
	};
}

#endif
