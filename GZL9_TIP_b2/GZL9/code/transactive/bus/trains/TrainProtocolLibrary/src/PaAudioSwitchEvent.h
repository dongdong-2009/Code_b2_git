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
#if !defined(PaAudioSwitchEvent_H)
#define PaAudioSwitchEvent_H


#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"

namespace TA_IRS_Bus
{

    /**
     * Incoming notification of a driver PA
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:12 PM
     */

    class PaAudioSwitchEvent : public TimsEvent
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
        PaAudioSwitchEvent( const TrainMessageData& data,
                            const std::string& sendingTsi,
                            const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~PaAudioSwitchEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static const TrainMessageType getStaticType();


    protected:

        /**
         * Gets the length of the expected message, so the message length can be validated.
         */
        virtual unsigned short getExpectedMessageLength() const;


    private:


        /**
         * Private default constructor
         */
        PaAudioSwitchEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:12 PM
     */
    typedef boost::shared_ptr< PaAudioSwitchEvent > PaAudioSwitchEventPtr;

}

#endif // !defined(PaAudioSwitchEvent_H)
