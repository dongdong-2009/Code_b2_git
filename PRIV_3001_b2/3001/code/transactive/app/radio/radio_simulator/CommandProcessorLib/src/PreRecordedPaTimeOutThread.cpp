#include "app/radio/radio_simulator/CommandProcessorLib/src/PreRecordedPaTimeOutThread.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_IRS_App
{

    TA_Base_Core::ReEntrantThreadLockable PreRecordedPaTimeoutThread::m_lockAbortList;
    std::list<PreRecordedPaTimeoutThread::PaRecordedTimeoutThreadId> PreRecordedPaTimeoutThread::m_abortList;
    
    TA_Base_Core::ReEntrantThreadLockable PreRecordedPaTimeoutThread::m_lockThreadWaitToClean; // lock m_threadWaitToClean
    std::queue<PreRecordedPaTimeoutThread* > PreRecordedPaTimeoutThread::m_threadWaitToClean;

    unsigned int PreRecordedPaTimeoutThread::m_timeOutValue = 10000; // ms

    PreRecordedPaTimeoutThread::PreRecordedPaTimeoutThread(PreRecordedPaTimeoutThread::PaRecordedTimeoutThreadId annId, 
                                IRadioCommsObserver* radioCommsObserver,
                                const std::vector<unsigned char> resp)
    {
        TA_ASSERT( radioCommsObserver != NULL, "radioCommsObserver != NULL" );
        m_radioCommsObserver = radioCommsObserver;
        m_threadId = annId;
        m_timeOutResp = resp;  
        m_threadMap[annId] = this;
    }
    /**
      * run
      * 
      * The run method must be implemented in any class that inherits from TA_Core::Thread.
      *
      */
    void PreRecordedPaTimeoutThread::run()
    {
        if ( m_timeOutValue == 0)
        {
            while (1)
            {
                this->sleep(100);
            }
        } 
        else
        {
            this->sleep(m_timeOutValue);
        }
        std::list<PaRecordedTimeoutThreadId>::iterator it;
        it = std::find(m_abortList.begin(), m_abortList.end(), m_threadId);
        if ( it != m_abortList.end())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "pre pa abort by operator ; id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
            m_lockThreadWaitToClean.acquire();
            m_threadWaitToClean.push(this);
            m_lockThreadWaitToClean.release();
            
        }
        else
        {   
            m_radioCommsObserver->sendData(m_timeOutResp);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Send pre pa timeout resp ; id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
        }
        
    }


    /**
      * terminate
      * 
      * The terminate method must be implemented in any class that inherits from TA_Core::Thread.
      *
      */
    void PreRecordedPaTimeoutThread::terminate()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Terminate pre pa timeout thread ; id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
    }

    PreRecordedPaTimeoutThread::~PreRecordedPaTimeoutThread()
    {
        terminateAndWait();
    }

    void PreRecordedPaTimeoutThread::terminateThread(PaRecordedTimeoutThreadId threadId)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Terminate pre pa timeout thread ; id = %d(%d)\n", m_threadId.first - 0, m_threadId.second - 0);
        m_threadMap[threadId]->terminate();
    }

    void PreRecordedPaTimeoutThread::cleanUpUnusefulThread()
    {
        m_lockThreadWaitToClean.acquire();
        while( !m_threadWaitToClean.empty() )
        {
            PreRecordedPaTimeoutThread* pThread = m_threadWaitToClean.front();
            m_threadWaitToClean.pop();
            pThread->terminate();
            delete pThread;
            pThread = NULL;
        }
        m_lockThreadWaitToClean.release();

    }

    void PreRecordedPaTimeoutThread::insertAbortThreadId(PreRecordedPaTimeoutThread::PaRecordedTimeoutThreadId threadId)
    {
        m_lockAbortList.acquire();
        m_abortList.push_back(threadId);
        m_lockAbortList.release();
    }

    void PreRecordedPaTimeoutThread::setTimeOutValue(unsigned int timeOutValue)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set time out value = %u \n", timeOutValue);
        m_timeOutValue = timeOutValue;
    }
}