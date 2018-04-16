#if !defined Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableDatapointPairManager
#define Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableDatapointPairManager
#pragma once

#include "ace/Singleton.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"
#include "app/bas/TimeTableManager/src/ModeAndTimeDatapointPair.h"
#include "app/bas/TimeTableManager/src/AgentAccessFactory.h"

namespace TA_IRS_App
{
	typedef std::vector<ModeAndTimeDatapointPair_SmartPtr> ModeAndTimeDatapointPairVct;
	typedef struct  
	{
		ModeAndTimeDatapointPairVct m_currentDay;
		ModeAndTimeDatapointPairVct m_nextDay;

	}ModeAndTimeDatapointPairVctPair;
	typedef std::map<unsigned long, ModeAndTimeDatapointPairVctPair> ModeAndTimeDatapointPairMap;

	class IModeBeginTimeUpdateInterface
	{
	public:
		virtual void updateModeBeginTime(const unsigned long locationKey, const unsigned long entityKey) = 0;
	};

	class TimeTableDatapointPairManager
	{
	public:
		friend class ModeAndTimeDatapointPair;
		TimeTableDatapointPairManager(void);
		~TimeTableDatapointPairManager(void);

	public:
		void uninit();
        void initializeTimeTablePairsByLocation( unsigned long locationKey );
        void getTimeTablePairsByLocation(unsigned long locationKey, bool isCurrentDay, ModeAndTimeDatapointPairVct& datapointsVct);
        ModeAndTimeDatapointPair_SmartPtr getTimeTablePairsByEntityKey( const unsigned long locationKey, const unsigned long entityKey, bool& bIsCurrentDay, bool& bIsNextDay );

		void registerModeBeginTimeUpdateListener(IModeBeginTimeUpdateInterface* listener);

	private:
		void getChildDataPointData(const std::string& dataNodeName, std::vector<TA_Base_Core::DataPointEntityDataPtr>& retValue);

	protected:
		void updateModeBeginTime(const unsigned long locationKey, const unsigned long entityKey);

	private:
        TA_Base_Core::NonReEntrantThreadLockable m_lckDatapointsMap;
		ModeAndTimeDatapointPairMap m_datapointsPairMap;
		IModeBeginTimeUpdateInterface* m_listener;
	};

	typedef ACE_Singleton<TimeTableDatapointPairManager,ACE_Recursive_Thread_Mutex> TimeTableDatapointPairManagerInstance;
}

#endif //Transactive_3002_GF_App_Bas_TimeTableManager_TimeTableDatapointPairManager