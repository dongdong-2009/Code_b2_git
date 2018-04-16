/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainDownloadManager.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * It manages each type of download/upgrade request, and has instances of
  * TrainLibraryDownloads, TrainLibraryUpgrades, TrainScheduleDownloads, TrainScheduleUpgrades
  *
  * It is a thread that periodically retries requests. It will update the window given in the
  * constructor with WM_UPDATE_DOWNLOADLIST messages.
  * wParam is a TrainDownloadStatus*, it must be deleted when used)
  *
  * Downloads started from other TTIS manager GUIs will also be received from
  * the TIS agent and shown in the list.
  */

#if !defined(TrainDownloadManager_H)
#define TrainDownloadManager_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/signs/ttis_manager/src/TrainLibraryDownloads.h"
#include "app/signs/ttis_manager/src/TrainLibraryUpgrades.h"
#include "app/signs/ttis_manager/src/TrainScheduleDownloads.h"
#include "app/signs/ttis_manager/src/TrainScheduleUpgrades.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/message/idl/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"


namespace TA_IRS_App
{

    class TrainDownloadManager : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    
    public:
	    

        /** 
          * TrainDownloadManager
          *
          * Constructor
          *
          * @param window   the window to receive the update messages.
          */
        TrainDownloadManager(CWnd* window);

        virtual ~TrainDownloadManager();


        /** 
          * startLibraryDownload
          *
          * Starts a library download to the given train.
          *
          * @param trainId The train to download to
          * @param retry whether to retry on failure or not
          *
          */
        void startLibraryDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool retry);


        /**
          * startLibraryUpgrade
          *
          * Starts a library upgrade to the given train
          *
          * @param trainList    The trains to upgrade
          * @param retry whether to retry on failure or not
          */
        void startLibraryUpgrade(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool retry);


        /**
          * startScheduleDownload
          *
          * Starts a schedule download to the given train.
          *
          * @param trainList    The trains to upgrade
          * @param retry whether to retry on failure or not
          */
        void startScheduleDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool retry);


        /**
          * startScheduleUpgrade
          *
          * Starts a schedule upgrade to the given train
          *
          * @param trainList    The trains to upgrade
          * @param retry whether to retry on failure or not
          */
        void startScheduleUpgrade(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool retry);


        /**
          * stopLibraryDownload
          *
          * Stops a library download from auto-retrying.
          *
          * @param trainList    The trains to upgrade
          */
        void stopLibraryDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList);


        /**
          * stopLibraryUpgrade
          *
          * stops a library upgrade from auto-retrying.
          *
          * @param trainList    The trains to upgrade
          */
        void stopLibraryUpgrade(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList);


        /**
          * stopScheduleDownload
          *
          * stops a schedule download from auto-retrying.
          *
          * @param trainList    The trains to upgrade
          */
        void stopScheduleDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList);


        /**
          * stopScheduleUpgrade
          *
          * stops a schedule upgrade from auto-retrying.
          *
          * @param trainList    The trains to upgrade
          */
        void stopScheduleUpgrade(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList);


        /**
          * start
          *
          * Called to start the worker threads
          */
        void start();


        /**
          * terminateAndWait
          *
          * stops the threads.
          */
        void terminateAndWait();


        /**
         * receiveSpecialisedMessage
         *
         * Receive notifications from the Tis Agent.
         *
         * @param message  the message, containing a download complete notification
         *                 the download may have succeeded or failed
         *
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );


    private:

        /**
         * subscribeToMessages
         *
         * Subscribes to messages for updates to TIS downloads
         *
         * @throw DatabaseException  if there is a problem communicating
         *                           with the database.
         *
         * @throw DataException      if a Tis Agent does not exist at the
         *                           given location, in the database.
         */
        void subscribeToMessages();


        /**
          * buildDownloadList
          *
          * gets the current downloads and sends any required updates.
          */
        void buildDownloadList();


        /**
          * processDownloadChange
          *
          * A download has been started or completed.
          * passes it on to the relevant class that handles the type of request it is.
          *
          * @param downloadChange The download/upgrade change
          */
        void processDownloadChange(const TA_Base_Core::TrainDownloadStatus& downloadChange);

        // the objects that handle the requests
        TrainLibraryDownloads m_libraryDownloads;
        TrainLibraryUpgrades m_libraryUpgrades;
        TrainScheduleDownloads m_scheduleDownloads;
        TrainScheduleUpgrades m_scheduleUpgrades;
    };


} // TA_IRS_App

#endif // !defined
