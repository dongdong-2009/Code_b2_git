#include "WorkThreadPool.h"
#include "WorkThread.h"
#include "core/utilities/src/DebugUtil.h"
#include <ace/Guard_T.h>
#include <boost/bind.hpp>

namespace TA_IRS_App
{
    WorkThreadPool::WorkThreadPool()
        : m_mutex(),
          m_condition(m_mutex),
          m_running(false)
    {
    }
    void WorkThreadPool::start(int numThreads)
    {
        TA_ASSERT(m_workers.empty(), "");
        m_running = true;
        for(int i = 0; i < numThreads; ++i)
        {
            m_workers.push_back(new WorkThread(boost::bind(&WorkThreadPool::runInThread, this)));
            m_workers[i]->start();
        }
    }
    void WorkThreadPool::stop()
    {
		{
	        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
	        m_running = false;
	        m_condition.broadcast();
		}
        for(std::vector<WorkThread*>::iterator it = m_workers.begin(); it != m_workers.end(); ++it)
        {
            (*it)->terminateAndWait();
            delete *it;
        }
        m_workers.clear();
        // Clear task queue
        m_taskQue = std::queue<Task>();
    }

    WorkThreadPool::Task WorkThreadPool::take()
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
        while(m_taskQue.empty() && m_running)
        {
            m_condition.wait();
        }
        Task task;
        if(!m_taskQue.empty())
        {
            task = m_taskQue.front();
            m_taskQue.pop();
        }
        return task;
    }

    void WorkThreadPool::run(const Task& task)
    {
        if(m_workers.empty())
        {
            task();
        }
        else
        {
            ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
            m_taskQue.push(task);
            m_condition.signal();
        }
    }

    void WorkThreadPool::runInThread()
    {
        while(m_running)
        {
            Task task(take());
            if(m_running && task)
            {
                task();
            }
        }
    }

}