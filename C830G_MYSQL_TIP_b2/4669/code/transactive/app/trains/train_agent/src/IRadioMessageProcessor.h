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
#if !defined(IRadioMessageProcessor_H)
#define IRadioMessageProcessor_H

#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * This will process messages from the local radio agent.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:43 PM
     */
    class IRadioMessageProcessor
    {

    public:

        /**
         * This is a handler for incoming SDS messages.
         *
         * @param data    The incoming data
         * @param senderTsi    The senders TSI
         * @param receiverTsi    The receivers TSI
         */
        virtual void processIncomingSds( const TA_IRS_Bus::TrainMessageData& data,
                                         const std::string& senderTsi,
                                         const std::string& receiverTsi ) = 0;


        /**
         * This is a handler for call end events
         *
         * @param consoleId    The console associated with the call
         * @param callId    The call ID
         */
        virtual void processCallEndEvent( unsigned long consoleId,
                                          unsigned long callId ) = 0;


        /**
         * This is a handler for data call end events
         *
         * @param itsi    The TSI the transfer was to
         * @param success    Whether the transfer was successful
         * @param details    The failure reason if the transfer wasnt successful
         */
        virtual void processDataCallEndEvent( const std::string& itsi,
                                              bool success,
                                              const std::string& details ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IRadioMessageProcessor_H)
