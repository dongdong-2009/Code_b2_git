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
#if !defined(CHANGEAREACOMMANDRECIEVEDEVENT_H)
#define CHANGEAREACOMMANDRECIEVEDEVENT_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/MiscDataTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a response to a change area command
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:03 PM
     */

    class ChangeAreaCommandReceivedEvent : public TimsEvent
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
        ChangeAreaCommandReceivedEvent( const TrainMessageData& data,
                                        const std::string& sendingTsi,
                                        const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~ChangeAreaCommandReceivedEvent();


        /**
         * Gets the type of message this event is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Gets the new area/mode of the train
         */
        unsigned char getArea() const;

        /**
         * Formats and returns a log string.
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
        ChangeAreaCommandReceivedEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the area field
         */
        static const unsigned short AREA_POSITION;


        /**
         * The area the train is set to
         */
        ETrainArea m_area;

		/**
		 * The Status 12 information
		 */
		unsigned char m_status12;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:03 PM
     */
    typedef  boost::shared_ptr< ChangeAreaCommandReceivedEvent > ChangeAreaCommandReceivedEventPtr;
}

#endif // !defined(ChangeAreaCommandReceivedEvent_H)
