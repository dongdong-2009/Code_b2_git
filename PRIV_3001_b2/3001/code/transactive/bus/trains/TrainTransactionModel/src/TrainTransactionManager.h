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
#if !defined(TrainTransactionManager_H)
#define TrainTransactionManager_H


#include "bus/trains/TrainTransactionModel/src/ITrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/IResponseReceiver.h"
#include "bus/trains/TrainTransactionModel/src/TrainTransaction.h"
#include "bus/trains/TrainTransactionModel/src/ScheduledTrainTransaction.h"
#include "bus/trains/TrainTransactionModel/src/TrainCommand.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"
#include "core/timers/src/ITimeoutCallback.h"

#include <map>
#include <list>


// forward declarations
namespace TA_Base_Core
{
    class SingletonTimerUtil;
    class ThreadPoolSingletonManager;
}


namespace TA_IRS_Bus
{
    class ITrainEventProcessor;
    class ITrainEventFactory;
    class TrainTimer;
    class IScheduledEventProcessor;
    class ICommandSender;
}


namespace TA_IRS_Bus
{

    /**
     * Used as a simple container to queue incoming data
     * @author adamr
     * @version 1.0
     * @created 23-Oct-2007 4:04:01 PM
     */

    struct IncomingTrainData
    {

    public:

        /**
         * The incoming data
         */
        TrainMessageData data;

        /**
         * The senders radio id
         */
        std::string radioId;

        /**
         * The destination radio id
         */
        std::string destinationRadioId;
    };


    /**
     * This is the transaction manager. It starts transactions and manages timeouts,
     * retries, and locking.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:57 PM
     */

    class TrainTransactionManager : public ITrainTransactionManager,
                                    public IResponseReceiver,
                                    public TA_Base_Core::QueueProcessor< IncomingTrainData >,
                                    public TA_Base_Core::ITimeoutCallback
                                    
    {

    public:

        /**
         * Constructor.
         *
         * @param defaultTimeout    The default message timeout in seconds
         * @param defaultRetries    The number of times to retry commands that timeout.
         * @param commandSender    The object used to send commands via radio
         * @param readerFactory    This is a factory that creates train events from
         * incoming data
         * @param statusManager    The status manager, used to get train information for
         * executing commands.
         */
        TrainTransactionManager( unsigned long defaultTimeout,
                                 int defaultRetries,
                                 ICommandSender& commandSender,
                                 ITrainEventFactory& readerFactory,
                                 TrainStatusManager& statusManager );


        /**
         * Destructor.
         *
         * Needs to clean up timer, and all train timers. needs to clean up scheduled
         * commands.
         *
         * Needs to stop the garbage collection thread.
         */
        ~TrainTransactionManager();


        /**
         * This allows for objects to register for train events of a certain type. They
         * will get the event only if it is not consumed by an existing transaction, so
         * only unsolicited events are passed through.
         *
         * Note that multiple handlers can exist for a single event type.
         *
         * @param eventType
         * @param listener
         */
        void registerForTrainEvents( TrainMessageType eventType,
                                     ITrainEventProcessor* listener );


        /**
         * This allows for objects to deregister for train events of a certain type.
         *
         * @param eventType
         * @param listener
         */
        void deregisterForTrainEvents( TrainMessageType eventType,
                                       ITrainEventProcessor* listener );


        /**
         * Receives data via radio
         *
         * @param data    The data
         * @param radioId    The sender
         * @param destinationRadioId    the receiver
         */
        virtual void receiveData( const TrainMessageData& data,
                                  const std::string& radioId,
                                  const std::string& destinationRadioId );



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
                                  const std::string& radioId = "" );


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
                                                       bool ignoreValidity = false );


        /**
         * Cancels all transactions for the given train ID's, and command numbers.
         * If the given command numbers are empty, then this will cancel all transactions
         * for the given trains.
         * If the given train ID's is empty, then this will cancel transactions for the
         * given command numbers for all trains.
         *
         * @param trainIdList    The list of train IDs
         * @param commandList
         */
        void cancelTransactions( const CommonTypes::TrainIdList& trainIdList,
                                 const TrainMessageTypeList& commandList );


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
                                      const std::string& radioId = "" );


        /**
         * Removes a scheduled command from the scheduled commands.
         *
         * @param commandId
         */
        void removeScheduledCommand( long commandId );


        /**
         * Starts a timer for a train.
         *
         * @param trainId
         * @param resource    The timer category
         * @param time    The time in seconds before the timer expires
         */
        void startTrainTimer( CommonTypes::TrainIdType trainId,
                              CommonTypes::ETrainResource resource,
                              unsigned long time );


        /**
         * Waits on a timer previously started for a train.
         *
         * @exception TrainTimeoutException If the timer expires.
         *
         * @param trainId
         * @param resource
         */
        void waitTrainTimer( CommonTypes::TrainIdType trainId,
                             CommonTypes::ETrainResource resource );


        /**
         * Signals a train timer.
         *
         * @exception InvalidTrainException if the timer doesnt exist
         *
         * @param trainId
         * @param resource
         */
        void signalTrainTimer( CommonTypes::TrainIdType trainId,
                               CommonTypes::ETrainResource resource );


        /**
         * Ends a timer previously started for a train.
         *
         * @param trainId
         * @param resource
         */
        void endTrainTimer( CommonTypes::TrainIdType trainId,
                            CommonTypes::ETrainResource resource );


        /**
         * This handles a timeout from a scheduled command. The corresponding command must
         * be fired off in its own thread.
         *
         * @param timerId
         * @param userData
         */
        virtual void timerExpired( long timerId, void* userData );


    protected:

        /**
         * Processes the incoming train data - the next item in the queue.
         *
         * @param newEvent
         */
        virtual void processEvent( boost::shared_ptr<IncomingTrainData> newEvent );


    private:

        /**
         * Private default constructor
         */
        TrainTransactionManager();


        /**
         * Adds a train transaction to the list of transactions. A transaction must be
         * added to the list in order to receive its reply event.
         *
         * @param transaction
         */
        void addTransaction( TrainTransactionPtr transaction );


        /**
         * Given some incoming data this attempts to either:
         * - give it to a transaction expecting it, or
         * - pass it on to any registered observers
         *
         * @param event
         */
        void handleIncomingData( TrainEventPtr event );


        /**
         * From the raw incoming data, creates a loggable string.
         * This is used when an appropriate event cannot be created - eg due to a protocol
         * error.
         *
         * @return a string to log
         *
         * @param data    The raw incoming data
         * @param radioId    The radio id of the sender
         * @param destinationRadioId    The radio id of the receiver
         */
        std::string getEventLogString( const TrainMessageData& data, const std::string& radioId, const std::string& destinationRadioId ) const;

    private:

        /**
         * Private typedefs
         */
        typedef std::list< ITrainEventProcessor* > TrainEventProcessorList;
        typedef std::map< TrainMessageType, TrainEventProcessorList > TrainEventProcessorMap;

        
        typedef std::map< CommonTypes::ETrainResource, TrainTimer* > ResourceTimerMap;
        typedef std::map< CommonTypes::TrainIdType, ResourceTimerMap > TrainTimerMap;

        typedef std::list< TrainTransactionPtr > TrainTransactionList;
        
        typedef std::map< long, ScheduledTrainTransactionPtr > ScheduledTrainTransactionMap;

        /**
         * This is the interface used to send commands to trains.
         */
        ICommandSender& m_commandSender;


        /**
         * The interface used to create readers for events.
         */
        ITrainEventFactory& m_eventFactory;


        /**
         * The default command response timeout for transactions
         */
        unsigned long m_defaultTimeout;


        /**
         * The default number of times to retry failed transactions.
         */
        int m_defaultRetries;


        /**
         * A map of Train Event Types to Train Event Processor(s)
         */
        TrainEventProcessorMap m_eventProcessors;


        /**
         * Used to lock the m_envenProcessors map
         */
        TA_Base_Core::NonReEntrantThreadLockable m_eventProcessorLock;


        /**
         * This is the collection of all running train timers.
         * Train timers are added to the map in startTrainTimer and deleted from the map
         * in endTrainTimer. Each train can have only one timer running for each resource
         * at any one time.
         */
        TrainTimerMap m_trainTimers;


        /**
         * A lock to protect the train timer map
         */
        TA_Base_Core::NonReEntrantThreadLockable m_trainTimerMapLock;


        /**
         * The list of train transactions in progress (waiting for a response)
         */
        TrainTransactionList m_transactions;


        /**
         * Lock for the transaction list
         */
        TA_Base_Core::NonReEntrantThreadLockable m_transactionLock;


        /**
         * The list of train transactions in progress (waiting for a response)
         */
        ScheduledTrainTransactionMap m_scheduledTransactions;


        /**
         * Lock for the scheduled transaction list
         */
        TA_Base_Core::NonReEntrantThreadLockable m_scheduledTransactionLock;


        /**
         * The train status manager - it is used to get the TSI and validity for trains
         * (from the core state)
         */
        TrainStatusManager& m_statusManager;


        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        TA_Base_Core::SingletonTimerUtil* m_timerUtil;
    };

}

#endif // !defined(TrainTransactionManager_H)
