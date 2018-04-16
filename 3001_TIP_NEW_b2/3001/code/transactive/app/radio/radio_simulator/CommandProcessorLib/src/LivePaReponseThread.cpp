#include "app/radio/radio_simulator/CommandProcessorLib/src/LivePaReponseThread.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_IRS_App
{

    TA_Base_Core::ReEntrantThreadLockable LivePaReponseThread::m_lock; // lock m_respMap
    std::map< LivePaReponseThread::LiveRespThreadId , std::vector<unsigned char> > LivePaReponseThread::m_respMap;

    TA_Base_Core::ReEntrantThreadLockable LivePaReponseThread::m_lockThreadWaitToClean; // lock m_threadWaitToClean
    std::queue<LivePaReponseThread* > LivePaReponseThread::m_threadWaitToClean;

    LivePaReponseThread::LivePaReponseThread(LivePaReponseThread::LiveRespThreadId annId, IRadioCommsObserver& radioCommsObserver, const std::vector<unsigned char> resp)
		: m_radioCommsObserver(&radioCommsObserver)
	{
        TA_ASSERT(NULL != m_radioCommsObserver, "radioCommsObserver is NULL");
        m_threadId = annId;
	
        m_livePaReset = resp;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Create live thread; id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
    }

    void LivePaReponseThread::run()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Live thread start ,id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
        Sleep(15000); // Sleep 20 seconds, wait for begin button enable
        short tryTimes = 3;
        std::map< LivePaReponseThread::LiveRespThreadId , std::vector<unsigned char> >::iterator it;
        std::vector<unsigned char> resp;

        while ( tryTimes >= 0)
        {
            m_lock.acquire();
            it = m_respMap.find(m_threadId);
            if ( it == m_respMap.end() ) // Can not find the resp
            {
               
				if ( NULL != m_radioCommsObserver )
				{
					 m_radioCommsObserver->sendData(resp);
				}
				 m_lock.release();
               
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Can not find resp,id = %d(%d), trytime = %u\n", m_threadId.first - 0, m_threadId.second - 0, tryTimes);
                tryTimes--;
            } 
            else // If find out, reset tryTimes to 3, erase it and send resp to radio agent
            {
                tryTimes = 3;
                resp = (*it).second;
                m_respMap.erase(it);
                m_lock.release();
                if ( 0xFF == resp[23])
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Receive terminate signal,id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
                    pushMyselfIntoCleanUpQueue();
                    return;
                }
				if ( NULL != m_radioCommsObserver )
				{
 					m_radioCommsObserver->sendData(resp);
// 					m_radioCommsObserver->sendData(resp);
// 					m_radioCommsObserver->sendData(resp);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Find out resp,id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
				}
               
            }
            
            Sleep(500); // Sleep one second
        }

        // Send live PA Reset
         m_radioCommsObserver->sendData(m_livePaReset);
//         m_radioCommsObserver->sendData(m_livePaReset);
//         m_radioCommsObserver->sendData(m_livePaReset);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Send live pa reset,id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);

        pushMyselfIntoCleanUpQueue();

    }
    void LivePaReponseThread::terminate()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Terminate live thread ; id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
        
    }
    LivePaReponseThread::~LivePaReponseThread()
    {
        terminateAndWait();
    }
    void LivePaReponseThread::insertRequestResetResponse(LivePaReponseThread::LiveRespThreadId threadId, const std::vector<unsigned char> resp)
    {
        m_lock.acquire();
        m_respMap[threadId] = resp;
        m_lock.release();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Insert request reset resp; id = %d(%d)\n", threadId.first - 0, threadId.second - 0);
    }

    void LivePaReponseThread::cleanUpUnusefulThread()
    {
        m_lockThreadWaitToClean.acquire();
        while( !m_threadWaitToClean.empty() )
        {
            LivePaReponseThread* pThread = m_threadWaitToClean.front();
            m_threadWaitToClean.pop();
            pThread->terminate();
            delete pThread;
            pThread = NULL;
        }
        m_lockThreadWaitToClean.release();
    }
    void LivePaReponseThread::pushMyselfIntoCleanUpQueue()
    {
        // Push myself into queue to wait being clean up
        m_lockThreadWaitToClean.acquire();
        m_threadWaitToClean.push(this);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Push live thread to clean queue; id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
        m_lockThreadWaitToClean.release();
    }
}