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
#if !defined(ITRAINEVENTFACTORY_H)
#define ITRAINEVENTFACTORY_H


#include "bus/trains/TrainTransactionModel/src/TrainEvent.h"


namespace TA_IRS_Bus
{
    /**
     * Given incoming train data, create an event for it.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:40 PM
     */

    class ITrainEventFactory
    {

    public:

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
                                           const std::string& destinationRadioId ) const = 0;


    };

}

#endif // !defined(ITrainEventFactory_H)
