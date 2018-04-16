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
#if !defined(TtisCommandReceivedEvent_H)
#define TtisCommandReceivedEvent_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Represents a response to a TTIS command
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:34 PM
     */

    class TtisCommandReceivedEvent : public TimsEvent
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
        TtisCommandReceivedEvent( const TrainMessageData& data,
                                  const std::string& sendingTsi,
                                  const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~TtisCommandReceivedEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Gets the command result
         */
        ProtocolTtisTypes::ETtisResult getResult() const;


        /**
         * Gets the original command (does not directly correlate to TIMS messages)
         */
        ETtisOriginalCommand getOriginalCommand() const;


        /**
         * If an upgrade was performed, this will contain the newest library version.
         */
        unsigned char getLibraryVersion() const;


        /**
         * Gets the schedule version
         */
        unsigned char getScheduleLibraryVersion() const;


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
        TtisCommandReceivedEvent();


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
         * Offset of the result field
         */
        static const unsigned short ORIGINAL_COMMAND_POSITION;


        /**
         * Offset of the result field
         */
        static const unsigned short LIBRARY_VERSION_POSITION;


        /**
         * Offset of the schedule library version
         */
        static const unsigned short SCHEDULE_LIBRARY_VERSION;


        /**
         * The result of the command
         */
        ProtocolTtisTypes::ETtisResult m_result;


        /**
         * The original command according to TTIS
         */
        ETtisOriginalCommand m_originalCommand;


        /**
         * The library version
         */
        unsigned char m_libraryVersion;


        /**
         * The schedule library version
         */
        unsigned char m_scheduleLibraryVersion;
    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:34 PM
     */
    typedef  boost::shared_ptr< TtisCommandReceivedEvent > TtisCommandReceivedEventPtr;

}   //end of namespace

#endif // !defined(TtisCommandReceivedEvent_H)
