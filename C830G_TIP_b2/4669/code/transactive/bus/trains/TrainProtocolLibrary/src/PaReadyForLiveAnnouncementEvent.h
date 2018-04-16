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
#if !defined(PaReadyForLiveAnnouncementEvent_H)
#define PaReadyForLiveAnnouncementEvent_H


#include "bus/trains/TrainProtocolLibrary/src/TimsEvent.h"


namespace TA_IRS_Bus
{

    /**
     * Incoming notification of readiness for a live announcement
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:17 PM
     */

    class PaReadyForLiveAnnouncementEvent : public TimsEvent
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
        PaReadyForLiveAnnouncementEvent( const TrainMessageData& data,
                                         const std::string& sendingTsi,
                                         const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~PaReadyForLiveAnnouncementEvent();


        /**
         * Gets the type of message this reader is for.
         */
        static TrainMessageType getStaticType();


        /**
         * Gets the original announcement id
         */
        unsigned short getAnnouncementId() const;


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
        PaReadyForLiveAnnouncementEvent();


        /**
         * Constant used by getStaticType
         */
        static const TrainMessageType MESSAGE_TYPE;


        /**
         * Expected length of the entire message
         */
        static const unsigned short EXPECTED_MESSAGE_LENGTH;


        /**
         * Offset of the announcement id field
         */
        static const unsigned short ANNOUNCEMENT_ID_POSITION;


        /**
         * The original announcement ID
         */
        unsigned char m_announcementId;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:17 PM
     */
    typedef boost::shared_ptr< PaReadyForLiveAnnouncementEvent > PaReadyForLiveAnnouncementEventPtr;

}

#endif // !defined(PaReadyForLiveAnnouncementEvent_H)
