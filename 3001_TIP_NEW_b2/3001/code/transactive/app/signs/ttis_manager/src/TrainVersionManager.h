/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainVersionManager.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to listen for train library and
  * time schedule version updates.
  *
  * Update messages are sent to the given window, they are:
  * WM_UPDATE_TRAINVERSION
  * wParam is a TrainLibraryAndTimeScheduleVersion*, it must be deleted when used
  *
  */

#if !defined(TrainVersionManager_H)
#define TrainVersionManager_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/message/idl/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <map>


namespace TA_IRS_App
{

    class TrainVersionManager
        : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
    
    public:
	    
        // used to decouple from corba type
        // its also stored in a map, so storing the corba type would be inefficient
        struct TrainLibraryAndTimeScheduleVersion
        {
            TA_IRS_Bus::CommonTypes::TrainIdType  trainNumber;
            unsigned short  predefinedLibraryVersion;
		    unsigned short  nextPredefinedLibraryVersion;
		    unsigned short  trainTimeScheduleVersion;
            unsigned short  nextTrainTimeScheduleVersion;

        };


        /**
          * TrainVersionManager
          *
          * Constructor
          *
          * @param window the window to receive the update messages.
          */
        TrainVersionManager(CWnd* window);


        virtual ~TrainVersionManager();


        /**
         * Receive notifications from the Tis Agent.
         *
         * @param message  the message, containing a download complete notification
         *                 the download may have succeeded or failed
         *
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );


        /**
          * getVersionForTrain
          *
          * Get the time schedule and pre-defined message versions for a particualr train.
          *
          * @param trainNumber The train id.
          *
          * @return the version information, if the train is not found, ie the TIS agent
          *         doesnt know it's status, then the versions will show 0.
          */
        TrainLibraryAndTimeScheduleVersion getVersionForTrain(TA_IRS_Bus::CommonTypes::TrainIdType trainNumber);


    private:

        /**
          * buildVersionList
          *
          * gets the current versions and sends any required updates.
          *
          */
        void buildVersionList();


        /**
         * subscribeToMessages
         *
         * Subscribes to messages for updates to TIS downloads and versions
         *
         * @throw DatabaseException  if there is a problem communicating
         *                           with the database.
         *
         * @throw DataException      if a Tis Agent does not exist at the
         *                           given location, in the database.
         */
        void subscribeToMessages();


        /**
          * processVersionChange
          *
          * processes a received TisTrainDataVersionUpdate message.
          *
          * @param downloadChange it tells us that a library or time schedule version has changed
          */
        void processVersionChange(const TA_Base_Core::TrainDataVersion& versionChange);


        // the callback window (for messages)
        CWnd* m_windowToUpdate;


        // the current version for each train
        typedef std::map<TA_IRS_Bus::CommonTypes::TrainIdType, TrainLibraryAndTimeScheduleVersion> TrainVersionMap;


        TrainVersionMap m_trainVersions;


        // a lock for the version map
        TA_Base_Core::NonReEntrantThreadLockable m_versionLock;
    };


} // TA_IRS_App

#endif // !defined
