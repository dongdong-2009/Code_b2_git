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

#include "bus/trains/TrainCorbaProxy/src/TisTypesConversion.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainTisCorba::ETtisJustification TisTypesConversion::convertToCorba( const ProtocolTtisTypes::ETtisJustification& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisJustification eTtisJustification;

        switch ( cppType )
        {

            case ProtocolTtisTypes::TtisJustificationLeft:
                eTtisJustification = TA_Base_Bus::ITrainTisCorba::TtisJustificationLeft;
                break;

            case ProtocolTtisTypes::TtisJustificationCentre:
                eTtisJustification = TA_Base_Bus::ITrainTisCorba::TtisJustificationCentre;
                break;

            case ProtocolTtisTypes::TtisJustificationRight:
                eTtisJustification = TA_Base_Bus::ITrainTisCorba::TtisJustificationRight;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisJustification;
    }

    TA_Base_Bus::ITrainTisCorba::TtisPredefinedDisplayParameters TisTypesConversion::convertToCorba( const TtisTypes::TtisPredefinedDisplayParameters& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TtisPredefinedDisplayParameters ttisPredefinedDisplayParameters;

        ttisPredefinedDisplayParameters.pidList= convertToCorba( cppType.pidList );
        ttisPredefinedDisplayParameters.libraryVersion = cppType.libraryVersion;
        ttisPredefinedDisplayParameters.messageId = cppType.messageId;
        ttisPredefinedDisplayParameters.perRun = cppType.perRun;       

        return ttisPredefinedDisplayParameters;
    }


    TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate TisTypesConversion::convertToCorba( const TtisTypes::TisLibraryVersionUpdate& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate tisLibraryVersionUpdate;

        tisLibraryVersionUpdate.libraryVersion = cppType.libraryVersion;
        tisLibraryVersionUpdate.scheduleVersion = cppType.scheduleVersion;
        tisLibraryVersionUpdate.trainId = cppType.trainId;

        return tisLibraryVersionUpdate;
    }


    TA_Base_Bus::ITrainTisCorba::LibraryDataType TisTypesConversion::convertToCorba( const TtisTypes::LibraryDataType& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::LibraryDataType libraryDataType;

        libraryDataType.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            libraryDataType[i] = cppType[i];
        }

        return libraryDataType;
    }


    TA_Base_Bus::ITrainTisCorba::TtisPidList TisTypesConversion::convertToCorba( const TtisTypes::TtisPidList& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TtisPidList ttisPidList;

        ttisPidList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); i++ )
        {
            ttisPidList[i] = convertToCorba( cppType[i] );
        }

        return ttisPidList;
    }


    TA_Base_Bus::ITrainTisCorba::ETtisPid TisTypesConversion::convertToCorba( const ProtocolTtisTypes::ETtisPid& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisPid eTtisPid;

        switch ( cppType )
        {

            case ProtocolTtisTypes::TtisPidAllTrain:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidAllTrain;
                break;

            case ProtocolTtisTypes::TtisPidCar1:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar1;
                break;

            case ProtocolTtisTypes::TtisPidCar2:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar2;
                break;

            case ProtocolTtisTypes::TtisPidCar3:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar3;
                break;

            case ProtocolTtisTypes::TtisPidCar1Pid1:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid1;
                break;

            case ProtocolTtisTypes::TtisPidCar1Pid2:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid2;
                break;

            case ProtocolTtisTypes::TtisPidCar1Pid3:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid3;
                break;

            case ProtocolTtisTypes::TtisPidCar1Pid4:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid4;
                break;

            case ProtocolTtisTypes::TtisPidCar2Pid1:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid1;
                break;

            case ProtocolTtisTypes::TtisPidCar2Pid2:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid2;
                break;

            case ProtocolTtisTypes::TtisPidCar2Pid3:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid3;
                break;

            case ProtocolTtisTypes::TtisPidCar2Pid4:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid4;
                break;

            case ProtocolTtisTypes::TtisPidCar3Pid1:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid1;
                break;

            case ProtocolTtisTypes::TtisPidCar3Pid2:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid2;
                break;

            case ProtocolTtisTypes::TtisPidCar3Pid3:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid3;
                break;

            case ProtocolTtisTypes::TtisPidCar3Pid4:
                eTtisPid = TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid4;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisPid;
    }


    TA_Base_Bus::ITrainTisCorba::ETtisOriginalCommand TisTypesConversion::convertToCorba( const TtisTypes::ETtisOriginalDisplayCommand& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisOriginalCommand eTtisOriginalCommand;

        switch ( cppType )
        {

            case TtisTypes::TtisFreeTextCommand:
                eTtisOriginalCommand = TA_Base_Bus::ITrainTisCorba::TtisFreeTextCommand;
                break;

            case TtisTypes::TtisPredefinedCommand:
                eTtisOriginalCommand = TA_Base_Bus::ITrainTisCorba::TtisFreeTextCommand;
                break;

            case TtisTypes::TtisResetCommand:
                eTtisOriginalCommand = TA_Base_Bus::ITrainTisCorba::TtisResetCommand;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisOriginalCommand;
    }


    TA_Base_Bus::ITrainTisCorba::ETtisDisplayMode TisTypesConversion::convertToCorba( const ProtocolTtisTypes::ETtisDisplayMode& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisDisplayMode eTtisDisplayMode;

        switch ( cppType )
        {

            case ProtocolTtisTypes::TtisDisplayModeScrollLeft:
                eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollLeft;
                break;

            case ProtocolTtisTypes::TtisDisplayModeScrollRight:
                eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollRight;
                break;

            case ProtocolTtisTypes::TtisDisplayModeScrollUp:
                eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollUp;
                break;

            case ProtocolTtisTypes::TtisDisplayModeScrollDown:
                eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollDown;
                break;

            case ProtocolTtisTypes::TtisDisplayModeInstantOn:
				eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeNormalStationary ;
                break;

            case ProtocolTtisTypes::TtisDisplayModeBlinking:
                eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeBlinking;
                break;

            case ProtocolTtisTypes::TtisDisplayModeWiping:
                eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeWiping;
                break;

            case ProtocolTtisTypes::TtisDisplayModeLikeSnow:
                eTtisDisplayMode = TA_Base_Bus::ITrainTisCorba::TtisDisplayModeLikeSnow;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisDisplayMode;
    }



    TA_Base_Bus::ITrainTisCorba::ETtisCommandResult TisTypesConversion::convertToCorba( const TtisTypes::ETtisCommandResult& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisCommandResult eTtisCommandResult;

        switch ( cppType )
        {

            case TtisTypes::TtisSuccess:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TtisSuccess;
                break;

            case TtisTypes::TtisInUse:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TtisInUse;
                break;

            case TtisTypes::TtisReset:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TtisReset;
                break;

            case TtisTypes::TtisFailed:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TtisFailed;
                break;

            case TtisTypes::TtisDifferentVersion:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TtisDifferentVersion;
                break;

            case TtisTypes::BadCommandBadChecksum:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::BadCommandBadChecksum;
                break;

            case TtisTypes::BadCommandUnknownCommand:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::BadCommandUnknownCommand;
                break;

            case TtisTypes::BadCommandBadParameter:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::BadCommandBadParameter;
                break;

            case TtisTypes::BadCommandUnexpectedCommand:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::BadCommandUnexpectedCommand;
                break;

            case TtisTypes::InvalidTrainError:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::InvalidTrainError;
                break;

            case TtisTypes::TransmissionFailureError:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TransmissionFailureError;
                break;

            case TtisTypes::TransactionTimeoutError:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TransactionTimeoutError;
                break;

            case TtisTypes::TransactionCancelledError:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TransactionCancelledError;
                break;

            case TtisTypes::MpuChangeoverError:
                eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::MpuChangeoverError;
                break;

			case TtisTypes::TtisDutyHandover:
				eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TtisDutyHandover;
				break;

			case TtisTypes::TtisEndDwnloadFailed:
				eTtisCommandResult = TA_Base_Bus::ITrainTisCorba::TtisEndDwnloadFailed;
				break;
            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisCommandResult;
    }


    TA_Base_Bus::ITrainTisCorba::TisReturnStatus TisTypesConversion::convertToCorba( const TtisTypes::TisReturnStatus& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TisReturnStatus tisReturnStatus;

        tisReturnStatus.trainId = cppType.trainId;
        tisReturnStatus.sessionId = cppType.sessionId.c_str();
        tisReturnStatus.timestamp = cppType.timestamp;
        tisReturnStatus.result = convertToCorba( cppType.result );
        tisReturnStatus.originalParameters = convertToCorba( cppType.originalParameters );

        return tisReturnStatus;
    }


    TA_Base_Bus::ITrainTisCorba::EDownloadResult TisTypesConversion::convertToCorba( const TtisTypes::EDownloadResult& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::EDownloadResult eDownloadResult;

        switch ( cppType )
        {

            case TtisTypes::Success:
                eDownloadResult = TA_Base_Bus::ITrainTisCorba::Success;
                break;

            case TtisTypes::TransferFailed:
                eDownloadResult = TA_Base_Bus::ITrainTisCorba::TransferFailed;
                break;

            case TtisTypes::TimsFailed:
                eDownloadResult = TA_Base_Bus::ITrainTisCorba::TimsFailed;
                break;

            case TtisTypes::TimedOut:
                eDownloadResult = TA_Base_Bus::ITrainTisCorba::TimedOut;
                break;

            case TtisTypes::MpuChangeover:
                eDownloadResult = TA_Base_Bus::ITrainTisCorba::MpuChangeover;
                break;

			case TtisTypes::Cancelled:
				eDownloadResult = TA_Base_Bus::ITrainTisCorba::Cancelled;
				break;
			
			case  TtisTypes::Upgraded:
				eDownloadResult = TA_Base_Bus::ITrainTisCorba::Upgraded;
				break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eDownloadResult;
    }


    TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList TisTypesConversion::convertToCorba( const TtisTypes::TisLibraryVersionList& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList tisLibraryVersionList;

        tisLibraryVersionList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            tisLibraryVersionList[i] = convertToCorba( cppType[i] );
        }

        return tisLibraryVersionList;
    }


    TA_Base_Bus::ITrainTisCorba::ETtisPriority TisTypesConversion::convertToCorba( const ProtocolTtisTypes::ETtisPriority& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisPriority eTtisPriority;

        switch ( cppType )
        {

            case ProtocolTtisTypes::ETtisPriorityOne:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPriorityOne;
                break;

            case ProtocolTtisTypes::ETtisPriorityTwo:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPriorityTwo;
                break;

            case ProtocolTtisTypes::ETtisPriorityThree:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPriorityThree;
                break;

            case ProtocolTtisTypes::ETtisPriorityFour:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPriorityFour;
                break;

            case ProtocolTtisTypes::ETtisPriorityFive:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPriorityFive;
                break;

            case ProtocolTtisTypes::ETtisPrioritySix:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPrioritySix;
                break;

            case ProtocolTtisTypes::ETtisPrioritySeven:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPrioritySeven;
                break;

            case ProtocolTtisTypes::ETtisPriorityEight:
                eTtisPriority = TA_Base_Bus::ITrainTisCorba::ETtisPriorityEight;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisPriority;
    }


    TA_Base_Bus::ITrainTisCorba::ETtisResetMessageType TisTypesConversion::convertToCorba( const ProtocolTtisTypes::ETtisResetMessageType& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisResetMessageType eTtisResetMessageType;

        switch ( cppType )
        {

            case ProtocolTtisTypes::TtisResetAll:
                eTtisResetMessageType = TA_Base_Bus::ITrainTisCorba::TtisResetAll;
                break;

            case ProtocolTtisTypes::TtisResetEmergency:
                eTtisResetMessageType = TA_Base_Bus::ITrainTisCorba::TtisResetEmergency;
                break;

            case ProtocolTtisTypes::TtisResetNormal:
                eTtisResetMessageType = TA_Base_Bus::ITrainTisCorba::TtisResetNormal;
                break;

            case ProtocolTtisTypes::TtisResetPriority6:
                eTtisResetMessageType = TA_Base_Bus::ITrainTisCorba::TtisResetPriority6;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisResetMessageType;
    }


    TA_Base_Bus::ITrainTisCorba::ETisUpdateType TisTypesConversion::convertToCorba( const TtisTypes::ETisUpdateType& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETisUpdateType eTisUpdateType;

        switch ( cppType )
        {

            case TtisTypes::LibraryDownloadStart:
                eTisUpdateType = TA_Base_Bus::ITrainTisCorba::LibraryDownloadStart;
                break;

            case TtisTypes::LibraryDownloadComplete:
                eTisUpdateType = TA_Base_Bus::ITrainTisCorba::LibraryDownloadComplete;
                break;

            case TtisTypes::LibraryUpgradeResult:
                eTisUpdateType = TA_Base_Bus::ITrainTisCorba::LibraryUpgradeResult;
                break;

            case TtisTypes::ScheduleDownloadStart:
                eTisUpdateType = TA_Base_Bus::ITrainTisCorba::ScheduleDownloadStart;
                break;

            case TtisTypes::ScheduleDownloadComplete:
                eTisUpdateType = TA_Base_Bus::ITrainTisCorba::ScheduleDownloadComplete;
                break;

            case TtisTypes::ScheduleUpgradeResult:
                eTisUpdateType = TA_Base_Bus::ITrainTisCorba::ScheduleUpgradeResult;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTisUpdateType;
    }


    TA_Base_Bus::ITrainTisCorba::ETtisFontSize TisTypesConversion::convertToCorba( const ProtocolTtisTypes::ETtisFontSize& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisFontSize eTtisFontSize;

        switch ( cppType )
        {

            case ProtocolTtisTypes::TtisFontSmall:
                eTtisFontSize = TA_Base_Bus::ITrainTisCorba::TtisFontSmall;
                break;

            case ProtocolTtisTypes::TtisFontLarge:
                eTtisFontSize = TA_Base_Bus::ITrainTisCorba::TtisFontLarge;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisFontSize;
    }



    TA_Base_Bus::ITrainStateUpdateCorba::TtdFrameList TisTypesConversion::convertToCorba( const TtisTypes::TtisFrameList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::TtdFrameList ttdFrameList;

        ttdFrameList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            ttdFrameList[i] = convertToCorba( cppType[i] ) ;
        }

        return ttdFrameList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::TtdFrame TisTypesConversion::convertToCorba( const TtisTypes::TtisFrameData& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::TtdFrame ttdFrame;

        ttdFrame.data.length( cppType.frameData.size() );

        for ( unsigned long i = 0; i < cppType.frameData.size(); ++i )
        {
            ttdFrame.data[i] = cppType.frameData[i];
        }

        return ttdFrame;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::ETtisDownloadType TisTypesConversion::convertToCorba( const TtisTypes::ETtisDownloadType& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::ETtisDownloadType eTtisDownloadType;

        switch ( cppType )
        {

            case TtisTypes::TtisNone:
                eTtisDownloadType = TA_Base_Bus::ITrainStateUpdateCorba::TtisNone;
                break;

            case TtisTypes::TtisLibrary:
                eTtisDownloadType = TA_Base_Bus::ITrainStateUpdateCorba::TtisLibrary;
                break;

            case TtisTypes::TtisSchedule:
                eTtisDownloadType = TA_Base_Bus::ITrainStateUpdateCorba::TtisSchedule;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisDownloadType;
    }


    TA_Base_Bus::ITrainTisCorba::ETtisIntensity TisTypesConversion::convertToCorba( const ProtocolTtisTypes::ETtisIntensity& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::ETtisIntensity eTtisIntensity;

        switch ( cppType )
        {

            case ProtocolTtisTypes::TtisIntensityNormal:
                eTtisIntensity = TA_Base_Bus::ITrainTisCorba::TtisIntensityNormal;
                break;

            case ProtocolTtisTypes::TtisIntensityHigh:
                eTtisIntensity = TA_Base_Bus::ITrainTisCorba::TtisIntensityHigh;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisIntensity;
    }


    TA_Base_Bus::ITrainTisCorba::TtisResetParameters TisTypesConversion::convertToCorba( const TtisTypes::TtisResetParameters& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TtisResetParameters ttisResetParameters;

        ttisResetParameters.messageType = convertToCorba( cppType.messageType );
        ttisResetParameters.pidList = convertToCorba( cppType.pidList );

        return ttisResetParameters;
    }


    TA_Base_Bus::ITrainTisCorba::TimeInterval TisTypesConversion::convertToCorba( const TtisTypes::TimeInterval& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TimeInterval timeInterval;

        timeInterval.startHour = cppType.startHour;
        timeInterval.startMin = cppType.startMin;
        timeInterval.stopHour = cppType.stopHour;
        timeInterval.stopMin = cppType.stopMin;

        return timeInterval;
    }


    TA_Base_Bus::ITrainTisCorba::TisLibraryUpdate TisTypesConversion::convertToCorba( const TtisTypes::TisLibraryUpdate& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TisLibraryUpdate tisLibraryUpdate;

        tisLibraryUpdate.commandStatus = convertToCorba( cppType.commandStatus );
        tisLibraryUpdate.downloadResult = convertToCorba( cppType.downloadResult );
        tisLibraryUpdate.trainId = cppType.trainId;
        tisLibraryUpdate.updateType = convertToCorba( cppType.updateType );
        tisLibraryUpdate.version = cppType.version;

        return tisLibraryUpdate;
    }


    TA_Base_Bus::ITrainTisCorba::TtisFreeTextDisplayParameters TisTypesConversion::convertToCorba( const TtisTypes::TtisFreeTextDisplayParameters& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::TtisFreeTextDisplayParameters ttisFreeTextDisplayParameters;

        ttisFreeTextDisplayParameters.pidList = convertToCorba( cppType.pidList );
        ttisFreeTextDisplayParameters.fontSize = convertToCorba( cppType.fontSize );
        ttisFreeTextDisplayParameters.justification = convertToCorba( cppType.justification );
        ttisFreeTextDisplayParameters.intensity = convertToCorba( cppType.intensity );
        ttisFreeTextDisplayParameters.displayMode = convertToCorba( cppType.displayMode );
        ttisFreeTextDisplayParameters.perRun = cppType.perRun ;        
        ttisFreeTextDisplayParameters.message = cppType.message.c_str();

        return ttisFreeTextDisplayParameters;
    }


    TA_Base_Bus::ITrainTisCorba::DisplayParameters TisTypesConversion::convertToCorba( const TtisTypes::DisplayParameters& cppType )
    {
        TA_Base_Bus::ITrainTisCorba::DisplayParameters displayParameters;

	TtisTypes::ETtisOriginalDisplayCommand originalCommand = cppType.getOriginalCommand();

        switch ( originalCommand )
        {

            case TtisTypes::TtisFreeTextCommand:
                displayParameters.freetextParameters( convertToCorba( cppType.getFreeTextParameters() ) );
                break;

            case TtisTypes::TtisPredefinedCommand:
                displayParameters.predefinedParameters( convertToCorba( cppType.getPredefinedParameters() ) );
                break;

            case TtisTypes::TtisResetCommand:
                displayParameters.resetParameters( convertToCorba( cppType.getResetParameters() ) );
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return displayParameters;
    }


    TtisTypes::TisReturnStatus TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisReturnStatus& CorbaType )
    {
        TtisTypes::TisReturnStatus tisReturnStatus;

        tisReturnStatus.trainId = CorbaType.trainId;
        tisReturnStatus.sessionId = CorbaType.sessionId.in();
        tisReturnStatus.timestamp = CorbaType.timestamp;
        tisReturnStatus.result = convertToCpp( CorbaType.result );
        tisReturnStatus.originalParameters = convertToCpp( CorbaType.originalParameters );

        return tisReturnStatus;
    }


    TtisTypes::ETtisOriginalDisplayCommand TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisOriginalCommand& CorbaType )
    {
        TtisTypes::ETtisOriginalDisplayCommand eTtisOriginalCommand;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisFreeTextCommand:
                eTtisOriginalCommand = TtisTypes::TtisFreeTextCommand;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPredefinedCommand:
                eTtisOriginalCommand = TtisTypes::TtisFreeTextCommand;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisResetCommand:
                eTtisOriginalCommand = TtisTypes::TtisResetCommand;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisOriginalCommand;
    }


    ProtocolTtisTypes::ETtisPid TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisPid& CorbaType )
    {
        ProtocolTtisTypes::ETtisPid eTtisPid;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisPidAllTrain:
                eTtisPid = ProtocolTtisTypes::TtisPidAllTrain;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar1:
                eTtisPid = ProtocolTtisTypes::TtisPidCar1;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar2:
                eTtisPid = ProtocolTtisTypes::TtisPidCar2;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar3:
                eTtisPid = ProtocolTtisTypes::TtisPidCar3;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid1:
                eTtisPid = ProtocolTtisTypes::TtisPidCar1Pid1;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid2:
                eTtisPid = ProtocolTtisTypes::TtisPidCar1Pid2;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid3:
                eTtisPid = ProtocolTtisTypes::TtisPidCar1Pid3;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar1Pid4:
                eTtisPid = ProtocolTtisTypes::TtisPidCar1Pid4;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid1:
                eTtisPid = ProtocolTtisTypes::TtisPidCar2Pid1;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid2:
                eTtisPid = ProtocolTtisTypes::TtisPidCar2Pid2;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid3:
                eTtisPid = ProtocolTtisTypes::TtisPidCar2Pid3;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar2Pid4:
                eTtisPid = ProtocolTtisTypes::TtisPidCar2Pid4;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid1:
                eTtisPid = ProtocolTtisTypes::TtisPidCar3Pid1;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid2:
                eTtisPid = ProtocolTtisTypes::TtisPidCar3Pid2;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid3:
                eTtisPid = ProtocolTtisTypes::TtisPidCar3Pid3;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisPidCar3Pid4:
                eTtisPid = ProtocolTtisTypes::TtisPidCar3Pid4;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisPid;
    }


    TtisTypes::TtisPidList TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisPidList& CorbaType )
    {
        TtisTypes::TtisPidList ttisPidList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            ttisPidList.push_back( convertToCpp( CorbaType[i] ) );
        }


        return ttisPidList;
    }


    TtisTypes::TisLibraryUpdate TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisLibraryUpdate& CorbaType )
    {
        TtisTypes::TisLibraryUpdate tisLibraryUpdate;

        tisLibraryUpdate.commandStatus = convertToCpp( CorbaType.commandStatus );
        tisLibraryUpdate.downloadResult = convertToCpp( CorbaType.downloadResult );
        tisLibraryUpdate.trainId = CorbaType.trainId;
        tisLibraryUpdate.updateType = convertToCpp( CorbaType.updateType );
        tisLibraryUpdate.version = CorbaType.version;

        return tisLibraryUpdate;
    }


    TtisTypes::TimeInterval TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TimeInterval& CorbaType )
    {
        TtisTypes::TimeInterval timeInterval;

        timeInterval.startHour = CorbaType.startHour;
        timeInterval.startMin = CorbaType.startMin;
        timeInterval.stopHour = CorbaType.stopHour;
        timeInterval.stopMin = CorbaType.stopMin;

        return timeInterval;
    }


    ProtocolTtisTypes::ETtisDisplayMode TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisDisplayMode& CorbaType )
    {
        ProtocolTtisTypes::ETtisDisplayMode eTtisDisplayMode;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollLeft:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeScrollLeft;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollRight:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeScrollRight;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollUp:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeScrollUp;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeScrollDown:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeScrollDown;
                break;

			case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeNormalStationary:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeInstantOn;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeBlinking:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeBlinking;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeWiping:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeWiping;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisDisplayModeLikeSnow:
                eTtisDisplayMode = ProtocolTtisTypes::TtisDisplayModeLikeSnow;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                //break;
        }

        return eTtisDisplayMode;
    }


    TtisTypes::DisplayParameters TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::DisplayParameters& CorbaType )
    {

        switch ( CorbaType._d() )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisFreeTextCommand:
                return TtisTypes::DisplayParameters( convertToCpp( CorbaType.freetextParameters() ) );
                //break;

            case TA_Base_Bus::ITrainTisCorba::TtisPredefinedCommand:
                return TtisTypes::DisplayParameters( convertToCpp( CorbaType.predefinedParameters() ) );
                //break;

            case TA_Base_Bus::ITrainTisCorba::TtisResetCommand:
                return TtisTypes::DisplayParameters( convertToCpp( CorbaType.resetParameters() ) );
                //break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                return TtisTypes::DisplayParameters();
                //break;
        }

    }


    ProtocolTtisTypes::ETtisJustification TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisJustification& CorbaType )
    {
        ProtocolTtisTypes::ETtisJustification eTtisJustification;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisJustificationLeft:
                eTtisJustification = ProtocolTtisTypes::TtisJustificationLeft;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisJustificationCentre:
                eTtisJustification = ProtocolTtisTypes::TtisJustificationCentre;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisJustificationRight:
                eTtisJustification = ProtocolTtisTypes::TtisJustificationRight;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTtisJustification;
    }


    TtisTypes::TtisPredefinedDisplayParameters TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisPredefinedDisplayParameters& CorbaType )
    {
        TtisTypes::TtisPredefinedDisplayParameters ttisPredefinedDisplayParameters;

        ttisPredefinedDisplayParameters.pidList = convertToCpp( CorbaType.pidList );
        ttisPredefinedDisplayParameters.libraryVersion = CorbaType.libraryVersion;
        ttisPredefinedDisplayParameters.perRun = CorbaType.perRun;
        ttisPredefinedDisplayParameters.messageId = CorbaType.messageId;
        return ttisPredefinedDisplayParameters;
    }


    TtisTypes::TisLibraryVersionUpdate TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate& CorbaType )
    {
        TtisTypes::TisLibraryVersionUpdate tisLibraryVersionUpdate;

        tisLibraryVersionUpdate.libraryVersion = CorbaType.libraryVersion;
        tisLibraryVersionUpdate.scheduleVersion = CorbaType.scheduleVersion;
        tisLibraryVersionUpdate.trainId = CorbaType.trainId;

        return tisLibraryVersionUpdate;
    }


    TtisTypes::TisLibraryVersionList TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList& CorbaType )
    {
        TtisTypes::TisLibraryVersionList tisLibraryVersionList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            tisLibraryVersionList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return tisLibraryVersionList;
    }


    TtisTypes::TtisResetParameters TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisResetParameters& CorbaType )
    {
        TtisTypes::TtisResetParameters ttisResetParameters;

        ttisResetParameters.messageType = convertToCpp( CorbaType.messageType );
        ttisResetParameters.pidList = convertToCpp( CorbaType.pidList );

        return ttisResetParameters;
    }


    TtisTypes::ETtisCommandResult TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisCommandResult& CorbaType )
    {
        TtisTypes::ETtisCommandResult eTtisCommandResult;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisSuccess:
                eTtisCommandResult = TtisTypes::TtisSuccess;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisInUse:
                eTtisCommandResult = TtisTypes::TtisInUse;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisReset:
                eTtisCommandResult = TtisTypes::TtisReset;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisFailed:
                eTtisCommandResult = TtisTypes::TtisFailed;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisDifferentVersion:
                eTtisCommandResult = TtisTypes::TtisDifferentVersion;
                break;

            case TA_Base_Bus::ITrainTisCorba::BadCommandBadChecksum:
                eTtisCommandResult = TtisTypes::BadCommandBadChecksum;
                break;

            case TA_Base_Bus::ITrainTisCorba::BadCommandUnknownCommand:
                eTtisCommandResult = TtisTypes::BadCommandUnknownCommand;
                break;

            case TA_Base_Bus::ITrainTisCorba::BadCommandBadParameter:
                eTtisCommandResult = TtisTypes::BadCommandBadParameter;
                break;

            case TA_Base_Bus::ITrainTisCorba::BadCommandUnexpectedCommand:
                eTtisCommandResult = TtisTypes::BadCommandUnexpectedCommand;
                break;

            case TA_Base_Bus::ITrainTisCorba::InvalidTrainError:
                eTtisCommandResult = TtisTypes::InvalidTrainError;
                break;

            case TA_Base_Bus::ITrainTisCorba::TransmissionFailureError:
                eTtisCommandResult = TtisTypes::TransmissionFailureError;
                break;

            case TA_Base_Bus::ITrainTisCorba::TransactionTimeoutError:
                eTtisCommandResult = TtisTypes::TransactionTimeoutError;
                break;

            case TA_Base_Bus::ITrainTisCorba::TransactionCancelledError:
                eTtisCommandResult = TtisTypes::TransactionCancelledError;
                break;

            case TA_Base_Bus::ITrainTisCorba::MpuChangeoverError:
                eTtisCommandResult = TtisTypes::MpuChangeoverError;
                break;

			case TA_Base_Bus::ITrainTisCorba::TtisDutyHandover:
				eTtisCommandResult = TtisTypes::TtisDutyHandover;
				break;
			case TA_Base_Bus::ITrainTisCorba::TtisEndDwnloadFailed:
				eTtisCommandResult = TtisTypes::TtisEndDwnloadFailed;
				break;
            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTtisCommandResult;
    }


    TtisTypes::EDownloadResult TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::EDownloadResult& CorbaType )
    {
        TtisTypes::EDownloadResult eDownloadResult;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::Success:
                eDownloadResult = TtisTypes::Success;
                break;

            case TA_Base_Bus::ITrainTisCorba::TransferFailed:
                eDownloadResult = TtisTypes::TransferFailed;
                break;

            case TA_Base_Bus::ITrainTisCorba::TimsFailed:
                eDownloadResult = TtisTypes::TimsFailed;
                break;

            case TA_Base_Bus::ITrainTisCorba::TimedOut:
                eDownloadResult = TtisTypes::TimedOut;
                break;

            case TA_Base_Bus::ITrainTisCorba::MpuChangeover:
                eDownloadResult = TtisTypes::MpuChangeover;
                break;
				
			case  TA_Base_Bus::ITrainTisCorba::Cancelled:
				eDownloadResult = TtisTypes::Cancelled;
				break;

			case TA_Base_Bus::ITrainTisCorba::Upgraded:
				eDownloadResult = TtisTypes::Upgraded;
				break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eDownloadResult;
    }


    ProtocolTtisTypes::ETtisPriority TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisPriority& CorbaType )
    {
        ProtocolTtisTypes::ETtisPriority eTtisPriority;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::ETtisPriorityOne:
                eTtisPriority = ProtocolTtisTypes::ETtisPriorityOne;
                break;

            case TA_Base_Bus::ITrainTisCorba::ETtisPriorityTwo:
                eTtisPriority = ProtocolTtisTypes::ETtisPriorityTwo;
                break;

            case TA_Base_Bus::ITrainTisCorba::ETtisPriorityThree:
                eTtisPriority = ProtocolTtisTypes::ETtisPriorityThree;
                break;

            case TA_Base_Bus::ITrainTisCorba::ETtisPriorityFour:
                eTtisPriority = ProtocolTtisTypes::ETtisPriorityFour;
                break;

            case TA_Base_Bus::ITrainTisCorba::ETtisPriorityFive:
                eTtisPriority = ProtocolTtisTypes::ETtisPriorityFive;
                break;

            case TA_Base_Bus::ITrainTisCorba::ETtisPrioritySix:
                eTtisPriority = ProtocolTtisTypes::ETtisPrioritySix;
                break;

            case TA_Base_Bus::ITrainTisCorba::ETtisPrioritySeven:
                eTtisPriority = ProtocolTtisTypes::ETtisPrioritySeven;
                break;

            case TA_Base_Bus::ITrainTisCorba::ETtisPriorityEight:
                eTtisPriority = ProtocolTtisTypes::ETtisPriorityEight;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTtisPriority;
    }


    ProtocolTtisTypes::ETtisResetMessageType TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisResetMessageType& CorbaType )
    {
        ProtocolTtisTypes::ETtisResetMessageType eTtisResetMessageType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisResetAll:
                eTtisResetMessageType = ProtocolTtisTypes::TtisResetAll;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisResetEmergency:
                eTtisResetMessageType = ProtocolTtisTypes::TtisResetEmergency;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisResetNormal:
                eTtisResetMessageType = ProtocolTtisTypes::TtisResetNormal;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisResetPriority6:
                eTtisResetMessageType = ProtocolTtisTypes::TtisResetPriority6;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTtisResetMessageType;
    }


    TtisTypes::ETisUpdateType TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETisUpdateType& CorbaType )
    {
        TtisTypes::ETisUpdateType eTisUpdateType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::LibraryDownloadStart:
                eTisUpdateType = TtisTypes::LibraryDownloadStart;
                break;

            case TA_Base_Bus::ITrainTisCorba::LibraryDownloadComplete:
                eTisUpdateType = TtisTypes::LibraryDownloadComplete;
                break;

            case TA_Base_Bus::ITrainTisCorba::LibraryUpgradeResult:
                eTisUpdateType = TtisTypes::LibraryUpgradeResult;
                break;

            case TA_Base_Bus::ITrainTisCorba::ScheduleDownloadStart:
                eTisUpdateType = TtisTypes::ScheduleDownloadStart;
                break;

            case TA_Base_Bus::ITrainTisCorba::ScheduleDownloadComplete:
                eTisUpdateType = TtisTypes::ScheduleDownloadComplete;
                break;

            case TA_Base_Bus::ITrainTisCorba::ScheduleUpgradeResult:
                eTisUpdateType = TtisTypes::ScheduleUpgradeResult;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTisUpdateType;
    }


    ProtocolTtisTypes::ETtisFontSize TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisFontSize& CorbaType )
    {
        ProtocolTtisTypes::ETtisFontSize eTtisFontSize;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisFontSmall:
                eTtisFontSize = ProtocolTtisTypes::TtisFontSmall;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisFontLarge:
                eTtisFontSize = ProtocolTtisTypes::TtisFontLarge;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTtisFontSize;
    }


    TtisTypes::TtisFrameList TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TtdFrameList& CorbaType )
    {
        TtisTypes::TtisFrameList ttisFrameList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            ttisFrameList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return ttisFrameList;
    }



    TtisTypes::TtisFrameData TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TtdFrame& CorbaType )
    {
        TtisTypes::TtisFrameData ttisFrameData;

        for ( unsigned long i = 0; i < CorbaType.data.length(); ++i )
        {
            ttisFrameData.frameData.push_back( CorbaType.data[i] );
        }

        return ttisFrameData;
    }


    TtisTypes::ETtisDownloadType TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETtisDownloadType& CorbaType )
    {
        TtisTypes::ETtisDownloadType eTtisDownloadType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainStateUpdateCorba::TtisNone:
                eTtisDownloadType = TtisTypes::TtisNone;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TtisLibrary:
                eTtisDownloadType = TtisTypes::TtisLibrary;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TtisSchedule:
                eTtisDownloadType = TtisTypes::TtisSchedule;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTtisDownloadType;
    }


    TtisTypes::LibraryDataType TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::LibraryDataType& CorbaType )
    {
        TtisTypes::LibraryDataType libraryDataType;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            libraryDataType.push_back( CorbaType[i] );
        }

        return libraryDataType;
    }


    TtisTypes::TtisFreeTextDisplayParameters TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisFreeTextDisplayParameters& CorbaType )
    {
        TtisTypes::TtisFreeTextDisplayParameters ttisFreeTextDisplayParameters;

        ttisFreeTextDisplayParameters.pidList = convertToCpp( CorbaType.pidList );
        ttisFreeTextDisplayParameters.fontSize = convertToCpp( CorbaType.fontSize );
        ttisFreeTextDisplayParameters.justification = convertToCpp( CorbaType.justification );
        ttisFreeTextDisplayParameters.intensity = convertToCpp( CorbaType.intensity );
        ttisFreeTextDisplayParameters.displayMode = convertToCpp( CorbaType.displayMode );
        ttisFreeTextDisplayParameters.perRun = CorbaType.perRun ;        
        ttisFreeTextDisplayParameters.message = CorbaType.message.in();

        return ttisFreeTextDisplayParameters;
    }


    ProtocolTtisTypes::ETtisIntensity TisTypesConversion::convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisIntensity& CorbaType )
    {
        ProtocolTtisTypes::ETtisIntensity eTtisIntensity;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainTisCorba::TtisIntensityNormal:
                eTtisIntensity = ProtocolTtisTypes::TtisIntensityNormal;
                break;

            case TA_Base_Bus::ITrainTisCorba::TtisIntensityHigh:
                eTtisIntensity = ProtocolTtisTypes::TtisIntensityHigh;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTtisIntensity;
    }

}
