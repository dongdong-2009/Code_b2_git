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

#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace CallTypes
    {

        std::string itsiListToString( const ItsiList& itsiList )
        {
            return listToString< ItsiList >( itsiList );
        }


        std::string OccCallRequest::getLogString() const
        {
            std::ostringstream result;

            result << "OccCallRequest [trainId: "
                   << static_cast< unsigned int>( trainId )
                   << "][timestamp: " << timestamp
                   << "]";

            return result.str();
        };


        std::string OccCallReset::getLogString() const
        {
            std::ostringstream result;

            result << "OccCallReset [trainId: "
                   << static_cast< unsigned int>( trainId )
				   << "[Reset Due to Failure :  "
				   << static_cast< unsigned int>( bResetDueToFailure )
                   << "]";

            return result.str();
        };


        std::string occCallRequestListToString( const OccCallRequestList& occCallRequestList )
        {
            return structListToString< OccCallRequestList >( occCallRequestList );
        }


        std::string voiceCallResultToString( EVoiceCallResult voiceCallResult )
        {
            switch ( voiceCallResult )
            {

                case CallTrainProceeding:
                    return "CallTrainProceeding";
                    //break;

                case CallTrainAudioRouteInUse:
                    return "CallTrainAudioRouteInUse";
                    //break;

                case CallTrainAudioRouteInUseOptionToOverride:
                    return "CallTrainAudioRouteInUseOptionToOverride";
                    //break;

                case CallTrainReset:
                    return "CallTrainReset";
                   // break;

                case BadCommandBadChecksum:
                    return "BadCommandBadChecksum";
                    //break;

                case BadCommandUnknownCommand:
                    return "BadCommandUnknownCommand";
                    //break;

                case BadCommandBadParameter:
                    return "BadCommandBadParameter";
                    //break;

                case BadCommandUnexpectedCommand:
                    return "BadCommandUnexpectedCommand";
                    //break;

                case InvalidTrainError:
                    return "InvalidTrainError";
                    //break;

                case TransmissionFailureError:
                    return "TransmissionFailureError";
                    //break;

                case TransactionTimeoutError:
                    return "TransactionTimeoutError";
                    //break;

                case TransactionCancelledError:
                    return "TransactionCancelledError";
                    //break;

                case MpuChangeoverError:
                    return "MpuChangeoverError";
                    //break;

                default:
                    break;
            }

            return "Unknown EVoiceCallResult";
        }


        std::string VoiceCallResponse::getLogString() const
        {
            std::ostringstream logString;

            logString << "VoiceCallResponse [trainId: "
                      << static_cast< unsigned int>( trainId )
                      << "][result: " << result
                      << "][details: " << details
                      << "]";

            return logString.str();
        }


        std::string voiceCallResponseListToString( const VoiceCallResponseList& voiceCallResponseList )
        {
            return structListToString< VoiceCallResponseList >( voiceCallResponseList );
        }


        std::string DriverCallResult::getLogString() const
        {
            std::ostringstream result;

            result << "DriverCallResult [consoleId: "
                   << consoleId
                   << "][callId: " << callId
                   << "]" << std::endl;

            result << voiceCallResponseListToString( resultList );

            return result.str();
        };


        std::string trainCallTypeTypeToString( ETrainCallType trainCallType )
        {
            switch ( trainCallType )
            {

                case NoCall:
                    return "NoCall";
                    //break;

                case TrainCallOcc:
                    return "TrainCallOcc";
                    //break;

                case OccCallTrain:
                    return "OccCallTrain";
                    //break;

                default:
                    break;
            }

            return "Unknown ETrainCallType";
        }


        std::string callPriorityToString( ECallPriority callPriority )
        {
            switch ( callPriority )
            {

                case Priority0:
                    return "Priority0";
                    //break;

                case Priority1:
                    return "Priority1";
                    //break;

                case Priority2:
                    return "Priority2";
                    //break;

                case Priority3:
                    return "Priority3";
                    //break;

                case Priority4:
                    return "Priority4";
                    //break;

                case Priority5:
                    return "Priority5";
                    //break;

                case Priority6:
                    return "Priority6";
                    //break;

                case Priority7:
                    return "Priority7";
                    //break;

                case Priority8:
                    return "Priority8";
                    //break;

                case Priority9:
                    return "Priority9";
                    //break;

                case Priority10:
                    return "Priority10";
                    //break;

                case Priority11:
                    return "Priority11";
                    //break;

                case Priority12:
                    return "Priority12";
                    //break;

                case Priority13:
                    return "Priority13";
                    //break;

                case Priority14:
                    return "Priority14";
                    //break;

                case Priority15:
                    return "Priority15";
                    //break;

                case DefaultNormalPriority:
                    return "DefaultNormalPriority";
                    //break;

                case DefaultEmergencyPriority:
                    return "DefaultEmergencyPriority";
                    //break;

                default:
                    break;
            }

            return "Unknown ECallPriority";
        }


        std::string callTypeToString( ECallType callType )
        {
            switch ( callType )
            {

                case SingleCall:
                    return "SingleCall";
                    //break;

                case GroupUnacknowledgeCall:
                    return "GroupUnacknowledgeCall";
                    //break;

                case GroupAcknowledgeCall:
                    return "GroupAcknowledgeCall";
                    //break;

                case GroupBroadcastCall:
                    return "GroupBroadcastCall";
                   // break;

                default:
                    break;
            }

            return "Unknown ECallType";
        }

		std::string TrainChangeover::getLogString() const
		{
			std::ostringstream result;

			result << "TrainChangover [trainId: "
				<< static_cast< unsigned int>( trainId )
				<< "][Radio resource ID :  "
				<< static_cast< unsigned int>( resourceId )
				<< "]";

			return result.str();
		};

    }

}

