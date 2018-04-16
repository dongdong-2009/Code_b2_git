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

#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainExceptionsCorba::ECctvCommandStatus CctvTypesConversion::convertToCorba( const ProtocolCctvTypes::ECctvCommandStatus& cppType )
    {
        TA_Base_Bus::ITrainExceptionsCorba::ECctvCommandStatus eCctvCommandStatus;

        switch ( cppType )
        {

            case ProtocolCctvTypes::CctvProceeding:
                eCctvCommandStatus = TA_Base_Bus::ITrainExceptionsCorba::CctvProceeding;
                break;

            case ProtocolCctvTypes::CctvInUse:
                eCctvCommandStatus = TA_Base_Bus::ITrainExceptionsCorba::CctvInUse;
                break;

            case ProtocolCctvTypes::CctvFailed:
                eCctvCommandStatus = TA_Base_Bus::ITrainExceptionsCorba::CctvFailed;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvCommandStatus;
    }


    TA_Base_Bus::ITrainCctvCorba::ECctvSequenceNumber CctvTypesConversion::convertToCorba( const ProtocolCctvTypes::ECctvSequenceNumber& cppType )
    {
        TA_Base_Bus::ITrainCctvCorba::ECctvSequenceNumber eCctvSequenceNumber;

        switch ( cppType )
        {

            case ProtocolCctvTypes::SequenceOne:
                eCctvSequenceNumber = TA_Base_Bus::ITrainCctvCorba::SequenceOne;
                break;

            case ProtocolCctvTypes::SequenceTwo:
                eCctvSequenceNumber = TA_Base_Bus::ITrainCctvCorba::SequenceTwo;
                break;

            case ProtocolCctvTypes::SequenceThree:
                eCctvSequenceNumber = TA_Base_Bus::ITrainCctvCorba::SequenceThree;
                break;

            case ProtocolCctvTypes::SequenceFour:
                eCctvSequenceNumber = TA_Base_Bus::ITrainCctvCorba::SequenceFour;
                break;

            case ProtocolCctvTypes::SequenceFive:
                eCctvSequenceNumber = TA_Base_Bus::ITrainCctvCorba::SequenceFive;
                break;

            case ProtocolCctvTypes::SequenceSix:
                eCctvSequenceNumber = TA_Base_Bus::ITrainCctvCorba::SequenceSix;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvSequenceNumber;
    }


    TA_Base_Bus::ITrainCctvCorba::ECctvSequenceMode CctvTypesConversion::convertToCorba( const ProtocolCctvTypes::ECctvSequenceMode& cppType )
    {
        TA_Base_Bus::ITrainCctvCorba::ECctvSequenceMode eCctvSequenceMode;

        switch ( cppType )
        {

            case ProtocolCctvTypes::SequenceHold:
                eCctvSequenceMode = TA_Base_Bus::ITrainCctvCorba::SequenceHold;
                break;

            case ProtocolCctvTypes::SequenceForward:
                eCctvSequenceMode = TA_Base_Bus::ITrainCctvCorba::SequenceForward;
                break;

            case ProtocolCctvTypes::SequenceReverse:
                eCctvSequenceMode = TA_Base_Bus::ITrainCctvCorba::SequenceReverse;
                break;

            case ProtocolCctvTypes::SequenceSkip:
                eCctvSequenceMode = TA_Base_Bus::ITrainCctvCorba::SequenceSkip;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvSequenceMode;
    }


    TA_Base_Bus::ITrainCctvCorba::ECctvSwitchMode CctvTypesConversion::convertToCorba( const CctvTypes::ECctvSwitchMode& cppType )
    {
        TA_Base_Bus::ITrainCctvCorba::ECctvSwitchMode eCctvSwitchMode;

        switch ( cppType )
        {

            case CctvTypes::CctvInactive:
                eCctvSwitchMode = TA_Base_Bus::ITrainCctvCorba::CctvInactive;
                break;

            case CctvTypes::CctvSequence:
                eCctvSwitchMode = TA_Base_Bus::ITrainCctvCorba::CctvSequence;
                break;

            case CctvTypes::CctvCamera:
                eCctvSwitchMode = TA_Base_Bus::ITrainCctvCorba::CctvCamera;
                break;

            case CctvTypes::CctvQuad:
                eCctvSwitchMode = TA_Base_Bus::ITrainCctvCorba::CctvQuad;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvSwitchMode;
    }


    TA_Base_Bus::ITrainCctvCorba::CctvSwitchState CctvTypesConversion::convertToCorba( const CctvTypes::CctvSwitchState& cppType )
    {
        TA_Base_Bus::ITrainCctvCorba::CctvSwitchState cctvSwitchState;

        cctvSwitchState.cctvState = convertToCorba( cppType.cctvState );
        cctvSwitchState.trainId = cppType.trainId;
        cctvSwitchState.item1 = cppType.item1;
        cctvSwitchState.item2 = cppType.item2;
        cctvSwitchState.item3 = cppType.item3;
        cctvSwitchState.item4 = cppType.item4;
        cctvSwitchState.preConflictTrain = cppType.preConflictTrain;
        cctvSwitchState.stageId = cppType.stageId;

        return  cctvSwitchState;
    }


    TA_Base_Bus::ITrainCctvCorba::ECctvCamera CctvTypesConversion::convertToCorba( const ProtocolCctvTypes::ECctvCamera& cppType )
    {
        TA_Base_Bus::ITrainCctvCorba::ECctvCamera eCctvCamera;

        switch ( cppType )
        {

            case ProtocolCctvTypes::CameraNone:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraNone;
                break;

            case ProtocolCctvTypes::CameraOne:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraOne;
                break;

            case ProtocolCctvTypes::CameraTwo:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraTwo;
                break;

            case ProtocolCctvTypes::CameraThree:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraThree;
                break;

            case ProtocolCctvTypes::CameraFour:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraFour;
                break;

            case ProtocolCctvTypes::CameraFive:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraFive;
                break;

            case ProtocolCctvTypes::CameraSix:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraSix;
                break;

            case ProtocolCctvTypes::CameraSeven:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraSeven;
                break;

            case ProtocolCctvTypes::CameraEight:
                eCctvCamera = TA_Base_Bus::ITrainCctvCorba::CameraEight;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvCamera;
    }


    TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence CctvTypesConversion::convertToCorba( const CctvTypes::TrainCameraSequence& cppType )
    {
        TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence trainCameraSequence;

        trainCameraSequence.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            trainCameraSequence[i] = convertToCorba( cppType[i] );
        }

        return  trainCameraSequence;
    }


    ProtocolCctvTypes::ECctvCommandStatus CctvTypesConversion::convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::ECctvCommandStatus& CorbaType )
    {
        ProtocolCctvTypes::ECctvCommandStatus eCctvCommandStatus;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainExceptionsCorba::CctvProceeding:
                eCctvCommandStatus = ProtocolCctvTypes::CctvProceeding;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::CctvInUse:
                eCctvCommandStatus = ProtocolCctvTypes::CctvInUse;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::CctvFailed:
                eCctvCommandStatus = ProtocolCctvTypes::CctvFailed;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvCommandStatus;
    }


    ProtocolCctvTypes::ECctvSequenceNumber CctvTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvSequenceNumber& CorbaType )
    {
        ProtocolCctvTypes::ECctvSequenceNumber eCctvSequenceNumber;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainCctvCorba::SequenceOne:
                eCctvSequenceNumber = ProtocolCctvTypes::SequenceOne;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceTwo:
                eCctvSequenceNumber = ProtocolCctvTypes::SequenceTwo;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceThree:
                eCctvSequenceNumber = ProtocolCctvTypes::SequenceThree;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceFour:
                eCctvSequenceNumber = ProtocolCctvTypes::SequenceFour;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceFive:
                eCctvSequenceNumber = ProtocolCctvTypes::SequenceFive;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceSix:
                eCctvSequenceNumber = ProtocolCctvTypes::SequenceSix;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvSequenceNumber;
    }


    ProtocolCctvTypes::ECctvSequenceMode CctvTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvSequenceMode& CorbaType )
    {
        ProtocolCctvTypes::ECctvSequenceMode eCctvSequenceMode;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainCctvCorba::SequenceHold:
                eCctvSequenceMode = ProtocolCctvTypes::SequenceHold;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceForward:
                eCctvSequenceMode = ProtocolCctvTypes::SequenceForward;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceReverse:
                eCctvSequenceMode = ProtocolCctvTypes::SequenceReverse;
                break;

            case TA_Base_Bus::ITrainCctvCorba::SequenceSkip:
                eCctvSequenceMode = ProtocolCctvTypes::SequenceSkip;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvSequenceMode;
    }


    CctvTypes::ECctvSwitchMode CctvTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvSwitchMode& CorbaType )
    {
        CctvTypes::ECctvSwitchMode eCctvSwitchMode;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainCctvCorba::CctvInactive:
                eCctvSwitchMode = CctvTypes::CctvInactive;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CctvSequence:
                eCctvSwitchMode = CctvTypes::CctvSequence;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CctvCamera:
                eCctvSwitchMode = CctvTypes::CctvCamera;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CctvQuad:
                eCctvSwitchMode = CctvTypes::CctvQuad;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvSwitchMode;
    }


    CctvTypes::CctvSwitchState CctvTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCctvCorba::CctvSwitchState& CorbaType )
    {
        CctvTypes::CctvSwitchState cctvSwitchState;

        cctvSwitchState.cctvState = convertToCpp( CorbaType.cctvState );
        cctvSwitchState.trainId = CorbaType.trainId;
        cctvSwitchState.item1 = CorbaType.item1;
        cctvSwitchState.item2 = CorbaType.item2;
        cctvSwitchState.item3 = CorbaType.item3;
        cctvSwitchState.item4 = CorbaType.item4;
        cctvSwitchState.preConflictTrain = CorbaType.preConflictTrain;
        cctvSwitchState.stageId = CorbaType.stageId;

        return  cctvSwitchState;
    }


    ProtocolCctvTypes::ECctvCamera CctvTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvCamera& CorbaType )
    {
        ProtocolCctvTypes::ECctvCamera eCctvCamera;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainCctvCorba::CameraNone:
                eCctvCamera = ProtocolCctvTypes::CameraNone;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraOne:
                eCctvCamera = ProtocolCctvTypes::CameraOne;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraTwo:
                eCctvCamera = ProtocolCctvTypes::CameraTwo;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraThree:
                eCctvCamera = ProtocolCctvTypes::CameraThree;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraFour:
                eCctvCamera = ProtocolCctvTypes::CameraFour;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraFive:
                eCctvCamera = ProtocolCctvTypes::CameraFive;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraSix:
                eCctvCamera = ProtocolCctvTypes::CameraSix;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraSeven:
                eCctvCamera = ProtocolCctvTypes::CameraSeven;
                break;

            case TA_Base_Bus::ITrainCctvCorba::CameraEight:
                eCctvCamera = ProtocolCctvTypes::CameraEight;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCctvCamera;
    }


    CctvTypes::TrainCameraSequence CctvTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence& CorbaType )
    {
        CctvTypes::TrainCameraSequence trainCameraSequence;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            trainCameraSequence.push_back( convertToCpp( CorbaType[i] ) );
        }

        return  trainCameraSequence;
    }

}
