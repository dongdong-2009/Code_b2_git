/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/TimetableManager.h $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#if !defined(APP_BAS_BASAgent_TimetableManager_H)
#define APP_BAS_BASAgent_TimetableManager_H

#include "app/bas/BASAgent/src/ITimetableExecutionProxy.h"
#include "app/bas/BASAgent/src/PeriodicThread.h"
#include "bus/bas/bas_agent_library/src/BasAgentProxy.h"

namespace TA_Base_Bus
{
    class IModbusHelper;
}

namespace TA_IRS_App
{
    typedef unsigned short WORD;
    class BASAgent;

    class TimetableManager: public ITimetableExecutionProxy
    {
    public:
        TimetableManager();
        virtual ~TimetableManager();

        virtual void executeTimeTable(TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType timeTableType, CORBA::Long tableKey, const char* sessionID, long locationKey);
        virtual void executeScheduleTimeTable(const std::string& weekday);
        virtual bool isAgentRunningControlMode();
        virtual void setControlMode(bool bControlMode);
    private:
        TimetableManager(const TimetableManager&);
        TimetableManager& operator=(const TimetableManager&);

        void sendTimeTable(const TA_IRS_Core::TimeTableScheduleInfoVct& timeTable, unsigned long registerAddress);
        void conversionHelper(const std::string& input, WORD& output);
        void conversionHelperForTime(const time_t tmInput, WORD& output);
		std::string formatTimeString( const time_t tmTime );

		void deleteTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, const std::string& strType);

        boost::shared_ptr<TA_Base_Bus::IModbusHelper>	m_modbusHelper;

        TA_Base_Bus::DataPoint* 						m_currentDayTTPtBGN;
        TA_Base_Bus::DataPoint* 						m_nextDayTTPtBGN;

        TA_Base_Core::NonReEntrantThreadLockable		m_TTLock;
        bool                                            m_runningInControlMode;

        TA_IRS_App::PeriodicThread*						m_schedule;
    };
}

#endif
