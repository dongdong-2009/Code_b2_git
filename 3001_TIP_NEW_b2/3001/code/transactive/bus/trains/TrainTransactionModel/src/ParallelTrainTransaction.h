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
#if !defined(ParallelTrainTransaction_H)
#define ParallelTrainTransaction_H


#include "bus/trains/TrainTransactionModel/src/NonBlockingTrainTransaction.h"

namespace TA_Base_Core
{
    class SingleThreadBarrier;
}

namespace TA_IRS_Bus
{
    /**
     * A transaction that blocks on a barrier with other transactions executing in
     * parallel. This is so a number of transactions can be parallelised and all exit
     * at the same time, and the observer (who started the transactions) blocks until
     * ALL parallel transactions have completed.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:44 PM
     */

    class ParallelTrainTransaction : public NonBlockingTrainTransaction
    {

    public:


        /**
         * Constructor, calls the parent class with the given parameters.
         * 
         * @param command    The command to execute
         * @param commandSender    The object used to send the command.
         * @param statusManager    object used to retrieve train state
         * @param timeout    How long to wait for the response
         * @param retries    How many times to re-attempt the command
         * @param barrier    the barrier to wait on when the transaction has completed.
         * @param ignoreValidity    If true, this command will be sent to the train even if it is considered invalid.
         * @param radioId    optionally specify the radio id
         */
        ParallelTrainTransaction( TrainCommandPtr command,
                                  ICommandSender& commandSender,
                                  TrainStatusManager& statusManager,
                                  unsigned long timeout,
                                  int retries,
                                  TA_Base_Core::SingleThreadBarrier& barrier,
                                  bool ignoreValidity = false,
                                  const std::string& radioId = "" );


        /**
         * Deletes the transaction.
         * Ensures the operation has completed first.
         */
        virtual ~ParallelTrainTransaction();


    protected:

        /**
         * This simply waits on the barrier for all other transactions to finish. When all
         * threads are at this barrier the transaction will end.
         */
        virtual void notifyObserversOfCompletion();


    private:

        /**
         * Private default constructor
         */
        ParallelTrainTransaction();
        ParallelTrainTransaction( const ParallelTrainTransaction& );
        void operator=( const ParallelTrainTransaction& );


        /**
         * This is the barrier to wait on until all transactions have completed. This
         * essentially blocks the end of the run method until all threads are ready to
         * exit.
         */
        TA_Base_Core::SingleThreadBarrier& m_barrier;

    };


    typedef boost::shared_ptr< ParallelTrainTransaction > ParallelTrainTransactionPtr;

}

#endif // !defined(ParallelTrainTransaction_H)
