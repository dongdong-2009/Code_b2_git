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
#if !defined(ABSTRACTPECEVENT_H)
#define ABSTRACTPECEVENT_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"


namespace TA_IRS_Bus
{

    /**
     * All PEC events inherit from this. They all specify a PEC number.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:52 PM
     */

    class AbstractPecEvent : public TimsEvent
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
        AbstractPecEvent( const TrainMessageData& data,
                          const std::string& sendingTsi,
                          const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~AbstractPecEvent();


        /**
         * Returns a loggable string with information about this command.
         *
         * @return a string to log
         */
        virtual std::string getLogString() const;


        /**
         * Gets the car this event is for
         */
        ProtocolPecTypes::EPecCarNumber getCar() const;


        /**
         * Gets the PEC this event is for
         */
        ProtocolPecTypes::EPecNumber getPec() const;
		
		/**
         * Gets the RqsOrAck this event is for
         */
		ProtocolPecTypes::EPECFlagNumber getRqsOrAck() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        AbstractPecEvent();


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * offset of the PEC number
         */
        static const unsigned short PEC_NUMBER_POSITION;

		/**
         * offset of the rqsOrAck
         */
        static const unsigned short PEC_RQSORACK_POSITION;

		/**
         * Request or Acknowledge
         */
		ProtocolPecTypes::EPECFlagNumber m_rqsOrAck;

        /**
         * The car number
         */
        ProtocolPecTypes::EPecCarNumber m_carNumber;


        /**
         * The PEC number within the car
         */
        ProtocolPecTypes::EPecNumber m_pecNumber;


        /**
         * The combined PEC byte as read from the event
         */
        unsigned char m_combinedPecByte;

    };


    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:52 PM
     */
    typedef boost::shared_ptr< AbstractPecEvent > AbstractPecEventPtr;

} //end of namespace

#endif // !defined(AbstractPecEvent_H)
