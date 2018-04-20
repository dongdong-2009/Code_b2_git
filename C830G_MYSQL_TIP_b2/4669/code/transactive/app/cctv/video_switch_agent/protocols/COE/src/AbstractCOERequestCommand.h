#ifndef ABSTRACT_COE_REQUEST_COMMAND_H
#define ABSTRACT_COE_REQUEST_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  */

#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"

#include "core/synchronisation/src/Semaphore.h"

namespace TA_IRS_App
{
    class AbstractCOERequestCommand : public virtual IAgentRequestCommand
    {
    public:

        /**
          * AbstractCOERequestCommand
          *
          * Constructs the command to send to the switching hardware regarding
          * the switch of an input to an output.
          *
          * @param      ESequencePlaybackCommand sequencePlaybackCommand
          *             The sequence playback command type to be sent.
          * @param      unsigned long videoOutputEntityKey
          *             The entity key of the VideoOutput the Sequence Playback 
          *             being controlled is on.
          * @param      COESequence& sequence
          *             The Sequence to control playback for.
          *
          * @exception  DatabaseException
          *             Thrown if the database cannot be contacted.
          * @exception  DataException
          *             Thrown if the data cannot be retrieved or is invalid.
          */
        AbstractCOERequestCommand();


        /**
          * ~AbstractCOERequestComand
          *
          * Standard destructor.
          */
        virtual ~AbstractCOERequestCommand();


        /**
          * getDataToSend
          *
          * Returns the data to be sent to the hardware.
          *
          * @return     std::string
          *             The data to be sent to the hardware.
          */
        virtual std::string getDataToSend();


        /**
          * isResponseExpected
          *
          * Returns true if there should be a response to this command.
          *
          * @return     bool
          *             True if a response is expected.
          *             False otherwise.
          */
        virtual bool isResponseExpected() {return true;}


        /** 
          * waitForResponseOrTimeOut
          *
          * description
          *
          */
        virtual void waitForCompletion();


        /** 
          * getCompletionState
          *
          * description
          *
          * @return 
          */
        virtual IAgentRequestCommand::ERequestCommandCompletionState getCompletionState();


        /** 
          * signalResponseOrTimeOut
          *
          * description
          *
          * @param responseWasReceived
          */
        virtual void signalResponse();


        /** 
          * signalCouldNotBeSent
          *
          * description
          *
          */
        virtual void signalCouldNotBeSent();


        /** 
          * signalTimedOut
          *
          * description
          *
          */
        virtual void signalTimedOut();

    protected:

        /** 
          * processGenericResponsePacketAttributes
          *
          * description
          *
          * @param data
          * @param minAcceptableLength
          * @param 
          *
          * @return 
          *
          */
        bool processGenericResponsePacketAttributes( const std::vector< unsigned char >& data, unsigned char command, unsigned int minAcceptableLength, unsigned int maxAcceptableLength );

        //
        // The data for the command that will be sent to the hardware.
        //

        std::string m_data;

        IAgentRequestCommand::ERequestCommandCompletionState m_completionState;

    private:

        //
        // Disable copy constructor and assignment operator.
        //

        AbstractCOERequestCommand( const AbstractCOERequestCommand& theAbstractCOERequestCommand );
        AbstractCOERequestCommand& operator=( const AbstractCOERequestCommand& );

        TA_Base_Core::Semaphore m_semaphore;
    };
}

#endif // ABSTRACT_COE_REQUEST_COMMAND_H
