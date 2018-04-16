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
#if !defined(TRAINEVENTFACTORYIMPL_H)
#define TRAINEVENTFACTORYIMPL_H


#include "bus/trains/TrainTransactionModel/src/ITrainEventFactory.h"


namespace TA_IRS_Bus
{
    /**
     * This implements the train event factory interface.
     * It creates incoming events.
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:32 PM
     */

    class TrainEventFactoryImpl : public ITrainEventFactory
    {

    public:


        /**
         * Constructor
         */
        TrainEventFactoryImpl();


        /**
         * Destructor
         */
        virtual ~TrainEventFactoryImpl();


        /**
         * Creates a train event for this data
         *
         * @return a TrainEvent that fits the given data
         *
         * @exception ProtocolException if the event could not be created
         *
         * @param data    the raw data
         * @param radioId    the sender
         * @param destinationRadioId    the receiver
         */
        virtual TrainEventPtr createEvent( const TrainMessageData& data,
                                           const std::string& radioId,
                                           const std::string& destinationRadioId ) const;

    private:


        /**
         * Given a train event (raw data), extract the message type
         *
         * @param data
         */
        TrainMessageType getMessageTypeFromEvent( const TrainMessageData& data ) const;
 
    };

}

#endif // !defined(TrainEventFactoryImpl_H)
