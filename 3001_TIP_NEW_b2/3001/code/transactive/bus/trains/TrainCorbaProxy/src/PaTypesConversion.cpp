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

#include "bus/trains/TrainCorbaProxy/src/PaTypesConversion.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainPaCorba::PaAnnouncementUpdate PaTypesConversion::convertToCorba( const PaTypes::PaAnnouncementUpdate& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::PaAnnouncementUpdate paAnnouncementUpdate;

        paAnnouncementUpdate.updateType = convertToCorba( cppType.updateType );
        paAnnouncementUpdate.annId = cppType.announcementId;

        return  paAnnouncementUpdate;
    }


    TA_Base_Bus::ITrainPaCorba::EPaAnnouncementUpdateType PaTypesConversion::convertToCorba( const PaTypes::EPaAnnouncementUpdateType& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::EPaAnnouncementUpdateType ePaAnnouncementUpdateType;

        switch ( cppType )
        {

            case PaTypes::AudioCallLost:
                ePaAnnouncementUpdateType = TA_Base_Bus::ITrainPaCorba::AudioCallLost;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  ePaAnnouncementUpdateType;
    }


    TA_Base_Bus::ITrainPaCorba::TrainLibraryInfo PaTypesConversion::convertToCorba( const PaTypes::TrainLibraryInfo& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::TrainLibraryInfo trainLibraryInfo;

        trainLibraryInfo.trainId = cppType.trainId;
        trainLibraryInfo.libraryVersion = cppType.libraryVersion;

        return  trainLibraryInfo;
    }


    TA_Base_Bus::ITrainPaCorba::EMessageCompletionStatus PaTypesConversion::convertToCorba( const ProtocolPaTypes::EMessageCompletionStatus& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::EMessageCompletionStatus eMessageCompletionStatus;

        switch ( cppType )
        {

            case ProtocolPaTypes::Successful:
                eMessageCompletionStatus = TA_Base_Bus::ITrainPaCorba::Successful;
                break;

            case ProtocolPaTypes::InterruptedByOcc:
                eMessageCompletionStatus = TA_Base_Bus::ITrainPaCorba::InterruptedByOcc;
                break;

            case ProtocolPaTypes::InterruptedByPsc:
                eMessageCompletionStatus = TA_Base_Bus::ITrainPaCorba::InterruptedByPsc;
                break;

            case ProtocolPaTypes::InterruptedByDriver:
                eMessageCompletionStatus = TA_Base_Bus::ITrainPaCorba::InterruptedByDriver;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eMessageCompletionStatus;
    }


    TA_Base_Bus::ITrainPaCorba::EPaUpdateType PaTypesConversion::convertToCorba( const PaTypes::EPaUpdateType& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::EPaUpdateType ePaUpdateType;

        switch ( cppType )
        {

            case PaTypes::PaCommandResult:
                ePaUpdateType = TA_Base_Bus::ITrainPaCorba::PaCommandResult;
                break;

            case PaTypes::AnnouncementReset:
                ePaUpdateType = TA_Base_Bus::ITrainPaCorba::AnnouncementReset;
                break;

            case PaTypes::CompletedAnnouncement:
                ePaUpdateType = TA_Base_Bus::ITrainPaCorba::CompletedAnnouncement;
                break;

            case PaTypes::LiveAnnouncementReady:
                ePaUpdateType = TA_Base_Bus::ITrainPaCorba::LiveAnnouncementReady;
                break;

            case PaTypes::LiveAnnouncementContinueRequired:
                ePaUpdateType = TA_Base_Bus::ITrainPaCorba::LiveAnnouncementContinueRequired;
                break;

            case PaTypes::LiveAnnouncementContinueTimeout:
                ePaUpdateType = TA_Base_Bus::ITrainPaCorba::LiveAnnouncementContinueTimeout;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  ePaUpdateType;
    }


    TA_Base_Bus::ITrainPaCorba::MessageSchedule PaTypesConversion::convertToCorba( const PaTypes::MessageSchedule& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::MessageSchedule messageSchedule;

        messageSchedule.duration = cppType.duration;
        messageSchedule.period = cppType.period;

        return  messageSchedule;
    }


    TA_Base_Bus::ITrainPaCorba::EPaCommandResult PaTypesConversion::convertToCorba( const PaTypes::EPaCommandResult& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::EPaCommandResult ePaCommandResult;

        switch ( cppType )
        {

            case PaTypes::PaProceeding:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaProceeding;
                break;

            case PaTypes::PaInUse:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaInUse;
                break;

            case PaTypes::PaInUseOptionToOverride:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaInUseOptionToOverride;
                break;

            case PaTypes::PaInUseFirstCycleCancelled:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaInUseFirstCycleCancelled;
                break;

            case PaTypes::PaReset:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaReset;
                break;

            case PaTypes::PaFailed:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaFailed;
                break;

            case PaTypes::PaDifferentOrigin:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaDifferentOrigin;
                break;

            case PaTypes::PaDifferentId:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaDifferentId;
                break;

            case PaTypes::PaDifferentVersion:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::PaDifferentVersion;
                break;

            case PaTypes::BadCommandBadChecksum:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::BadCommandBadChecksum;
                break;

            case PaTypes::BadCommandUnknownCommand:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::BadCommandUnknownCommand;
                break;

            case PaTypes::BadCommandBadParameter:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::BadCommandBadParameter;
                break;

            case PaTypes::BadCommandUnexpectedCommand:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::BadCommandUnexpectedCommand;
                break;

            case PaTypes::InvalidTrainError:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::InvalidTrainError;
                break;

            case PaTypes::TransmissionFailureError:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::TransmissionFailureError;
                break;

            case PaTypes::TransactionTimeoutError:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::TransactionTimeoutError;
                break;

            case PaTypes::TransactionCancelledError:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::TransactionCancelledError;
                break;

            case PaTypes::MpuChangeoverError:
                ePaCommandResult = TA_Base_Bus::ITrainPaCorba::MpuChangeoverError;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  ePaCommandResult;
    }


    TA_Base_Bus::ITrainPaCorba::AnnouncementId PaTypesConversion::convertToCorba( const PaTypes::AnnouncementId& cppType )
    {
        return  cppType;
    }


    TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList PaTypesConversion::convertToCorba( const PaTypes::TrainLibraryInfoList& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList trainLibraryInfoList;

        trainLibraryInfoList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            trainLibraryInfoList[i] = convertToCorba( cppType[i] );
        }

        return  trainLibraryInfoList;
    }


    TA_Base_Bus::ITrainPaCorba::PaUpdateEvent PaTypesConversion::convertToCorba( const PaTypes::PaUpdateEvent& cppType )
    {
        TA_Base_Bus::ITrainPaCorba::PaUpdateEvent paUpdateEvent;

        paUpdateEvent.annId = cppType.announcementId;
        paUpdateEvent.commandResult = convertToCorba( cppType.commandResult );
        paUpdateEvent.completionStatus = convertToCorba( cppType.completionStatus );
        paUpdateEvent.sessionId = cppType.sessionId.c_str();
        paUpdateEvent.trainId = cppType.trainId ;
        paUpdateEvent.updateType = convertToCorba( cppType.updateType );

        return  paUpdateEvent;
    }


    PaTypes::EPaAnnouncementUpdateType PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::EPaAnnouncementUpdateType& CorbaType )
    {
        PaTypes::EPaAnnouncementUpdateType ePaAnnouncementUpdateType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPaCorba::AudioCallLost:
                ePaAnnouncementUpdateType = PaTypes::AudioCallLost;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  ePaAnnouncementUpdateType;
    }


    PaTypes::PaUpdateEvent PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::PaUpdateEvent& CorbaType )
    {
        PaTypes::PaUpdateEvent paUpdateEvent;

        paUpdateEvent.announcementId = CorbaType.annId;
        paUpdateEvent.commandResult = convertToCpp( CorbaType.commandResult );
        paUpdateEvent.completionStatus = convertToCpp( CorbaType.completionStatus );
        paUpdateEvent.sessionId = CorbaType.sessionId.in();
        paUpdateEvent.trainId = CorbaType.trainId ;
        paUpdateEvent.updateType = convertToCpp( CorbaType.updateType );

        return  paUpdateEvent;
    }


    PaTypes::TrainLibraryInfoList PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList& CorbaType )
    {
        PaTypes::TrainLibraryInfoList trainLibraryInfoList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            trainLibraryInfoList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return  trainLibraryInfoList;
    }


    PaTypes::AnnouncementId PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::AnnouncementId& CorbaType )
    {
        return  CorbaType;
    }


    PaTypes::EPaCommandResult PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::EPaCommandResult& CorbaType )
    {
        PaTypes::EPaCommandResult ePaCommandResult;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPaCorba::PaProceeding:
                ePaCommandResult = PaTypes::PaProceeding;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaInUse:
                ePaCommandResult = PaTypes::PaInUse;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaInUseOptionToOverride:
                ePaCommandResult = PaTypes::PaInUseOptionToOverride;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaInUseFirstCycleCancelled:
                ePaCommandResult = PaTypes::PaInUseFirstCycleCancelled;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaReset:
                ePaCommandResult = PaTypes::PaReset;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaFailed:
                ePaCommandResult = PaTypes::PaFailed;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaDifferentOrigin:
                ePaCommandResult = PaTypes::PaDifferentOrigin;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaDifferentId:
                ePaCommandResult = PaTypes::PaDifferentId;
                break;

            case TA_Base_Bus::ITrainPaCorba::PaDifferentVersion:
                ePaCommandResult = PaTypes::PaDifferentVersion;
                break;

            case TA_Base_Bus::ITrainPaCorba::BadCommandBadChecksum:
                ePaCommandResult = PaTypes::BadCommandBadChecksum;
                break;

            case TA_Base_Bus::ITrainPaCorba::BadCommandUnknownCommand:
                ePaCommandResult = PaTypes::BadCommandUnknownCommand;
                break;

            case TA_Base_Bus::ITrainPaCorba::BadCommandBadParameter:
                ePaCommandResult = PaTypes::BadCommandBadParameter;
                break;

            case TA_Base_Bus::ITrainPaCorba::BadCommandUnexpectedCommand:
                ePaCommandResult = PaTypes::BadCommandUnexpectedCommand;
                break;

            case TA_Base_Bus::ITrainPaCorba::InvalidTrainError:
                ePaCommandResult = PaTypes::InvalidTrainError;
                break;

            case TA_Base_Bus::ITrainPaCorba::TransmissionFailureError:
                ePaCommandResult = PaTypes::TransmissionFailureError;
                break;

            case TA_Base_Bus::ITrainPaCorba::TransactionTimeoutError:
                ePaCommandResult = PaTypes::TransactionTimeoutError;
                break;

            case TA_Base_Bus::ITrainPaCorba::TransactionCancelledError:
                ePaCommandResult = PaTypes::TransactionCancelledError;
                break;

            case TA_Base_Bus::ITrainPaCorba::MpuChangeoverError:
                ePaCommandResult = PaTypes::MpuChangeoverError;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  ePaCommandResult;
    }


    PaTypes::MessageSchedule PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::MessageSchedule& CorbaType )
    {
        PaTypes::MessageSchedule messageSchedule;

        messageSchedule.duration = CorbaType.duration;
        messageSchedule.period = CorbaType.period;

        return  messageSchedule;
    }


    PaTypes::EPaUpdateType PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::EPaUpdateType& CorbaType )
    {
        PaTypes::EPaUpdateType ePaUpdateType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPaCorba::PaCommandResult:
                ePaUpdateType = PaTypes::PaCommandResult;
                break;

            case TA_Base_Bus::ITrainPaCorba::AnnouncementReset:
                ePaUpdateType = PaTypes::AnnouncementReset;
                break;

            case TA_Base_Bus::ITrainPaCorba::CompletedAnnouncement:
                ePaUpdateType = PaTypes::CompletedAnnouncement;
                break;

            case TA_Base_Bus::ITrainPaCorba::LiveAnnouncementReady:
                ePaUpdateType = PaTypes::LiveAnnouncementReady;
                break;

            case TA_Base_Bus::ITrainPaCorba::LiveAnnouncementContinueRequired:
                ePaUpdateType = PaTypes::LiveAnnouncementContinueRequired;
                break;

            case TA_Base_Bus::ITrainPaCorba::LiveAnnouncementContinueTimeout:
                ePaUpdateType = PaTypes::LiveAnnouncementContinueTimeout;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  ePaUpdateType;
    }


    ProtocolPaTypes::EMessageCompletionStatus PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::EMessageCompletionStatus& CorbaType )
    {
        ProtocolPaTypes::EMessageCompletionStatus eMessageCompletionStatus;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPaCorba::Successful:
                eMessageCompletionStatus = ProtocolPaTypes::Successful;
                break;

            case TA_Base_Bus::ITrainPaCorba::InterruptedByOcc:
                eMessageCompletionStatus = ProtocolPaTypes::InterruptedByOcc;
                break;

            case TA_Base_Bus::ITrainPaCorba::InterruptedByPsc:
                eMessageCompletionStatus = ProtocolPaTypes::InterruptedByPsc;
                break;

            case TA_Base_Bus::ITrainPaCorba::InterruptedByDriver:
                eMessageCompletionStatus = ProtocolPaTypes::InterruptedByDriver;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eMessageCompletionStatus;
    }


    PaTypes::PaAnnouncementUpdate PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::PaAnnouncementUpdate& CorbaType )
    {
        PaTypes::PaAnnouncementUpdate paAnnouncementUpdate;

        paAnnouncementUpdate.updateType = convertToCpp( CorbaType.updateType );
        paAnnouncementUpdate.announcementId = CorbaType.annId;

        return  paAnnouncementUpdate;
    }


    PaTypes::TrainLibraryInfo PaTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPaCorba::TrainLibraryInfo& CorbaType )
    {
        PaTypes::TrainLibraryInfo trainLibraryInfo;

        trainLibraryInfo.libraryVersion = CorbaType.libraryVersion;
        trainLibraryInfo.trainId = CorbaType.trainId;

        return  trainLibraryInfo;
    }

}
