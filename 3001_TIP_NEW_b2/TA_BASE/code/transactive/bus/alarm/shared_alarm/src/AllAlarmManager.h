#if !defined(__ALL_ALARM_MANAGER_INCLUDED__)
#define __ALL_ALARM_MANAGER_INCLUDED__

#pragma once
#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"
#include "bus/alarm/shared_alarm/src/ICheckVisibility.h"
#include "bus/alarm/shared_alarm/src/IDutyAlarmAccess.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	// forward declaration
	class TA_SharedAlarm;

	typedef std::map<DWORD, AlarmDataItem*> DutyTempList;
	typedef std::map<unsigned long, std::list<unsigned long> > LocationSubSytemList;

	class SHAREDALARM_API AllAlarmManager 
	{
	public:
		static AllAlarmManager& getInstance();
		static void freeInstance();
		~AllAlarmManager(void);
		//release all alarm items
		void deleteAllAlarmItems(); 
		
		unsigned long getAllAlarmCount();
		
		virtual bool insertAlarm(AlarmDataItem* pData, DutyTempList * pDutyList = NULL);
		
		virtual bool updateAlarm(AlarmDataItem* pData, const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
		
		virtual bool deleteAlarm(std::string& strAlarmID);

		unsigned long removeLocationAlarms(unsigned long locationKey);
		
		void buildIndexWithDuty(ICheckVisibility* pObjFilter, IDutyAlarmAccess* pObjDutyAlarmAccess);
		
		void UpdateDutyIndex(LocationSubSytemList& changes, ICheckVisibility* pObjFilter, IDutyAlarmAccess* pObjDutyAlarmAccess);

		virtual AlarmDataItem* findAlarm(std::string alarmID);
		
		void setAllAlarmsMaxCount(unsigned long ulMaxCount);

		int deleteItem(TA_Base_Bus::AlarmDataItem* pAlarmDataItem);

	private:	// functions
		AllAlarmManager(void);
		
		unsigned long removeLocationAlarmsInternal(unsigned long locationKey);

		std::string _formatTime(long ulTime);
		
	private:	// variables
		static AllAlarmManager * m_pInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;	 
		TA_Base_Core::NonReEntrantThreadLockable m_MapLock;

		// Set this Variable to enforce limit on the container.
		unsigned long m_ulMaxAlarmCount;
	};

}

#endif // __ALL_ALARM_MANAGER_INCLUDED__