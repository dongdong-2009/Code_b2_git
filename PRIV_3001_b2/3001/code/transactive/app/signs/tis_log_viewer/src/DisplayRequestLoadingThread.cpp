/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/DisplayRequestLoadingThread.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * Messages are sent to the given window, they are:
  * WM_UPDATE_DOWNLOADLIST (wParam is a TrainDownloadStatus*, it must be deleted when used)
  */

#include <time.h>

#include "DisplayRequestLoadingThread.h"
#include "UserMessages.h"
#include "TisLogViewer.h"


#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"


namespace
{
    // default refresh (1 minute)
    static const unsigned long DEFAULT_REFRESH_PERIOD_IN_SECONDS = 60;
    // static const unsigned long DEFAULT_REFRESH_PERIOD_IN_SECONDS = 10;

    // wake the thread more often than the refresh timeout - otherwise
    static const unsigned long REFRESH_CHECK_TIME_IN_MS = 1000;
}

namespace TA_IRS_App
{

    using TA_Base_Core::RunParams;
    using TA_Base_Core::ThreadGuard;
    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::DatabaseException;
    using TA_Base_Core::DataException;

    using TA_IRS_Core::DisplayRequestAccessFactory;
    using TA_IRS_Core::IDisplayRequest;

    DisplayRequestLoadingThread::DisplayRequestLoadingThread(CWnd* window)
        : m_windowToUpdate(window),
          m_running(false),
          m_maxTime(0),
          m_refreshTimeout(DEFAULT_REFRESH_PERIOD_IN_SECONDS),
          m_initialDisplayRequestSemaphore( 0 ),
          m_initialDisplayRequestLoadingThread( m_initlalDisplayRequests, m_initialDisplayRequestSemaphore)
    {
        FUNCTION_ENTRY( "DisplayRequestLoadingThread" );

        TA_ASSERT(m_windowToUpdate != NULL, "Window to update is null");

        // get the timeout parameter from the main app object
        TisLogViewerApp* theApp = dynamic_cast<TisLogViewerApp*>( AfxGetApp() );
        TA_ASSERT( theApp != NULL, "The Application object cannot be NULL" );

        m_refreshTimeout = theApp->getRefreshIntervalInSecs();

        if (m_refreshTimeout < 1)
        {
            m_refreshTimeout = DEFAULT_REFRESH_PERIOD_IN_SECONDS;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Reload thread using a refresh period of %d seconds.", m_refreshTimeout );

        m_initialDisplayRequestLoadingThread.start();

        FUNCTION_EXIT;
    }


    DisplayRequestLoadingThread::~DisplayRequestLoadingThread()
    {
        FUNCTION_ENTRY( "~DisplayRequestLoadingThread" );
        FUNCTION_EXIT;
    }


    void DisplayRequestLoadingThread::sendInitialUpdates()
    {
        FUNCTION_ENTRY( "sendInitialUpdates" );

        m_initialDisplayRequestSemaphore.wait();

        // tell the owning window to expect updates
        sendUpdate( WM_DISPLAY_REQUEST_LOAD_START, m_initlalDisplayRequests.size() );

        // send the items to the GUI
        loadItems( m_initlalDisplayRequests );

        // tell the owning window not to expect updates
        sendUpdate( WM_DISPLAY_REQUEST_LOAD_END );

        FUNCTION_EXIT;
    }


    void DisplayRequestLoadingThread::run()
    {
        FUNCTION_ENTRY( "run" );

        m_running = true;

        sendInitialUpdates();

        std::vector<IDisplayRequest*> requests;

        // start the periodic refresh
        unsigned int elapsedTime = 0;

        while(m_running)
        {
            // wait
            sleep( REFRESH_CHECK_TIME_IN_MS );

            // load the latest records

            if (elapsedTime >= m_refreshTimeout)
            {
                elapsedTime = 0;

                try
                {
                    // load requests from the database
                    requests = DisplayRequestAccessFactory::getInstance().getDisplayRequestsGreaterThan( m_maxTime );

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Refresh - Loaded %d new display request logs.", requests.size() );

                    // add requests to the GUI
                    loadItems(requests);
                }
                catch (DatabaseException& dbe)
                {
                    LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", dbe.what() );
                }
                catch (DataException& de)
                {
                    LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DataException", de.what() );

                    // dont display the pop up if there is no data
                    // thats not really an error
                    if (de.getFailType() != DataException::NO_VALUE)
                    {
                        std::stringstream error;
                        error << "Database error while loading display request logs. " << de.what();
                    }
                }
            }
            else
            {
                elapsedTime += (REFRESH_CHECK_TIME_IN_MS / 1000);
            }
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestLoadingThread::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        m_running = false;

        FUNCTION_EXIT;
    }


    void DisplayRequestLoadingThread::loadItems(std::vector<IDisplayRequest*>& requests)
    {
        FUNCTION_ENTRY( "loadItems" );

        // for each request
        // while the thread is still running
        std::vector<IDisplayRequest*>::iterator iter;
        for (iter = requests.begin(); (iter != requests.end()) && m_running; iter++)
        {
            // add it to the list
            try
            {
                // keep the maximum key updated
                m_maxTime = max( (*iter)->getCreatedTime(), m_maxTime );

                // send the message
                sendUpdate(*iter);
            }
            catch ( DataException& de )
            {
                // this means the request entry is broken
                LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DataException", de.what() );

                // must delete the object to prevent memory leaks
                delete *iter;
                *iter = NULL;
            }
        }

        // if the thread was stopped before all requests were loaded
        // then clean up the remaining requests
        for ( ; iter != requests.end(); iter++)
        {
            delete *iter;
            *iter = NULL;
        }

        requests.clear();

        FUNCTION_EXIT;
    }


    void DisplayRequestLoadingThread::sendUpdate(IDisplayRequest* update)
    {
        FUNCTION_ENTRY( "sendUpdate" );

        TA_ASSERT(m_windowToUpdate != NULL, "window to update is null");

        int ret = m_windowToUpdate->PostMessage( WM_ADD_DISPLAY_REQUEST, reinterpret_cast<unsigned int>(update) );
        if ( 0 == ret )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Error posting WM_ADD_DISPLAY_REQUEST message.");

            // clean up
            delete update;
            update = NULL;
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestLoadingThread::sendUpdate(int messageNumber, unsigned int data /* = 0 */)
    {
        FUNCTION_ENTRY( "sendUpdate" );

        TA_ASSERT(m_windowToUpdate != NULL, "window to update is null");

        int ret = m_windowToUpdate->PostMessage( messageNumber, data );

        if ( 0 == ret )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Error posting WM_DISPLAY_REQUEST_LOAD_* message." );
        }

        FUNCTION_EXIT;
    }


    InitialDisplayRequestLoadingThread::InitialDisplayRequestLoadingThread( std::vector<TA_IRS_Core::IDisplayRequest*>& requests,
                                                                            TA_Base_Core::Semaphore& semaphore )
        : m_initlalDisplayRequests( requests ),
          m_semaphore( semaphore )
    {
    }


    void InitialDisplayRequestLoadingThread::run()
    {
        FUNCTION_ENTRY( "bulkLoadInitialDisplayRequests" );

        try
        {
            // load requests from the database
            m_initlalDisplayRequests = DisplayRequestAccessFactory::getInstance().getDisplayRequests();

            DisplayRequestAccessFactory::getInstance().bulkLoadDisplayRequest( m_initlalDisplayRequests );

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Loaded %d display request logs.", m_initlalDisplayRequests.size() );
        }
        catch ( DatabaseException& dbe )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
            UserMessages::getInstance().displayError("Database error while bulk loading display request logs.");
        }
        // the only difference when handling a database versus a dataexception
        // is that a dataexception will give some sort of error with the data that will
        // need to be fixed.
        // also no data is not really an error.
        catch ( DataException& de )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );

            // dont display the pop up if there is no data
            // thats not really an error
            if ( de.getFailType() != DataException::NO_VALUE )
            {
                std::stringstream error;
                error << "Database error while bulk loading display request logs. " << de.what();
                UserMessages::getInstance().displayError(error.str().c_str());
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknowen Exception", "" );
            UserMessages::getInstance().displayError("Unknowen error while bulk loading display request logs.");
        }

        m_semaphore.post();
        FUNCTION_EXIT;
    }

} // TA_App

