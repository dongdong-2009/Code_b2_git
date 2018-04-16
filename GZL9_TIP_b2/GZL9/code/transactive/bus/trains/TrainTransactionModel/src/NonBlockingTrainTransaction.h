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

#if !defined(NonBlockingTrainTransaction_H)
#define NonBlockingTrainTransaction_H


#include "bus/trains/TrainTransactionModel/src/TrainTransaction.h"
#include "core/threads/src/IWorkItem.h"


namespace TA_IRS_Bus
{

    /**
     * A train transaction that runs in its own thread. It does not block, and must notify when finished (or failed).
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:43 PM
     */

	class NonBlockingTrainTransaction : public TrainTransaction,
                                        public TA_Base_Core::IWorkItem
    {

    public:

        /**
         * Constructor, calls the parent class with the given parameters.
         * 
         * @param command    The command to execute
         * @param commandSender    The object used to send the command.
         * @param statusManager    the object used to retrieve train state
         * @param timeout    How long to wait for the response
         * @param retries    How many times to re-attempt the command
         * @param ignoreValidity    If true, this command will be sent to the train even if it is considered invalid.
         * @param radioId    optionally specify the radio id
         */
        NonBlockingTrainTransaction( TrainCommandPtr command,
                                     ICommandSender& commandSender,
                                     TrainStatusManager& statusManager,
                                     unsigned long timeout,
                                     int retries,
                                     bool ignoreValidity = false,
                                     const std::string& radioId = "" );


        /**
         * Destructor
         */
        virtual ~NonBlockingTrainTransaction();


        /**
         * Executes the work item. Called by the thread pool.
         * no exceptions are thrown
         */
        virtual void executeWorkItem();


        /**
         * Gets the error status when the transaction is complete
         */
        ETrainError getStatus() const;


    protected:

        /**
         * This is implemented by specialised classes to inform observers
         * of transaction completion.
         * It will be called at the very end of the command execution.
         */
        virtual void notifyObserversOfCompletion() = 0;


    private:

        /**
         * Private default constructor
         */
        NonBlockingTrainTransaction();

        /**
         * This contains the error status of the transaction when it has finished
         */
        ETrainError m_errorStatus;

    };


    typedef boost::shared_ptr< NonBlockingTrainTransaction > NonBlockingTrainTransactionPtr;

}

#endif // !defined(NonBlockingTrainTransaction_H)
