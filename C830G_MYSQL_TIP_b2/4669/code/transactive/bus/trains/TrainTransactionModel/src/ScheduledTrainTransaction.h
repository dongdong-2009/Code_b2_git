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
#if !defined(ScheduledTrainTransaction_H)
#define ScheduledTrainTransaction_H


#include "bus/trains/TrainTransactionModel/src/NonBlockingTrainTransaction.h"


// forward declarations
namespace TA_IRS_Bus
{
    class IScheduledEventProcessor;
}


namespace TA_IRS_Bus
{
    /**
     * This is a scheduled command, the result is given to the appropriate event
     * processor.
     *
     * This does not block in the calling thread, it executes in its own thread.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:46 PM
     */

    class ScheduledTrainTransaction : public NonBlockingTrainTransaction
    {

    public:


        /**
         * Constructor, calls the parent class with the given parameters, but forces retries to 0.
         * 
         * @param command    The command to execute
         * @param commandSender    The object used to send the command.
         * @param statusManager    object used to retrieve train state
         * @param timeout    How long to wait for the response
         * @param callback    The object to call back on with the result
         * @param recurring    Whether this command has multiple executions
         * @param ignoreValidity    If true, this command will be sent to the train even if it is considered invalid.
         * @param radioId    Optionally specify the radio ID to send to
         */
        ScheduledTrainTransaction( TrainCommandPtr command,
                                   ICommandSender& commandSender,
                                   TrainStatusManager& statusManager,
                                   unsigned long timeout,
                                   IScheduledEventProcessor& callback,
                                   bool recurring,
                                   bool ignoreValidity = false,
                                   const std::string& radioId = "" );


        /**
         * Deletes the object.
         */
        virtual ~ScheduledTrainTransaction();


        /**
         * Sets the timer ID for this transactions timer
         * 
         * @param timerId    The timer ID for the scheduled timer
         */
        void setTimerId( long timerId );


        /**
         * Gets the timer ID to see if this timer is scheduled
         * 
         * @return the timer ID, or 0 if not scheduled
         */
        long getTimerId() const;


        /**
         * This will indicate to this object it has just been queued for execution
         */
        void setQueued();


        /**
         * This will check if this transaction is queued pending execution
         * 
         * @return true if this transaction is queued, false if not queued or executing
         */
        bool isQueued() const;


        /**
         * Checks if this command can be executed multiple times
         * 
         * @return true if this command is recurring
         */
        bool isRecurring() const;


        /**
         * Sets this command to non recurring in case it is recurring
         */
        void cancelRecurrence();


    protected:

        /**
         * This is called when the transaction is complete.
         * It notifies the observer of this scheduled transaction that the transaction is
         * complete and of its results.
         */
        virtual void notifyObserversOfCompletion();


    private:

        /**
         * Private default constructor
         */
        ScheduledTrainTransaction();


        /**
         * This is the callback for the command's results.
         */
        IScheduledEventProcessor& m_callback;


        /**
         * If true, this command must reset itself at the end of each execution to allow for multiple executions
         */
        bool m_recurring;


        /**
         * This is the id of the timer that is scheduled for this command
         */
        long m_timerId;


        /**
         * This is a flag to determine if the command is queued pending execution.
         * If this is true it means its timer has expired and it will be executed
         */
        bool m_queued;

    };


    typedef boost::shared_ptr< ScheduledTrainTransaction > ScheduledTrainTransactionPtr;

}

#endif // !defined(ScheduledTrainTransaction_H)
