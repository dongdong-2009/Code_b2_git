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
#if !defined(IScheduledEventProcessor_H)
#define IScheduledEventProcessor_H


#include "bus/trains/TrainTransactionModel/src/TrainCommand.h"


namespace TA_IRS_Bus
{
    /**
     * This is the callback for the results of scheduled train commands.
     *
     * The results of each scheduled event will be passed on to this interface. If a
     * scheduled command has a response event it will be given, if it times out, or
     * there is another error this interface will be notified of the failure.
     *
     * For commands without responses, no news is good news - they can be assumed
     * successful.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:40 PM
     */

    class IScheduledEventProcessor
    {

    public:

        /**
         * Handles the result of a successful train command.
         * It is up to the protocol specific layer to determine whether the Event contains
         * a good or bad response.
         *
         * @param scheduleId    This is the task id of the scheduled command.
         * @param command    The command that was executed, along with the response.
         *
         */
        virtual void receiveResponse( long scheduleId,
                                      TrainCommandPtr command ) = 0;


        /**
         * This handles an error response from a scheduled command.
         * At this level an error response will not include a TrainEvent, so long as a
         * TrainEvent is received for the command the status is good, it is then up to the
         * protocol specific layer to determine whether the Event contains a good or bad
         * response.
         *
         * @param scheduleId    The scheduled commands task ID.
         * @param command    A copy of the command that was executed, the original command
         *                   may still be scheduled so it is not modified.
         * @param error    The error encountered executing the command.
         */
        virtual void receiveErrorResponse( long scheduleId,
                                           TrainCommandPtr command,
                                           ETrainError error ) = 0;

    };
}

#endif // !defined(IScheduledEventProcessor_H)
