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


#include "bus/trains/TrainTransactionModel/src/TrainTransaction.h"
#include "bus/trains/TrainTransactionModel/src/TrainStatusManager.h"
#include "bus/trains/TrainTransactionModel/src/ICommandSender.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <iomanip>


namespace TA_IRS_Bus
{

    TrainTransaction::TrainTransaction( TrainCommandPtr command,
                                        ICommandSender& commandSender,
                                        TrainStatusManager& statusManager,
                                        unsigned long timeout,
                                        int retries,
                                        bool ignoreValidity,
                                        const std::string& radioId )
          : m_command( command ),
            m_sender( commandSender ),
            m_statusManager( statusManager ),
            m_semaphore( 0 ),
            m_timeout( timeout ),
            m_retries( retries ),
            m_ignoreValidity( ignoreValidity ),
            m_radioId( radioId ),
            m_cancelled( false ),
            m_inProgress( false ),
            m_complete( false )
    {
        FUNCTION_ENTRY( "TrainTransaction" );
        FUNCTION_EXIT;
    }


    TrainTransaction::~TrainTransaction()
    {
        FUNCTION_ENTRY( "~TrainTransaction" );
        FUNCTION_EXIT;
    }


    CommonTypes::TrainIdType TrainTransaction::getTrainId() const
    {
        FUNCTION_ENTRY( "getTrainId" );
        FUNCTION_EXIT;
        return m_command->getTrainId();
    }


    TrainMessageType TrainTransaction::getCommandType() const
    {
        FUNCTION_ENTRY( "getCommandType" );
        FUNCTION_EXIT;
        return m_command->getCommandType();
    }



    bool TrainTransaction::isInProgress() const 
    {
        FUNCTION_ENTRY( "isInProgress" );
        FUNCTION_EXIT;
        // booleans are atomic, no lock required
        return m_inProgress;
    }


    bool TrainTransaction::isComplete() const 
    {
    	FUNCTION_ENTRY( "isComplete" );
        FUNCTION_EXIT;
        // booleans are atomic, no lock required
        return m_complete;
    }


    void TrainTransaction::execute()
    {
        FUNCTION_ENTRY( "execute" );

        // first the state of the command must be reset
        resetTransaction();

        try
        {

            // if the command has been cancelled,
            // then the command shouldnt be started
            if ( true == m_cancelled )
            {
                // throw the exception out, dont send the command
                TA_THROW( TransactionCancelledException( "Transaction cancelled" ) );
            }

            // then the actual command must be executed
            executeCommand();
        }
        catch ( ... )
        {
            // the command is not in progress
            m_inProgress = false;

            // reset the cancelled flag
            m_cancelled = false;

            // set the transaction to complete
            m_complete = true;

            // propagate the exception
            throw;
        }

        FUNCTION_EXIT;
    }


    void TrainTransaction::cancel()
    {
        FUNCTION_ENTRY( "cancel" );

        TA_THREADGUARD( m_lock );

        // sets m_cancelled to true
        m_cancelled = true;

        // signals m_semaphore
        m_semaphore.post();

        FUNCTION_EXIT;
    }


    bool TrainTransaction::receiveResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "receiveResponse" );

        // attempt to give the response to the command
        TrainCommand::TrainResponseResult result = m_command->setResponse( response );

        // if the command was accepted
        if ( true == result.transactionComplete )
        {
            // unblock the semaphore - this transaction is done
            m_semaphore.post();

            // return true if the command consumes the response,
            // false if the command does not consume the response
            FUNCTION_EXIT;
            return result.eventConsumed;
        }

        FUNCTION_EXIT;
        return false;
    }


    void TrainTransaction::resetTransaction()
    {
        FUNCTION_ENTRY( "receiveResponse" );

        // resets the response of the command back to nothing
        m_command->resetResponse();

        // the command is not in progress
        m_inProgress = false;

        // the command is not complete
        m_complete = false;
        
        // now cancelled is slightly different
        // it is possible to cancel a transaction before it is executed
        // so this flag cannot be reset here
        // if the flag is not set, then we must ensure the semaphore will block
        // when the command is sent

        TA_THREADGUARD( m_lock );
        if ( false == m_cancelled )
        {
            // make sure this semaphore has a count of 0
            while( m_semaphore.tryWait() ){}
        }

        FUNCTION_EXIT;
    }


    void TrainTransaction::executeCommand()
    {
        FUNCTION_ENTRY( "executeCommand" );


        //
        // Step 1.
        // Get minimal information about the train:
        // 1. Its radio ID to send the command to
        // 2. Whether it is a valid train or not
        //


        // if validity is being ignored, this sets the train to valid
        // otherwise the train's validity is initialised to false
        bool trainValid = m_ignoreValidity;
        
        // this will set the initial radio ID to the radio ID manually specified
        // if this is empty, then the current one must come from the state
        std::string radioId = m_radioId;

        // only get the train state if necessary
        if ( ( false == trainValid ) ||     // not ignoring validity
             ( true == radioId.empty() ) )  // no manual radio ID given
        {
            // get the train state from the status manager
            CoreTrainStatePtr coreTrainStatePtr = m_statusManager.getCoreTrainState( m_command->getTrainId() );

            // must lock the state to query
            TrainStateLock lock( coreTrainStatePtr );

            // if already considered valid, this will be quick (wont even check validity)
            // if not considered valid, it comes from the state
            trainValid = trainValid || coreTrainStatePtr->isValid();

            // if the radio ID is empty, then we must use the one from the state
            if ( true == radioId.empty() )
            {
                radioId = coreTrainStatePtr->getRadioId();
            }
        }

        // if the train is not valid then this command cant be sent
        if ( false == trainValid )
        {
            TA_THROW( InvalidTrainException( "Train is not valid", m_command->getTrainId() ) );
        }

        // If the radio ID is not set, throw an InvalidTrainException
        if ( true == radioId.empty() )
        {
            TA_THROW( InvalidTrainException( "Radio ID is not set", m_command->getTrainId() ) );
        }


        //
        // Step 2
        // Get the data to send to the train
        // Format log strings etc, so this isnt done on every retry
        //


        // command
        TrainMessageData cmdData = m_command->getMessageData();

        // loggable string
        std::string logString = m_command->getLogString();

        // raw data string
        std::string rawData;

        // only do this if the debug level is verbose enough
        if ( TA_Base_Core::DebugUtil::getInstance().getLevel() >=
             TA_Base_Core::DebugUtil::DebugDebug )
        {
            std::ostringstream strData;
            for ( TrainMessageData::iterator iter = cmdData.begin();
                  iter != cmdData.end(); ++iter )
            {
                strData << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( *iter ) << " ";
            }

            rawData = strData.str();
        }


        //
        // Step 3
        // Start the command/response loop
        // Send the command until either:
        // - a valid response is received
        // - the transaction is cancelled
        // - the transaction times out
        // Also retry the transaction on timeout or send failure
        //

        bool responseExpected = ( false == m_command->getResponseTypes().empty() );

        // initialise end conditions
        bool executed = false;
        
        // the command is now in progress
        m_inProgress = true;

        // keep going until something stops the loop
        for ( int retries = 0; 
              false == executed; // note that retries is not tested here, it is tested within the loop
              ++retries )
        {
            // Send Command via radio
            try
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "SDS message OUT: [SENDING] %s",
                             logString.c_str() );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "SDS message OUT: [SENDING DATA] %s",
                             rawData.c_str() );

                m_sender.sendData( radioId, cmdData );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "SDS message OUT: [SUCCESS] %s",
                             logString.c_str() );

            }
            catch ( TrainTransmissionFailureException& ) // dont override the reason
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "SDS message OUT: [FAILED] %s",
                             logString.c_str() );

                // this means the command can't be sent, there is really no use retrying.
                // However the customer likes to see the attempt even if it is futile
                if ( retries >= m_retries )
                {
                    // the max retries has been exceeded
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                 "Transaction failed after %d retries. [TrainId=%d][Command=%d]",
                                 retries, m_command->getTrainId(), m_command->getCommandType() );

                    // exit via exception
                    throw;
                }
                
                // try to re-send
                //continue; //limin--, TD20779
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "SDS message OUT: [FAILED] %s",
                             logString.c_str() );

                // Same deal as above, except someone has been naughty and thrown some crap exception
                if ( retries >= m_retries )
                {
                    // the max retries has been exceeded
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                 "Transaction failed after %d retries. [TrainId=%d][Command=%d]",
                                 retries, m_command->getTrainId(), m_command->getCommandType() );

                    // exit via exception
                    TA_THROW( TrainTransmissionFailureException( "Error sending command to train" ) );
                }

                // try to re-send
                //continue; //limin--, TD20779
            }

            // Check if there ahould be a response to this command
            if ( false == responseExpected )
            {
                // there is no response expected, this transaction is done
                // exit normally
                executed = true;
                break;
            }

            // If there is an expected response, wait on the timed semaphore
            // until it is signalled, or it times out waiting.
            m_semaphore.timedWait( m_timeout );		

            // Get the response from the command (to see if the command completed successfully)
            TrainEventPtr response = m_command->getResponse();

            // check if a response was received
            if ( response )
            {
                // This transaction is done
                
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "Received appropriate response. [TrainId=%d][Command=%d][Response=%d]",
                             m_command->getTrainId(), m_command->getCommandType(),
                             response->getEventType() );

                // exit normally
                executed = true;
                break;
            }
            else
            {
                // this means no response was received
                // the operation timed out, or was cancelled

                // first check for a cancel
                if ( true == m_cancelled )
                {
                    // This transaction is done
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "Transaction was cancelled. [TrainId=%d][Command=%d]",
                                 m_command->getTrainId(), m_command->getCommandType() );

                    // exit via exception
                    TA_THROW( TransactionCancelledException( "Transaction cancelled" ) )
                }

                // the last possible reason for failure is timeout
                // so check if the number of retries has been exceeded
                if ( retries >= m_retries )
                {
                    // the max retries has been exceeded
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "Transaction timed out after %d retries. [TrainId=%d][Command=%d]",
                                 retries, m_command->getTrainId(), m_command->getCommandType() );

                    // exit via exception
                    std::ostringstream reason;
                    reason << "No response after " << retries << " retries";
                    TA_THROW( TrainTimeoutException( reason.str().c_str() ) );
                }
            }
        }

        // the command is not in progress
        m_inProgress = false;

        // reset the cancelled flag
        m_cancelled = false;

        // set the transaction to complete
        m_complete = true;

        // ensure a response was received if one was expected
        if ( true == responseExpected )
        {
            TA_ASSERT( NULL != m_command->getResponse().get(), "There must be a valid response to exit a transaction normally" );
        }

        FUNCTION_EXIT;
    }
}
