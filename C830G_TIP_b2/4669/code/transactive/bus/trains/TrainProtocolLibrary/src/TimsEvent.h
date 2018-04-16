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
#if !defined(TimsEvent_H)
#define TimsEvent_H


#include "bus/trains/TrainTransactionModel/src/TrainEvent.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"

#include <vector>


namespace TA_IRS_Bus
{
    /**
     * Generic Train Event for the TIMS protocol.
     * Provides the basic TIMS fields, and methods to get data.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:31 PM
     */

    class TimsEvent : public TrainEvent
    {

    public:

        /**
         * Position of the message type field (used by the event factory)
         */
        static const unsigned short MESSAGE_TYPE_POSITION;


        /**
         * Create a train event from incoming data
         *
         * @exception ProtocolException if the event could not be created ie the event is invalid
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        TimsEvent( const TrainMessageData& data,
                   const std::string& sendingTsi,
                   const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~TimsEvent();


        /**
         * gets the ID of the train the event is for
         */
        virtual CommonTypes::TrainIdType getTrainId() const;


        /**
         * Gets the type of reply event
         */
        virtual TrainMessageType getEventType() const;


        /**
         * Gets the message origin
         */
        ProtocolCommonTypes::ETimsOrigin getOrigin() const;


        /**
         * Formats and returns a log string.
         */
        virtual std::string getLogString() const;


    protected:

        /**
         * Gets the byte at the given address.
         *
         * @exception ProtocolException if the address is not valid.
         *
         * @param position
         */
        unsigned char getByte( unsigned short position ) const;


        /**
         * Gets the word at the given address.
         *
         * @exception ProtocolException if the address is not valid.
         *
         * @param position
         */
        unsigned short getWord( unsigned short position ) const;


        /**
         * Gets a number bytes, starting from the given address.
         *
         * No conversion is performed, this does not read words, only bytes.
         *
         * @exception ProtocolException if the address is not valid, or the length is too long.
         *
         * @param position    the first byte
         * @param length    the number of bytes to read
         */
        TrainMessageData getBytes( unsigned short position,
                                   unsigned short length ) const;


        /**
         * Gets the length of the expected message, so the message length can be validated.
         * For variable length messages do not override this function.
         *
         * @return an expected message length for fixed length messages.
         */
        virtual unsigned short getExpectedMessageLength() const;


        /**
         * Gets the length of the event's user data (minus checksum)
         */
        unsigned int getUserDataLength() const;


    private:


        /**
         * Private default constructor
         */
        TimsEvent();


        /**
         * Verifies the checksum
         *
         * @exception ProtocolException if the data is not valid.
         *
         *
         * @param data    the data to verify
         */
        void verifyChecksum( const TrainMessageData& data ) const;


        /**
         * Position of the Train ID field
         */
        static const unsigned short TRAIN_ID_POSITION;


        /**
         * Position of the message length field
         */
        static const unsigned short MESSAGE_LENGTH_POSITION;


        /**
         * Position of the origin field
         */
        static const unsigned short ORIGIN_POSITION;


        /**
         * Position of the checksum field from the end (back) of the message
         */
        static const unsigned short CHECKSUM_POSITION;


        /**
         * 99 is the design limit.
         */
        static const unsigned char MAX_TRAIN_ID;


        /**
         * The maximum length of a message
         */
        static const unsigned short MAX_MESSAGE_LENGTH;


        /**
         * The train ID extracted from the message
         */
        CommonTypes::TrainIdType m_trainId;


        /**
         * The message type extracted from the message
         */
        TrainMessageType m_messageType;


        /**
         * The length extracted from the message
         */
        unsigned char m_length;


        /**
         * The origin extracted from the message
         */
        ProtocolCommonTypes::ETimsOrigin m_origin;


        /**
         * The CRC extracted from the message
         */
        unsigned short m_crc;

    };


    /**
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:31 PM
     */
    typedef boost::shared_ptr< TimsEvent > TimsEventPtr;

}

#endif // !defined(TimsEvent_H)
