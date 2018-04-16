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
#if !defined(TtisTypes_H)
#define TtisTypes_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"

#include <boost/variant.hpp>

#include <vector>


namespace TA_IRS_Bus
{

    namespace TtisTypes
    {

        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        typedef std::vector< unsigned char > LibraryDataType;


        /**
         * This is a global update, sent out to all agents when the TTIS predefined
         * library or schedule version changes. It only updates on the current in use
         * version, it does not update the recently downloaded, but not upgraded version.
         *
         * It is sent as the result of a train wakeup, or a successful upgrade command.
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        struct TisLibraryVersionUpdate
        {
            /**
             * The train the update is for
             */
            CommonTypes::TrainIdType trainId;

            /**
             * This is the current library version on the train.
             */
            unsigned long libraryVersion;

            /**
             * This is the current schedule version on the train.
             */
            unsigned long scheduleVersion;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        typedef std::vector< TisLibraryVersionUpdate > TisLibraryVersionList;


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        enum ETisUpdateType
        {
            /**
             * A library download has started
             */
            LibraryDownloadStart = 1,
            /**
             * A library download has completed
             */
            LibraryDownloadComplete = 2,
            /**
             * The result of a library upgrade command
             */
            LibraryUpgradeResult = 3,
            /**
             * A schedule download has started
             */
            ScheduleDownloadStart = 4,
            /**
             * A schedule download has completed
             */
            ScheduleDownloadComplete = 5,
            /**
             * The result of a schedule upgrade command
             */
            ScheduleUpgradeResult = 6
        };


        /**
         * Result of a TTIS command
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        enum ETtisCommandResult
        {
            /**
             * The operation was successful
             */
            TtisSuccess = 1,
            /**
             * The operation was in use
             */
            TtisInUse = 2,
            /**
             * The operation was reset
             */
            TtisReset = 3,
            /**
             * The operation failed
             */
            TtisFailed = 4,
            /**
             * The operation was different version
             */
            TtisDifferentVersion = 5,
            /**
             * A bad command response was received, with a bad checksum error.
             */
            BadCommandBadChecksum = 6,
            /**
             * A bad command response was received, with an unknown command error.
             */
            BadCommandUnknownCommand = 7,
            /**
             * A bad command response was received, with a bad parameter error.
             */
            BadCommandBadParameter = 8,
            /**
             * A bad command response was received, with an unexpected command error.
             */
            BadCommandUnexpectedCommand = 9,
            /**
             * A bad command response was received, with an invalid train error.
             */
            InvalidTrainError = 10,
            /**
             * A bad command response was received, with a transmission failure error.
             */
            TransmissionFailureError = 11,
            /**
             * A bad command response was received, with a transaction timeout error.
             */
            TransactionTimeoutError = 12,
            /**
             * A bad command response was received, with a canceled transaction  error.
             */
            TransactionCancelledError = 13,
            /**
             * A bad command response was received, with a Mpu changeover error.
             */
            MpuChangeoverError = 14,

			TtisDutyHandover = 15,

			TtisEndDwnloadFailed = 16
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        enum EDownloadResult
        {
            /**
             * The download completed successfully
             */
            Success = 1,
            /**
             * The download failed due to a circuit mode failure
             */
            TransferFailed = 2,
            /**
             * The download failed according to the train
             */
            TimsFailed = 3,
            /**
             * The transfer completed, but the TIMS response was not received in the timeout
             * period
             */
            TimedOut = 4,
            /**
             * An MPU changeover occurred during the transfer, it is considered failed
             */
            MpuChangeover = 5,
			/**
			 * Cancelled by Occ
			 */
			Cancelled = 6,
			/**
			 * Library Upgraded
			 */
		    Upgraded = 7
        };


        /**
         * This is the update sent when a library/schedule download or upgrade is
         * performed.
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        struct TisLibraryUpdate
        {
            /**
             * The train this update is for
             */
            CommonTypes::TrainIdType trainId;

            /**
             * The type of update this update carries
             */
            ETisUpdateType updateType;

            /**
             * This is applicable to all update types except LibraryDownloadComplete and
             * ScheduleDownloadComplete.
             * It indicates the result of the TIMS message that was sent to trigger the given
             * action.
             * If the update type is a download start - this indicates whether the download
             * started, and if not, which error.
             * If the update type is an upgradet - this indicates whether the upgrade was
             * successful, and if not, which error.
             */
            ETtisCommandResult commandStatus;

            /**
             * The result of the download, this is applicable to the LibraryDownloadComplete
             * and ScheduleDownloadComplete update types. It is sent only after the transfer
             * was successfully started, and has since completed. This is the result of the
             * download process.
             */
            EDownloadResult downloadResult;

            /**
             * This is applicable to the *DownloadComplete update types, if the download is
             * successful it indicates the version successfully downloaded to the train.
             * This is also applicable to the *Upgrade update types, if the upgrade is
             * successful it indicates the new current version on the train.
             */
            unsigned long version;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * This is the original TTIS command. It caters only for the display/reset
         * operations.
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        enum ETtisOriginalDisplayCommand
        {
            TtisFreeTextCommand = 0,
            TtisPredefinedCommand = 1,
            TtisResetCommand = 2
        };


        /**
         * The TTIS library that is being downloaded (library, schedule, or none)
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        enum ETtisDownloadType
        {
            TtisNone = 0,
            TtisLibrary = 1,
            TtisSchedule = 2
        };

		enum EPDSLibraryDownloadConst
		{
			PDSLibDownloadMakeDataCall = 1,
			PDSLibDownloadBegin,
			PDSLibDownloadResend,
			PDSLibDownloadCancelled,
			PDSLibDownloadCallDisconnection,
			PDSLibDownloadStart,
			PDSLibDownloadComplete,
			PDSLibDownloadTimerExpireFailed,
			PDSLibDownloadTimerExpireSuccess,
			PDSLibDownloadTimerEventWait,
			PDSLibDownloadTimerSuccessWait,
			PDSLibDownloadTimerStopEventRecv
		};

        /**
         * This is used to store the raw frame data from a TTD Frame.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */

        struct TtisFrameData
        {
            /**
             * The raw frame data, extracted from the TTD Frame object.
             */
            LibraryDataType frameData;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        typedef std::vector< ProtocolTtisTypes::ETtisPid > TtisPidList;


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:37 PM
         */
        struct TimeInterval
        {
            unsigned char startHour;

            unsigned char startMin;

            unsigned char stopHour;

            unsigned char stopMin;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:38 PM
         */
        typedef std::vector< TtisFrameData > TtisFrameList;


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:38 PM
         */
        struct TtisFreeTextDisplayParameters
        {
            /**
             * The list of PIDs to display on. Multiple PIDs will result in multiple messages
             * to each train.
             */
            TtisPidList pidList;

            /**
             * The font size on the PID
             */
            ProtocolTtisTypes::ETtisFontSize fontSize;

            /**
             * The justification display attribute
             */
            ProtocolTtisTypes::ETtisJustification justification;

            /**
             * The intensity display attribute
             */
            ProtocolTtisTypes::ETtisIntensity intensity;

            /**
             * The display mode display attribute
             */
            ProtocolTtisTypes::ETtisDisplayMode displayMode;

            /**
             * The message priority
             */
            unsigned char perRun;
           

            /**
             * The actual message text, maximum 125 characters
             */
            std::string message;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:38 PM
         */
        struct TtisPredefinedDisplayParameters
        {

            /**
             * The list of PIDs to display on. Multiple PIDs will result in multiple messages
             * to each train.
             */
            TtisPidList pidList;

            /**
             * The TTIS library version the message came from
             */
            unsigned int libraryVersion;

       

            /**
             * The message number in the secion of the predefined message library
             */
            unsigned short messageId;

            /**
             * The message priority
             */
           unsigned char perRun;
          
            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:38 PM
         */
        struct TtisResetParameters
        {
            /**
             * The list of PIDs to display on. Multiple PIDs will result in multiple messages
             * to each train.
             */
            TtisPidList pidList;

            /**
             * The types of messages to reset
             */
            ProtocolTtisTypes::ETtisResetMessageType messageType;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * A container that holds the display parameters for any type of display request
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:38 PM
         */
        class DisplayParameters
        {

        public:

            /**
             *
             * constructor
             */
            DisplayParameters();

            /**
             *
             * destructor
             */
            virtual ~DisplayParameters();

            /**
             * Stores the free text parameters internally
             *
             * @param freeTextParameters    The free text parameters to set
             */
            DisplayParameters( const TtisFreeTextDisplayParameters& freeTextParameters );

            /**
             * Stores the predefined parameters internally
             *
             * @param predefinedParameters    The predefined parameters to set
             */
            DisplayParameters( const TtisPredefinedDisplayParameters& predefinedParameters );

            /**
             * Stores the reset parameters internally
             *
             * @param resetParameters    The reset parameters to set
             */
            DisplayParameters( const TtisResetParameters& resetParameters );

            /**
             * This gets the original command, and hence the data type this object holds
             *
             * @return the original command
             */
            ETtisOriginalDisplayCommand getOriginalCommand() const;//limin, added const

            /**
             * Stores the free text parameters internally
             *
             * @param freeTextParameters    The free text parameters to set
             */
            void operator=( const TtisFreeTextDisplayParameters& freeTextParameters );

            /**
             * Stores the predefined parameters internally
             *
             * @param predefinedParameters    The predefined parameters to set
             */
            void operator=( const TtisPredefinedDisplayParameters& predefinedParameters );

            /**
             * Stores the reset parameters internally
             *
             * @param resetParameters    The reset parameters to set
             */
            void operator=( const TtisResetParameters& resetParameters );

            /**
             * Gets the free text display parameters.
             * If this does not hold free text parameters this will assert
             * @see getOriginalCommand
             *
             * @return the free text parameters
             */
            TtisFreeTextDisplayParameters getFreeTextParameters() const;

            /**
             * Gets the predefined display parameters.
             * If this does not hold predefined parameters this will assert
             * @see getOriginalCommand
             *
             * @return the predefined parameters
             */
            TtisPredefinedDisplayParameters getPredefinedParameters() const;

            /**
             * Gets the reset parameters.
             * If this does not hold reset parameters this will assert
             * @see getOriginalCommand
             *
             * @return the reset parameters
             */
            TtisResetParameters getResetParameters() const;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;


        private:

            /**
             * @author adamr
             * @version 1.0
             * @created 13-Nov-2007 3:50:38 PM
             */
            typedef boost::variant< TtisResetParameters, TtisPredefinedDisplayParameters, TtisFreeTextDisplayParameters > DisplayParameterVariant;
            ETtisOriginalDisplayCommand m_originalCommand;

            /**
             * Stores the actual display parameters
             */
            DisplayParameterVariant m_displayParameters;

        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:38 PM
         */
        struct TisReturnStatus
        {
            /**
             * The train the request was for
             */
            CommonTypes::TrainIdType trainId;

            /**
             * The operator who submitted the message
             */
            std::string sessionId;

            /**
             * the time_t of the time the message was sent. This coupled with the session ID
             * can provide a type of transaction ID for the GUI.
             */
            time_t timestamp;

            /**
             * The result of the command, inidicating success/failure and the failure type
             */
            ETtisCommandResult result;

            /**
             * These are the original display parameters (for auditing purposes etc).
             * The union type will tell the original request type (_d is an
             * ETtisOriginalCommand)
             */
            DisplayParameters originalParameters;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * Utility functions to print simple types to string
         */
        std::string libraryDataTypeToString( const LibraryDataType& libraryDataType );
        std::string tisLibraryVersionListToString( const TisLibraryVersionList& tisLibraryVersionList );
        std::string tisUpdateTypeToString( ETisUpdateType tisUpdateType );
        std::string ttisCommandResultToString( ETtisCommandResult ttisCommandResult );
        std::string downloadResultToString( EDownloadResult downloadResult );
        std::string ttisOriginalDisplayCommandToString( ETtisOriginalDisplayCommand ttisOriginalDisplayCommand );
        std::string ttisDownloadTypeToString( ETtisDownloadType ttisDownloadType );
        std::string ttisPidListToString( const TtisPidList& ttisPidList );
        std::string ttisFrameListToString( const TtisFrameList& ttisFrameList );

    }

}

#endif // !defined(TtisTypes_H)
