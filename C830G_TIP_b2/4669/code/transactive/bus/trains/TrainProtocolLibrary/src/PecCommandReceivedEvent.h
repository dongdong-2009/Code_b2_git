/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(PecCommandReceivedEvent_H)
#define PecCommandReceivedEvent_H

#include "bus/trains/TrainProtocolLibrary/src/AbstractPecEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a PEC command received event.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:22 PM
     */

    class PecCommandReceivedEvent : public AbstractPecEvent
    {

    public:

        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        PecCommandReceivedEvent( const TrainMessageData& data,
                                 const std::string& sendingTsi,
                                 const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~PecCommandReceivedEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Returns a loggable string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString() const;


        /**
         * Gets the result of the PEC command
         */
        ProtocolPecTypes::EPecCommandResult getResult() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        PecCommandReceivedEvent();


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Offset of the command result
         */
        static const unsigned short RESULT_POSITION;


        /**
         * The result of the command
         */
        ProtocolPecTypes::EPecCommandResult m_pecResult;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:22 PM
     */
    typedef  boost::shared_ptr< PecCommandReceivedEvent > PecCommandReceivedEventPtr;

}

#endif // !defined(PecCommandReceivedEvent_H)
