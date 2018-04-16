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

#include "app/trains/train_agent/src/TrainVoiceCallRequest.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{


    TrainVoiceCallRequest::TrainVoiceCallRequest( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                  unsigned char origin,
                                                  TA_IRS_Bus::RequestForCallTrainCommand::ECallTrainRequestMode requestMode )
          : m_trainId( trainId ),
            m_origin( origin ),
            m_requestMode( requestMode ),
            // intialise as if no response has been received
            m_result( TA_IRS_Bus::CallTypes::TransactionTimeoutError ),
            m_details( "Response Not Received" ),
            m_audioMode( TA_IRS_Bus::CallTrainCommandReceivedEvent::Normal )
    {
        FUNCTION_ENTRY( "TrainVoiceCallRequest" );
        FUNCTION_EXIT;
    }


    TrainVoiceCallRequest::~TrainVoiceCallRequest()
    {
        FUNCTION_ENTRY( "~TrainVoiceCallRequest" );
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::TrainIdType TrainVoiceCallRequest::getTrainId() const
    {
        FUNCTION_ENTRY( "getTrainId" );
        FUNCTION_EXIT;
        return m_trainId;
    }


    unsigned char TrainVoiceCallRequest::getOrigin() const
    {
        FUNCTION_ENTRY( "getOrigin" );
        FUNCTION_EXIT;
        return m_origin;
    }


    TA_IRS_Bus::RequestForCallTrainCommand::ECallTrainRequestMode TrainVoiceCallRequest::getRequestMode() const
    {
        FUNCTION_ENTRY( "getRequestMode" );
        FUNCTION_EXIT;
        return m_requestMode;
    }


    void TrainVoiceCallRequest::setResult( TA_IRS_Bus::ProtocolCallTypes::ECallTrainCommandStatus status,
                                           TA_IRS_Bus::CallTrainCommandReceivedEvent::EAudioMode audioMode )
    {
        FUNCTION_ENTRY( "setResult( ECallTrainCommandStatus, EAudioMode )" );

        // based on the result, set the appropriate state in m_result

        switch ( status )
        {

            case TA_IRS_Bus::ProtocolCallTypes::CallTrainProceeding:
                m_result = TA_IRS_Bus::CallTypes::CallTrainProceeding;
                m_details = "Call Train Proceeding";
                break;

            case TA_IRS_Bus::ProtocolCallTypes::CallTrainAudioRouteInUse:
                m_result = TA_IRS_Bus::CallTypes::CallTrainAudioRouteInUse;
                m_details = "Train Busy";
                break;

            case TA_IRS_Bus::ProtocolCallTypes::CallTrainAudioRouteInUseOptionToOverride:
                m_result = TA_IRS_Bus::CallTypes::CallTrainAudioRouteInUseOptionToOverride;
                m_details = "Train Busy, option to override";
                break;

            case TA_IRS_Bus::ProtocolCallTypes::CallTrainReset:
                m_result = TA_IRS_Bus::CallTypes::CallTrainReset;
                m_details = "Call Train Reset";
                break;

            default:
                TA_ASSERT( false, "Invalid ECallTrainCommandStatus" );
                break;
        }

        m_audioMode = audioMode;

        FUNCTION_EXIT;
    }


    void TrainVoiceCallRequest::setResult( TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError status,
                                           const std::string& details )
    {
        FUNCTION_ENTRY( "setResult( EBadCommandError, details )" );

        // based on the result, sets the appropriate state in m_result

        switch ( status )
        {

            case TA_IRS_Bus::ProtocolCommonTypes::BadChecksum:
                m_result = TA_IRS_Bus::CallTypes::BadCommandBadChecksum;
                m_details = "Bad Command - Bad Checksum";
                break;

            case TA_IRS_Bus::ProtocolCommonTypes::UnknownCommand:
                m_result = TA_IRS_Bus::CallTypes::BadCommandUnknownCommand;
                m_details = "Bad Command - Unknown Command";
                break;

            case TA_IRS_Bus::ProtocolCommonTypes::BadParameter:
                m_result = TA_IRS_Bus::CallTypes::BadCommandBadParameter;
                m_details = "Bad Command - Bad Parameter";
                break;

            case TA_IRS_Bus::ProtocolCommonTypes::UnexpectedCommand:
                m_result = TA_IRS_Bus::CallTypes::BadCommandUnexpectedCommand;
                m_details = "Bad Command - Unexpected Command";
                break;

            default:
                TA_ASSERT( false, "Invalid EBadCommandError" );
                break;
        }

        if ( false == details.empty() )
        {
            m_details = details;
        }

        FUNCTION_EXIT;
    }


    void TrainVoiceCallRequest::setResult( TA_IRS_Bus::ChangeoverStatusEvent::EMpuMode status,
                                           const std::string& details )
    {
        FUNCTION_ENTRY( "setResult( EMpuMode, details )" );

        m_result = TA_IRS_Bus::CallTypes::MpuChangeoverError;
        m_details = "MPU Changeover";

        if ( false == details.empty() )
        {
            m_details = details;
        }

        FUNCTION_EXIT;
    }


    void TrainVoiceCallRequest::setInvalidTrainResult( const std::string& details )
    {
        FUNCTION_ENTRY( "setInvalidTrainResult" );

        m_result = TA_IRS_Bus::CallTypes::InvalidTrainError;
        m_details = "Invalid Train";

        if ( false == details.empty() )
        {
            m_details = details;
        }

        FUNCTION_EXIT;
    }


    void TrainVoiceCallRequest::setTransmissionFailureResult( const std::string& details )
    {
        FUNCTION_ENTRY( "setTransmissionFailureResult" );

        m_result = TA_IRS_Bus::CallTypes::TransmissionFailureError;
        m_details = "Transmission Failure";

        if ( false == details.empty() )
        {
            m_details = details;
        }

        FUNCTION_EXIT;
    }


    void TrainVoiceCallRequest::setTransactionTimeoutResult( const std::string& details )
    {
        FUNCTION_ENTRY( "setTransactionTimeoutResult" );

        m_result = TA_IRS_Bus::CallTypes::TransactionTimeoutError;
        m_details = "Response Not Received";

        if ( false == details.empty() )
        {
            m_details = details;
        }

        FUNCTION_EXIT;
    }


    void TrainVoiceCallRequest::setTransactionCancelledResult( const std::string& details )
    {
        FUNCTION_ENTRY( "setTransactionCancelledResult" );

        m_result = TA_IRS_Bus::CallTypes::TransactionCancelledError;
        m_details = "Operation Cancelled";

        if ( false == details.empty() )
        {
            m_details = details;
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CallTypes::VoiceCallResponse TrainVoiceCallRequest::getCommandResult() const
    {
        FUNCTION_ENTRY( "getCommandResult" );
        
        TA_IRS_Bus::CallTypes::VoiceCallResponse response;
        response.trainId = m_trainId;
        response.result = m_result;
        response.details = m_details;
        
        FUNCTION_EXIT;
        return response;
    }


    bool TrainVoiceCallRequest::isSuccessful() const
    {
        FUNCTION_ENTRY( "isSuccessful" );
        FUNCTION_EXIT;
        return ( TA_IRS_Bus::CallTypes::CallTrainProceeding == m_result );
    }


    TA_IRS_Bus::CallTypes::EVoiceCallResult TrainVoiceCallRequest::getResultCode()
    {
        FUNCTION_ENTRY( "getResultCode" );
        FUNCTION_EXIT;
        return m_result;
    }


    TA_IRS_Bus::CallTrainCommandReceivedEvent::EAudioMode TrainVoiceCallRequest::getAudioMode() const
    {
        FUNCTION_ENTRY( "getAudioMode" );
        FUNCTION_EXIT;
        return m_audioMode;
    }

} // TA_IRS_App

