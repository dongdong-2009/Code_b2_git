/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/TrainDownloadUpdateSubscriber.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to listen for download complete update messages from the Train Agent
  *
  */

#if !defined(TRAINDOWNLOADUPDATESUBSCRIBER_H)
#define TRAINDOWNLOADUPDATESUBSCRIBER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ITISTrainCommandsCorbaDef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/threads/src/Thread.h"


/////////////////////////////////////////////////////////////////////////////
// TrainDownloadListUpdateSubscriber window

namespace TA_IRS_App
{

    class TrainDownloadUpdateSubscriber :
           public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    
    public:
	    
         /** 
          * TrainDownloadListUpdateSubscriber
          *
          * Constructor
          *
          * @param window   the window to receive the update messages.
          */
        TrainDownloadUpdateSubscriber( unsigned long locationKey );

        virtual ~TrainDownloadUpdateSubscriber();

        /**
         * Receive notifications from the Train Agent.
         *
         * @param message  the message, containing a download complete notification
         *                 the download may have succeeded or failed
         * 
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );

    private:


        /** 
          * updateVersionList
          *
          * gets the current versions and sends any required updates.
          *
          */
        void updateVersionList();


        /** 
          * updateDownloadList
          *
          * gets the current downloads and sends any required updates.
          *
          */
        void updateDownloadList();

        /**
         * Subscribes to notifications for updates to TIS downloads
         *
         * @throw DatabaseException  if there is a problem communicating
         *                           with the database.
         *
         * @throw DataException      if a Train Agent does not exist at the
         *                           given location, in the database.
         */
        void subscribeToNotifications();


        /** 
          * processTisUpdate
          *
          * processes a received TIS update.
          *
          * @param tisNotification it tells us that a download has completed
          */
        void processTisUpdate(const TA_Base_Bus::ITISTrainCommandsCorbaDef::TISNotificationEvent& tisNotification);

        // a lock for the download list
        TA_Base_Core::NonReEntrantThreadLockable m_downloadLock;

		unsigned long m_locationKey;
    
    };


} // TA_App

#endif // !defined
