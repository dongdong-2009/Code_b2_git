#include "StdAfx.h"
#include "SystemControllerAdmin.h"
#include "ManagedProcess.h"
#include "DummyAgent.h"
#include "ProcessManager.h"
#include "Utilities.h"
#include "core/utilities/src/Hostname.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;

#define READ_GUARD(l) boost::shared_lock<boost::shared_mutex> read_guard(l)
#define WRITE_GUARD(l) boost::unique_lock<boost::shared_mutex> write_guard(l)

SystemControllerAdmin::SystemControllerAdmin()
{
    activate();

    m_hostname = TA_Base_Core::Hostname::getHostname();
    m_locationName = Utilities::getLocationName();

    if (RunParams::getInstance().isSet("WithDummyAgents"))
    {
        createDummyAgents();
    }
}

TA_Base_Core::ProcessDataSeq* SystemControllerAdmin::getProcessData()
{
    READ_GUARD(m_lock);

    if (RunParams::getInstance().isSet("TestZIOP"))
    {
        TA_Base_Core::ProcessDataSeq* data = new TA_Base_Core::ProcessDataSeq;
        const size_t MAX_SIZE = 1000;
        data->length(MAX_SIZE);

        for (size_t i = 0; i < MAX_SIZE; ++i)
        {
            BOOST_FOREACH(const std::string& name, m_agentNames)
            {
                if (i < MAX_SIZE)
                {
                    (*data)[i] = m_processDataMap[name];
                }
            }
        }

        return data;
    }

    TA_Base_Core::ProcessDataSeq* data = new TA_Base_Core::ProcessDataSeq;
    size_t size = m_processes.size();
    data->length(size);
    size_t i = 0;

    BOOST_FOREACH (const std::string& name, m_agentNames)
    {
        (*data)[i++] = m_processDataMap[name];
    }

    return data;
}

void SystemControllerAdmin::activate()
{
    try
    {
        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "SystemControllerAdmin");
    }
    catch (const CORBA::SystemException&)
    {
    }

    activateServantWithName("SystemControllerAdmin");
}

void SystemControllerAdmin::deactivate()
{
    deactivateServant();
}

void SystemControllerAdmin::createDummyAgents()
{
    const char* DEFAULT_AGENTS[] = { "Alarm", "Authentication", "Duty", "NotificationService", "Rights", "SystemStatus", "DataNode", "Mms", "OnlinePrinting", "Scheduling", "Plan", "Pa", "VideoSwitch" };
    std::vector<std::string> dummyAgents(DEFAULT_AGENTS, DEFAULT_AGENTS + sizeof(DEFAULT_AGENTS) / sizeof(char*));

    if (RunParams::getInstance().isSet("DummyAgents"))
    {
        boost::split(dummyAgents, RunParams::getInstance().get("DummyAgents"), boost::is_any_of(",:/;|"), boost::token_compress_on);
    }

    time_t currentTime = time(NULL);

    BOOST_FOREACH(const std::string& name, dummyAgents)
    {
        std::string entityName = makeEntityName(name);
        m_agentNames.push_back(entityName);
        m_processes[entityName] = DummyAgentPtr(new DummyAgent(m_processDataMap[entityName], entityName, m_hostname, RunningControl, RunningControl, Control, currentTime, 0, "INFO"));
    }
}

void SystemControllerAdmin::startProcess(const char* entityName)
{
    ManagedProcessPtr process = getManagedProcess(entityName);

    if (process)
    {
        process->startProcess();
    }
}

void SystemControllerAdmin::stopProcess(const char* entityName)
{
    ManagedProcessPtr process = getManagedProcess(entityName);

    if (process)
    {
        process->stopProcess();
    }
}

void SystemControllerAdmin::changeAgentOperationMode(const char* entityName, ::TA_Base_Core::EOperationMode operationMode)
{
    ManagedProcessPtr process = getManagedProcess(entityName);

    if (process)
    {
        process->changeOperationMode(operationMode);
    }
}

void SystemControllerAdmin::setRunParam(const char* entityName, const ::TA_Base_Core::RunParam& param)
{
    ManagedProcessPtr process = getManagedProcess(entityName);

    if (process)
    {
        process->setParam(param.name.in(), param.value.in());
    }
}

ManagedProcessPtr SystemControllerAdmin::getManagedProcess(const std::string& entityName)
{
    READ_GUARD(m_lock);
    return Utilities::find(entityName, m_processes);
}

ManagedProcessPtr SystemControllerAdmin::getManagedProcess(unsigned long id)
{
    READ_GUARD(m_lock);
    return Utilities::find(id, m_id2processes);
}

void SystemControllerAdmin::shutdown(const char*)
{
    BOOST_FOREACH(Name2ManagedProcessMap::value_type& v, m_processes)
    {
        v.second->stopProcess();
    }

    deactivate();
}

std::string SystemControllerAdmin::makeEntityName(const std::string& agentName)
{
    return m_locationName + agentName + "MonitoredProcess";
}

void SystemControllerAdmin::registerManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess,
                                                   TA_Base_Core::ProcessId p_processID,
                                                   CORBA::ULong applicationType,
                                                   const char* applicationName)
{
    std::string entityName = convertToEntityName(applicationName);
    std::string ProcessId = entityName;
    std::string host = m_hostname;
    EProcessStatus state = TA_Base_Core::RunningControl;
    EProcessStatus requestedState = TA_Base_Core::RunningControl;
    time_t started = time(NULL);
    size_t restarts = 0;
    EOperationMode operationMode = TA_Base_Core::Control;
    std::string debugLevel = "INFO";

    try
    {
        TA_Base_Core::RunParamSeq_var params = p_managedProcess->getParams();

        for (size_t i = 0; i < params->length(); ++i)
        {
            if (boost::equals(RPARAM_DEBUGLEVEL, params[i].name.in()))
            {
                debugLevel = params[i].value.in();
            }
            else if (boost::equals(RPARAM_OPERATIONMODE, params[i].name.in()))
            {
                if (boost::equals(RPARAM_CONTROL, params[i].value.in()))
                {
                    operationMode = TA_Base_Core::Control;
                }
                else if (boost::equals(RPARAM_MONITOR, params[i].value.in()))
                {
                    operationMode = TA_Base_Core::Monitor;
                }
            }
        }
    }
    catch (...)
    {
    }

    ManagedProcessPtr process = getManagedProcess(entityName);

    WRITE_GUARD(m_lock);

    if (process)
    {
        ProcessData& data = process->m_data;
        data.started = started;
        data.state = state;
        data.requestedState = requestedState;
        data.operationMode = operationMode;
        data.restarts++;
        data.debugLevel = debugLevel.c_str();
    }
    else
    {
        process.reset(new ManagedProcess(m_processDataMap[entityName], ProcessId, host, state, requestedState, operationMode, started, restarts, debugLevel));
        m_processes[entityName] = process;
        m_agentNames.push_back(entityName);
    }

    process->m_managedProcess = TA_Base_Core::IManagedProcessCorbaDef::_duplicate(p_managedProcess);
    m_id2processes[p_processID] = process;
    boost::thread t(boost::bind(&SystemControllerAdmin::waitProcessThreadFunc, this, p_processID, process));
}

void SystemControllerAdmin::registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp, TA_Base_Core::ProcessId p_processId)
{
    ManagedProcessPtr process = getManagedProcess(p_processId);

    if (process)
    {
        WRITE_GUARD(m_lock);
        process->m_managedAgent = TA_Base_Core::IManagedAgentCorbaDef::_narrow(p_managedApp);
    }
}

bool SystemControllerAdmin::setClientRunParam(int id, const std::string& paramName, const std::string& paramValue)
{
    ManagedProcessPtr process = getManagedProcess(id);

    if (process)
    {
        process->setParam(paramName, paramValue);
    }

    return true;
}

void SystemControllerAdmin::changeMode(bool isControl, unsigned long id)
{
    ManagedProcessPtr process = getManagedProcess(id);

    if (process)
    {
        WRITE_GUARD(m_lock);
        process->m_data.operationMode = (isControl ? TA_Base_Core::Control : TA_Base_Core::Monitor);
    }
}

void SystemControllerAdmin::statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId)
{
    ManagedProcessPtr process = getManagedProcess(p_processId);

    if (process)
    {
        WRITE_GUARD(m_lock);
        process->m_data.state = status;
        process->m_data.requestedState = status;
    }
}

void SystemControllerAdmin::terminateApp(unsigned long id)
{
    ManagedProcessPtr process = getManagedProcess(id);

    if (process)
    {
        WRITE_GUARD(m_lock);
        process->m_data.requestedState = TA_Base_Core::Stopped;
        process->m_data.started = 0;
    }
}

void SystemControllerAdmin::terminating(TA_Base_Core::ProcessId p_processID, TA_Base_Core::EManagedProcessTerminateCode p_code)
{
    ManagedProcessPtr process = getManagedProcess(p_processID);

    if (process)
    {
        WRITE_GUARD(m_lock);
        process->m_data.state = TA_Base_Core::Stopped;
        process->m_data.requestedState = TA_Base_Core::Stopped;
        process->m_data.operationMode = TA_Base_Core::NotApplicable;
        process->m_data.started = 0;
        process->m_managedProcess = TA_Base_Core::IManagedProcessCorbaDef::_nil();
        process->m_managedAgent = TA_Base_Core::IManagedAgentCorbaDef::_nil();
    }
}

std::string SystemControllerAdmin::convertToEntityName(const std::string& agentName)
{
    return boost::regex_replace(agentName, boost::regex("Agent$"), "MonitoredProcess");
}

void SystemControllerAdmin::waitProcessThreadFunc(unsigned long id, ManagedProcessPtr process)
{
    HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, false, id);

    if (handle)
    {
        ::WaitForSingleObject(handle, INFINITE);
    }

    terminating(id, TA_Base_Core::NotKnown);

    try
    {
        ProcessManager::getInstance().getClient(id);
        ProcessManager::getInstance().terminating(id, TA_Base_Core::NotKnown);
    }
    catch (...)
    {
    }

    WRITE_GUARD(m_lock);
    m_id2processes.erase(id);
}

bool SystemControllerAdmin::isAnyRunning()
{
    READ_GUARD(m_lock);
    return m_id2processes.size();
}

void SystemControllerAdmin::forceKillAll()
{
    READ_GUARD(m_lock);

    BOOST_FOREACH (Pid2ManagedProcessMap::value_type& pair, m_id2processes)
    {
        HANDLE handle = ::OpenProcess(PROCESS_ALL_ACCESS, false, pair.first);
        TerminateProcess(handle, 0);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SystemControllerAdmin::forceKillAll - %s (%d)", pair.second->m_data.ProcessId.in(), pair.first);
    }
}
