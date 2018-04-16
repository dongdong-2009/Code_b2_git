/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert V
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(PaTypes_H)
#define PaTypes_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"

#include <vector>
#include <string>

namespace TA_IRS_Bus
{

    namespace PaTypes
    {

        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        struct MessageSchedule
        {
            /**
             * The period for message repetition (message is repeated every 'period' seconds)
             */
            unsigned short period;


            /**
             * The duration for message play
             * (message is repeated every 'period' seconds for 'duration' seconds)
             */
            unsigned short duration;


            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * For PA version info. This must be sent system wide - a broadcast message.
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        struct TrainLibraryInfo
        {
            CommonTypes::TrainIdType trainId;

            //unsigned short libraryVersion;
            unsigned long libraryVersion;


            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        typedef std::vector< TrainLibraryInfo > TrainLibraryInfoList;


        /**
         * @author Robert V
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        typedef unsigned char AnnouncementId;


        /**
         * Applies to a global PA Live announcement, rather than a train by train basis
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        enum EPaAnnouncementUpdateType
        {
            AudioCallLost = 0
        };


        /**
         * This is an update for a live announcement as a whole.
         * It applies to all trains within the given announcement,
         * rather than a single train in a single announcement.
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        struct PaAnnouncementUpdate
        {
            /**
             * The announcement ID this update is for
             */
            AnnouncementId announcementId;

            /**
             * The type of the update - the event that occurred.
             */
            EPaAnnouncementUpdateType updateType;


            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        enum EPaUpdateType
        {
            /**
             * This is the result of a PA command sent to the train.
             * See the commandResult field for futher status
             */
            PaCommandResult = 0,
            /**
             * The announcement has been reset by the train
             */
            AnnouncementReset = 1,
            /**
             * A pre-recorded or cyclic announcement message has completed
             * See the completionStatus field for the completion result
             */
            CompletedAnnouncement = 2,
            /**
             * The train in the live announcement is ready for the audio connection (The begin operation)
             */
            LiveAnnouncementReady = 3,
            /**
             * The live announcement requires a PA continue or the audio will be dropped
             */
            LiveAnnouncementContinueRequired = 4,
            /**
             * Occurs when a continue request is not received for a train during a live announcement.
             * This can indicate the train communication has been lost
             */
            LiveAnnouncementContinueTimeout = 5
        };


        /**
         * This is used to convey the result of a train command asynchronously.
         * It indicates success or failure, and if failure the error type.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        enum EPaCommandResult
        {
            PaProceeding = 1,
            PaInUse = 2,
            PaInUseOptionToOverride = 3,
            PaInUseFirstCycleCancelled = 4,
            PaReset = 5,
            PaFailed = 6,
            PaDifferentOrigin = 7,
            PaDifferentId = 8,
            PaDifferentVersion = 9,
            /**
             * A bad command response was received, with a bad checksum error.
             */
            BadCommandBadChecksum = 10,
            /**
             * A bad command response was received, with an unknown command error.
             */
            BadCommandUnknownCommand = 11,
            /**
             * A bad command response was received, with a bad parameter error.
             */
            BadCommandBadParameter = 12,
            /**
             * A bad command response was received, with an unexpected command error.
             */
            BadCommandUnexpectedCommand = 13,
            InvalidTrainError = 14,
            TransmissionFailureError = 15,
            TransactionTimeoutError = 16,
            TransactionCancelledError = 17,
            MpuChangeoverError = 18
        };


        /**
         * PA Notifications
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:31 PM
         */
        struct PaUpdateEvent
        {
            /**
             * The train this update is for
             */
            CommonTypes::TrainIdType trainId;

            /**
             * The announcement ID this update is for
             */
            AnnouncementId announcementId;

            /**
             * The session ID that is responsible for this announcement
             */
            std::string sessionId;

            EPaUpdateType updateType;

            /**
             * This is the result of the PA command
             */
            EPaCommandResult commandResult;

            /**
             * This is the result of the cyclic or pre recorded message completion
             */
            ProtocolPaTypes::EMessageCompletionStatus completionStatus;


            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string trainLibraryInfoListToString( const TrainLibraryInfoList& trainLibraryInfoList );
        std::string paAnnouncementUpdateTypeToString( EPaAnnouncementUpdateType paAnnouncementUpdateType );
        std::string paUpdateTypeToString( EPaUpdateType paUpdateType );
        std::string paCommandResultToString( EPaCommandResult paCommandResult );

    }

}

#endif // !defined(PaTypes_H)
