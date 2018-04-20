/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * Messages are sent to the given window, they are:
  * WM_UPDATE_DOWNLOADLIST (wParam is a TrainDownloadStatus*, it must be deleted when used)
  */

#include "stdafx.h"

#include "app/signs/ttis_manager/src/resource.h"

#include "app/signs/ttis_manager/src/AbstractTrainDownloads.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{

    using TA_Base_Core::RunParams;
    using TA_Base_Core::ThreadGuard;
    using TA_Base_Core::DebugUtil;


    const std::string AbstractTrainDownloads::RPARAM_REQUEST_RETRY_INTERVAL = "RequestRetryInterval";

    const unsigned long AbstractTrainDownloads::RPARAM_REQUEST_RETRY_INTERVAL_DEFAULT_VALUE = 5000;


    AbstractTrainDownloads::AbstractTrainDownloads( CWnd* window, bool canSubmitMultiple )
        : m_windowToUpdate(window),
          m_canSubmitMultiple( canSubmitMultiple ),
          m_requests(),
          m_requestRetryInterval(RPARAM_REQUEST_RETRY_INTERVAL_DEFAULT_VALUE),
          m_running(false)
    {
        FUNCTION_ENTRY( "AbstractTrainDownloads" );

        TA_ASSERT(m_windowToUpdate != NULL, "Window to update is null");

        // if not set, or the conversion to an unsigned int fails
        // then the default value set above will be used
        if ( RunParams::getInstance().isSet(RPARAM_REQUEST_RETRY_INTERVAL.c_str()) )
        {
            std::stringstream temp;
            temp << RunParams::getInstance().get(RPARAM_REQUEST_RETRY_INTERVAL.c_str());

            temp >> m_requestRetryInterval;
        }

        LOG_GENERIC (SourceInfo, DebugUtil::DebugInfo,
            "AbstractTrainDownloads is using a retry interval of %d ms", m_requestRetryInterval);

        FUNCTION_EXIT;
    }


    AbstractTrainDownloads::~AbstractTrainDownloads()
    {
        FUNCTION_ENTRY( "~AbstractTrainDownloads" );

        // just in case the thread didnt get stopped
        terminateAndWait();

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::startDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool autoRetry)
    {
        FUNCTION_ENTRY( "startDownload" );

        TA_THREADGUARD( m_requestLock );

        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator iter = trainList.begin();
              iter != trainList.end(); ++iter  )
        {

            // before inserting, see if another the same is already there
            DownloadInformationList::iterator requestIter;
            for ( requestIter = m_requests.begin(); m_requests.end() != requestIter; ++requestIter )
            {
                if ( *iter == requestIter->trainId )
                {
                    // found it
                    break;
                }
            }

            // if there was another in the list already
            if ( requestIter != m_requests.end() )
            {
                LOG_GENERIC ( SourceInfo, DebugUtil::DebugDebug,
                    "Found a an existing request for train %d, type %d",
                    *iter, getDownloadType() );

                // just update the retry field if necessary
                if ( true == autoRetry )
                {
                    requestIter->autoRetry = autoRetry;
                }

                // based on the download state, update the display
                if (requestIter->inProgress)
                {
                    notifyDownloadInProgress( *requestIter );
                }
                else
                {
                    notifyDownloadPending( *requestIter );
                }
            }
            // if there wasnt one in progress
            else
            {
                LOG_GENERIC ( SourceInfo, DebugUtil::DebugDebug,
                    "This is a new request for train %d, type %d",
                    *iter, getDownloadType() );

                DownloadInformation newRequest;
                newRequest.trainId = *iter;
                newRequest.autoRetry = autoRetry;
                newRequest.retryCount = 0;
                newRequest.lastError = "";
                newRequest.inProgress = false;

                // insert new request
                m_requests.push_back( newRequest );

                // uodate the gui with the latest changes
                notifyDownloadPending( m_requests.back() );
            }
        }

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::stopRetrying(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList)
    {
        FUNCTION_ENTRY( "stopRetrying" );

        TA_THREADGUARD( m_requestLock );

        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator iter = trainList.begin();
              iter != trainList.end(); ++iter )
        {
            // see if the request is there
            DownloadInformationList::iterator requestIter;
            for ( requestIter = m_requests.begin(); m_requests.end() != requestIter; ++requestIter )
            {
                if ( *iter == requestIter->trainId )
                {
                    // found it
                    break;
                }
            }

            if ( requestIter != m_requests.end() )
            {
                // set auto retry to false
                requestIter->autoRetry = false;

                // if the item is in progress, we can only stop it retrying
                // the request must remain in the list until it completes
                if ( true == requestIter->inProgress )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                        "Set auto retry to false for active download. Train: %d Type: %d",
                        *iter, getDownloadType() );

                    // update the display text to remove the "auto retry on failure" text
                    notifyDownloadInProgress( *requestIter );
                }
                // if its not in progress, remove it from the list
                else
                {
                    // send an update to the window to say that this was cancelled
                    notifyDownloadCancelled( *requestIter );

                    // remove the request
                    m_requests.erase( requestIter );

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                        "Removed queued download attempt. Train: %d Type: %d",
                        *iter, getDownloadType() );
                }

            }
        }

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::processDownloadStart(TA_IRS_Bus::CommonTypes::TrainIdType trainId)
    {
        FUNCTION_ENTRY( "processDownloadStart" );

        TA_THREADGUARD( m_requestLock );

        // find the entry in the map
        DownloadInformationList::iterator requestIter;
        for ( requestIter = m_requests.begin(); m_requests.end() != requestIter; ++requestIter )
        {
            if ( trainId == requestIter->trainId )
            {
                // found it
                break;
            }
        }

        // if its found
        if ( requestIter != m_requests.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                "Notified of a download/upgrade in progress. Setting status to in progress. Train: %d Type: %d",
                trainId, getDownloadType() );

            // update it to be in progress
            requestIter->inProgress = true;

            // notify window of download in progress
            notifyDownloadInProgress( *requestIter );
        }
        // it wasnt found, add it to the list
        else
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                "Notified of a download/upgrade in progress. Train: %d Type: %d",
                trainId, getDownloadType() );

            DownloadInformation newRequest;
            newRequest.trainId = trainId;
            newRequest.autoRetry = false;
            newRequest.retryCount = 1;
            newRequest.lastError = "";
            newRequest.inProgress = true;

            // insert new request
            m_requests.push_back( newRequest );

            // notify window of download in progress
            notifyDownloadInProgress( m_requests.back() );
        }

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::processDownloadFinish(TA_IRS_Bus::CommonTypes::TrainIdType trainId, bool success, const std::string& errorDetails)
    {
        FUNCTION_ENTRY( "processDownloadFinish" );

        TA_THREADGUARD( m_requestLock );

        // item that will update the GUI
        DownloadInformation download;
        download.trainId = trainId;
        download.autoRetry = false;
        download.retryCount = 1;
        download.lastError = errorDetails;
        download.inProgress = false;

        // find the entry in the map
        DownloadInformationList::iterator requestIter;
        for ( requestIter = m_requests.begin(); m_requests.end() != requestIter; ++requestIter )
        {
            if ( trainId == requestIter->trainId )
            {
                // found it
                break;
            }
        }

        // if its found
        if ( requestIter != m_requests.end() )
        {
            // update the download status
            requestIter->inProgress = false;
            requestIter->lastError = errorDetails;

            // if the download succeeded, ensure that auto retry is set to false
            // beacuse it wont be auto retrying
            if ( true == success )
            {
                requestIter->autoRetry = false;
            }

            // copy the details into the structure defined above
            download = *requestIter;

            // the download only remains in the list if it failed, and is set to auto retry.
            if ( false == requestIter->autoRetry )
            {
                // if its not set to auto retry - remove it from the list
                m_requests.erase(requestIter);

                LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug,
                    "Notified of a download/upgrade complete, removing from list. Train: %d Type: %d",
                    trainId, getDownloadType() );
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Notified of a download/upgrade complete. Train: %d Type: %d",
                    trainId, getDownloadType());
            }
        }

        // send an update that says this download finished
        notifyDownloadFinished( download, success );

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::run()
    {
        FUNCTION_ENTRY( "run" );

        m_running = true;

        while ( true == m_running )
        {
            // find all items that need to be submitted to the agent
            std::vector< unsigned int > trainsToSubmit;

            if ( true == m_canSubmitMultiple )
            {
                TA_THREADGUARD( m_requestLock );
                for ( DownloadInformationList::iterator iter = m_requests.begin();
                      iter != m_requests.end(); ++iter )
                {
                    // anything not in progress needs to be submitted
                    if ( false == iter->inProgress )
                    {
                        trainsToSubmit.push_back( iter->trainId );

                        // increment the retry count
                        ++(iter->retryCount);

                        // set it to in progress so that the item cant be removed from m_requests
                        iter->inProgress = true;
                    }
                }
            }
            else
            {
                // only a single request can be submitted,
                // if no other request is in progress

                bool requestInProgress = false;

                TA_THREADGUARD( m_requestLock );

                for ( DownloadInformationList::iterator iter = m_requests.begin();
                      iter != m_requests.end(); ++iter )
                {
                    // anything not in progress needs to be submitted
                    if ( true == iter->inProgress )
                    {
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Train %d is in progress", iter->trainId);
                        requestInProgress = true;

                        // found one in progress, cannot continue
                        break;
                    }
                }

                // if there are none in progress
                if ( false == requestInProgress )
                {
                    // queue the first one
                    if ( false == m_requests.empty() )
                    {
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Train %d insert to request list", m_requests.front().trainId);
                        trainsToSubmit.push_back( m_requests.front().trainId );

                        // increment the retry count
                        ++(m_requests.front().retryCount);

                        // set it to in progress so that the item cant be removed from m_requests
                        m_requests.front().inProgress = true;
                    }
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "There is none download request");
					}
                }
            }

            // only need to do this if there are actions to perform
            if ( false == trainsToSubmit.empty() )
            {
                // submit all the requests
                TA_Base_Core::TrainList corbaTrainList;
                corbaTrainList.length( trainsToSubmit.size() );

                unsigned int i = 0;
                for ( std::vector< unsigned int >::iterator iter = trainsToSubmit.begin();
                      iter != trainsToSubmit.end(); ++iter )
                {
                    corbaTrainList[ i++ ] = *iter;
                }

                std::string errorString;
                bool success = false;
                try
                {
                    success = processRequests( corbaTrainList, errorString );
                }
                catch (...)
                {
                    // protect against child classes not catching something
                }

                // now the GUI will be updated to in progress as the results of each train are received asynchronously
                // however, if the submission failed, then the trains must be marked failed as there will
                // be no events received.

                if ( false == success )
                {
                    TA_THREADGUARD( m_requestLock );

                    // update all to failed
                    for ( std::vector< unsigned int >::iterator trainIter = trainsToSubmit.begin();
                          trainIter != trainsToSubmit.end(); ++trainIter )
                    {
                        processDownloadFinish( *trainIter, false, errorString );
                    }
                }

            }

            // wait for the retry period
            m_semaphore.timedWait( m_requestRetryInterval );
        }

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        m_running = false;
        m_semaphore.post();

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::notifyDownloadPending(const DownloadInformation& download)
    {
        FUNCTION_ENTRY( "notifyDownloadPending" );

        DownloadStatusDisplay update;

        update.trainId = download.trainId;
        update.downloadType = getDownloadType();
        update.autoRetry = download.autoRetry;
        update.currentState = DownloadStatusDisplay::Pending;

        // now build the display message
        std::stringstream message;
        message << "Starting.";

        if ( true == download.autoRetry )
        {
            message << " Auto Retry on Failure.";
        }

        update.displayMessage = message.str();

        // send the message to the window
        sendUpdate( update );

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::notifyDownloadInProgress(const DownloadInformation& download)
    {
        FUNCTION_ENTRY( "notifyDownloadInProgress" );

        DownloadStatusDisplay update;

        update.trainId = download.trainId;
        update.downloadType = getDownloadType();
        update.autoRetry = download.autoRetry;
        update.currentState = DownloadStatusDisplay::InProgress;

        // now build the display message
        std::stringstream message;
        // if it is not auto retry, or it this is the first attempt
        if ( ( false == download.autoRetry ) ||
             ( download.retryCount < 2 ) )
        {
            message << "In Progress.";

            if ( true == download.autoRetry )
            {
                message << " Auto Retry on Failure.";
            }
        }
        else
        {
            // it is auto retry and the retry count is > 1
            message << "Auto Retrying. " << download.retryCount << " attempts.";
        }

        update.displayMessage = message.str();

        // send the message to the window
        sendUpdate(update);

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::notifyDownloadFinished(const DownloadInformation& download, bool success)
    {
        FUNCTION_ENTRY( "notifyDownloadFinished" );

        DownloadStatusDisplay update;

        update.trainId = download.trainId;
        update.downloadType = getDownloadType();
        update.autoRetry = download.autoRetry;
        update.currentState = DownloadStatusDisplay::Finished;

        // now build the display message
        std::stringstream message;
        if ( true == success )
        {
            if ( download.retryCount > 1 )
            {
                message << "Completed after " << download.retryCount << " attempts.";
            }
            else
            {
                message << "Completed.";
            }
        }
        else
        {
            // if it is going to auto retry
            if ( true == download.autoRetry )
            {
                message << "Failed - Auto Retrying. " << download.retryCount << " attempts.";
            }
            else
            {
                // if multiple attemps have been made (ie it was set to auto retry at one stage)
                if ( download.retryCount > 1 )
                {
                    message << "Failed after " << download.retryCount << " attempts. " << download.lastError;
                }
                else
                {
                    // it has just failed and will not be retried
                    message << "Failed. " << download.lastError;
                }
            }
        }

        update.displayMessage = message.str();

        // send the message to the window
        sendUpdate(update);

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::notifyDownloadCancelled(const DownloadInformation& download)
    {
        FUNCTION_ENTRY( "notifyDownloadCancelled" );

        DownloadStatusDisplay update;

        update.trainId = download.trainId;
        update.downloadType = getDownloadType();
        update.autoRetry = download.autoRetry;
        update.currentState = DownloadStatusDisplay::Finished;

        // now build the display message
        std::stringstream message;
        message << "Failed. Cancelled after " << download.retryCount
                << " attempts. " << download.lastError;
        update.displayMessage = message.str();

        // send the message to the window
        sendUpdate( update );

        FUNCTION_EXIT;
    }


    void AbstractTrainDownloads::sendUpdate(const DownloadStatusDisplay& update)
    {
        FUNCTION_ENTRY( "sendUpdate" );

        TA_ASSERT(m_windowToUpdate != NULL, "window to update is null");

        DownloadStatusDisplay* updatePointer = new DownloadStatusDisplay( update );

        int ret = m_windowToUpdate->PostMessage( WM_UPDATE_DOWNLOADLIST, reinterpret_cast<unsigned int>( updatePointer ) );
        if ( ret == 0 )
        {
            LOG_GENERIC(
            SourceInfo, DebugUtil::DebugError,
            "Error posting WM_UPDATE_DOWNLOADLIST message.");

            // clean up
            delete updatePointer;
            updatePointer = NULL;
        }

        FUNCTION_EXIT;
    }


} // TA_IRS_App

