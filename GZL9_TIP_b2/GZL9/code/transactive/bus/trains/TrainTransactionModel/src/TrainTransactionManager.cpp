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


#include "bus/trains/TrainTransactionModel/src/TrainTransactionManager.h"
#include "bus/trains/TrainTransactionModel/src/ICommandSender.h"
#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"
#include "bus/trains/TrainTransactionModel/src/ITrainEventFactory.h"
#include "bus/trains/TrainTransactionModel/src/TrainTimer.h"
#include "bus/trains/TrainTransactionModel/src/IScheduledEventProcessor.h"
#include "bus/trains/TrainTransactionModel/src/TrainEventWorkItem.h"
#include "bus/trains/TrainTransactionModel/src/ScheduledTrainTransaction.h"
#include "bus/trains/TrainTransactionModel/src/ParallelTrainTransaction.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"

#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/DebugUtil.h"

#include <iomanip>


namespace TA_IRS_Bus
{
    TrainTransactionManager::TrainTransactionManager( unsigned long defaultTimeout,
                                                      int defaultRetries,
                                                      ICommandSender& commandSender,
                                                      ITrainEventFactory& readerFactory,
                                                      TrainStatusManager& statusManager )
          : m_commandSender( commandSender ),
            m_eventFactory( readerFactory ),
            m_defaultTimeout( defaultTimeout ),
            m_defaultRetries( defaultRetries ),
            m_statusManager( statusManager ),
            m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() ),
            m_timerUtil ( &TA_Base_Core::SingletonTimerUtil::getInstance() )
    {
        FUNCTION_ENTRY( "TrainTransactionManager" );
        
        // start this queue processor
        start();

        FUNCTION_EXIT;
    }


    TrainTransactionManager::~TrainTransactionManager()
    {
        FUNCTION_ENTRY( "~TrainTransactionManager" );

        // Abort all active transactions.
        {
            TA_THREADGUARD( m_transactionLock );

            for ( TrainTransactionList::iterator iter = m_transactions.begin();
                  iter != m_transactions.end(); /* incremented in loop */ )
            {
                (*iter)->cancel();
                m_transactions.erase( iter++ );
            }
        }

        // cancel all scheduled transactions
        {
            TA_THREADGUARD( m_scheduledTransactionLock );

            // cancel all timers
            m_timerUtil->stopPeriodicTimeOutClock( this );

            for ( ScheduledTrainTransactionMap::iterator iter = m_scheduledTransactions.begin();
                  iter != m_scheduledTransactions.end(); /* incremented in loop */ )
            {
                iter->second->cancelRecurrence();
                iter->second->cancel();
                m_scheduledTransactions.erase( iter++ );
            }
        }

        // clear m_timerUtil (removeInstance)
        m_timerUtil->removeInstance();

        // remove instance of thread pool
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        // stops this queue processor
        terminateAndWait();

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::registerForTrainEvents( TrainMessageType eventType, ITrainEventProcessor* listener )
    {
        FUNCTION_ENTRY( "registerForTrainEvents" );

        // Simply adds an entry into m_eventProcessors

        TA_ASSERT( listener != NULL, "listener should not be a NULL!" );

        TA_THREADGUARD( m_eventProcessorLock );

        // find the existing map entry
        TrainEventProcessorMap::iterator eventTypeIter = m_eventProcessors.find( eventType );
        
        if ( eventTypeIter == m_eventProcessors.end() )
        {
            // no entries yet, create one
            m_eventProcessors[eventType].push_back( listener );
        }
        else
        {
            // exsiting list, check for duplicates
            TrainEventProcessorList::iterator listenerIter = std::find( eventTypeIter->second.begin(),
                                                                        eventTypeIter->second.end(),
                                                                        listener );

            // if not existing, add the listener
            if ( listenerIter == eventTypeIter->second.end() )
            {
                eventTypeIter->second.push_back( listener );
            }
        }

        FUNCTION_EXIT;

    }


    void TrainTransactionManager::deregisterForTrainEvents( TrainMessageType eventType, ITrainEventProcessor* listener )
    {
        FUNCTION_ENTRY( "deregisterForTrainEvents" );

        TA_ASSERT( listener != NULL, "listener should not be a NULL!" );

        TA_THREADGUARD( m_eventProcessorLock );

        // find existing message type
        TrainEventProcessorMap::iterator eventTypeIter = m_eventProcessors.find( eventType );

        if ( eventTypeIter != m_eventProcessors.end() )
        {
            // found the message type, now find the exact listener
            // note that it is unique, due to the way it is added in registerForTrainEvents
            TrainEventProcessorList::iterator listenerIter = std::find( eventTypeIter->second.begin(),
                                                                        eventTypeIter->second.end(),
                                                                        listener );

            if ( listenerIter != eventTypeIter->second.end() )
            {
                // matching listener found, remove it
                eventTypeIter->second.erase( listenerIter );
            }
        }

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::receiveData( const TrainMessageData& data,
                                               const std::string& radioId,
                                               const std::string& destinationRadioId )
    {
        FUNCTION_ENTRY( "receiveData" );

        // This creates an IncomingTrainData structure with the given data
        boost::shared_ptr<IncomingTrainData> pIncomingTrainData( new IncomingTrainData );
        pIncomingTrainData->data = data;
        pIncomingTrainData->destinationRadioId = destinationRadioId;
        pIncomingTrainData->radioId = radioId;

        // It puts it in a boost shared pointer, so it can then call insert()
        // to queue the data for processing in another thread
        processEvent( pIncomingTrainData );

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::sendCommand( TrainCommandPtr command,
                                               unsigned long timeout,
                                               int retries,
                                               bool ignoreValidity,
                                               const std::string& radioId )
    {
        FUNCTION_ENTRY( "sendCommand" );

        unsigned long applicableTimeout = timeout;
        int applicableRetries = retries;

        if ( timeout == 0 )
        {
            applicableTimeout = m_defaultTimeout;
        }

        if ( retries == -1 )
        {
            applicableRetries = m_defaultRetries;
        }

        // create the transaction
        TrainTransactionPtr transaction( new TrainTransaction( command,
                                                               m_commandSender,
                                                               m_statusManager,
                                                               applicableTimeout,
                                                               applicableRetries,
                                                               ignoreValidity,
                                                               radioId ) );
        // register it with the internal list
        addTransaction( transaction );

        // execute the transaction, blocking for the completion
        transaction->execute();

        // the transaction will be auto removed when complete
        // in the incoming data processing function

        FUNCTION_EXIT;
    }


    std::vector<ETrainError> TrainTransactionManager::sendCommands( const TrainCommandList& commands,
                                                                    unsigned long timeout,
                                                                    int retries,
                                                                    bool ignoreValidity )
    {
        FUNCTION_ENTRY( "sendCommands" );

        unsigned long applicableTimeout = timeout;
        int applicableRetries = retries;

        if ( timeout == 0 )
        {
            applicableTimeout = m_defaultTimeout;
        }

        if ( retries == -1 )
        {
            applicableRetries = m_defaultRetries;
        }


        std::list< ParallelTrainTransactionPtr > parallelTransactionList;
        TA_Base_Core::SingleThreadBarrier barrier( commands.size() );

        // create all transactions first without queuing, in case there is an error
        for ( TrainCommandList::const_iterator commandIter = commands.begin();
              commandIter != commands.end(); ++commandIter )
        {
            ParallelTrainTransactionPtr transaction( new ParallelTrainTransaction( commandIter->command,
                                                                                   m_commandSender,
                                                                                   m_statusManager,
                                                                                   applicableTimeout,
                                                                                   applicableRetries,
                                                                                   barrier,
                                                                                   ignoreValidity,
                                                                                   commandIter->radioId ) );

            // register it with the internal list
            addTransaction( transaction );

            // keep track of it in this function
            parallelTransactionList.push_back( transaction );
        }

        // all are created, they must be queued
        std::list< ParallelTrainTransactionPtr >::iterator transactionIter;
        
        for( transactionIter = parallelTransactionList.begin();
             transactionIter != parallelTransactionList.end(); ++transactionIter )
        {
            // push each transaction into the thread pool
            m_threadPool.queueWorkItem( *transactionIter );
        }

        // Wait for all transactions to complete their tasks
        barrier.wait(); 

        // collect all status
        std::vector<ETrainError> statusList;

        for( transactionIter = parallelTransactionList.begin();
             transactionIter != parallelTransactionList.end(); ++transactionIter )
        {
            statusList.push_back( ( *transactionIter )->getStatus() );
        }

        FUNCTION_EXIT;
        return statusList;
    }


    void TrainTransactionManager::cancelTransactions( const CommonTypes::TrainIdList& trainIdList,
                                                      const TrainMessageTypeList& commandList )
    {
        FUNCTION_ENTRY( "cancelTransactions" );

        // acquire the m_transactionLock
        TA_THREADGUARD( m_transactionLock );

        // for each transaction in m_transactions

        for ( TrainTransactionList::iterator transactionIter = m_transactions.begin();
              transactionIter != m_transactions.end(); ++transactionIter )
        {
            // by default match all transactions
            // and only reject match if criteria is given that does not match
            bool matches = true;

            // some train id's were specified
            if ( false == trainIdList.empty() )
            {
                CommonTypes::TrainIdList::const_iterator trainIdIter = trainIdList.find ( (*transactionIter)->getTrainId() );
                if ( trainIdList.end() == trainIdIter )
                {
                    // if its not in the set, then there is no match
                    matches = false;
                }
            }

            // if there is still a match
            // check if some command IDs are specified
            if ( ( true == matches ) &&
                 ( false == commandList.empty() ) )
            {
                TrainMessageTypeList::const_iterator commandIter = commandList.find( (*transactionIter)->getCommandType() );
                if ( commandList.end() == commandIter )
                {
                    // if its not in the set, then there is no match
                    matches = false;
                }
            }

            // if the transaction matches the criteria
            // cancel the transaction (if it is not complete)
            if ( ( true == matches ) &&
                 ( false == (*transactionIter)->isComplete() ) )
            {
                (*transactionIter)->cancel();

                // we can not remove the transaction from m_transactions
                // just in case this is a recurring scheduled command
            }
        }

        FUNCTION_EXIT;
    }


    long TrainTransactionManager::scheduleCommand( TrainCommandPtr command,
                                                   IScheduledEventProcessor& responseHandler,
                                                   unsigned long scheduleTime,
                                                   bool recurring,
                                                   unsigned long commandTimeout,
                                                   bool ignoreValidity,
                                                   const std::string& radioId )
    {
        FUNCTION_ENTRY( "scheduleCommand" );

        unsigned long applicableTimeout = commandTimeout;

        if ( commandTimeout == 0 )
        {
            applicableTimeout = m_defaultTimeout;
        }

        // create a new ScheduledTransaction
        ScheduledTrainTransactionPtr transaction( new ScheduledTrainTransaction( command,
                                                                                 m_commandSender,
                                                                                 m_statusManager,
                                                                                 applicableTimeout,
                                                                                 responseHandler,
                                                                                 recurring,
                                                                                 ignoreValidity,
                                                                                 radioId ) );

        
        TA_THREADGUARD( m_scheduledTransactionLock );

        // Schedules the command using SingletonTimerUtil.
        long scheduleId = m_timerUtil->startPeriodicTimeOutClock( this, scheduleTime, !recurring );

        // sets the timer in the transaction
        transaction->setTimerId( scheduleId );

        // add the transaction to the list of scheduled transactions
        // no need to check for duplicated, timer IDs are unique
        m_scheduledTransactions[ scheduleId ] = transaction;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Scheduled command added [Command=%d][TrainId=%d][Timeout=%d]"
                     "[Recurring=%d][IgnoreValidity=%d][radioId=%s][TimerId=%d]",
                     command->getCommandType(), command->getTrainId(), applicableTimeout,
                     recurring, ignoreValidity, radioId.c_str(), scheduleId );

        FUNCTION_EXIT;
        return scheduleId;
    }


    void TrainTransactionManager::removeScheduledCommand( long commandId )
    {
        FUNCTION_ENTRY( "removeScheduledCommand" );

        TA_THREADGUARD( m_scheduledTransactionLock );

        // first thing to do is to cancel the timer
        m_timerUtil->stopPeriodicTimeOutClock( this, commandId );

        // second, the command must be found and cancelled

        ScheduledTrainTransactionMap::iterator findIter = m_scheduledTransactions.find( commandId );

        if ( m_scheduledTransactions.end() != findIter )
        {
            // this timer exists
            // we do not cancel the command if it is in progress or queued
            // but we make sure it does not run again
            findIter->second->cancelRecurrence();

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Scheduled command removed [Command=%d][TrainId=%d][TimerId=%d]",
                     findIter->second->getCommandType(),
                     findIter->second->getTrainId(),
                     commandId );

            // remove this from the scheduled map
            m_scheduledTransactions.erase( findIter );
        }
        else
        {
            // this timer is not known
            // it is either already queued, in progress, or completed
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Scheduled command with Timer %d can not be found to cancel",
                         commandId );
        }

        // if there are any timers in m_transactions then these are already queued
        // or in progress, these cant be stopped using this method
        // They will not be recurring, because otherwise they would have existed in
        // m_scheduledTransactions and been found above and already set non-recurring
        // the only ScheduledTrainTransactions remaining in execution will be those that are
        // already executing and they will be cleaned up on completion like a regular transaction

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::startTrainTimer( CommonTypes::TrainIdType trainId,
                                                   CommonTypes::ETrainResource resource,
                                                   unsigned long time )
    {
        FUNCTION_ENTRY( "startTrainTimer" );
        
        TA_THREADGUARD( m_trainTimerMapLock );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "startTrainTimer(): trainId=%d resource=%d time=%d",
                     trainId, resource, time );

        TrainTimerMap::iterator trainIter = m_trainTimers.find( trainId );

        if ( trainIter != m_trainTimers.end() )
        {
            ResourceTimerMap::iterator resourceIter = trainIter->second.find( resource );

            if ( resourceIter != trainIter->second.end() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "startTrainTimer(): existing timer found for trainId=%d resource=%d time=%d",
                             trainId, resource, time );

                TA_THROW( InvalidTrainException( "Existing timer found for train", trainId ) );
            }
        }

        TrainTimer* timer = new TrainTimer( time, *m_timerUtil );

        m_trainTimers[trainId][resource] = timer;

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::waitTrainTimer( CommonTypes::TrainIdType trainId,
                                                  CommonTypes::ETrainResource resource )
    {
        FUNCTION_ENTRY( "waitTrainTimer" );

        // looks up a train timer in the map and calls wait
        // If the timer isnt found then throw an InvalidTrainException

        TrainTimer* timer = NULL;

        {
            TA_THREADGUARD( m_trainTimerMapLock );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "waitTrainTimer(): trainId=%d resource=%d",
                         trainId, resource );

            TrainTimerMap::iterator trainIter = m_trainTimers.find( trainId );

            if ( trainIter != m_trainTimers.end() )
            {
                ResourceTimerMap::iterator resourceIter = trainIter->second.find( resource );

                if ( resourceIter != trainIter->second.end() )
                {
                    timer = resourceIter->second;
                }
            }
        }

        if ( NULL == timer )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "waitTrainTimer(): existing timer not found for trainId=%d resource=%d time=%d",
                         trainId, resource, time );

            TA_THROW( InvalidTrainException( "Timer not started for train", trainId ) );
        }

        // wait on the timer
        timer->wait();

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::signalTrainTimer( CommonTypes::TrainIdType trainId,
                                                    CommonTypes::ETrainResource resource )
    {
        FUNCTION_ENTRY( "signalTrainTimer" );

        // looks up a train timer in the map and call signal
        // If the timer isnt found then throw an InvalidTrainException

        TrainTimer* timer = NULL;

        {
            TA_THREADGUARD( m_trainTimerMapLock );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "signalTrainTimer(): trainId=%d resource=%d",
                         trainId, resource );

            TrainTimerMap::iterator trainIter = m_trainTimers.find( trainId );

            if ( trainIter != m_trainTimers.end() )
            {
                ResourceTimerMap::iterator resourceIter = trainIter->second.find( resource );

                if ( resourceIter != trainIter->second.end() )
                {
                    timer = resourceIter->second;
                }
            }
        }

        if ( NULL == timer )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "signalTrainTimer(): existing timer not found for trainId=%d resource=%d time=%d",
                         trainId, resource, time );

            TA_THROW( InvalidTrainException( "Timer not started for train", trainId ) );
        }

        // signal the timer
        timer->signal();

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::endTrainTimer( CommonTypes::TrainIdType trainId,
                                                 CommonTypes::ETrainResource resource )
    {
        FUNCTION_ENTRY( "endTrainTimer" );


        TrainTimer* timer = NULL;

        {
            TA_THREADGUARD( m_trainTimerMapLock );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "endTrainTimer(): trainId=%d resource=%d",
                         trainId, resource );

            TrainTimerMap::iterator trainIter = m_trainTimers.find( trainId );

            if ( trainIter != m_trainTimers.end() )
            {
                ResourceTimerMap::iterator resourceIter = trainIter->second.find( resource );

                if ( resourceIter != trainIter->second.end() )
                {
                    // get the timer
                    timer = resourceIter->second;

                    // remove it from the map
                    trainIter->second.erase( resourceIter );
                }
            }
        }

        if ( NULL == timer )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "endTrainTimer(): existing timer not found for trainId=%d resource=%d time=%d",
                         trainId, resource, time );

            TA_THROW( InvalidTrainException( "Timer not found for train", trainId ) );
        }

        // delete the timer
        delete timer;
        timer = NULL;

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::timerExpired( long timerId, void* userData )
    {
        FUNCTION_ENTRY( "timerExpired" );

        // the timer ID, get the scheduled command

        TA_THREADGUARD( m_scheduledTransactionLock );

        ScheduledTrainTransactionMap::iterator findIter = m_scheduledTransactions.find( timerId );

        if ( m_scheduledTransactions.end() == findIter )
        {
            // this timer is not known, its an error
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Timer %d expired with no scheduled command", timerId );

            FUNCTION_EXIT;
            return;
        }

        // check if the transaction is already running
        // this is to prevent transactions 'stacking up' in case they
        // take too long to execute and dont finish by the next time

        if ( ( true == findIter->second->isInProgress() ) ||
             ( true == findIter->second->isQueued() ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Scheduled command skipped [Command=%d][TrainId=%d][TimerId=%d]",
                         findIter->second->getCommandType(),
                         findIter->second->getTrainId(),
                         timerId );

            FUNCTION_EXIT;
            return;
        }

        // the transaction must be queued
        findIter->second->setQueued();
        addTransaction( findIter->second );
        m_threadPool.queueWorkItem( findIter->second );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Scheduled command queued [Command=%d][TrainId=%d][TimerId=%d]",
                     findIter->second->getCommandType(),
                     findIter->second->getTrainId(),
                     timerId );
        
        // if this is not a recurring transaction, it can be removed from the map
        if ( false == findIter->second->isRecurring() )
        {
            m_scheduledTransactions.erase( findIter );
        }

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::processEvent( boost::shared_ptr<IncomingTrainData> newEvent )
    {
        FUNCTION_ENTRY( "processEvent" );

        // This will use the ITrainEventFactory to create the incoming event
        // If an event cannot be created an error will be logged and the event dropped.
        // Valid events will be given to handleIncomingData()

        try
        {
            TrainEventPtr event = m_eventFactory.createEvent( newEvent->data, newEvent->radioId, newEvent->destinationRadioId );

            // log the details of the received data
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "SDS message IN: [recv] %s", event->getLogString().c_str() );

            handleIncomingData( event );
        }
        catch ( ProtocolException& pe )
        {
            // Event will be dropped since we cannot create a reader for it
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Event reader cannot be created. [%s] [%s] [%s]",
                         pe.what(), pe.getFieldName().c_str(), pe.getFieldValue().c_str() );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Event: %s", getEventLogString( newEvent->data, newEvent->radioId, newEvent->destinationRadioId ).c_str() );
        }

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::addTransaction( TrainTransactionPtr transaction )
    {
        FUNCTION_ENTRY( "addTransaction" );

        TA_THREADGUARD( m_transactionLock );
        m_transactions.push_back( transaction );

        FUNCTION_EXIT;
    }


    void TrainTransactionManager::handleIncomingData( TrainEventPtr event )
    {
        FUNCTION_ENTRY( "handleIncomingData" );

        // First, all pending transactions will be checked to see if
        // the event is for one of them (by train ID and by event type):
        
        // transactions are given the response if they are in progress
        // no further checking is done here, train and message type matching is done within
        // the train command, so there is no need to double check here

        // also, any completed transactions are removed here

        {
            TA_THREADGUARD( m_transactionLock );

            bool eventConsumed = false;

            for( TrainTransactionList::iterator transactionIter = m_transactions.begin();
                 transactionIter != m_transactions.end(); /* incremented in loop */ )
            {
                // if the transaction is in progress, then it might want this event
                if ( true == (*transactionIter)->isInProgress() )
                {
                    eventConsumed = (*transactionIter)->receiveResponse( event );
                }

                // also check for completed transactions
                if ( true == (*transactionIter)->isComplete() )
                {
                    // completed transactions can be removed
                    m_transactions.erase( transactionIter++ );
                }
                else
                {
                    // otherwise just move onto the next one
                    ++transactionIter;
                }

                // if the event was consumed, processing stops here
                if ( true == eventConsumed )
                {
                    FUNCTION_EXIT;
                    return;
                }
            }
        }

        // Second, if the Event is not consumed by any pending transactions
        // it will be passed on to any processors registered for the given event type.
        // m_eventProcessors is checked for any registered observers of this event
        // the event is passed on to all interested observers.

        // also keep a flag whether any event processors handled this event
        bool isProcessed = false;

        {
            TA_THREADGUARD( m_eventProcessorLock );
            
            TrainEventProcessorMap::iterator eventTypeIter = m_eventProcessors.find( event->getEventType() );

            if ( eventTypeIter != m_eventProcessors.end() )
            {
                // for each registered listener
                for ( TrainEventProcessorList::iterator processorIter = eventTypeIter->second.begin();
                      processorIter != eventTypeIter->second.end(); ++processorIter )
                {
                    // handled by at least one processor
                    isProcessed = true;

                    // event processing should go in a new thread to prevent this one being blocked.
                    TA_Base_Core::IWorkItemPtr workItem( new TrainEventWorkItem( **processorIter, event ) );
                    m_threadPool.queueWorkItem( workItem );
                }
            }
        }


        // Lastly, if no observers are found then this is an orphaned event.
        // Log its details as a warning.
        if ( false == isProcessed )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "UNHANDLED: No one is interested in Event: %s",
                         event->getLogString().c_str() );
        }

        FUNCTION_EXIT;
    }


    std::string TrainTransactionManager::getEventLogString( const TrainMessageData& data,
                                                            const std::string& radioId,
                                                            const std::string& destinationRadioId ) const
    {
        FUNCTION_ENTRY( "getEventLogString" );

        // Logs the sending tsi, the destination tsi, and the raw data in hex format.
        std::stringstream dataStream;
        dataStream << "[TSI=" << radioId << "][Destination TSI=" << destinationRadioId << "][Data=";

        TrainMessageData::const_iterator iter = data.begin();

        dataStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( *iter );
        
        ++iter;

        for ( ; iter != data.end(); ++iter )
        {
            dataStream << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>( *iter );
        }

        dataStream << "]" << std::ends;

        FUNCTION_EXIT;
        return dataStream.str();
    }
}