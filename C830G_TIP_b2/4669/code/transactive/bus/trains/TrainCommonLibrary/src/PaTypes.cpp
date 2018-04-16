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

#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace PaTypes
    {

        std::string MessageSchedule::getLogString() const
        {
            std::ostringstream result;

            result << "MessageSchedule [period: "
                   << period
                   << "][duration: " << duration
                   << "]";

            return result.str();
        }


        std::string TrainLibraryInfo::getLogString() const
        {
            std::ostringstream result;

            result << "TrainLibraryInfo [trainId: "
                   << static_cast< unsigned int>( trainId )
                   << "][libraryVersion: " << libraryVersion
                   << "]";

            return result.str();
        }


        std::string trainLibraryInfoListToString( const TrainLibraryInfoList& trainLibraryInfoList )
        {
            return structListToString< TrainLibraryInfoList >( trainLibraryInfoList );
        }


        std::string paAnnouncementUpdateTypeToString( EPaAnnouncementUpdateType paAnnouncementUpdateType )
        {
            switch ( paAnnouncementUpdateType )
            {

                case AudioCallLost:
                    return "Audio Call Lost";
                    break;

                default:
                    break;
            }

            return "Unknown EPaAnnouncementUpdateType";
        }


        std::string PaAnnouncementUpdate::getLogString() const
        {
            std::ostringstream result;

            result << "PaAnnouncementUpdate [announcementId: "
                   << static_cast< unsigned int>( announcementId )
                   << "][updateType: " << paAnnouncementUpdateTypeToString( updateType )
                   << "]";

            return result.str();
        }


        std::string paUpdateTypeToString( EPaUpdateType paUpdateType )
        {
            switch ( paUpdateType )
            {

                case PaCommandResult:
                    return "PaCommandResult";
                    break;

                case AnnouncementReset:
                    return "AnnouncementReset";
                    break;

                case CompletedAnnouncement:
                    return "CompletedAnnouncement";
                    break;

                case LiveAnnouncementReady:
                    return "LiveAnnouncementReady";
                    break;

                case LiveAnnouncementContinueRequired:
                    return "LiveAnnouncementContinueRequired";
                    break;

                case LiveAnnouncementContinueTimeout:
                    return "LiveAnnouncementContinueTimeout";
                    break;

                default:
                    break;
            }

            return "Unknown EPaUpdateType";
        }


        std::string paCommandResultToString( EPaCommandResult paCommandResult )
        {
            switch ( paCommandResult )
            {

                case PaProceeding:
                    return "PaProceeding";
                    break;

                case PaInUse:
                    return "PaInUse";
                    break;

                case PaInUseOptionToOverride:
                    return "PaInUseOptionToOverride";
                    break;

                case PaInUseFirstCycleCancelled:
                    return "PaInUseFirstCycleCancelled";
                    break;

                case PaReset:
                    return "PaReset";
                    break;

                case PaFailed:
                    return "PaFailed";
                    break;

                case PaDifferentOrigin:
                    return "PaDifferentOrigin";
                    break;

                case PaDifferentId:
                    return "PaDifferentId";
                    break;

                case PaDifferentVersion:
                    return "PaDifferentVersion";
                    break;

                case BadCommandBadChecksum:
                    return "BadCommandBadChecksum";
                    break;

                case BadCommandUnknownCommand:
                    return "BadCommandUnknownCommand";
                    break;

                case BadCommandBadParameter:
                    return "BadCommandBadParameter";
                    break;

                case BadCommandUnexpectedCommand:
                    return "BadCommandUnexpectedCommand";
                    break;

                case InvalidTrainError:
                    return "InvalidTrainError";
                    break;

                case TransmissionFailureError:
                    return "TransmissionFailureError";
                    break;

                case TransactionTimeoutError:
                    return "TransactionTimeoutError";
                    break;

                case TransactionCancelledError:
                    return "TransactionCancelledError";
                    break;

                case MpuChangeoverError:
                    return "MpuChangeoverError";
                    break;

                default:
                    break;
            }

            return "Unknown EPaCommandResult";
        }


        std::string PaUpdateEvent::getLogString() const
        {
            std::ostringstream result;

            result << "PaUpdateEvent [trainId: " << static_cast< unsigned int>( trainId )
                   << "][announcementId: " << static_cast< unsigned int>( announcementId )
                   << "][sessionId: " << sessionId
                   << "][updateType: " << paUpdateTypeToString( updateType )
                   << "][commandResult: " << paCommandResultToString( commandResult )
                   << "][completionStatus: " << ProtocolPaTypes::messageCompletionStatusToString( completionStatus )
                   << "]";

            return result.str();
        }

    }

}

