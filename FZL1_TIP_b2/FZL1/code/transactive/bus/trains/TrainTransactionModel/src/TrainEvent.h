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

#if !defined(TrainEvent_H)
#define TrainEvent_H


#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{
    /**
     * This is the response from a train command. It can be solicited via a
     * TrainCommand, or unsolicited (something that the train sends of its own accord).
     * It can also be both.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:49 PM
     */

    class TrainEvent
    {

    public:


        /**
         * Create a train event from incoming data
         *
         * @param data    The incoming data
         * @param sendingTsi    who sent it
         * @param destinationTsi    where it was sent to
         */
        TrainEvent( const TrainMessageData& data,
                    const std::string& sendingTsi,
                    const std::string& destinationTsi );


        /**
         * Destructor
         */
        virtual ~TrainEvent();


        /**
         * gets the ID of the train the event is for
         */
        virtual CommonTypes::TrainIdType getTrainId() const = 0;


        /**
         * Gets the type of reply event
         */
        virtual TrainMessageType getEventType() const = 0;


        /**
         * Formats and returns a loggable string. This can be overidden to provide more detail.
         * Overriding methods should first call this method, then appent their own data.
         *
         * @return a loggable string
         */
        virtual std::string getLogString() const;


        /**
         * Get the TSI the event came from
         *
         * @return the originating tsi (by const ref)
         */
        const std::string& getTsi() const;


        /**
         * Get the TSI the event was sent to
         *
         * @return the destination tsi (by const ref)
         */
        const std::string& getDestinationTsi() const;


        /**
         * gets the time the event was received.
         */
        time_t getTime() const;


        /**
         * Gets the raw data so the reader can do all the decoding.
         */
        const TrainMessageData& getData() const;


        /**
         * Gets the length of the event's data
         */
        unsigned int getDataLength() const;


    private:


        /**
          * Private default constructor
          */
        TrainEvent();


        /**
         * The Radio Number of the sender
         */
        std::string m_senderTsi;


        /**
         * The Radio Number of the receiver
         */
        std::string m_destinationTsi;


        /**
         * The raw data
         */
        TrainMessageData m_data;


        /**
         * The timestamp of the incoming data.
         */
        time_t m_timestamp;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:49 PM
     */
    typedef boost::shared_ptr< TrainEvent > TrainEventPtr;

}

#endif // !defined(TrainEvent_H)
