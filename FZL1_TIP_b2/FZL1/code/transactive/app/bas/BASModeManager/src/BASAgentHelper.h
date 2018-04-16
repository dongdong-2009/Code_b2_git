#pragma once
#include "ace/Singleton.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/bas/bas_agent/idl/src/IStationBasAgentCorbaDef.h"
#include "app/bas/BASModeManager/src/TASingleton.h"
#include "app/bas/BASModeManager/src/CommonDef.h"

Namespace_TA_IRS_APP_Begin
class BASAgentHelper
{
public:
	friend class BasAgentHelperMap;
	BASAgentHelper(unsigned long locationKey);
	~BASAgentHelper(void);
protected:
	unsigned long getAgentEntityKey();
	std::string getAgentEntityName();
	unsigned long getAgentEntityTypeKey();
	unsigned long getAgentEntitySubsystemKey();
	std::string getStationName();
	std::string getCurrentExcuteModeDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);
	std::string getCurrentExcuteModeBitDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);
	std::string getCurrentExcuteModeBitSetDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);
	std::string getCurrentExcuteModeStatusDataPointName(TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);
protected:
	//void executeStationMode(unsigned long subsystemKey, unsigned long stationMode, 
	//						bool canOverrideStationMode, const std::string& sessionId);
private:
	void getEntityDataList(bool loadParameter, TA_Base_Core::IEntityDataList& dataList);
	std::string getEntityParameterValue(const std::string strParameterName);
private:
	typedef TA_Base_Core::NamedObject<TA_IRS_Bus::IStationBasAgentCorbaDef,
		TA_IRS_Bus::IStationBasAgentCorbaDef_ptr,
		TA_IRS_Bus::IStationBasAgentCorbaDef_var> BASAgentObject;
	
	typedef std::map<unsigned long, std::string> DataPointNameMap;
	typedef std::map<unsigned long, std::string>::iterator DataPointNameMapIterator;
	std::auto_ptr<BASAgentObject> m_BasAgentPtr;

	unsigned long m_locationKey;
	std::string m_strBasAgentEntityName;
	unsigned long m_BasAgentEntityKey;
	unsigned long m_BasAgentEntityTypeKey;
	unsigned long m_BasAgentSubsystemKey;
	std::string m_strStationName;
	std::string m_strCurrentExcuteModeDataPointTokenName;
	std::string m_strCurrentExcuteModeBitDataPointTokenName;
	std::string m_strCurrentExcuteModeStatusDataPointTokenName;
	DataPointNameMap m_CurrentExcuteModeDataPointName;
	DataPointNameMap m_CurrentExcuteModeBitDataPointName;
	DataPointNameMap m_CurrentExcuteModeBitSetDataPointName;
	DataPointNameMap m_CurrentExcuteModeStatusDataPointName;
};
typedef boost::shared_ptr<BASAgentHelper> BASAgentHelper_SmartPtr;

class BasAgentHelperMap
{
public:
	unsigned long getAgentEntityKey(unsigned long locationKey);
	std::string getAgentEntityName(unsigned long locationKey);
	unsigned long getAgentEntityTypeKey(unsigned long locationKey);
	unsigned long getAgentEntitySubsystemKey(unsigned long locationKey);
	std::string getStationName(unsigned long locationKey);
	std::string getCurrentExcuteModeDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);
	std::string getCurrentExcuteModeBitDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);
	std::string getCurrentExcuteModeBitSetDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);
	std::string getCurrentExcuteModeStatusDataPointName(unsigned long locationKey, TA_IRS_Core::SubSystem_SmartPtr& subsystemPtr);

	//void executeStationMode(unsigned long locationKey, unsigned long subsystemKey, unsigned long stationMode, 
	//					bool canOverrideStationMode, const std::string& sessionId);
private:
	BASAgentHelper_SmartPtr getBasAgentHelperByLocationKey(unsigned long locationKey);
private:
	std::map<unsigned long, BASAgentHelper_SmartPtr> m_basAgentHelperMap;
};
typedef TA_Base_Core::TASingleton<BasAgentHelperMap, TASINGLETONLOCK> BASAgentHelperInstance;
Namespace_TA_IRS_APP_End
