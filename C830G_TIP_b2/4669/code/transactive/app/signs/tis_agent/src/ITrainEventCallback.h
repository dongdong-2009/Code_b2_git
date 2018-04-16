/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(ITrainEventCallback_H)
#define ITrainEventCallback_H


#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <string>


namespace TA_IRS_App
{
    /**
     * Used to notify of updates from the train agent. For command results, and message library downloads and updates.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:39:11 PM
     */
    class ITrainEventCallback
    {

    public:

        /**
         * Whether a message library or time schedule
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:39:11 PM
         */
        enum ETrainLibraryType
        {
        	MessageLibrary = 0,
        	TimeSchedule = 1
        };


        ITrainEventCallback() {

        }

        virtual ~ITrainEventCallback() {

        }



        /**
         * This will update the library versions stored on the train in train wakeup.
         * It will override any next version downloaded to the train.
         * 
         * @param trainId    The train number
         * @param libraryVersion    The library version on the train
         * @param scheduleVersion    The time schedule version on the train
         */
        virtual void updateTrainLibraryVersions(TA_IRS_Bus::CommonTypes::TrainIdType trainId, unsigned short libraryVersion,
			unsigned short nextLibraryVersion, unsigned short scheduleVersion, unsigned short nextScheduleVersion) =0;


        /**
         * This indicates that a train dowload has started successfully.
         * 
         * @param trainId    The train
         * @param downloadType    Whether a message library or time schedule
         * @param version
         */
        virtual void trainDownloadStarted(TA_IRS_Bus::CommonTypes::TrainIdType trainId, TA_IRS_App::ITrainEventCallback::ETrainLibraryType downloadType, unsigned short version) =0;


        /**
         * This indicates that a train dowload has completed.
         * 
         * @param trainId    The train
         * @param downloadType    Whether a message library or time schedule
         * @param success    Whether the dowload completed successfully or not
         * @param statusString    The download status string to display to the user
         * @param version    The library or schedule version downloaded to the train if successful
         */
        virtual void trainDownloadComplete(TA_IRS_Bus::CommonTypes::TrainIdType trainId, TA_IRS_App::ITrainEventCallback::ETrainLibraryType downloadType, bool success, std::string statusString, unsigned short version) =0;


        /**
         * This indicates that a train upgrade has completed.
         * 
         * @param trainId    The train
         * @param downloadType    Whether a message library or time schedule
         * @param success    Whether the dowload completed successfully or not
         * @param statusString    The download status string to display to the user
         * @param version    The library or schedule version upgraded on the train if successful
         */
        virtual void trainUpgradeComplete(TA_IRS_Bus::CommonTypes::TrainIdType trainId, TA_IRS_App::ITrainEventCallback::ETrainLibraryType downloadType, bool success, std::string statusString, unsigned short version) =0;


        /**
         * This notifies that a free text message command sent to a train has completed, and its status.
         * 
         * @param trainId    The train this request is for
         * @param timestamp    The time the request was initiated (this must be sent back to the original client GUI)
         * @param sessionId    The originating session ID
         * @param displayParameters    The original message parameters
         * @param success    whether the command succeeded or not
         * @param statusString    The error status (for user display)
         */
        virtual void freeTextMessageRequestComplete(TA_IRS_Bus::CommonTypes::TrainIdType trainId, time_t timestamp, std::string sessionId, const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters, bool success, std::string statusString) =0;


        /**
         * This notifies that a predefined message command sent to a train has completed, and its status.
         * 
         * @param trainId    The train this request is for
         * @param timestamp    The time the request was initiated (this must be sent back to the original client GUI)
         * @param sessionId    The originating session ID
         * @param displayParameters    The original message parameters
         * @param success    whether the command succeeded or not
         * @param statusString    The error status (for user display)
         */
        virtual void predefinedMessageRequestComplete(TA_IRS_Bus::CommonTypes::TrainIdType trainId, time_t timestamp, std::string sessionId, const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters, bool success, std::string statusString) =0;


        /**
         * This notifies that a clear command sent to a train has completed, and its status.
         * 
         * @param trainId    The train this request is for
         * @param timestamp    The time the request was initiated (this must be sent back to the original client GUI)
         * @param sessionId    The originating session ID
         * @param displayParameters    The original message parameters
         * @param success    whether the command succeeded or not
         * @param statusString    The error status (for user display)
         */
        virtual void clearRequestComplete(TA_IRS_Bus::CommonTypes::TrainIdType trainId, time_t timestamp, std::string sessionId, const TA_Base_Core::TTISMessageResetParameters& displayParameters, bool success, std::string statusString) =0;


        /**
         * This will verify that the train has matching library and schedule versions to the ISCS versions.
         * It will also ensure that if the alarm should be raised by this agent it is, or also if the alarm should be closed it is.
         * 
         * This is called when a train's version updates, or when a train moves.
         * 
         * @param trainId    The train to verify
         */
        virtual void verifyTrainDataVersions(TA_IRS_Bus::CommonTypes::TrainIdType trainId) =0;

    };

}
#endif // !defined(ITrainEventCallback_H)
