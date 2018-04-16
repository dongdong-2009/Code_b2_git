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


#include "bus/trains/TrainTransactionModel/src/TrainStatusManager.h"
#include "bus/trains/TrainTransactionModel/src/StateChangeWorkItem.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    TrainStatusManager::TrainStatusManager()
            : m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() )
    {
        FUNCTION_ENTRY( "TrainStateLock" );
        FUNCTION_EXIT;
    }


    TrainStatusManager::~TrainStatusManager()
    {
        FUNCTION_ENTRY( "TrainStateLock" );

        m_trainStateMap.clear();
        
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        FUNCTION_EXIT;
    }


    void TrainStatusManager::addTrain( CoreTrainStatePtr train )
    {
        FUNCTION_ENTRY( "addTrain" );

        TA_THREADGUARD( m_mapLock );
        
        TrainIdToResourceStateMap::iterator findIter = m_trainStateMap.find( train->getTrainId() );

        if ( findIter != m_trainStateMap.end() )
        {
            // Found trainID exist in map
            // Throw Exception
            TA_THROW( InvalidTrainException( "Existing Train Cannot Be Added.", train->getTrainId() ) );
        }
        else
        {
            // not found, must initialise state and add it to the map
            
            train->setTrainStatusManager( *this );

            // this is the internal map to be held in m_trainStateMap
            // it must be initialised with one entry as CoreResource->CoreTrainStatePtr
            TrainResourceToStateMap newTrainMap;
            newTrainMap.insert( TrainResourceToStateMap::value_type( train->getResourceCategory(), train ) );

            // now the new map must go into the main map
            m_trainStateMap.insert( TrainIdToResourceStateMap::value_type( train->getTrainId(), newTrainMap ) );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Train %d added into m_trainStateMap",
                     static_cast< unsigned int >( train->getTrainId() ) );

        FUNCTION_EXIT;
    }


    void TrainStatusManager::addTrainState( TrainStatePtr trainState )
    {
        FUNCTION_ENTRY( "addTrainState" );

        // 1. Checks the train exists, an exception is thrown if it doesnt
        // 2. Checks that the train does not already have a status object of the given
        //    resource category, exception if it does
        // 3. adds a newed copy of the object into the map

        TA_THREADGUARD( m_mapLock );

        TrainIdToResourceStateMap::iterator findIter = m_trainStateMap.find( trainState->getTrainId() );

        if ( findIter == m_trainStateMap.end() )
        {
            // Not Found trainID in map
            TA_THROW( InvalidTrainException( "Train ID Not Found in TrainStateMap.", trainState->getTrainId() ) );
        }
        else
        {
            // Adds a pointer to a newed copy of the object into m_trainStateMap
            TrainResourceToStateMap::iterator resourceFindIter = ( findIter->second ).find( trainState->getResourceCategory() );

            if ( resourceFindIter != findIter->second.end() )
            {
                // Found Existing Resource
                TA_THROW( InvalidTrainException( "Existing ResourceType Found with this TrainID in the TrainStatusMap.", trainState->getTrainId() ) );
            }
            else
            {
                trainState->setTrainStatusManager( *this );
                ( findIter->second )[ trainState->getResourceCategory() ] = trainState;
            }
        }

        FUNCTION_EXIT;
    }


    TrainStatePtr TrainStatusManager::getTrainState( CommonTypes::TrainIdType trainId, CommonTypes::ETrainResource resourceCategory ) const
    {
        FUNCTION_ENTRY( "getTrainState" );
        
        // Given a train ID and state, look up the object in the map.
        // If it doesnt exist, throw an exception.

        TA_THREADGUARD( m_mapLock );
        
        TrainIdToResourceStateMap::const_iterator findIter = m_trainStateMap.find( trainId );

        // train ID not found
        if ( findIter == m_trainStateMap.end() )
        {
            TA_THROW( InvalidTrainException( "Train could not be found", trainId ) );
        }

        // now find the correct resource within the sub-map
        TrainResourceToStateMap::const_iterator statusFindIter = ( findIter->second ).find( resourceCategory );

        if ( statusFindIter == ( findIter->second ).end() )
        {
            TA_THROW( InvalidTrainException( "Train resource could not be found", trainId ) );
        }

        FUNCTION_EXIT;
        return statusFindIter->second;
    }


    CoreTrainStatePtr TrainStatusManager::getCoreTrainState( CommonTypes::TrainIdType trainId ) const
    {
        FUNCTION_ENTRY( "getCoreTrainState" );

        // Given a train ID and state, look up the object in the map.
        // If it doesnt exist, throw an exception.

        TrainStatePtr trainState = getTrainState( trainId, CommonTypes::CoreResource );

        CoreTrainStatePtr coreTrainState( boost::dynamic_pointer_cast< CoreTrainStatePtr::element_type >( trainState ) );

        TA_ASSERT( NULL != coreTrainState.get(), "Cast to CoreTrainStatePtr failed" );

        FUNCTION_EXIT;
        return coreTrainState;
    }


    TrainIdToStateMap TrainStatusManager::getAllTrainStates( CommonTypes::ETrainResource resource ) const
    {
        FUNCTION_ENTRY( "getAllTrainStates" );

        TA_THREADGUARD( m_mapLock );

        TrainIdToStateMap result;
        
        for ( TrainIdToResourceStateMap::const_iterator iter = m_trainStateMap.begin();
              iter != m_trainStateMap.end(); ++iter )
        {
            TrainResourceToStateMap::const_iterator findIter = iter->second.find( resource );

            if ( findIter != iter->second.end() )
            {
                result[ iter->first ] = findIter->second;
            }
        }

        FUNCTION_EXIT;
        return result;
    }


    void TrainStatusManager::registerForStateChanges( CommonTypes::ETrainResource resourceType, ITrainStateUpdateListener& listener )
    {
        FUNCTION_ENTRY( "TrainStateLock" );

        // find the listeners who's key is resourceType
        TrainStateUpdateListenerMap::iterator resourceIter = m_trainStatusListenerMap.find( resourceType );

        if ( resourceIter != m_trainStatusListenerMap.end() )
        {
            // check if the listener already exists
            TrainStateUpdateListenerSet::iterator listenerIter = resourceIter->second.find( &listener );
            if ( listenerIter != resourceIter->second.end() )
            {
                // Existing, Listener Can't Be Added twice.
                FUNCTION_EXIT;
                return;
            }

            resourceIter->second.insert( &listener );
        }
        else
        {
            // no observers, add the set
            TrainStateUpdateListenerSet newListeners;
            newListeners.insert( &listener );

            m_trainStatusListenerMap[ resourceType ] = newListeners;
        }

        FUNCTION_EXIT;
    }


    void TrainStatusManager::deregisterForStateChanges( CommonTypes::ETrainResource resourceType, ITrainStateUpdateListener& listener )
    {
        FUNCTION_ENTRY( "deregisterForStateChanges" );

        // find the listeners who's key is resourceType
        TrainStateUpdateListenerMap::iterator resourceIter = m_trainStatusListenerMap.find( resourceType );

        if ( resourceIter == m_trainStatusListenerMap.end() )
        {
            // no observers for this resource
            FUNCTION_EXIT;
            return;
        }


        // check if the listener exists
        TrainStateUpdateListenerSet::iterator listenerIter = resourceIter->second.find( &listener );
        if ( listenerIter == resourceIter->second.end() )
        {
            // No matching listener found
            FUNCTION_EXIT;
            return;
        }

        // remove the listener from the set
        resourceIter->second.erase( listenerIter );

        FUNCTION_EXIT;
    }


    void TrainStatusManager::notifyStateChanged( CommonTypes::TrainIdType trainId, CommonTypes::ETrainResource resource ) const
    {
        FUNCTION_ENTRY( "notifyStateChanged" );

        try
        {
            // find the state object
            TrainStatePtr trainState = getTrainState( trainId, resource );

            // find the listeners who's key is resourceType
            TrainStateUpdateListenerMap::const_iterator resourceIter = m_trainStatusListenerMap.find( resource );

            if ( resourceIter == m_trainStatusListenerMap.end() )
            {
                // no observers for this resource
                FUNCTION_EXIT;
                return;
            }


            // for each listener
            for ( TrainStateUpdateListenerSet::const_iterator listenerIter = resourceIter->second.begin();
                  listenerIter != resourceIter->second.end(); ++listenerIter )
            {
                // notify of state change using a work item in the thread pool
                TA_Base_Core::IWorkItemPtr workItem( new StateChangeWorkItem( *(*listenerIter), trainState ) );
                m_threadPool.queueWorkItem( workItem );
            }

        }
        catch ( InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "InvalidTrainException", e.what() );

            TA_ASSERT( false, "Train State Lock has notified of a state change to a non existent state" );
        }

        FUNCTION_EXIT;
    }
}
