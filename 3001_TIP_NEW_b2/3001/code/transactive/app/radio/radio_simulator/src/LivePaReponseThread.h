#ifndef __LIVE_PA_REPONSE_THREAD_
#define __LIVE_PA_REPONSE_THREAD_

#include "app/radio/radio_simulator/src/RadioCommsObserver.h"

#include "core/threads/src/Thread.h"

#include <queue>
#include <utility>

namespace TA_IRS_App
{
    class LivePaReponseThread : public virtual TA_Base_Core::Thread
    {
    public:

        typedef std::pair<unsigned char, unsigned char> LiveRespThreadId; // first element is announcement Id, second is train Id

        LivePaReponseThread(LiveRespThreadId annId, RadioCommsObserver* radioCommsObserver, const std::vector<unsigned char> resp);
        /**
          * run
          * 
          * The run method must be implemented in any class that inherits from TA_Core::Thread.
          *
          */
        void run();


        /**
          * terminate
          * 
          * The terminate method must be implemented in any class that inherits from TA_Core::Thread.
          *
          */
        void terminate();

        virtual ~LivePaReponseThread();

        static void insertRequestResetResponse(LivePaReponseThread::LiveRespThreadId threadId, const std::vector<unsigned char> resp);
        static void cleanUpUnusefulThread();
    protected:
    private:
        
        LivePaReponseThread();
        LivePaReponseThread(const LivePaReponseThread&);
        LivePaReponseThread& operator=(const LivePaReponseThread&);
        void pushMyselfIntoCleanUpQueue();

        static TA_Base_Core::ReEntrantThreadLockable m_lock; // lock m_respMap
        static std::map<LiveRespThreadId , std::vector<unsigned char> > m_respMap;
        //static std::map<unsigned char, std::vector<unsigned char> > m_respMap;

        static TA_Base_Core::ReEntrantThreadLockable m_lockThreadWaitToClean; // lock m_threadWaitToClean
        static std::queue<LivePaReponseThread* > m_threadWaitToClean;

        //unsigned char m_announcementId;
        LiveRespThreadId m_threadId;
        RadioCommsObserver* m_radioCommsObserver;
        std::vector<unsigned char> m_livePaReset;

    };
}
#endif