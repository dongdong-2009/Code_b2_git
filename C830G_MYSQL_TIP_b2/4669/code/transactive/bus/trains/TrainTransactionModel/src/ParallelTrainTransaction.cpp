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


#include "bus/trains/TrainTransactionModel/src/ParallelTrainTransaction.h"

#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    ParallelTrainTransaction::ParallelTrainTransaction( TrainCommandPtr command,
                                                        ICommandSender& commandSender,
                                                        TrainStatusManager& statusManager,
                                                        unsigned long timeout,
                                                        int retries,
                                                        TA_Base_Core::SingleThreadBarrier& barrier,
                                                        bool ignoreValidity,
                                                        const std::string& radioId )
            : NonBlockingTrainTransaction( command,
                                           commandSender,
                                           statusManager,
                                           timeout,
                                           retries,
                                           ignoreValidity,
                                           radioId ),
              m_barrier( barrier )
    {
        FUNCTION_ENTRY( "ParallelTrainTransaction" );
        FUNCTION_EXIT;
    }


    ParallelTrainTransaction::~ParallelTrainTransaction()
    {
        FUNCTION_ENTRY( "~ParallelTrainTransaction" );
        FUNCTION_EXIT;
    }


    void ParallelTrainTransaction::notifyObserversOfCompletion()
    {
        FUNCTION_ENTRY( "notifyObserversOfCompletion" );

        // unblock the thread waiting on transactions to complete
        // once each transaction signals the barrier it will exit
        m_barrier.post();

        FUNCTION_EXIT;
    }
}
