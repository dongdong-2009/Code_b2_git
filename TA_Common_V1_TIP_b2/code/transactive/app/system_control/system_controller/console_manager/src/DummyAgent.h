#pragma once
#include "ManagedProcess.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

struct DummyAgent : ManagedProcess, TA_Base_Core::Thread
{
    DummyAgent(TA_Base_Core::ProcessData& data,
               const std::string& ProcessId,
               const std::string& host,
               TA_Base_Core::EProcessStatus state,
               TA_Base_Core::EProcessStatus requestedState,
               TA_Base_Core::EOperationMode operationMode,
               time_t started,
               size_t restarts,
               const std::string& debugLevel);

    virtual void startProcess();
    virtual void stopProcess();
    virtual void changeOperationMode(TA_Base_Core::EOperationMode mode);
    virtual void setParam(const std::string& name, const std::string& value);

    virtual void run();
    virtual void randomSleep();
    virtual void terminate() { m_running = false; m_semaphore.post(); }

    bool m_running;
    TA_Base_Core::Semaphore m_semaphore;
    TA_Base_Core::EOperationMode m_defaultOperationMode;
};

typedef boost::shared_ptr<DummyAgent> DummyAgentPtr;
