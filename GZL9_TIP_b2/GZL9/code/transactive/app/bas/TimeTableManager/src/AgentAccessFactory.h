#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_AgentAccessFactory)
#define Transactive_3002_GF_App_Bas_TimeTableManager_AgentAccessFactory
#if _MSC_VER > 1000
#pragma once
#endif

#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/bas/bas_agent/idl/src/IStationBasAgentCorbaDef.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"

namespace TA_IRS_App
{
	typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IStationBasAgentCorbaDef,
		TA_IRS_Bus::IStationBasAgentCorbaDef_ptr,
		TA_IRS_Bus::IStationBasAgentCorbaDef_var> BASAgentObject;
	typedef std::map<unsigned long, BASAgentObject*> BasAgentMap;
	typedef std::map<unsigned long, BASAgentObject*>::iterator BasAgentMapIterator;

	typedef struct  
	{
		std::string todayTimeTableModeCodeFeedback;
		std::string todayTimeTableBeginTimeFeedback;
		std::string tomorrowTimeTableModeCodeFeedback;
		std::string tomorrowTimeTableBeginTimeFeedback;
	} TimeTableDatapointNameInfo;
	typedef std::vector<TimeTableDatapointNameInfo> TimeTableDatapointNameInfos;
	typedef std::map<unsigned long, TimeTableDatapointNameInfos> DataPointNameMap;
	typedef std::map<unsigned long, TimeTableDatapointNameInfos>::iterator DataPointNameMapIterator;

	class AgentAccessFactory
	{
	public:
		~AgentAccessFactory(void);
	public:
		static AgentAccessFactory& getInstance();
		std::string getAgentEntityName(unsigned long locationKey);

		BASAgentObject& getDefaultBasAgent();
		BASAgentObject& getBasAgentByLocation(unsigned long locationKey);

		void getTimeTableDatapointNamesByLocation(unsigned long locationKey, TimeTableDatapointNameInfos& datapointsName);

		//void executeTimeTable(unsigned long locationKey, unsigned long tableKey, const std::string& sessionId, bool isTodayTimeTable);
	protected:
		void getEntityDataList(unsigned long locationKey, bool loadParameter, TA_Base_Core::IEntityDataList& dataList);

		std::string getEntityParameter(const std::string& strEntityName, const std::string& strParameterName);
	private:
		AgentAccessFactory(void);
	private:
		BasAgentMap m_basAgentMap;
		std::string m_strBasAgentEntityName;

		static AgentAccessFactory* m_AgentAccessFactoryInstance;
		static TA_Base_Core::NonReEntrantThreadLockable m_instanceLock;
		TA_Base_Core::NonReEntrantThreadLockable m_agentMapLock;

		DataPointNameMap m_datapointsNames;
	};
}


#endif //Transactive_3002_GF_App_Bas_TimeTableManager_AgentAccessFactory
