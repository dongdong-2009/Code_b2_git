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
#if !defined(CallTrainCommandReceivedEvent_H)
#define CallTrainCommandReceivedEvent_H

#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCallTypes.h"


namespace TA_IRS_Bus
{

    /**
     * Response to a call train command
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:57 PM
     */

    class CallTrainCommandReceivedEvent : public TimsEvent
    {

    public:

        /**
         * Indicates which MPU to make the audio call to, the active or the opposite.
         * @author Adam Radics
         * @version 1.0
         * @created 05-Feb-2008 3:19:57 PM
         */
        enum EAudioMode
        {
            Normal = 0,
            Opposite = 1
        };


        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie this is not the right kind of event or the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        CallTrainCommandReceivedEvent( const TrainMessageData& data,
                                       const std::string& sendingTsi,
                                       const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~CallTrainCommandReceivedEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Formats and returns a log string.
         */
        virtual std::string getLogString() const;


        /**
         * Gets the status of the previous command
         */
        ProtocolCallTypes::ECallTrainCommandStatus getStatus() const;


        /**
         * Gets the way the audio call must be set up. To the active MPU or the other one.
         */
        EAudioMode getAudioMode() const;


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:

        /**
         * Private default constructor
         */
        CallTrainCommandReceivedEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the status field
         */
        static const unsigned short STATUS_POSITION;


        /**
         * Offset of the audio mode field
         */
        static const unsigned short AUDIO_MODE_POSITION;


        /**
         * The result of the request
         */
        ProtocolCallTypes::ECallTrainCommandStatus m_commandStatus;


        /**
         * which MPU to make the audio call to, the active or the opposite.
         */
        EAudioMode m_audioMode;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:19:57 PM
     */
    typedef boost::shared_ptr< CallTrainCommandReceivedEvent > CallTrainCommandReceivedEventPtr;

}

#endif // !defined(CallTrainCommandReceivedEvent_H)
