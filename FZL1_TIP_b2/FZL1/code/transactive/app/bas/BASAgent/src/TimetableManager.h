/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/TimetableManager.h $
  * @author:	Huang Jian
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/01/26 16:54:00 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  *
  *
  */
#if !defined(GF_3002_app_bas_BASAgent_TimetableManager_H)
#define GF_3002_app_bas_BASAgent_TimetableManager_H

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

        TA_Base_Bus::DataPoint* m_currentDayTTPtBGN;
        TA_Base_Bus::DataPoint* m_nextDayTTPtBGN;

        TA_Base_Core::NonReEntrantThreadLockable		m_TTLock;
        bool                                            m_runningInControlMode;

        TA_IRS_App::PeriodicThread*				m_schedule;
    };
}

#endif