/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitor.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * 
 * Encapsulates the logic for monitoring the duty status of regions and subsystems. 
 */

#if !defined(DUTYMONITOR_H_INCLUDED)
#define DUTYMONITOR_H_INCLUDED

#include "DutyMonitorAlarmSender.h"

namespace TA_Base_App
{
	class DutyMonitor
	{

	public:
		DutyMonitor();
		~DutyMonitor();

		void add(const TA_Base_Bus::DASType::DbKey& region);
		void rem(const TA_Base_Bus::DASType::DbKey& region);
		void add(const TA_Base_Bus::DASType::DbKey& region, const TA_Base_Bus::DASType::DbKey& subsystem);
		void rem(const TA_Base_Bus::DASType::DbKey& region, const TA_Base_Bus::DASType::DbKey& subsystem);

		void check();

	private:

		class MonitorInfo
		{
		public:
			MonitorInfo();

			void operator++(int);
			void operator--(int);

			bool isOk();
			bool isChecked();
		
		private:
			unsigned long   m_dutyCount;
			ACE_Time_Value m_zeroSince;
			bool            m_checked;
		};

		typedef std::map<TA_Base_Bus::DASType::DbKey,MonitorInfo> SubMonitorMap;

		class RegMonitorInfo
		{
		public:
			MonitorInfo   m_monitorInfo;
			SubMonitorMap m_subMonitorMap;
		};

		typedef std::map<TA_Base_Bus::DASType::DbKey,RegMonitorInfo> RegMonitorMap;

		RegMonitorMap m_regMonitorMap;

		TA_Base_Core::NonReEntrantThreadLockable m_accessLock;

		DutyMonitorAlarmSender m_alarmSender;
	};
} // TA_Base_Bus

#endif // !defined(DUTYMONITOR_H_INCLUDED)
