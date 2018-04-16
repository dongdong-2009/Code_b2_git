#pragma once
#include "Singleton.h"
#include "core/corba/src/ServantBase.h"
#include "core/process_management/idl/src/ISystemControllerAdminCorbaDef.h"
#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/process_management/IDL/src/IManagedApplicationCorbaDef.h"

using namespace TA_Base_Core;
struct ManagedProcess;
class ProcessManager;
typedef boost::shared_ptr<ManagedProcess> ManagedProcessPtr;
typedef std::map<std::string, ProcessData> ProcessDataMap;
typedef std::map<std::string, ManagedProcessPtr> Name2ManagedProcessMap;
typedef std::map<unsigned long, ManagedProcessPtr> Pid2ManagedProcessMap;

struct SystemControllerAdmin
    : public virtual POA_TA_Base_Core::ISystemControllerAdminCorbaDef,
      public virtual TA_Base_Core::ServantBase,
      public Singleton<SystemControllerAdmin>
{
    friend class ProcessManager;

public:

    SystemControllerAdmin();

    virtual void activate();
    virtual void deactivate();
    virtual void forceKillAll();

protected:

    virtual TA_Base_Core::ProcessDataSeq* getProcessData();
    virtual void shutdown(const char* hostname);
    virtual void startProcess(const char* entityName);
    virtual void changeAgentOperationMode(const char* entityName, ::TA_Base_Core::EOperationMode operationMode);
    virtual void stopProcess(const char* entityName);
    virtual void setRunParam(const char* entityName, const ::TA_Base_Core::RunParam& param);
    virtual TA_Base_Core::AgentStatusEnum getAgentsStatus() { return TA_Base_Core::Normal; }
    virtual void notifyStandbyAgent(const char* entityName, const char* hostName) {}

protected: //ProcessManager

    void changeMode(bool isControl, unsigned long id);
    bool setClientRunParam(int id, const std::string& paramName, const std::string& paramValue);
    void registerManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr, TA_Base_Core::ProcessId, CORBA::ULong, const char*);
    void registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr, TA_Base_Core::ProcessId);
    void statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId);
    void terminateApp(unsigned long id);
    void terminating(TA_Base_Core::ProcessId p_processID, TA_Base_Core::EManagedProcessTerminateCode p_code);

protected:

    void createDummyAgents();
    ManagedProcessPtr getManagedProcess(const std::string& entityName);
    ManagedProcessPtr getManagedProcess(unsigned long id);
    std::string makeEntityName(const std::string& agentName);
    static std::string convertToEntityName(const std::string& agentName);
    void waitProcessThreadFunc(unsigned long id, ManagedProcessPtr process);
    bool isAnyRunning();

protected:

    std::string m_hostname;
    std::string m_locationName;
    ProcessDataMap m_processDataMap;
    Name2ManagedProcessMap m_processes;
    Pid2ManagedProcessMap m_id2processes;
    std::vector<std::string> m_agentNames;
    boost::shared_mutex m_lock;
};
