#ifndef WORKTHREADPOOL_H
#define WORKTHREADPOOL_H

#include "WorkThread.h"
#include <ace/Recursive_Thread_Mutex.h>
#include <queue>
#include <boost/function.hpp>
#include <ace/Condition_Recursive_Thread_Mutex.h>
#include <boost/noncopyable.hpp>

namespace TA_IRS_App
{
    class WorkThread;
    class WorkThreadPool : boost::noncopyable
    {
        public:
            WorkThreadPool();
			typedef WorkThread::ThreadFunc Task;
            //typedef boost::function<void ()> Task;
            void start(int numThreads);
            void stop();
            Task WorkThreadPool::take();
            void run(const Task& task);
        private:
            // No throw
            void runInThread();
            std::queue<Task> m_taskQue;
            std::vector<WorkThread*> m_workers;
            ACE_Condition_Recursive_Thread_Mutex m_condition;
            ACE_Recursive_Thread_Mutex m_mutex;
            bool m_running;
    };
}

#endif