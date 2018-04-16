#include "StdAfx.h"
#include "DummyAgent.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;

DummyAgent::DummyAgent(ProcessData& data,
                       const std::string& ProcessId,
                       const std::string& host,
                       EProcessStatus state,
                       EProcessStatus requestedState,
                       EOperationMode operationMode,
                       time_t started,
                       size_t restarts,
                       const std::string& debugLevel)
    : ManagedProcess(data, ProcessId, host, state, requestedState, operationMode, started, restarts, debugLevel),
      m_defaultOperationMode(operationMode)
{
    start();
}

void DummyAgent::startProcess()
{
    m_data.requestedState = Startup;
    m_semaphore.post();
}

void DummyAgent::stopProcess()
{
    m_data.requestedState = Stopped;
    m_semaphore.post();
}

void DummyAgent::changeOperationMode(EOperationMode mode)
{
    if (m_data.state != TA_Base_Core::Stopped)
    {
        m_data.operationMode = mode;
        m_semaphore.post();
    }
    else
    {
        m_defaultOperationMode = mode;
    }
}

void DummyAgent::setParam(const std::string& name, const std::string& value)
{
    if (RPARAM_DEBUGLEVEL == name)
    {
        m_data.debugLevel = value.c_str();
    }
}

void DummyAgent::run()
{
    while (m_running)
    {
        m_semaphore.wait();

        switch (m_data.requestedState)
        {
        case Startup:
            if (m_data.state == Stopped)
            {
                m_data.started = time(NULL);
                m_data.restarts++;
                m_data.state = Startup;
                randomSleep();
                m_data.operationMode = m_defaultOperationMode;
                m_data.requestedState = (m_defaultOperationMode == Control ? RunningControl : RunningMonitor);
                randomSleep();
                m_data.state = (m_defaultOperationMode == Control ? GoingToControl : GoingToMonitor);
                randomSleep();
                m_data.state = (m_defaultOperationMode == Control ? RunningControl : RunningMonitor);
            }

            break;

        case Stopped:
            if (m_data.state != Stopped)
            {
                m_data.state = Terminating;
                randomSleep();
                randomSleep();
                m_data.state = Stopped;
                m_data.operationMode = NotApplicable;
                m_data.started = 0;
            }

            break;

        default:
            NULL;
        }

        switch (m_data.operationMode)
        {
        case Control:
            m_defaultOperationMode = Control;

            if (m_data.state != RunningControl)
            {
                m_data.requestedState = RunningControl;
                randomSleep();
                m_data.state = GoingToControl;
                randomSleep();
                m_data.state = RunningControl;
            }

            break;

        case Monitor:
            m_defaultOperationMode = Monitor;

            if (m_data.state != RunningMonitor)
            {
                m_data.requestedState = RunningMonitor;
                randomSleep();
                m_data.state = GoingToMonitor;
                randomSleep();
                m_data.state = RunningMonitor;
            }

            break;

        default:
            NULL;
        }
    }
}

void DummyAgent::randomSleep()
{
    static boost::random::mt19937 gen;
    static boost::random::uniform_int_distribution<> rand(0, 1000);
    Thread::sleep(rand(gen));
}
