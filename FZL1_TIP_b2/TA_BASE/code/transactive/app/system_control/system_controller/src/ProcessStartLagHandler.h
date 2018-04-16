#ifndef PROCESSSTARTLAGHANDLER_H
#define PROCESSSTARTLAGHANDLER_H
#include <boost/assign.hpp>
#include "core/timers/src/MonotonicTimer.h"
#include <algorithm>

namespace TA_Base_App
{
    class ProcessManager;
    class ManagedProcessInfo;

    struct CompareManagedProcessInfoByStartLag
    {
        bool operator()(ManagedProcessInfo* lhs, ManagedProcessInfo* rhs) const
        {
            return lhs->getStartLag() < rhs->getStartLag();
        }
    };

    class ProcessStartLagHandler : ACE_Task<ACE_MT_SYNCH>
    {
    public:

        ProcessStartLagHandler(ProcessManager& processManager, size_t processNumber)
            : m_processManager(processManager),
              m_processNumber(processNumber),
              m_timeout(300),
              m_timer(true)
        {
            activate();
        }

    protected:

        virtual int svc()
        {
            getAllProcesses();

            while (!timeout())
            {
                while (isAnyProcessInTransientStatus() && !timeout())
                {
                    TA_Base_Core::Thread::sleep(20);
                }

                ManagedProcessInfo* process = getUnStartedProcess();

                if (NULL == process)
                {
                    return 0;
                }

                process->shouldWaitStartLag(false);
            }

            return 0;
        }

        bool isAnyProcessInTransientStatus()
        {
            using namespace TA_Base_Core;
            static std::set<EProcessStatus> transientStatusSet = boost::assign::list_of(Startup)(GoingToControl)(GoingToMonitor)(Terminating)(GoingToStandby);

            BOOST_FOREACH(ManagedProcessInfo* process, m_processes)
            {
                if (transientStatusSet.find(process->getProcessStatus()) != transientStatusSet.end())
                {
                    return true;
                }
            }

            return false;
        }

        ManagedProcessInfo* getUnStartedProcess()
        {
            BOOST_FOREACH(ManagedProcessInfo* process, m_processes)
            {
                if (TA_Base_Core::Unstarted == process->getProcessStatus())
                {
                    return process;
                }
            }

            return NULL;
        }

        void getAllProcesses()
        {
            m_processes = ProcessManager::getInstance().getAllManagedProcessInfo();

            while ((m_processes.size() != m_processNumber) && (m_timer.elapsed_s() < 10))
            {
                m_processes = m_processManager.getAllManagedProcessInfo();

                TA_Base_Core::Thread::sleep(20);
            }

            CompareManagedProcessInfoByStartLag compare;
            std::sort(m_processes.begin(), m_processes.end(), compare);
        }

        bool timeout()
        {
            return m_timeout < m_timer.elapsed_s();
        }

    protected:

        size_t m_processNumber;
        ProcessManager& m_processManager;
        TA_Base_Core::MonotonicTimer m_timer;
        unsigned long m_timeout;
        std::vector<ManagedProcessInfo*> m_processes;
    };
}

#endif
