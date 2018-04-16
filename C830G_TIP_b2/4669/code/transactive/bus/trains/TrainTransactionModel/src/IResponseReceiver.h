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
#if !defined(IResponseReceiver_H)
#define IResponseReceiver_H


#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"


namespace TA_IRS_Bus
{
    /**
     * This is used to abstract from the radio implementation. When something is
     * received via radio this is how the transaction manager receives it.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:39 PM
     */

    class IResponseReceiver
    {

    public:

        /**
         * Receives data via radio
         *
         * @param data    The data
         * @param radioId    The sender
         * @param destinationRadioId    the receiver
         */
        virtual void receiveData( const TrainMessageData& data,
                                  const std::string& radioId,
                                  const std::string& destinationRadioId ) = 0;

    };

}

#endif // !defined(IResponseReceiver_H)
