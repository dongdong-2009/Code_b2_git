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
#if !defined(TrainVoiceCallRequest_H)
#define TrainVoiceCallRequest_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCallTypes.h"

// these contain the various protocol response types
#include "bus/trains/TrainProtocolLibrary/src/RequestForCallTrainCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"

#include <vector>

namespace TA_IRS_App
{

    /**
     * This is a simple object used to track the status of a voice call request to a train
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:39 PM
     */
    class TrainVoiceCallRequest
    {

    public:

        /**
         * Constructor
         *
         * @param trainId    The train this request is for
         * @param origin    The operator MFT origin
         * @param requestMode    Whether a normal request or override
         */
        TrainVoiceCallRequest( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                               unsigned char origin,
                               TA_IRS_Bus::RequestForCallTrainCommand::ECallTrainRequestMode requestMode );


        /**
         * Destructor
         */
        ~TrainVoiceCallRequest();


        /**
         * Gets the train ID this request is for
         *
         * @return the train id
         */
        TA_IRS_Bus::CommonTypes::TrainIdType getTrainId() const;


        /**
         * Gets the origin to put in the outgoing command
         *
         * @return the origin
         */
        unsigned char getOrigin() const;


        /**
         * Gets the request mode to put in the outgoing command
         *
         * @return the request mode
         */
        TA_IRS_Bus::RequestForCallTrainCommand::ECallTrainRequestMode getRequestMode() const;


        /**
         * This will set the result of the command that was sent to the train.
         * This handles a valid response from the train
         *
         * @param status    the status received from a call train command response
         * @param audioMode    The audio mode the train requests
         */
        void setResult( TA_IRS_Bus::ProtocolCallTypes::ECallTrainCommandStatus status,
                        TA_IRS_Bus::CallTrainCommandReceivedEvent::EAudioMode audioMode );


        /**
         * This will set the result of the command that was sent to the train.
         * This handles a bad command response
         *
         * @param status    the status received from a bad command response
         * @param details    The error details
         */
        void setResult( TA_IRS_Bus::ProtocolCommonTypes::EBadCommandError status,
                        const std::string& details = "" );


        /**
         * This will set the result of the command that was sent to the train.
         * This handles an MPU changeover command response
         *
         * @param status    the status received from a changeover status event response
         * @param details    Optional error details
         */
        void setResult( TA_IRS_Bus::ChangeoverStatusEvent::EMpuMode status,
                        const std::string& details = "" );


        /**
         * This will set the command to an Invalid Train response
         *
         * @param details    exception details
         */
        void setInvalidTrainResult( const std::string& details );


        /**
         * This will set the command to an Transmission Failure response
         *
         * @param details    exception details
         */
        void setTransmissionFailureResult( const std::string& details );


        /**
         * This will set the command to a Transaction Timeout response
         *
         * @param details    exception details
         */
        void setTransactionTimeoutResult( const std::string& details );


        /**
         * This will set the command to an Transaction Cancelled response
         *
         * @param details    exception details
         */
        void setTransactionCancelledResult( const std::string& details );


        /**
         * This will create and return a structure suitable to pass back to the client as a result
         *
         * @return a VoiceCallResponse structure for the result of this command
         */
        TA_IRS_Bus::CallTypes::VoiceCallResponse getCommandResult() const;


        /**
         * Check whether this command responded that it was OK to start a call.
         *
         * @return true if this train will accept the incoming call
         */
        bool isSuccessful() const;


        /**
         * Get the result of the command, whether it succeeded, or the error type if it failed
         *
         * @return the result code
         */
        TA_IRS_Bus::CallTypes::EVoiceCallResult getResultCode();


        /**
         * In case this is successful, check which MPU the call must be made to
         *
         * @return the MPU to call
         */
        TA_IRS_Bus::CallTrainCommandReceivedEvent::EAudioMode getAudioMode() const;

    private:

        /**
         * Private default constructor
         */
        TrainVoiceCallRequest();
        TrainVoiceCallRequest( const TrainVoiceCallRequest& rhs );


        /**
         * The ID of the train this request is for
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The command origin
         */
        unsigned char m_origin;


        /**
         * The command request mode
         */
        TA_IRS_Bus::RequestForCallTrainCommand::ECallTrainRequestMode m_requestMode;


        /**
         * The result of the command, whether it succeeded, or the error type if it failed
         * Initialise to TransactionTimeoutError in the constructor, because if a response is not received this is the most accurate status.
         */
        TA_IRS_Bus::CallTypes::EVoiceCallResult m_result;


        /**
         * Optional extra information for debug logging purposes
         */
        std::string m_details;


        /**
         * The MPU to call, where the driver is
         */
        TA_IRS_Bus::CallTrainCommandReceivedEvent::EAudioMode m_audioMode;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:39 PM
     */
    typedef boost::shared_ptr < TrainVoiceCallRequest > TrainVoiceCallRequestPtr;
    typedef std::vector < TrainVoiceCallRequestPtr > TrainVoiceCallRequestList;

} // TA_IRS_App

#endif // !defined(TrainVoiceCallRequest_H)
