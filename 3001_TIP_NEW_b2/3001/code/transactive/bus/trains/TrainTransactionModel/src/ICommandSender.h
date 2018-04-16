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
#if !defined(ICommandSender_H)
#define ICommandSender_H


#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"

#include <string>


namespace TA_IRS_Bus
{
    /**
     * This is an abstraction from the actual radio system, it is used to send
     * commands to a receiver.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:38 PM
     */

    class ICommandSender
    {

    public:

        /**
         * Sends the data to the given radio number.
         *
         * @exception TrainTransmissionFailureException if the send fails for some reason
         *
         * @param radioId    Where to send the data
         * @param data    the data
         */
        virtual void sendData( const std::string& radioId, const TrainMessageData& data ) = 0;

    };

}

#endif // !defined(ICommandSender_H)
