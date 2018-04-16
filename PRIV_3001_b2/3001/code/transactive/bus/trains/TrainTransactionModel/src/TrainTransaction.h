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
#if !defined(TrainTransaction_H)
#define TrainTransaction_H


#include "bus/trains/TrainTransactionModel/src/TrainCommand.h"
#include "bus/trains/TrainTransactionModel/src/TrainEvent.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/timers/src/TimedWaitSemaphore.h"


// forward declarations
namespace TA_IRS_Bus
{
    class ICommandSender;
    class TrainStatusManager;
}


namespace TA_IRS_Bus
{
    /**
     * This is a train transaction. It executes a command and waits for a response.
     *
     * It allows for a specified timeout and a number of retries.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:56 PM
     */

    class TrainTransaction
    {

    public:

        /**
         * Constructor
         * 
         * @param command    The command to execute
         * @param commandSender    The object used to send the command.
         * @param statusManager    object used to retrieve train state
         * @param timeout   How long to wait for the response
         * @param retries   How many times to re-attempt the command
         * @param ignoreValidity    If true, this command will be sent to the train even if it is considered invalid.
         * @param radioId    Optionally specify the radio ID to send to
         */
        TrainTransaction( TrainCommandPtr command,
                          ICommandSender& commandSender,
                          TrainStatusManager& statusManager,
                          unsigned long timeout,
                          int retries,
                          bool ignoreValidity = false,
                          const std::string& radioId = "" );


        /**
         * Destructor
         */
        virtual ~TrainTransaction();


        /**
         * Gets the train ID from the command
         */
        CommonTypes::TrainIdType getTrainId() const;


        /**
         * Gets the command ID from the command
         */
        TrainMessageType getCommandType() const;


        /**
         * Checks if the transaction is in progress.
         * 
         * @return true if this transaction is in progress, false otherwise
         */
        bool isInProgress() const;


        /**
         * Checks if this transaction has executed.
         * Note this is not related to isInProgress, because a command is not
         * complete before it is in progress.
         * 
         * @return true if this transaction is complete, false otherwise
         */
        bool isComplete() const;


        /**
         * Executes the command, it is synchronous and sends the command and waits for the response
         * 
         * @exception InvalidTrainException if the train the command is destined for is not valid or unknown.
         * @exception TrainTimeoutException if no response is received for the command after the specified timeouts and retries
         * @exception TrainTransmissionFailureException if the command cannot be sent over radio
         * @exception TransactionCancelledException if the command was cancelled
         */
        void execute();


        /**
         * This will cancel the transaction if it is in progress
         * (or if it hasnt started yet)
         */
        void cancel();


        /**
         * This response matches the expected response type and train ID.
         * It signals the end of the transaction (possibly).
         *
         * The command will give the final yes/no as to whether this response is for the
         * given command as the response may need parsing to determine whether it is the
         * right response.
         *
         * @return true if the response is for this transaction, false otherwise.
         *
         * @param response the potential response
         */
        bool receiveResponse( TrainEventPtr response );


    private:

        /**
         * Private default constructor
         */
        TrainTransaction();

        
        /**
         * Clears any previous responses, and resets the transaction so it is ready for re-execution
         */
        void resetTransaction();


        /**
         * Actually executes the command
         *
         * @exception InvalidTrainException if the train the command is destined for is not valid or unknown.
         * @exception TrainTimeoutException if no response is received for the command after the specified timeouts and retries
         * @exception TrainTransmissionFailureException if the command cannot be sent over radio
         * @exception TransactionCancelledException if the command was cancelled
         */
        void executeCommand();


    protected:

        /**
         * The command to execute
         */
        TrainCommandPtr m_command;
    
    
    private:

        /**
         * Used to actually send the command.
         */
        ICommandSender& m_sender;


        /**
         * Used to get train state
         */
        TrainStatusManager& m_statusManager;


        /**
         * This is used to wait for a response to a transaction. It is a timed semaphore so the wait can have a timeout parameter.
         */
        TA_Base_Core::TimedWaitSemaphore m_semaphore;


        /**
         * Command timeout in seconds
         */
        unsigned long m_timeout;


        /**
         * number of times to retry commands that timeout
         */
        int m_retries;


        /**
         * Whether to send the command even if the train is considered invalid
         */
        bool m_ignoreValidity;


        /**
         * The radio number to send the command to
         */
        std::string m_radioId;


        /**
         * true if this transaction was cancelled before execution completed
         */
        volatile bool m_cancelled;


        /**
         * true if this transaction is executing
         */
        volatile bool m_inProgress;


        /**
         * Signals whether the transaction is complete
         */
        bool m_complete;

        /**
         * This is used to protect internal state that can
         * be modified from an external thread
         */
        TA_Base_Core::NonReEntrantThreadLockable m_lock;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:56 PM
     */
    typedef boost::shared_ptr< TrainTransaction > TrainTransactionPtr;

}

#endif // !defined(TrainTransaction_H)
