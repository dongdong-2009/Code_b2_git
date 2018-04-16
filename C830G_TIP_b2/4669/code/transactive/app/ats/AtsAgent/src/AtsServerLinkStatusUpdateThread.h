// AtsServerLinkStatusUpdateThread.h: interface for the AtsServerLinkStatusUpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSSERVERLINKSTATUSUPDATETHREAD_H__C595673F_8DB0_4845_BDA9_411CDB08CA29__INCLUDED_)
#define AFX_ATSSERVERLINKSTATUSUPDATETHREAD_H__C595673F_8DB0_4845_BDA9_411CDB08CA29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/TimedWaitSemaphore.h"

namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_IRS_App
{
    class AtsServerLinkStatusUpdateThread : public TA_Base_Core::Thread
    {
    public:
        
        /**
         * Constructor
         */
        AtsServerLinkStatusUpdateThread();


        /**
         * Destructor
         */
        virtual ~AtsServerLinkStatusUpdateThread();

        
        /**
         * Try to set the server status datapoint
         *
         * @return if the datapoint was the server status datapoint and has been set
         */
        bool setDataPoint( TA_Base_Bus::DataPoint* dataPoint );

        
        /**
         * Sets the link status and causes the datapoint update
         */
        void setLinkStatus( bool status );

        
        /**
         * Processing loop
         */
        void run();

        /**
         * Signals the thread to stop, called by terminateAndWait
         */
        void terminate();

    
    private:

        /**
         * The server status datapoint
         */
        TA_Base_Bus::DataPoint* m_dataPoint;

        /**
         * Whether to stop running
         */
        bool m_terminate;

        /**
         * The server link status
         */
        volatile bool m_status;

        /**
         * Used to control the thread
         */
        TA_Base_Core::TimedWaitSemaphore m_semaphore;
    };

} // TA_IRS_App

#endif // !defined(AFX_ATSSERVERLINKSTATUSUPDATETHREAD_H__C595673F_8DB0_4845_BDA9_411CDB08CA29__INCLUDED_)
