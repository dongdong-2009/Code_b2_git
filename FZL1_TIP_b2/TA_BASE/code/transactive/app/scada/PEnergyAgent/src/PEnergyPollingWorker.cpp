#include "PEnergyPollingWorker.h"
#include "PEnergyManager.h"
#include "bus/scada/102_library/src/IEC102DataStruct.h"
#include <iostream>
#include "core/utilities/src/DebugUtil.h"
#include <time.h>

using TA_Base_Core::DebugUtil;
namespace TA_Base_App
{
	PEnergyPollingWorker::PEnergyPollingWorker( PEnergyManager& pEnergyManager,
		TA_Base_Bus::IEC102Library& pIEC102Library, 
                                              IEC102CommandType command, 
                                              int time_out,                                          
											  int startDelaySecond,
                                              unsigned char rad,
                                              std::vector<int>& vEquipmentStartAddress,
                                              std::vector<int>& vEquipmentEndAddress )
    :
    PEnergyTimerWorker(time_out,startDelaySecond),
    m_pEnergyManager(pEnergyManager),
	m_pIEC102Library(pIEC102Library),
    m_commandType( command ),
    m_rad(rad),
    m_queryStartTime( 0 ),
    m_queryEndTime( 0 )
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "START Command: CommandType=%d, DataPollInSec(time_out)=%d, DataFrequency(rad)=%d,StartDelay=%d", m_commandType, time_out, m_rad,startDelaySecond);
		/*if ( command == GET_POWER_HISTORY_120)
            std::cout << "120 command into polling schedule" << std::endl;
        else if ( command == GET_POWER_TELEMETRY_171)
            std::cout << "171 command into polling schedule" << std::endl;
        else if ( command == GET_POWER_TELEMETRY_172)
            std::cout << "172 command into polling schedule" << std::endl;*/
		m_DataStartAddr = vEquipmentStartAddress;
		m_DataEndAddr = vEquipmentEndAddress;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Command address ranges=%d", m_DataStartAddr.size());
		if(m_DataStartAddr.size() != m_DataEndAddr.size())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Command Start-address-values(%d) != End-address-values(%d)", m_DataStartAddr.size(), m_DataEndAddr.size());
		}
	}
    
    PEnergyPollingWorker::~PEnergyPollingWorker()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "END Command: CommandType=%d", m_commandType);
        setInServiceState( false );
    }
/*
	void PEnergyPollingWorker::getStartEndTimesFromDB(TA_Base_Core::IEC102PollingConfig objConfig)
    {
		m_pKey = objConfig.Pkey;
		if(objConfig.StartTime > 0)
		{
			m_queryStartTime = objConfig.StartTime;
			m_queryEndTime = now;//objConfig.StartTime + PEnergyTimerWorker::m_timeOut;
		}
		else
		{
			time_t now = time(0); // Get the system time
			m_queryStartTime = now - m_rad;
		}
	}

	void PEnergyPollingWorker::saveStartEndTimesinDB(IEC102CommandType commandType, time_t queryEndTime)
    {
		//m_pEnergyManager.saveStartEndTimesinDB(m_pKey, commandType, queryEndTime);
		m_commandType = commandType;
		m_queryEndTime = queryEndTime;
	}
*/
    void PEnergyPollingWorker::process()
    {
		int iCnt=0;
		TA_Base_Bus::FrameEnergyData return_data;
        
        // m_queryStartTime = m_queryEndTime;
		m_queryEndTime = time(0); //current system time, set query end time for polling
		// no needs second, so change it to 0
		struct tm *p_tm;
		p_tm = localtime(&m_queryEndTime);
		p_tm->tm_sec = 0;
		m_queryEndTime = mktime(p_tm);
		// changing finished
        m_queryStartTime = m_pEnergyManager.calcStartTime(m_queryEndTime, m_rad);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Query start time is %d , end time is %d", m_queryStartTime, m_queryEndTime);
        
        switch( m_commandType )
        {
            case CALL_LINK_STATUS:
				m_pIEC102Library.callLinkStatus();
                break;
			
            case GET_POWER_HISTORY_120:
				for(iCnt = 0; iCnt < m_DataStartAddr.size(); iCnt++)
				{
					return_data = m_pIEC102Library.getPowerEnergy_C_CI_NR_2( m_DataStartAddr[iCnt],
                                                                               m_DataEndAddr[iCnt],
                                                                               m_queryStartTime,
                                                                               m_queryEndTime,
																			   m_rad );
					m_pEnergyManager.parseReceivedDataFrame( return_data, m_commandType );
				}
                break;
				
            case GET_POWER_TELEMETRY_171:
				for(iCnt = 0; iCnt < m_DataStartAddr.size(); iCnt++)
				{				
					return_data = m_pIEC102Library.getRemoteMeasurement_C_YC_NA_2( m_DataStartAddr[iCnt],
																						 m_DataEndAddr[iCnt] );
					m_pEnergyManager.parseReceivedDataFrame( return_data, m_commandType );
				}
					
                break;
				
            case GET_POWER_TELEMETRY_172:
                for(iCnt = 0; iCnt < m_DataStartAddr.size(); iCnt++)
				{
					return_data = m_pIEC102Library.getRemoteMeasurement_C_YC_TA_2( m_DataStartAddr[iCnt],
                                                                               m_DataEndAddr[iCnt],
                                                                               m_queryStartTime,
                                                                               m_queryEndTime,
                                                                               m_rad );
					m_pEnergyManager.parseReceivedDataFrame( return_data, m_commandType );
				}				
                break;
                
            default:
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Command INVALID");
				break;
       }
    }
}

