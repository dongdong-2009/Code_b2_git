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
#if !defined(TestCallEvent_H)
#define TestCallEvent_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Represents an incoming test call
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:28 PM
     */

    class TestCallEvent : public TimsEvent
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
        TestCallEvent( const TrainMessageData& data,
                       const std::string& sendingTsi,
                       const std::string& destinationTsi );

        
        /**
         * Destructor
         */
        virtual ~TestCallEvent();


        /**
         * Gets the type of message this event is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Formats and returns a loggable string.
         */
        virtual std::string getLogString() const;


        /**
         * Gets the TTIS library version on the train
         */
        unsigned short getTtisVersion() const;


        /**
         * Gets the PA library version on the train
         */
        unsigned short getPaVersion() const;


        /**
         * Gets the Schedule library version on the train
         */
        unsigned short getScheduleVersion() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        TestCallEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the ttis library version field
         */
        static const unsigned short TTIS_VERSION_POSITION;


        /**
         * Offset of the pa library version field
         */
        static const unsigned short PA_VERSION_POSITION;


        /**
         * Offset of the schedule library version field
         */
        static const unsigned short SCHEDULE_VERSION_POSITION;


        /**
         * The PA version number
         */
        unsigned short m_paVersion;


        /**
         * The TTIS version number
         */
        unsigned short m_ttisVersion;


        /**
         * The schedule version number
         */
        unsigned short m_scheduleVersion;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:28 PM
     */
    typedef boost::shared_ptr< TestCallEvent > TestCallEventPtr;

}

#endif // !defined(TestCallEvent_H)
