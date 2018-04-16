#pragma once
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/process_management/IDL/src/IManagedAgentCorbaDef.h"

struct ManagedProcess
{
    ManagedProcess(TA_Base_Core::ProcessData& data,
                   const std::string& ProcessId,
                   const std::string& host,
                   TA_Base_Core::EProcessStatus state,
                   TA_Base_Core::EProcessStatus requestedState,
                   TA_Base_Core::EOperationMode operationMode,
                   time_t started,
                   size_t restarts,
                   const std::string& debugLevel);
    virtual ~ManagedProcess() {}

    virtual void startProcess() {}
    virtual void stopProcess();
    virtual void changeOperationMode(TA_Base_Core::EOperationMode mode);
    virtual void setParam(const std::string& name, const std::string& value);

    TA_Base_Core::ProcessData& m_data;
    TA_Base_Core::IManagedProcessCorbaDef_var m_managedProcess;
    TA_Base_Core::IManagedAgentCorbaDef_var m_managedAgent;
};

typedef boost::shared_ptr<ManagedProcess> ManagedProcessPtr;
