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

#include "bus/trains/TrainTransactionModel/src/NonBlockingTrainTransaction.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    NonBlockingTrainTransaction::NonBlockingTrainTransaction( TrainCommandPtr command,
                                                              ICommandSender& commandSender,
                                                              TrainStatusManager& statusManager,
                                                              unsigned long timeout,
                                                              int retries,
                                                              bool ignoreValidity,
                                                              const std::string& radioId )
            : TrainTransaction( command,
                                commandSender,
                                statusManager,
                                timeout,
                                retries,
                                ignoreValidity,
                                radioId )
    {
        FUNCTION_ENTRY( "NonBlockingTrainTransaction" );
        FUNCTION_EXIT;
    }


    NonBlockingTrainTransaction::~NonBlockingTrainTransaction()
    {
        FUNCTION_ENTRY( "~NonBlockingTrainTransaction" );
        FUNCTION_EXIT;
    }


    void NonBlockingTrainTransaction::executeWorkItem()
    {
        FUNCTION_ENTRY( "executeWorkItem" );

        // initialise error status
        m_errorStatus = NoError;

        try
        {
            execute();
        }
        catch ( const InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "InvalidTrainException", e.what() );

            m_errorStatus = InvalidTrain;
        }
        catch ( const TrainTimeoutException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainTimeoutException", e.what() );

            m_errorStatus = TrainTimeout;
        }
        catch ( const TrainTransmissionFailureException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TrainTransmissionFailureException", e.what() );

            m_errorStatus = TransmissionFailure;
        }
        catch ( const TransactionCancelledException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactionCancelledException", e.what() );

            m_errorStatus = TransactionCancelled;
        }
        catch ( const ProtocolException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "ProtocolException", e.what() );

            m_errorStatus = ProtocolError;
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "Performing execute()" );

            m_errorStatus = GeneralFailure;
        }

        notifyObserversOfCompletion();

        FUNCTION_EXIT;
    }


    ETrainError NonBlockingTrainTransaction::getStatus() const
    {
        FUNCTION_ENTRY( "getStatus" );
        FUNCTION_EXIT;
        return m_errorStatus;
    }
}
