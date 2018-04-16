/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/DisplayRequestLoadingThread.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/05/12 17:46:29 $
  * Last modified by:  $Author: builder $
  *
  * Loads display requests in the background.
  * When a display request has been loaded it will send a WM_ADD_DISPLAY_REQUEST
  * to the list control. The attached data is an IDisplayRequest*, the list then becomes the
  * owner of the IDisplayRequest object.
  *
  * When loading is about to begin, a WM_DISPLAY_REQUEST_LOAD_START will be sent,
  * and when loading is finished, a WM_DISPLAY_REQUEST_LOAD_END will be sent,
  * these should be used to provide an hourglass cursor and stop the list from
  * redrawing.
  */


#if !defined(DisplayRequestLoadingThread_H)
#define DisplayRequestLoadingThread_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/synchronisation/src/Semaphore.h"


// the messages to send
#define WM_DISPLAY_REQUEST_LOAD_START   (WM_USER + 110)
#define WM_ADD_DISPLAY_REQUEST          (WM_USER + 111)
#define WM_ADD_DISPLAY_REQUESTS         (WM_USER + 113)
#define WM_DISPLAY_REQUEST_LOAD_END     (WM_USER + 112)


namespace TA_IRS_Core
{
    class IDisplayRequest;
} 


namespace TA_IRS_App
{

    class InitialDisplayRequestLoadingThread : public TA_Base_Core::Thread
    {
    public:
        
        InitialDisplayRequestLoadingThread( std::vector<TA_IRS_Core::IDisplayRequest*>& requests, TA_Base_Core::Semaphore& semaphore );
        virtual ~InitialDisplayRequestLoadingThread() { terminateAndWait(); };

        virtual void run();
        
        virtual void terminate() {};
        
    private:
        TA_Base_Core::Semaphore& m_semaphore;
        std::vector<TA_IRS_Core::IDisplayRequest*>& m_initlalDisplayRequests;
    };


    class DisplayRequestLoadingThread : public TA_Base_Core::Thread
    {

    public:

        /**
          * DisplayRequestLoadingThread
          *
          * Constructor
          *
          * @param window   the window to receive the update messages.
          */
        DisplayRequestLoadingThread(CWnd* window);

        virtual ~DisplayRequestLoadingThread();


        /**
          * run
          *
          * Load the requests.
          */
        void run();


        /**
          * terminate
          *
          * stops the thread
          */
        virtual void terminate();

    private:

        /**
          * loadItems
          *
          * goes through the items and sends them to the GUI
          *
          * @param requests the requests to add. THis method cleans them up as it goes.
          */
        void loadItems(std::vector<TA_IRS_Core::IDisplayRequest*>& requests);


        /**
          * sendUpdate
          *
          * Posts the WM_ADD_DISPLAY_REQUEST message to the window.
          *
          * @param update   The item data
          */
        void sendUpdate(TA_IRS_Core::IDisplayRequest* update);

        /**
          * sendUpdate
          *
          * Posts WM_DISPLAY_REQUEST_LOAD_START or WM_DISPLAY_REQUEST_LOAD_END
          * to the window
          *
          * @param messageNumber WM_DISPLAY_REQUEST_LOAD_START or WM_DISPLAY_REQUEST_LOAD_END
          * @param data          the number of items for a WM_DISPLAY_REQUEST_LOAD_START
          */
        void sendUpdate(int messageNumber, unsigned int data = 0);

		void sendUpdate(std::vector<TA_IRS_Core::IDisplayRequest*> *updatePiece);
        void sendInitialUpdates();

        // the callback window (for messages)
        CWnd* m_windowToUpdate;

        // is this thread running?
        volatile bool m_running;

        // this is the last record loaded - used to refresh.
        time_t m_maxTime;
        // how often the data refreshes
        unsigned long m_refreshTimeout;

        std::vector<TA_IRS_Core::IDisplayRequest*> m_initlalDisplayRequests;

        TA_Base_Core::Semaphore m_initialDisplayRequestSemaphore;

        InitialDisplayRequestLoadingThread m_initialDisplayRequestLoadingThread;
    };


} // TA_App

#endif // !defined
