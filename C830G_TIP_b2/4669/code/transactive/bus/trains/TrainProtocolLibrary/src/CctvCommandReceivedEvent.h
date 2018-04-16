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
#if !defined(CctvCommandReceivedEvent_H)
#define CctvCommandReceivedEvent_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a response to a cctv command
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:59 PM
     */

    class CctvCommandReceivedEvent : public TimsEvent
    {

    public:

        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie this is not the right kind of event or the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        CctvCommandReceivedEvent( const TrainMessageData& data,
                                  const std::string& sendingTsi,
                                  const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~CctvCommandReceivedEvent();


        /**
         * Gets the type of message this event is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Gets the command result
         */
        ProtocolCctvTypes::ECctvCommandStatus getResult() const;


        /**
         * Formats and returns a loggable string.
         */
        virtual std::string getLogString() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        CctvCommandReceivedEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the result field
         */
        static const unsigned short RESULT_POSITION;


        /**
         * The command result
         */
        ProtocolCctvTypes::ECctvCommandStatus m_result;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:59 PM
     */
    typedef  boost::shared_ptr< CctvCommandReceivedEvent > CctvCommandReceivedEventPtr;

}  //end of namespace

#endif // !defined(CctvCommandReceivedEvent_H)
