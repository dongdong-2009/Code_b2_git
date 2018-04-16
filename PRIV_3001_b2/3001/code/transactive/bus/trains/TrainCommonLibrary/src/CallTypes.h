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

#if !defined(CallTypes_H)
#define CallTypes_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include <string>
#include <vector>


namespace TA_IRS_Bus
{

    namespace CallTypes
    {

        /**
         * A list of normal non-train radio users
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        typedef std::vector< std::string > ItsiList;


        /**
         * This indicates an OCC call request has been activated or updated. It is sent to
         * the call banner and Radio Manager to display the call request. Additional
         * information about the train can be obtained from the train selection component.
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        struct OccCallRequest
        {
            /**
             * The train ID
             */
            CommonTypes::TrainIdType trainId;

            /**
             * The time of the call request
             */
            time_t timestamp;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * This indicates an OCC call request has been answered or cancelled. Radio
         * Manager and banner should remove the call entry.
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        struct OccCallReset
        {
            /**
             * The train ID
             */
			
			OccCallReset ()
			{
				bResetDueToFailure = false;
			}

            CommonTypes::TrainIdType trainId;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

			bool bResetDueToFailure; 
        };

        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        typedef std::vector< OccCallRequest > OccCallRequestList;


        /**
         * Represents the result of a call train command
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        enum EVoiceCallResult
        {
            /**
             * A call train response was received, and the call is proceeding
             */
            CallTrainProceeding = 1,
            /**
             * A call train response was received, and the call failed due to the audio being
             * in use.
             */
            CallTrainAudioRouteInUse = 2,
            /**
             * A call train response was received, and the call failed due to the audio being
             * in use, it will succeed if an option to override is given.
             */
            CallTrainAudioRouteInUseOptionToOverride = 3,
            /**
             * A call train response was received, and the call failed due to the call being
             * reset
             */
            CallTrainReset = 4,
            /**
             * A bad command response was received, with a bad checksum error.
             */
            BadCommandBadChecksum = 5,
            /**
             * A bad command response was received, with an unknown command error.
             */
            BadCommandUnknownCommand = 6,
            /**
             * A bad command response was received, with a bad parameter error.
              */
            BadCommandBadParameter = 7,
            /**
             * A bad command response was received, with an unexpected command error.
             */
            BadCommandUnexpectedCommand = 8,
            /**
             * A bad command response was received, with an invalid train error.
             */
            InvalidTrainError = 9,
            /**
             * A bad command response was received, with a transmission failure error.
             */
            TransmissionFailureError = 10,
            /**
             * A bad command response was received, with a transaction timeout error.
             */
            TransactionTimeoutError = 11,
            /**
             * A bad command response was received, with a transaction cancelled error.
             */
            TransactionCancelledError = 12,
            /**
             * A bad command response was received, with a Mpu changeover error.
             */
            MpuChangeoverError = 13
        };


        /**
         * The result of a voice call to a single train
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        struct VoiceCallResponse
        {
            /**
             * The ID of the train
             */
            CommonTypes::TrainIdType trainId;

            /**
             * The result of the command, whether it succeeded, or the error type if it failed
             */
            EVoiceCallResult result;

            /**
             * Optional extra information for debug logging purposes
             */
            std::string details;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        typedef std::vector< VoiceCallResponse > VoiceCallResponseList;


        /**
         * The result of a call to one or more trains. Each train that was included in the
         * call request will have an entry here.
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        struct DriverCallResult
        {
            /**
             * The console ID the call was made from. 0 if no call made.
             */
            unsigned long consoleId;

            /**
             * The call ID of the call. 0 if no call made.
             */
            unsigned long callId;

            /**
             * Result of the operation on each train
             */
            VoiceCallResponseList resultList;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * The type of call in progress to the train
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        enum ETrainCallType
        {
            NoCall = 0,
            TrainCallOcc = 1,
            OccCallTrain = 2
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        enum ECallPriority
        {
            Priority0 = 0,
            Priority1 = 1,
            Priority2 = 2,
            Priority3 = 3,
            Priority4 = 4, 
            Priority5 = 5,
            Priority6 = 6,
            Priority7 = 7,
            Priority8 = 8,
            Priority9 = 9,
            Priority10 = 10,
            Priority11 = 11,
            Priority12 = 12,
            Priority13 = 13,
            Priority14 = 14,
            Priority15 = 15,
            DefaultNormalPriority = 16,
            DefaultEmergencyPriority = 17
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:40 PM
         */
        enum ECallType
        {
            SingleCall = 0,
            GroupUnacknowledgeCall = 1,
            GroupAcknowledgeCall = 2,
            GroupBroadcastCall = 3
        };

		/**
         * This indicates an Train Changeover has been activated. It is sent to
         * the RadioManager to end the call based on resource id and pop up a message
         * to inform the operator that the call will be terminated due to Train changeover.
         */
        struct TrainChangeover
        {
            /**
             * The train ID
             */
            CommonTypes::TrainIdType trainId;

            /**
             * The radio resource id to be terminated
             */
            unsigned long resourceId;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string itsiListToString( const ItsiList& itsiList );
        std::string occCallRequestListToString( const OccCallRequestList& occCallRequestList );
        std::string voiceCallResultToString( EVoiceCallResult voiceCallResult );
        std::string voiceCallResponseListToString( const VoiceCallResponseList& voiceCallResponseList );
        std::string trainCallTypeTypeToString( ETrainCallType trainCallType );
        std::string callPriorityToString( ECallPriority callPriority );
        std::string callTypeToString( ECallType callType );

    }

}

#endif // !defined(CallTypes_H)
