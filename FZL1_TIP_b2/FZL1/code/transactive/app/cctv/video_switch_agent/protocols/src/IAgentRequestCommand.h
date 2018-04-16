#ifndef IAGENT_REQUEST_COMMAND_H
#define IAGENT_REQUEST_COMMAND_H

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h $
 * @author:  Katherine Thomson
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2013/07/30 17:42:30 $
 * Last modified by:  $Author: huang.wenbo $
 * 
 * This interface is used by the communication handler to handle
 * requests made by the agent for the hardware.
 *
 * This interface may be used by a protocol library,
 * but it is not mandatory.
 */

#include <string>
#include <vector>

namespace TA_IRS_App
{
    class IAgentRequestCommand
    {
    public:

        enum ERequestCommandCompletionState
        {
            SUCCEEDED,
            COULD_NOT_BE_SENT,
            TIMED_OUT,
            RECEIVED_ERRONEOUS_RESPONSE,
			RECEIVED_UNEXPECTED_PACKET,
            COMMAND_LIFECYCLE_INCOMPLETE
        };

        /**
          * ~IAgentRequestCommand
          *
          * Standard destructor.
          */

        virtual ~IAgentRequestCommand() {};

        /**
          * getDataToSend
          *
          * Returns the data to be sent to the hardware.
          *
          * @return     std::string
          *             The data to be sent to the hardware.
          */

        virtual std::string getDataToSend() = 0;

        /**
          * isResponseExpected
          *
          * Returns true if there should be a response to this command.
          *
          * @return     bool
          *             True if a response is expected.
          *             False otherwise.
          */

        virtual bool isResponseExpected() = 0;

        /**
          * processResponse
          *
          * Processes the response to this command that has been received from
          * the switching hardware.
          *
          * @param      ResponseData& data
          *             The data to process.
          *
          * @return     bool
          *             True if the data was a valid response for this command.
          *             False otherwise.
          */

        virtual bool processResponse( const std::vector< unsigned char >& data ) = 0;

        /** 
          * waitForResponseOrTimeOut
          *
          * description
          *
          */

        virtual void waitForCompletion() = 0;

        /** 
          * getCompletionState
          *
          * description
          *
          * @return 
          */

        virtual ERequestCommandCompletionState getCompletionState() = 0;

        /** 
          * signalResponseOrTimeOut
          *
          * description
          *
          * @param responseWasReceived
          */

        virtual void signalResponse() = 0;

        /** 
          * signalCouldNotBeSent
          *
          * description
          *
          */

        virtual void signalCouldNotBeSent() = 0;

        /** 
          * signalTimedOut
          *
          * description
          *
          */

        virtual void signalTimedOut() = 0;




    }; // IAgentRequestCommand

} // TA_IRS_App

#endif // IAGENT_REQUEST_COMMAND_H