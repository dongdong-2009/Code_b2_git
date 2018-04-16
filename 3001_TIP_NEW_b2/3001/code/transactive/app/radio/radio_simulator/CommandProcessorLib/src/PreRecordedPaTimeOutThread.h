#ifndef __PRERECORDEDPA_TIMEOUT_THREAD_
#define __PRERECORDEDPA_TIMEOUT_THREAD_

#include "app/radio/radio_simulator/TrainSimulatorLib/src/RadioCommsObserver.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"

#include "core/threads/src/Thread.h"

#include <queue>
#include <utility>

namespace TA_IRS_App
{
    class PreRecordedPaTimeoutThread: public virtual TA_Base_Core::Thread
    {
    public:
        typedef std::pair<unsigned char, unsigned char> PaRecordedTimeoutThreadId; // First element is announcement Id, second is train Id

        PreRecordedPaTimeoutThread(PaRecordedTimeoutThreadId annId, IRadioCommsObserver* radioCommsObserver, const std::vector<unsigned char> resp);
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

        virtual ~PreRecordedPaTimeoutThread();

        static void insertAbortThreadId(PreRecordedPaTimeoutThread::PaRecordedTimeoutThreadId threadId);
        static void cleanUpUnusefulThread();
        static void setTimeOutValue(unsigned int timeOutValue);

        void terminateThread(PaRecordedTimeoutThreadId threadId);

    protected:
    private:
        PreRecordedPaTimeoutThread();
        PreRecordedPaTimeoutThread(const PreRecordedPaTimeoutThread&);
        PreRecordedPaTimeoutThread& operator=(const PreRecordedPaTimeoutThread&);

        static TA_Base_Core::ReEntrantThreadLockable m_lockAbortList;
        static std::list<PaRecordedTimeoutThreadId> m_abortList;

        static TA_Base_Core::ReEntrantThreadLockable m_lockThreadWaitToClean; // lock m_threadWaitToClean
        static std::queue<PreRecordedPaTimeoutThread* > m_threadWaitToClean;

        static unsigned int m_timeOutValue;

        std::map<PaRecordedTimeoutThreadId, PreRecordedPaTimeoutThread* > m_threadMap;
        //unsigned char m_announcementId;
        PaRecordedTimeoutThreadId m_threadId;
        IRadioCommsObserver* m_radioCommsObserver;
        std::vector<unsigned char> m_timeOutResp;
    };
}
#endif