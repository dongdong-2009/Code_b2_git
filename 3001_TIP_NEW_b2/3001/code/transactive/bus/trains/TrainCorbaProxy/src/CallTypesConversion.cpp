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

#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainStateUpdateCorba::ETrainCallType CallTypesConversion::convertToCorba( const CallTypes::ETrainCallType& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::ETrainCallType eTrainCallType;

        switch ( cppType )
        {

            case CallTypes::NoCall:
                eTrainCallType = TA_Base_Bus::ITrainStateUpdateCorba::NoCall;
                break;

            case CallTypes::TrainCallOcc:
                eTrainCallType = TA_Base_Bus::ITrainStateUpdateCorba::TrainCallOcc;
                break;

            case CallTypes::OccCallTrain:
                eTrainCallType = TA_Base_Bus::ITrainStateUpdateCorba::OccCallTrain;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTrainCallType;
    }


    TA_Base_Bus::ITrainDriverCallCorba::OccCallReset CallTypesConversion::convertToCorba( const CallTypes::OccCallReset& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::OccCallReset occCallReset;

        occCallReset.trainId = cppType.trainId;
		occCallReset.bResetDueToFailure = cppType.bResetDueToFailure;

        return occCallReset;
    }
	

    TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList CallTypesConversion::convertToCorba( const CallTypes::OccCallRequestList& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList occCallRequestList;

        occCallRequestList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); ++i )
        {
            occCallRequestList[i] = convertToCorba( cppType[i] );
        }

        return occCallRequestList;
    }


    TA_Base_Bus::ITrainDriverCallCorba::ItsiList CallTypesConversion::convertToCorba( const CallTypes::ItsiList& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::ItsiList itsiList;

        itsiList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); ++i )
        {
            itsiList[i] = CORBA::string_dup( cppType[i].c_str() );
        }

        return itsiList;
    }


    TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponseList CallTypesConversion::convertToCorba( const CallTypes::VoiceCallResponseList& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponseList voiceCallResponseList;

        voiceCallResponseList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); ++i )
        {
            voiceCallResponseList[i] = convertToCorba( cppType[i] );
        }

        return voiceCallResponseList;
    }


    TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult CallTypesConversion::convertToCorba( const CallTypes::DriverCallResult& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult driverCallResult;

        driverCallResult.callId = cppType.callId;
        driverCallResult.consoleId = cppType.consoleId;
        driverCallResult.resultList = convertToCorba( cppType.resultList );

        return driverCallResult;
    }


    TA_Base_Bus::ITrainDriverCallCorba::EVoiceCallResult CallTypesConversion::convertToCorba( const CallTypes::EVoiceCallResult& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::EVoiceCallResult eVoiceCallResult;

        switch ( cppType )
        {

            case CallTypes::CallTrainProceeding:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::CallTrainProceeding;
                break;

            case CallTypes::CallTrainAudioRouteInUse:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::CallTrainAudioRouteInUse;
                break;

            case CallTypes::CallTrainAudioRouteInUseOptionToOverride:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::CallTrainAudioRouteInUseOptionToOverride;
                break;

            case CallTypes::CallTrainReset:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::CallTrainReset;
                break;

            case CallTypes::BadCommandBadChecksum:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::BadCommandBadChecksum;
                break;

            case CallTypes::BadCommandUnknownCommand:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::BadCommandUnknownCommand;
                break;

            case CallTypes::BadCommandBadParameter:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::BadCommandBadParameter;
                break;

            case CallTypes::BadCommandUnexpectedCommand:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::BadCommandUnexpectedCommand;
                break;

            case CallTypes::InvalidTrainError:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::InvalidTrainError;
                break;

            case CallTypes::TransmissionFailureError:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::TransmissionFailureError;
                break;

            case CallTypes::TransactionTimeoutError:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::TransactionTimeoutError;
                break;

            case CallTypes::TransactionCancelledError:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::TransactionCancelledError;
                break;

            case CallTypes::MpuChangeoverError:
                eVoiceCallResult = TA_Base_Bus::ITrainDriverCallCorba::MpuChangeoverError;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eVoiceCallResult;
    }


    TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponse CallTypesConversion::convertToCorba( const CallTypes::VoiceCallResponse& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponse voiceCallResponse;

        voiceCallResponse.details = cppType.details.c_str();
        voiceCallResponse.trainId = cppType.trainId;
        voiceCallResponse.result = convertToCorba( cppType.result );

        return voiceCallResponse;
    }


    TA_Base_Bus::ITrainDriverCallCorba::OccCallRequest CallTypesConversion::convertToCorba( const CallTypes::OccCallRequest& cppType )
    {
        TA_Base_Bus::ITrainDriverCallCorba::OccCallRequest occCallRequest;

        occCallRequest.timestamp = cppType.timestamp;
        occCallRequest.trainId = cppType.trainId;

        return  occCallRequest;
    }


    TA_Base_Bus::IRadioSessionCorbaDef::ECallType CallTypesConversion::convertToCorba( const CallTypes::ECallType& cppType )
    {
        TA_Base_Bus::IRadioSessionCorbaDef::ECallType eCallType;

        switch ( cppType )
        {

            case CallTypes::SingleCall:
                eCallType = TA_Base_Bus::IRadioSessionCorbaDef::SingleCall;
                break;

            case CallTypes::GroupUnacknowledgeCall:
                eCallType = TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall;
                break;

            case CallTypes::GroupAcknowledgeCall:
                eCallType = TA_Base_Bus::IRadioSessionCorbaDef::GroupAcknowledgeCall;
                break;

            case CallTypes::GroupBroadcastCall:
                eCallType = TA_Base_Bus::IRadioSessionCorbaDef::GroupBroadcastCall;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCallType;
    }


    TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority CallTypesConversion::convertToCorba( const CallTypes::ECallPriority& cppType )
    {
        TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority eCallPriority;

        switch ( cppType )
        {

            case CallTypes::Priority0:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority0;
                break;

            case CallTypes::Priority1:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority1;
                break;

            case CallTypes::Priority2:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority2;
                break;

            case CallTypes::Priority3:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority3;
                break;

            case CallTypes::Priority4:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority4;
                break;

            case CallTypes::Priority5:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority5;
                break;

            case CallTypes::Priority6:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority6;
                break;

            case CallTypes::Priority7:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority7;
                break;

            case CallTypes::Priority8:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority8;
                break;

            case CallTypes::Priority9:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority9;
                break;

            case CallTypes::Priority10:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority10;
                break;

            case CallTypes::Priority11:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority11;
                break;

            case CallTypes::Priority12:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority12;
                break;

            case CallTypes::Priority13:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority13;
                break;

            case CallTypes::Priority14:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority14;
                break;

            case CallTypes::Priority15:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::Priority15;
                break;

            case CallTypes::DefaultNormalPriority:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::DefaultNormalPriority;
                break;

            case CallTypes::DefaultEmergencyPriority:
                eCallPriority = TA_Base_Bus::IRadioSessionCorbaDef::DefaultEmergencyPriority;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eCallPriority;
    }


    CallTypes::ETrainCallType CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETrainCallType& CorbaType )
    {
        CallTypes::ETrainCallType eTrainCallType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainStateUpdateCorba::NoCall:
                eTrainCallType = CallTypes::NoCall;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TrainCallOcc:
                eTrainCallType = CallTypes::TrainCallOcc;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::OccCallTrain:
                eTrainCallType = CallTypes::OccCallTrain;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return  eTrainCallType;
    }

    CallTypes::VoiceCallResponse CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponse& CorbaType )
    {
        CallTypes::VoiceCallResponse voiceCallResponse;

        voiceCallResponse.details = CorbaType.details.in();
        voiceCallResponse.trainId = CorbaType.trainId;
        voiceCallResponse.result = convertToCpp( CorbaType.result );

        return  voiceCallResponse;
    }

    CallTypes::EVoiceCallResult CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::EVoiceCallResult& CorbaType )
    {
        CallTypes::EVoiceCallResult eVoiceCallResult;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainDriverCallCorba::CallTrainProceeding:
                eVoiceCallResult = CallTypes::CallTrainProceeding;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::CallTrainAudioRouteInUse:
                eVoiceCallResult = CallTypes::CallTrainAudioRouteInUse;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::CallTrainAudioRouteInUseOptionToOverride:
                eVoiceCallResult = CallTypes::CallTrainAudioRouteInUseOptionToOverride;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::CallTrainReset:
                eVoiceCallResult = CallTypes::CallTrainReset;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::BadCommandBadChecksum:
                eVoiceCallResult = CallTypes::BadCommandBadChecksum;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::BadCommandUnknownCommand:
                eVoiceCallResult = CallTypes::BadCommandUnknownCommand;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::BadCommandBadParameter:
                eVoiceCallResult = CallTypes::BadCommandBadParameter;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::BadCommandUnexpectedCommand:
                eVoiceCallResult = CallTypes::BadCommandUnexpectedCommand;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::InvalidTrainError:
                eVoiceCallResult = CallTypes::InvalidTrainError;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::TransmissionFailureError:
                eVoiceCallResult = CallTypes::TransmissionFailureError;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::TransactionTimeoutError:
                eVoiceCallResult = CallTypes::TransactionTimeoutError;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::TransactionCancelledError:
                eVoiceCallResult = CallTypes::TransactionCancelledError;
                break;

            case TA_Base_Bus::ITrainDriverCallCorba::MpuChangeoverError:
                eVoiceCallResult = CallTypes::MpuChangeoverError;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }


        return  eVoiceCallResult;
    }

    CallTypes::DriverCallResult CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult& CorbaType )
    {
        CallTypes::DriverCallResult driverCallResult;

        driverCallResult.callId = CorbaType.callId;
        driverCallResult.consoleId = CorbaType.consoleId;
        driverCallResult.resultList = convertToCpp( CorbaType.resultList );

        return  driverCallResult;
    }

    CallTypes::VoiceCallResponseList CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponseList& CorbaType )
    {
        CallTypes::VoiceCallResponseList voiceCallResponseList;

        for ( unsigned int i = 0; i < CorbaType.length(); ++i )
        {
            CallTypes::VoiceCallResponse voiceCallResponse;
            voiceCallResponse = convertToCpp( CorbaType[i] );
            voiceCallResponseList.push_back( voiceCallResponse );
        }

        return  voiceCallResponseList;
    }

    CallTypes::ItsiList CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::ItsiList& CorbaType )
    {
        CallTypes::ItsiList itsiList;

        for ( unsigned int i = 0; i < CorbaType.length(); ++i )
        {
            std::string str( CorbaType[i] );
            itsiList.push_back( str );
        }

        return  itsiList;
    }


    CallTypes::OccCallRequestList CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList& CorbaType )
    {

        CallTypes::OccCallRequestList occCallRequestList;

        for ( unsigned int i = 0; i < CorbaType.length(); ++i )
        {
            CallTypes::OccCallRequest occCallRequest;
            occCallRequest = convertToCpp( CorbaType[i] );
            occCallRequestList.push_back( occCallRequest );
        }

        return  occCallRequestList;
    }


    CallTypes::OccCallRequest CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::OccCallRequest& CorbaType )
    {
        CallTypes::OccCallRequest occCallRequest;
        occCallRequest.timestamp = CorbaType.timestamp;
        occCallRequest.trainId = CorbaType.trainId;
        return  occCallRequest;
    }


    CallTypes::OccCallReset CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::OccCallReset& CorbaType )
    {
        CallTypes::OccCallReset occCallReset;
        occCallReset.trainId = CorbaType.trainId;
		occCallReset.bResetDueToFailure = CorbaType.bResetDueToFailure;
        return  occCallReset;
    }


    CallTypes::ECallType CallTypesConversion::convertToCpp( const TA_Base_Bus::IRadioSessionCorbaDef::ECallType& CorbaType )
    {
        CallTypes::ECallType eCallType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::IRadioSessionCorbaDef::SingleCall:
                eCallType = CallTypes::SingleCall;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall:
                eCallType = CallTypes::GroupUnacknowledgeCall;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::GroupAcknowledgeCall:
                eCallType = CallTypes::GroupAcknowledgeCall;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::GroupBroadcastCall:
                eCallType = CallTypes::GroupBroadcastCall;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return   eCallType;
    }


    CallTypes::ECallPriority CallTypesConversion::convertToCpp( const TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority& CorbaType )
    {
        CallTypes::ECallPriority eCallPriority;

        switch ( CorbaType )
        {

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority0:
                eCallPriority = CallTypes::Priority0;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority1:
                eCallPriority = CallTypes::Priority1;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority2:
                eCallPriority = CallTypes::Priority2;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority3:
                eCallPriority = CallTypes::Priority3;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority4:
                eCallPriority = CallTypes::Priority4;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority5:
                eCallPriority = CallTypes::Priority5;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority6:
                eCallPriority = CallTypes::Priority6;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority7:
                eCallPriority = CallTypes::Priority7;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority8:
                eCallPriority = CallTypes::Priority8;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority9:
                eCallPriority = CallTypes::Priority9;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority10:
                eCallPriority = CallTypes::Priority10;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority11:
                eCallPriority = CallTypes::Priority11;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority12:
                eCallPriority = CallTypes::Priority12;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority13:
                eCallPriority = CallTypes::Priority13;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority14:
                eCallPriority = CallTypes::Priority14;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::Priority15:
                eCallPriority = CallTypes::Priority15;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::DefaultNormalPriority:
                eCallPriority = CallTypes::DefaultNormalPriority;
                break;

            case TA_Base_Bus::IRadioSessionCorbaDef::DefaultEmergencyPriority:
                eCallPriority = CallTypes::DefaultEmergencyPriority;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }


        return  eCallPriority;
    }

	TA_Base_Bus::ITrainDriverCallCorba::TrainChangeover CallTypesConversion::convertToCorba( const CallTypes::TrainChangeover& cppType )
	{
		TA_Base_Bus::ITrainDriverCallCorba::TrainChangeover trainChangeover;

		trainChangeover.trainId = cppType.trainId;
		trainChangeover.resourceId = cppType.resourceId;

		return trainChangeover;
	}

	CallTypes::TrainChangeover CallTypesConversion::convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::TrainChangeover& CorbaType )
	{
		CallTypes::TrainChangeover trainChangeover;

		trainChangeover.trainId = CorbaType.trainId;
		trainChangeover.resourceId = CorbaType.resourceId;

		return  trainChangeover;
	}



}
