/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/RadioMessageProcessor.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/CallEndWorkItem.h"
#include "app/trains/train_agent/src/DataCallEndWorkItem.h"

#include "bus/trains/TrainTransactionModel/src/IResponseReceiver.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>

namespace TA_IRS_App
{

    RadioMessageProcessor::RadioMessageProcessor( TA_IRS_Bus::IResponseReceiver& responseReceiver,
                                                  ITrainStatusManager& statusManager )
            : m_responseReceiver( responseReceiver ),
              m_statusManager( statusManager ),
              m_threadPool( TA_Base_Core::ThreadPoolSingletonManager::getInstance() )
    {
        FUNCTION_ENTRY( "RadioMessageProcessor" );
        FUNCTION_EXIT;
    }



    RadioMessageProcessor::~RadioMessageProcessor()
    {
        FUNCTION_ENTRY( "~RadioMessageProcessor" );

        // clean up the thread pool
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( m_threadPool );
        m_threadPool = NULL;

        FUNCTION_EXIT;
    }


    void RadioMessageProcessor::processIncomingSds( const TA_IRS_Bus::TrainMessageData& data,
                                                    const std::string& senderTsi,
                                                    const std::string& receiverTsi )
    {
        FUNCTION_ENTRY( "processIncomingSds" );

        // pass the data directly onto m_responseReceiver
        m_responseReceiver.receiveData( data, senderTsi, receiverTsi );

        FUNCTION_EXIT;
    }


    void RadioMessageProcessor::processCallEndEvent( unsigned long consoleId,
                                                     unsigned long callId )
    {
        FUNCTION_ENTRY( "processCallEndEvent" );

        try
        {
            // Check to see if this is a call involving trains using the status manager
            // this will throw an exception if its not a train call
            TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails =
                m_statusManager.getDetailsForRadioCall( consoleId, callId );

            // if there are some details this is a train call

            // remove the call details from the status manager
            m_statusManager.removeRadioCall( consoleId, callId );

            // get the trains involved in this call
            TA_IRS_Bus::CommonTypes::TrainIdList trains =
                m_statusManager.getTrainsInvolvedInRadioCall( callDetails );

            // log the details, so when messages start going out to trains to clean up
            // we can see why and it doesnt look like a bug on our side
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                         "Radio call ended %s for trains [%s]",
                         callDetails.getLogString().c_str(),
                         TA_IRS_Bus::CommonTypes::trainIdListToString( trains ).c_str() );

            // Acquire the call end observer lock
            // for each observer, create a work item and queue it
            // there is no processing done here, so it is ok to acquire the lock for the entire operation
            // because the processing is simply queued in a thread pool
            TA_THREADGUARD( m_callEndObserverLock );

            CallEndObserverList& observerList = m_callEndObservers[ callDetails.resourceType ];

            for ( CallEndObserverList::iterator iter = observerList.begin();
                  observerList.end() != iter; ++iter )
            {
                TA_Base_Core::IWorkItemPtr newWorkItem( new CallEndWorkItem( *( *iter ), callDetails, trains ) );

                m_threadPool->queueWorkItem( newWorkItem );
            }
        }
        catch ( TA_IRS_Bus::TrainRadioException& )
        {
            // log at debug, because this will happen for all radio calls not involving trains
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "End of radio call [consoleId: %d][callId: %d] is not a call involving trains",
                         consoleId, callId );
        }

        FUNCTION_EXIT;
    }


    void RadioMessageProcessor::processDataCallEndEvent( const std::string& itsi,
                                                         bool success,
                                                         const std::string& details )
    {
        FUNCTION_ENTRY( "processDataCallEndEvent" );

        // Find the train ID for the TSI, if it is not a train, return
        TA_IRS_Bus::CommonTypes::TrainIdType trainId = m_statusManager.getTrainIdFromTsi( itsi );

        if ( 0 == trainId )
        {
            // this should be logged at warning level
            // because for this project, all circuit mode transfers are for trains
            // nothing else uses this functionality
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "End of circuit mode transfer [itsi: %s][success: %d][details: %s] is not for a train",
                         itsi.c_str(), success, details.c_str() );

            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,

                     "End of circuit mode transfer [trainId: %d][itsi: %s][success: %d][details: %s]",
                     trainId, itsi.c_str(), success, details.c_str() );


        // if it is a train, then for each observer, create and queue a DataCallEndWorkItem

        TA_THREADGUARD( m_dataCallEndObserverLock );

        for ( DataCallEndObserverList::iterator iter = m_dataCallEndObservers.begin();
              m_dataCallEndObservers.end() != iter; ++iter )
        {
            TA_Base_Core::IWorkItemPtr newWorkItem( new DataCallEndWorkItem( *( *iter ), trainId, success, details ) );

            m_threadPool->queueWorkItem( newWorkItem );
        }

        FUNCTION_EXIT;
    }


    void RadioMessageProcessor::addCallEndObserver( ICallEndObserver* observer,
                                                    TA_IRS_Bus::CommonTypes::ETrainResource resource )
    {
        FUNCTION_ENTRY( "addCallEndObserver" );

        // Acquire the lock
        TA_THREADGUARD( m_callEndObserverLock );

        // this will add the entry if it doesnt exist
        // note its a reference not a copy...
        CallEndObserverList& observerList = m_callEndObservers[ resource ];

        // If the observer doesnt already exist, add it to the list
        CallEndObserverList::iterator findIter = std::find( observerList.begin(),
                                                            observerList.end(),
                                                            observer );

        if ( observerList.end() == findIter )
        {
            observerList.push_back( observer );
        }

        FUNCTION_EXIT;
    }


    void RadioMessageProcessor::removeCallEndObserver( ICallEndObserver* observer,
                                                       TA_IRS_Bus::CommonTypes::ETrainResource resource )
    {
        FUNCTION_ENTRY( "removeCallEndObserver" );

        // Acquire the lock
        TA_THREADGUARD( m_callEndObserverLock );

        // this will add the entry if it doesnt exist
        // note its a reference not a copy...
        CallEndObserverList& observerList = m_callEndObservers[ resource ];

        // If the observer exists, remove it from the list
        CallEndObserverList::iterator findIter = std::find( observerList.begin(),
                                                            observerList.end(),
                                                            observer );

        if ( observerList.end() != findIter )
        {
            observerList.erase( findIter );
        }

        FUNCTION_EXIT;
    }


    void RadioMessageProcessor::addDataCallEndObserver( IDataCallEndObserver* observer )
    {
        FUNCTION_ENTRY( "addDataCallEndObserver" );

        // Acquire the lock
        TA_THREADGUARD( m_dataCallEndObserverLock );

        // If the observer doesnt already exist, add it to the list
        DataCallEndObserverList::iterator findIter = std::find( m_dataCallEndObservers.begin(),
                                                                m_dataCallEndObservers.end(),
                                                                observer );

        if ( m_dataCallEndObservers.end() == findIter )
        {
            m_dataCallEndObservers.push_back( observer );
        }

        FUNCTION_EXIT;
    }


    void RadioMessageProcessor::removeDataCallEndObserver( IDataCallEndObserver* observer )
    {
        FUNCTION_ENTRY( "removeDataCallEndObserver" );

        // Acquire the lock
        TA_THREADGUARD( m_dataCallEndObserverLock );

        // If the observer exists, remove it from the list
        DataCallEndObserverList::iterator findIter = std::find( m_dataCallEndObservers.begin(),
                                                                m_dataCallEndObservers.end(),
                                                                observer );

        if ( m_dataCallEndObservers.end() != findIter )
        {
            m_dataCallEndObservers.erase( findIter );
        }

        FUNCTION_EXIT;
    }

} // TA_IRS_App
