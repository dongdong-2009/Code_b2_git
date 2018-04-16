/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(ITrainTransactionManager_H)
#define ITrainTransactionManager_H


#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"
#include "bus/trains/TrainTransactionModel/src/TrainCommand.h"


// forward declarations
namespace TA_IRS_Bus
{
    class ITrainEventProcessor;
    class IScheduledEventProcessor;
}


namespace TA_IRS_Bus
{
    /**
     * @author adamr
     * @version 1.0
     * @created 30-Jan-2008 2:53:44 PM
     */

    class ITrainTransactionManager
    {

    public:


        /**
         * This allows for objects to register for train events of a certain type.
         * They will get the event only if it is not consumed by an existing transaction,
         * so only unsolicited events are passed through.
         *
         * Note that multiple handlers can exist for a single event type.
         *
         * @param eventType
         * @param listener
         */
        virtual void registerForTrainEvents( TrainMessageType eventType,
                                             ITrainEventProcessor* listener ) = 0;


        /**
         * This allows for objects to deregister for train events of a certain type.
         *
         * @param eventType
         * @param listener
         */
        virtual void deregisterForTrainEvents( TrainMessageType eventType,
                                               ITrainEventProcessor* listener ) = 0;


        /**
         * Sends a command to the train. Optionally waits for a response (depending on the command).
         *
         * This will also allow the command to timeout and retry a number of times.
         *
         * This also allows sending to a specific radio ID - say responding to the test
         * call from a backup on board computer rather than the primary.
         *
         * Only one command per resource category can be executed on a train at one time.
         *
         * @exception TrainTransmissionFailureException if the command could not be sent
         * @exception TrainTimeoutException if the command times out after the specified timeout and retries
         * @exception InvalidTrainException if the train (or its TSI) cannot be found
         *
         * @param command    The command to send
         * @param timeout    The time to wait for the response (if any) in seconds. 0 is to use the default.
         * @param retries    The number of times to retry the command if it times out (no response) -1 is to use the default
         * @param ignoreValidity    If true, this command will be sent to the train even if it is considered invalid.
         * @param radioId    optionally send to a specific radio ID
         */
        virtual void sendCommand( TrainCommandPtr command,
                                  unsigned long timeout = 0,
                                  int retries = -1,
                                  bool ignoreValidity = false,
                                  const std::string& radioId = "" ) = 0;


        /**
         * This is the same as sendCommand, except it sends a number of commands simultaneously.
         * Each command will be for a different train.
         *
         * If the command has a response associated with it, this method will block until all commands have either timed out or completed.
         *
         * @return A vector of statuses, for each command sent. The status order matches exactly the input vector.
         *
         * @param commands    The list of commands to execute
         * @param timeout    The number of seconds to wait for the response. If 0 the default will be used.
         * @param retries    The number of times to retry a command that timed out. If -1 the default will be used.
         * @param ignoreValidity    If true, this command will be sent to the train even if it is considered invalid.
         */
        virtual std::vector<ETrainError> sendCommands( const TrainCommandList& commands,
                                                       unsigned long timeout = 0,
                                                       int retries = -1,
                                                       bool ignoreValidity = false ) = 0;


        /**
         * Cancels all transactions for the given train ID's, and command numbers.
         * If the given command numbers are empty, then this will cancel all transactions for the given trains.
         * If the given train ID's is empty, then this will cancel transactions for the given command numbers for all trains.
         *
         * @param trainIdList    The list of train IDs
         * @param commandList
         */
        virtual void cancelTransactions( const CommonTypes::TrainIdList& trainIdList,
                                         const TrainMessageTypeList& commandList ) = 0;


        /**
         * Schedules a command to be periodically executed.
         * Scheduled commands are never retried, they are only given one shot.
         *
         * If a command is still in progress when the next execution occurs,
         * the next execution is aborted. This prevents slow commands "stacking up".
         *
         * @return The schedule id for later cancellation.
         *
         * @param command
         * @param responseHandler    The callback for the results of each command executed
         * @param scheduleTime    The number of milliseconds from now to execute this command
         * @param recurring    whether this command repeats
         * @param commandTimeout    The timeout to use for this command
         * @param ignoreValidity    If true, this command will be sent to the train even if it is considered invalid.
         * @param radioId    optionally send to a specific radio ID
         */
        virtual long scheduleCommand( TrainCommandPtr command,
                                      IScheduledEventProcessor& responseHandler,
                                      unsigned long scheduleTime,
                                      bool recurring = false,
                                      unsigned long commandTimeout = 0,
                                      bool ignoreValidity = false,
                                      const std::string& radioId = "" ) = 0;


        /**
         * Removes a scheduled command from the scheduled commands.
         *
         * @param commandId
         */
        virtual void removeScheduledCommand( long commandId ) = 0;


        /**
         * Starts a timer for a train.
         *
         * @param trainId
         * @param resource    The timer category
         * @param time    The time in seconds before the timer expires
         */
        virtual void startTrainTimer( CommonTypes::TrainIdType trainId,
                                      CommonTypes::ETrainResource resource,
                                      unsigned long time ) = 0;


        /**
         * Waits on a timer previously started for a train.
         *
         * @exception TrainTimeoutException If the timer expires.
         *
         * @param trainId
         * @param resource
         */
        virtual void waitTrainTimer( CommonTypes::TrainIdType trainId,
                                     CommonTypes::ETrainResource resource ) = 0;


        /**
         * Signals a train timer.
         *
         * @exception InvalidTrainException if the timer doesnt exist
         *
         * @param trainId
         * @param resource
         */
        virtual void signalTrainTimer( CommonTypes::TrainIdType trainId,
                                       CommonTypes::ETrainResource resource ) = 0;


        /**
         * Ends a timer previously started for a train.
         *
         * @param trainId
         * @param resource
         */
        virtual void endTrainTimer( CommonTypes::TrainIdType trainId, 
                                    CommonTypes::ETrainResource resource ) = 0;

    };

}

#endif // !defined(ITrainTransactionManager_H)
