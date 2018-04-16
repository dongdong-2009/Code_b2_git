/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Cameron Rochester
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/StructuredEventConsumer.h"
#include "core/message/src/StructuredEventConsumerDispatchers.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/TA_CosUtility.h"
#include <algorithm>


using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    #define RPARAM_WAIT_MESSAGE_TIME_MS "WaitMessageTimeMs"
    const unsigned long DEFAULT_WAIT_MESSAGE_TIME_MS = 100;


    // SECQueueHandler receives events from the queue (via IQueueHandler<T>), filters out
    // duplicate and out-of-order events (using SeqNumFilter) and dispatches them to the
    // appropriate client IEventHandlers (using the DomainNameDispatcher).
    class SECQueueHandler : public IQueueHandler<CosNotification::StructuredEvent>
    {
    public:
        SECQueueHandler();
        ~SECQueueHandler();

        void addSubscriber(IEventHandler* handler, SubscriptionFilter* filter);
        void removeSubscriber(SubscriptionFilter* filter);

        void matchConstraint( SubscriptionFilter* filter ) const;

        // Implementation of IQueueHandler<CosNotification::StructuredEvent>
        virtual bool onDispatchObjects(const std::vector<CosNotification::StructuredEvent*>& batch);

        SeqNumFilter& getSeqNumFilter() { return m_seqNumFilter; };

    private:
        // Filters out duplicate events (ie. events that were send redundantly)
        SeqNumFilter m_seqNumFilter;

        // Helper for dispatching events to subscribers based on the [domain_name, type_name]
        // of the event.
        DomainNameDispatcher m_dispatcher;

        // DomainNameDispatcher is NOT threadsafe.
        mutable TA_Base_Core::NonReEntrantThreadLockable m_lock;
    };


    SECQueueHandler::SECQueueHandler()
        : m_seqNumFilter(1)   // Leftmost occurrence of SEQ_NUM_FIELDNAME
    {
        // TODO LPP NOW: Uncomment once SeqNumFilter is finished.
        //m_seqNumFilter.start();
    }


    SECQueueHandler::~SECQueueHandler()
    {
        m_seqNumFilter.terminateAndWait();
    }


    void SECQueueHandler::addSubscriber(IEventHandler* handler, SubscriptionFilter* filter)
    {
        TA_THREADGUARD( m_lock );
        m_dispatcher.addSubscriber(handler, filter);
    }


    void SECQueueHandler::removeSubscriber(SubscriptionFilter* filter)
    {
        TA_THREADGUARD( m_lock );
        m_dispatcher.removeSubscriber(filter);
    }


    void SECQueueHandler::matchConstraint( SubscriptionFilter* filter ) const
    {
        TA_Assert(filter != NULL);

        TA_THREADGUARD( m_lock );
        SubscriptionFilter* matchingFilter = m_dispatcher.findMatchingFilter(filter);
        if ( matchingFilter != NULL )
        {
            filter->setID( matchingFilter->getID() );
        }
    }


    bool SECQueueHandler::onDispatchObjects(const std::vector<CosNotification::StructuredEvent*>& batch)
    {
        // The DomainNameDispatcher dispatches events one at a time.
        for ( unsigned int i = 0; i < batch.size(); ++i )
        {
            LOG3( SourceInfo, DebugUtil::DebugMsgPub,
                "SECQueueHandler::onDispatchObjects(): %s dispatching event %d: %s",
                gPtrToStr(this).c_str(), i, gStringFromEventHeader(*batch[i]).c_str() );

            TA_Assert(batch[i] != NULL);

            if ( SeqNumFilter::EVENT_DUPLICATE != m_seqNumFilter.isDuplicateEvent(*batch[i]) )
            {
                TA_THREADGUARD( m_lock );
                gLogSpecificMessage(*batch[i], "SECQueueHandler::onDispatchObjects");
                m_dispatcher.dispatchMessage(*batch[i]);
            }
        }
        
        return true;
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    SortableBatchingQueue::SortableBatchingQueue()
        : WAIT_MESSAGE_TIME_MS( TA_Base_Core::getRunParamValue( RPARAM_WAIT_MESSAGE_TIME_MS, DEFAULT_WAIT_MESSAGE_TIME_MS ) )
    {
    }


    void SortableBatchingQueue::sortMessageQueue( std::deque< CosNotification::StructuredEvent* >& messages )
    {
        if ( 1 == messages.size() )
        {
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SortableBatchingQueue::sortMessageQueue(): begin sorting, queueSize=%d", messages.size() );

        //                appender_id                          index in queue
        typedef std::map< std::string, std::pair< std::vector< unsigned long >, std::set< StructuredEventCompareBySequenceNumberWrap > > > SortingMap;
        SortingMap sorting_map;

        for ( size_t i = 0; i < messages.size(); ++i )
        {
            const char* appenderId;
            unsigned long seqNum;
            getAppenderIdAndSeqNum( *messages[ i ], appenderId, seqNum );

            StructuredEventCompareBySequenceNumberWrap t( messages[ i ], seqNum, i );

            sorting_map[ appenderId ].first.push_back( i );
            sorting_map[ appenderId ].second.insert( t );
        }

        for ( SortingMap::iterator mapIt = sorting_map.begin(); mapIt != sorting_map.end(); ++mapIt )
        {
            std::set< StructuredEventCompareBySequenceNumberWrap >::iterator setIt = mapIt->second.second.begin();
            for ( size_t i = 0; i < mapIt->second.first.size(); ++i, ++setIt )
            {
                messages[ mapIt->second.first[ i ] ] = setIt->m_msg;
            }
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SortableBatchingQueue::sortMessageQueue(): sorting finished, queueSize=%d", messages.size() );
    }


    bool SortableBatchingQueue::isOutOfOrder( const std::vector<CosNotification::StructuredEvent*>& events )
    {
        SeqNumFilter seqNumFilterCopy = dynamic_cast<TA_Base_Core::SECQueueHandler*>( m_handler )->getSeqNumFilter();

        for ( size_t i = 0; i < events.size(); ++i )
        {
            if ( SeqNumFilter::EVENT_OUT_OF_ORDER == seqNumFilterCopy.isDuplicateEvent( *events[ i ] ) )
            {
                return true;
            }
        }

        return false;
    }


    void SortableBatchingQueue::pushObjects(/*takes*/ const std::vector<CosNotification::StructuredEvent*>& objs)
    {
        TA_Assert(objs.size() > 0);

        TA_THREADGUARD( m_queueCacheLock );

        // MSVC6 limitation: The template method template<class InIt> void insert(iterator it, InIt first, InIt last)
        // is not available, so we have to use a slightly more convoluted notation.
        // According to the documentation, deque<T> iterators are invalidated when elements are inserted.
        m_queueCache.resize(m_queueCache.size() + objs.size());
        Queue_t::iterator iter = m_queueCache.end() - objs.size();
        std::copy(objs.begin(), objs.end(), iter);

        // The wait is signalled if the queue becomes large enough AND we are not currently
        // in the FAILED state.
        if ( !m_failed && m_queueCache.size() >= m_batchSize )
        {
            ThreadGuard guard(m_queueCondition.getLinkedThreadLockable());
            m_queueCondition.signal();
        }
    }


    unsigned long SortableBatchingQueue::getQueueFromCache()
    {
        TA_THREADGUARD( m_queueCacheLock );

        if ( false == m_queueCache.empty() )
        {
            if ( true == m_queue.empty() )
            {
                m_queue.swap( m_queueCache );
            }
            else
            {
                m_queue.resize(m_queue.size() + m_queueCache.size());
                Queue_t::iterator iter = m_queue.end() - m_queueCache.size();
                std::copy(m_queueCache.begin(), m_queueCache.end(), iter);
                m_queueCache.clear();
            }
        }

        return m_queue.size();
    }


    void SortableBatchingQueue::run()
    {
        if ( false == m_terminated )
        {
            m_running = true;
        }

        TA_Assert(m_batchSize != 0);

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SortableBatchingQueue::run(): %s begin to run", gPtrToStr(this).c_str() );

        //xxs add for test, dispose the exception
        try
        {
            while ( true )
            {
                // TODO LPP: Do we need to check m_running at various places (eg. before calling
                // dispatchBatchInOrder)?
                bool timerExpired = false;
                {
                    ThreadGuard guard(m_queueCondition.getLinkedThreadLockable());
                    if ( m_running )
                    {
                        // Note that timedWait() returns 1 if successfully signalled or 0 if time expired.
                        timerExpired = !m_queueCondition.timedWait(m_failed ? m_batchFailedIntervalMs : m_batchSucceededIntervalMs);
                    }
                }

                if ( !m_running )
                {
                    LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "SortableBatchingQueue::run(): %s will be signalled by termniate", gPtrToStr(this).c_str() );
                    // We were signalled by terminate()
                    break;
                }

                // If we are in the FAILED state, we should NOT have been signalled by pushObjects()
                if ( (true == m_failed) && (false == timerExpired) )
                {
                    LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                            "SortableBatchingQueue::run(): %s, ERROR - we are in the FAILED state, we should NOT have been signalled by pushObjects()",
                            gPtrToStr(this).c_str() );

                    continue;
                }

                // The timedWait() will unblock periodically even if the queue is empty.
                // If it unblocked because we signalled it (ie. the queue size is larger than the batch
                // size), we send  batches of events until there are fewer than m_batchSize objects in
                // the queue.
                // If it unblocked because the timer expired, at least one batch will be sent even if it
                // is smaller than the batch size.
                while ( ( 0 < m_queue.size() || 0 < getQueueFromCache() ) && ( m_batchSize <= m_queue.size() || m_batchSize <= getQueueFromCache() || timerExpired ) )
                {
                    // A partial batch will only be sent if the timer expired and there were less than
                    // m_batchSize items on the queue.
                    timerExpired = false;

                    EDispatchResult success = dispatchBatchInOrder();

                    if ( DISPATCH_SUCCESS == success )
                    {
                        m_failed = false;
                    }
                    else if ( DISPATCH_OUT_OF_ORDER == success )
                    {
                        m_failed = false;
                        break;
                    }
                    else
                    {
                        // The dispatch failed.  We will wait up to m_batchFailedIntervalMs, then try again.
                        m_failed = true;
                        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                            "SortableBatchingQueue::run(): %s, dispatchBatchInOrder failed", gPtrToStr(this).c_str() );
                        break;
                    }
                }
            }
        }
        //it seems could not happen
        catch(TransactiveException & e)
        {
            std::string msg( "SortableBatchingQueue::run -- " );
            msg.append( gPtrToStr(this) );
            msg.append( " has TransactiveException:");
            msg.append( e.what() );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
        }
        catch (std::exception& e)
        {
            std::string msg( "SortableBatchingQueue::run -- " );
            msg.append( gPtrToStr(this) );
            msg.append( " has std::exception:");
            msg.append( e.what() );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "SortableBatchingQueue::run -- " );
            msg.append( gPtrToStr(this) );
            msg.append( " has CORBA::exception:");
            msg.append( CorbaUtil::getInstance().exceptionToString(e) );
            LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
        }
        catch(...)
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "SortableBatchingQueue::run has exception: %s ", gPtrToStr(this).c_str() );
        }

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "SortableBatchingQueue::run(): %s stop running", gPtrToStr(this).c_str() );
    }


    SortableBatchingQueue::EDispatchResult SortableBatchingQueue::dispatchBatchInOrder()
    {
        // Copy up to m_batchSize items from the queue
        std::vector<CosNotification::StructuredEvent*> batch;
        batch.reserve(m_batchSize);

        int queueSize = 0;

        {
            TA_Assert(m_queue.size() > 0);
            TA_Assert(m_batchSize > 0);

            for ( Queue_t::iterator iter = m_queue.begin(); (iter != m_queue.end()) && (batch.size() < m_batchSize); ++iter )
            {
                batch.push_back(*iter);
            }

            // We access the queue size in the LOG statement below, so we need a snapshot while we have the lock.
            queueSize = m_queue.size() + m_queueCache.size();

            TA_Assert(batch.size() > 0);
        }

        // Dispatch to the handler
        LOG4(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "SortableBatchingQueue::dispatchBatchInOrder(): %s dispatching %d of %d objects on queue to handler %s",
            gPtrToStr(this).c_str(), batch.size(), queueSize, gPtrToStr(m_handler).c_str() );

        bool success = false;
        if ( m_handler != NULL )
        {
            TA_Assert(m_handler != NULL);

            // This try-catch block is an aid for debugging.  Without it, the application might
            // terminate without warning if exceptions are thrown (because the exception will
            // propagate out of the Threads run() method).
            try
            {
                if ( true == isOutOfOrder( batch ) )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "SortableBatchingQueue::dispatchBatchInOrder(): out of order detected, WAIT_MESSAGE_TIME_MS=%d, queueSize=%d",
                        WAIT_MESSAGE_TIME_MS, queueSize );

                    TA_Base_Core::Thread::sleep( WAIT_MESSAGE_TIME_MS );

                    getQueueFromCache();

                    sortMessageQueue( m_queue );

                    batch.clear();

                    for ( Queue_t::iterator iter = m_queue.begin(); (iter != m_queue.end()) && (batch.size() < m_batchSize); ++iter )
                    {
                        batch.push_back(*iter);
                    }
                }

                // Exceptions must not propagate out of IQueueHandler<T>::onDispatchObjects.
                success = m_handler->onDispatchObjects(batch);
            }
            catch (WinStructuredException& e)
            {
                std::ostringstream stm;
                stm << "runThread(): WinStructuredException exceptionCode == " << std::hex << e.m_exceptionCode;
                LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), stm.str().c_str() );

                DebugUtil::getInstance().logStackTrace(gContextFromExceptionInfo(e.m_exceptionInfo));

                // TA_Assert(0);
            }
            catch (std::exception& e)
            {
                std::string msg( "SortableBatchingQueue::dispatchBatchInOrder(): " );
                msg.append( e.what() );
                LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
                // TA_Assert(0);
            }
            catch (CORBA::Exception& e)
            {
                std::string msg( "SortableBatchingQueue::dispatchBatchInOrder(): " );
                msg.append( CorbaUtil::getInstance().exceptionToString(e) );
                LOG( SourceInfo, DebugUtil::ExceptionCatch, typeid(e).name(), msg.c_str() );
                // TA_Assert(0);
            }
            catch (...)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "SortableBatchingQueue::dispatchBatchInOrder(): Unknown exception" );
                // TA_Assert(0);
            }
        }
        else
        {
            // Events are discarded immediately if there is no handler
            success = true;
        }

        if ( success )
        {
            //TD17994,jianghp
            // Delete the dispatched objects
            for ( std::vector<CosNotification::StructuredEvent*>::iterator iter = batch.begin(); iter != batch.end(); ++iter )
            {
                delete *iter;
                *iter = NULL; //TD17994,jianghp
            }

            // Truncate the dispatched objects from the front of the queue
            m_queue.erase(m_queue.begin(), m_queue.begin() + batch.size());
        }
        else
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "SortableBatchingQueue::dispatchBatchInOrder: %s FAILED to dispatch objects as handler returned false",
                gPtrToStr(this).c_str() );
        }

        return static_cast<EDispatchResult>( success );
    }


    void SortableBatchingQueue::getAppenderIdAndSeqNum( const CosNotification::StructuredEvent& event, const char*& evtAppenderId, unsigned long& evtSeqNum )
    {
        // Locate the relevant seqNum (SEQ_NUM_FIELDNAME) and appenderId (EVENT_SUPPLIER_FIELDNAME) fields.
        // TODO LPP: Allow occurrences in other positions to be found.

        size_t header_length = event.header.variable_header.length();
        for ( size_t i = 0; i < header_length; ++i )
        {
            if ( 0 == strcmp( event.header.variable_header[i].name, SEQ_NUM_FIELDNAME ) )
            {
                event.header.variable_header[ i ].value >>= evtSeqNum;
                event.header.variable_header[ i + 1 ].value >>= evtAppenderId;
                return;
            }
        }
    }


/////////////////////////////////////////////////////////////////////////////////////////

    StructuredEventConsumer::StructuredEventConsumer(/*takes*/ IAdminProvider* adminProvider, bool handleOutOfOrder /* = false */ )
        : m_handler(new SECQueueHandler),
          m_adminProvider(adminProvider)
    {
        TA_Assert(m_adminProvider.get() != NULL);

        LOG1( SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventConsumer::StructuredEventConsumer(): Creating %s", gPtrToStr(this).c_str() );

        if ( true == handleOutOfOrder )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Using SortableBatchingQueue for StructuredEventConsumer" );
            m_queue.reset( new SortableBatchingQueue );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Using BatchingQueue for StructuredEventConsumer" );
            m_queue.reset( new BatchingQueue<CosNotification::StructuredEvent> );
        }

        m_queue->setBatchingProperties( getRunParamValue(RPARAM_BATCHSIZE, DEFAULT_BATCH_SIZE),
                                        getRunParamValue(RPARAM_SUCCESSBATCHINGINTERVAL, DEFAULT_SUCCESS_BATCH_INTERVAL),
                                        getRunParamValue(RPARAM_FAILEDBATCHINGINTERVAL, DEFAULT_FAILED_BATCH_INTERVAL) );

        m_queue->setQueueHandler(m_handler.get());
        m_queue->start();
    }


    StructuredEventConsumer::~StructuredEventConsumer()
    {
        LOG1( SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventConsumer::~StructuredEventConsumer(): Destroying %s", gPtrToStr(this).c_str() );

        m_queue->terminateAndWait();

        for ( ChannelConnections::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter )
        {
            ConsumerChannelConnection* connection = iter->second;

            // Check that all connections have been disconnected.  If any are still valid,
            // it is likely that disconnectAllEventChannels() has not been called.
            TA_Assert(connection != NULL);
            TA_Assert(!connection->m_channelConsumer.hasEventChannel());

            // TODO LPP: Should we call setHandler(NULL) on each CosNotifyChannelConsumer?

            delete connection;
            connection = NULL;
        }

        m_connections.clear();

        // It seems reasonable to require that subscribers must already have been detached by
        // now.  detachSubscriber() must be called for each subscriber before the SEC is
        // destroyed.  For example, the SubscriptionFilterSet will ensure that
        // SubscriptionManager::unsubscribeToMessages() is called for each subscriber.

        // TODO LPP: This assertion should be re-enabled.  Refer to TD11793 and StructuredEventConsumerDispatchers.cpp.
        //TA_Assert(!hasSubscribers());
    }


    bool StructuredEventConsumer::removeDeadConnection( const std::string& serviceAddr )
    {
        LOG2(SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventConsumer::removeDeadConnection(): %s disconnecting from channel on %s",
            gPtrToStr(this).c_str(), serviceAddr.c_str() );

        bool ret = false;

        {
            TA_THREADGUARD( m_lock );

            ChannelConnections::iterator it = m_connections.find( serviceAddr );
            TA_Assert(it != m_connections.end());

            // NOTE that we don't remove the invalid connection from the map.
            TA_Assert(it->second != NULL);
            it->second->m_constraintSet.setFilter(CosNotifyFilter::Filter::_nil());

            ret = it->second->m_channelConsumer.setEventChannel(CosNotifyChannelAdmin::ConsumerAdmin::_nil(), false);
        }

        LOG3(SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventConsumer::removeDeadConnection(): %s disconnected from channel on %s result: %d.",
            gPtrToStr(this).c_str(), serviceAddr.c_str(), ret);

        return ret;
    }


    bool StructuredEventConsumer::addConnection( const std::string& serviceAddr, const CosNotifyChannelAdmin::EventChannel_ptr channel )
    {
        TA_Assert(!CORBA::is_nil(channel));

        LOG2(SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventConsumer::addConnection(): %s connecting to channel on %s", gPtrToStr(this).c_str(), serviceAddr.c_str() );

        TA_THREADGUARD( m_lock );

        ChannelConnections::iterator it = m_connections.find( serviceAddr );
        if ( it == m_connections.end() )
        {
            // The connection has not been added to the map before.
            std::pair<ChannelConnections::iterator, bool> result =
                m_connections.insert( ChannelConnections::value_type(serviceAddr, new ConsumerChannelConnection()) );
            TA_Assert(result.second);

            TA_Assert(result.first->second != NULL);
            result.first->second->m_channelConsumer.setEventHandler(this);
            result.first->second->m_channelConsumer.setServiceAddr( serviceAddr.c_str() );
            it = result.first;
        }

        // Make sure the connection is not already valid (if it is, addConnection() has
        // been called without an intervening call to removeDeadConnection()).
        TA_Assert(it->second != NULL);
        TA_Assert(!it->second->m_channelConsumer.hasEventChannel());

        bool result = false;

        CosNotifyChannelAdmin::ConsumerAdmin_var consumerAdmin = m_adminProvider->getConsumerAdmin(channel);
        if ( !CORBA::is_nil(consumerAdmin) )
        {
            result = it->second->m_channelConsumer.setEventChannel(consumerAdmin);
            it->second->m_constraintSet.setFilter(it->second->m_channelConsumer.getFilter());

            if ( result && !addAllExpressionsToConstraintSet( it->second->m_constraintSet ) )
            {
                // We were unable to add the constraints (possibly the event channel is unreachable).
                it->second->m_constraintSet.setFilter(CosNotifyFilter::Filter::_nil());
                it->second->m_channelConsumer.setEventChannel(CosNotifyChannelAdmin::ConsumerAdmin::_nil());

                result = false;
            }
        }

        return result;
    }


    void StructuredEventConsumer::disconnectAllEventChannels()
    {
        LOG2( SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventConsumer::disconnectAllEventChannels(): %s disconnecting up to %d channels",
            gPtrToStr(this).c_str(), m_connections.size() );

        TA_THREADGUARD( m_lock );

        for ( ChannelConnections::iterator iter = m_connections.begin(); iter != m_connections.end(); ++iter )
        {
            TA_Assert(iter->second != NULL);
            iter->second->m_constraintSet.setFilter(CosNotifyFilter::Filter::_nil());
            iter->second->m_channelConsumer.setEventChannel(CosNotifyChannelAdmin::ConsumerAdmin::_nil(), true);
        }
    }    


    bool StructuredEventConsumer::onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
    {
        // TODO LPP: It would be helpful if we knew which connection the events came from.
        // Perhaps need to use a separate handler for each connection.

        LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
            "StructuredEventConsumer::onEventsReceived: %s received %d events to push onto queue",
            gPtrToStr(this).c_str(), events.size() );

        // We have to make a copy of the events because the BatchingQueue takes ownership
        // of them.
        std::vector<CosNotification::StructuredEvent*> eventsCopy;
        eventsCopy.reserve(events.size());

        for ( unsigned int i = 0; i < events.size(); ++i )
        {
            TA_Assert(events[i] != NULL);
            LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
                "CQUEUE  EVT %d: %s", i, gStringFromEventHeader(*events[i]).c_str() );

            gLogSpecificMessage(*events[i],"StructuredEventConsumer::onEventsReceived()" );
            eventsCopy.push_back(new CosNotification::StructuredEvent(*events[i]));
        }

        m_queue->pushObjects(eventsCopy);
        return true;
    }


    bool StructuredEventConsumer::attachSubscriberIfCompatible( IEventHandler* handler, SubscriptionFilter* filter )
    {
        TA_Assert(filter != NULL);

        TA_THREADGUARD( m_lock );

        if ( !matchConstraint(filter) )      // Accesses m_subscriptions
        {
            return false;
        }

        // Add the new constraint expression to each connection's constraint set
        for ( ChannelConnections::iterator it = m_connections.begin(); it != m_connections.end(); it++ )
        {
            // The constraint can only be added if the connection is valid (since it is added
            // to the corba filter object).
            if ( it->second->m_channelConsumer.hasEventChannel() )
            {
                try
                {
                    // EXCEPTION SAFETY: This can throw CORBA exceptions.
                    // clientId doesn't change since we are the only client
                    it->second->m_constraintSet.addExpression(0, filter->getID(), filter->getFilterCriteria());
                }
                catch (CORBA::Exception& e)
                {
                    std::string msg( "StructuredEventConsumer::attachSubscriberIfCompatible(): " );
                    msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );

                    // TODO LPP NOW: Need to consider what should happen if this doesn't work.  For example,
                    // should we invalidate m_channelConsumer?  This is probably INCORRECT as written.
                    // The subscriber won't have attached properly.
                }
            }
        }

        m_handler->addSubscriber( handler, filter );
        m_subscriptions.push_back( filter );

        LOG3(SourceInfo, DebugUtil::DebugDebug,
            "StructuredEventConsumer::attachSubscriberIfCompatible(): %s has attached subscriber "
            "IEventHandler=%s, SubscriptionFilter=%s",
            gPtrToStr(this).c_str(), gPtrToStr(handler).c_str(), gPtrToStr(filter).c_str() );

        return true;
    }


    bool StructuredEventConsumer::detachSubscriber( SubscriptionFilter* filter )
    {
        TA_Assert(filter != NULL);

        TA_THREADGUARD( m_lock );

        std::vector<SubscriptionFilter*>::iterator it = std::find( m_subscriptions.begin(), m_subscriptions.end(), filter );
        if ( it != m_subscriptions.end() )
        {
            m_subscriptions.erase( it );
            m_handler->removeSubscriber( filter );

            LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "StructuredEventConsumer::detachSubscriber(): %s has removed %s",
                gPtrToStr(this).c_str(), gPtrToStr(filter).c_str() );

            // Remove the constraint expression from all connections
            for ( ChannelConnections::iterator it = m_connections.begin(); it != m_connections.end(); ++it )
            {
                if ( it->second->m_channelConsumer.hasEventChannel() )
                {
                    try
                    {
                        // EXCEPTION SAFETY: This can throw CORBA exceptions.
                        // clientId doesn't change since we are the only client
                        it->second->m_constraintSet.removeExpression(0, filter->getID());
                    }
                    catch (CORBA::Exception& e)
                    {
                        std::string msg( "StructuredEventConsumer::detachSubscriber(): " );
                        msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
                        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", msg.c_str() );

                        // TODO LPP NOW: Need to consider what should happen if this doesn't work.  For example,
                        // should we invalidate m_channelConsumer?  This is probably INCORRECT as written.
                        // The subscriber won't have detached properly.
                    }
                }
            }

            return true;
        }

        return false;
    }


    bool StructuredEventConsumer::hasSubscribers() const
    {
        TA_THREADGUARD( m_lock );
        return ( !m_subscriptions.empty() );
    }


    bool StructuredEventConsumer::addAllExpressionsToConstraintSet( RefCountedConstraintSet& constraintSet )
    {
        try
        {
            std::vector<SubscriptionFilter*>::iterator it;
            for ( it = m_subscriptions.begin(); it != m_subscriptions.end(); ++it )
            {
                // clientId doesn't change since we are the only client
                constraintSet.addExpression(0, (*it)->getID(), (*it)->getFilterCriteria());
            }

            return true;
        }
        catch (CORBA::Exception& e)
        {
            std::string msg(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
            msg += ": Exception caught while calling RefCountedConstraintSet::addExpression()";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "CORBA", msg.c_str());

            return false;
        }
    }


    // This function was previously SubscriptionFilter::canShareSubscription.
    bool constraintExpressionsMatch( const CosNotifyFilter::ConstraintExp& left, const CosNotifyFilter::ConstraintExp& right )
    {
        return ( 0 == strcmp( left.constraint_expr.in(), right.constraint_expr.in() ) );
    }


    bool StructuredEventConsumer::matchConstraint( SubscriptionFilter* filter ) const
    {
        // TODO LPP: Work out exactly how the constraint matching algorithm works.

        TA_ASSERT( NULL != filter , "The filter to be matched was NULL");

        if ( 0 == m_subscriptions.size() )
        {
            // Any filter will match this consumer if no subscriptions have been added to the consumer.
            return true;
        }

        // If we can share with the first subscription, we can share with all of them!
        // TODO LPP: Why is the above a true statement?
        if ( ! constraintExpressionsMatch((*m_subscriptions.begin())->getFilterCriteria(), filter->getFilterCriteria()) )
        {
            // There is nothing we can share so we don't need to try any others
            return false;
        }

        m_handler->matchConstraint(filter);
        return true;
    }

}; // TA_Base_Core


