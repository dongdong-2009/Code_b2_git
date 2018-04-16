/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/AbstractTrainDownloads.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * Messages are sent to the given window, they are:
  * WM_UPDATE_DOWNLOADLIST (wParam is a DownloadStatusDisplay*, it must be deleted when used)
  */


#if !defined(AbstractTrainDownloads_H)
#define AbstractTrainDownloads_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/TimedWaitSemaphore.h"

#include <list>
#include <map>
#include <string>


namespace TA_IRS_App
{

    /**
     * @version 1.0
     * @created 04-Apr-2008 1:47:51 PM
     */
    enum TrainDownloadType
    {
        LibraryDownload,
        ScheduleDownload,
        LibraryUpgrade,
        ScheduleUpgrade
    };


    /**
     * @version 1.0
     * @created 04-Apr-2008 1:47:51 PM
     */
    struct DownloadStatusDisplay
    {

    public:

        /**
         * @version 1.0
         * @created 04-Apr-2008 1:47:51 PM
         */
        enum Status
        {
            /**
             * A download has been requested, but not attempted yet
             */
            Pending,
            /**
             * A download is in progress
             */
            InProgress,
            /**
             * A download was in progress, and is failed or succeeded
             */
            Finished
        };


        TA_IRS_Bus::CommonTypes::TrainIdType trainId;


        TrainDownloadType downloadType;


        bool autoRetry;


        Status currentState;


        std::string displayMessage;

    };


    /**
     * @version 1.0
     * @created 04-Apr-2008 1:47:50 PM
     */
    class AbstractTrainDownloads : public TA_Base_Core::Thread
    {

    public:


        /**
         * Constructor
         *
         * @param window    the window to receive the update messages
         */
        AbstractTrainDownloads( CWnd* window, bool canSubmitMultiple );


        /**
         * Destructor
         */
        virtual ~AbstractTrainDownloads();


        /**
         * Starts a download/upgrade to the given trains
         *
         * @param trainList    The list of trains to start the download on
         * @param autoRetry    whether to retry on failure or not
         */
        void startDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool autoRetry);


        /**
         * Stops a download/upgrade to the given trains automatically retrying.
         *
         * @param trainList    The trains to stop retrying
         */
        void stopRetrying(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList);


        /**
         * When a download is started, this is called.
         * It updates the internal list.
         * It also causes the window to be updated.
         *
         * @param trainId    The ID of the train that has started downloading
         */
        void processDownloadStart(TA_IRS_Bus::CommonTypes::TrainIdType trainId);


        /**
         * When a download has finished, update the status of the internal list.
         * It also causes the window to be updated.
         *
         * @param trainId    The ID of the train that has finished downloading
         * @param success    Whether the download completed successfully or not
         * @param errorDetails    If it failed, some error details should be included.
         */
        void processDownloadFinish(TA_IRS_Bus::CommonTypes::TrainIdType trainId, bool success = true, const std::string& errorDetails = "");


        /**
         * Execute the pending downloads for this object.
         */
        void run();


        /**
         * stops the thread
         */
        virtual void terminate();

    protected:

        /**
         * This will perform the actual download/upgrade request to the TIS agent.
         * It will return success or failure to submit the operation, and optionally an error code. Further status is updated by incoming messages.
         *
         * @return true if the request was submitted, false if there was an error encountered
         *
         * @param trainList    The list of trains to perform the actual request on
         * @param errorString    Optionally this will contain an error string if the download fails
         */
        virtual bool processRequests(const TA_Base_Core::TrainList& trainList, std::string& errorString) =0;


        /**
         * Subclasses will return what type of train download they do.
         *
         * @return A download/upgrade type
         */
        virtual TrainDownloadType getDownloadType() =0;

    private:

        /**
         * The information about a download in progress or queued to be started.
         * @version 1.0
         * @created 04-Apr-2008 1:47:51 PM
         */
        struct DownloadInformation
        {

        public:

            /**
             * the train id
             */
            TA_IRS_Bus::CommonTypes::TrainIdType trainId;


            /**
             * whether to auto retry on failure
             */
            bool autoRetry;


            /**
             * the number of retries so fat
             */
            unsigned int retryCount;


            /**
             * the last error encountered
             */
            std::string lastError;


            /**
             * whether its currentl in progress
             */
            bool inProgress;

        };

        /**
         * @author adamr
         * @version 1.0
         * @created 04-Apr-2008 1:47:51 PM
         */
        typedef std::list< DownloadInformation > DownloadInformationList;


        /**
         * Send an update to the window telling it a download is queued
         *
         * @param download    The download information structure
         */
        void notifyDownloadPending(const DownloadInformation& download);


        /**
         * Send an update to the window telling it a download has started
         *
         * @param download    The download information structure
         */
        void notifyDownloadInProgress(const DownloadInformation& download);


        /**
         * Send an update to the window telling it a download has finished
         *
         * @param download    The download information structure
         * @param success    whether the download succeeded or not
         */
        void notifyDownloadFinished(const DownloadInformation& download, bool success);


        /**
         * Send an update to the window telling it a download has been cancelled
         *
         * @param download    The download information structure
         */
        void notifyDownloadCancelled(const DownloadInformation& download);


        /**
         * Posts an update to the interested window.
         *
         * @param update    update the update to post
         */
        void sendUpdate(const DownloadStatusDisplay& update);


        static const std::string RPARAM_REQUEST_RETRY_INTERVAL;


        static const unsigned long RPARAM_REQUEST_RETRY_INTERVAL_DEFAULT_VALUE;


        /**
         * the callback window (for messages)
         */
        CWnd* m_windowToUpdate;


        /**
         * Multiple upgrades can be performed simulataneously,
         * but not multiple downloads
         */
        bool m_canSubmitMultiple;


        /**
         * This is each request that is to be, or has been submitted to the agent.
         * It holds the last result, number of retries, and retry status.
         */
        DownloadInformationList m_requests;


        /**
         * lock for list
         */
        TA_Base_Core::ReEntrantThreadLockable m_requestLock;


        /**
         * the retry interval for requests
         */
        unsigned long m_requestRetryInterval;


        /**
         * is this thread running?
         */
        volatile bool m_running;


        /**
         * The semaphore controlling the run loop
         */
        TA_Base_Core::TimedWaitSemaphore m_semaphore;


        TA_Base_Core::NonReEntrantThreadLockable m_bufferedRequestListLock;

    };


}

#endif // !defined
