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

#if !defined(TrainStatusManager_H)
#define TrainStatusManager_H


#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"
#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "bus/trains/TrainTransactionModel/src/CoreTrainState.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <set>
#include <map>


// forward declarations
namespace TA_Base_Core
{
    class ThreadPoolSingletonManager;
}


namespace TA_IRS_Bus
{
    class ITrainStateUpdateListener;


    /**
     * Maps train resource to states, for a given train ID
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:37 PM
     */
    typedef std::map< CommonTypes::ETrainResource, TrainStatePtr > TrainResourceToStateMap;


    /**
     * This maps a train ID, to the map of resources to states for that train
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:37 PM
     */
    typedef std::map< CommonTypes::TrainIdType, TrainResourceToStateMap > TrainIdToResourceStateMap;


    /**
     * Maps resources to train states - for all trains
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:37 PM
     */
    typedef std::map< CommonTypes::TrainIdType, TrainStatePtr > TrainIdToStateMap;
}


namespace TA_IRS_Bus
{

    /**
      * This is the status manager. It keeps track of the status of each train.
      * @author Adam Radics
      * @version 1.0
      * @created 22-Oct-2007 12:19:53 PM
      */

    class TrainStatusManager
    {

    public:


        /** 
          * Constructor
          */
        TrainStatusManager();


        /** 
          * Destructor
          */
        virtual ~TrainStatusManager();


        /**
         * Adds a new train into the system.
         *
         * @exception InvalidTrainException if the train already exists
         *
         * @param train    The must know details about the new train.
         */
        void addTrain( CoreTrainStatePtr train );


        /**
         * Adds a state for an already added train.
         * Sets this object as the train status manager in the TrainState object
         *
         * @param trainState
         */
        void addTrainState( TrainStatePtr trainState );


        /**
         * Gets the corresponding train state given a resource type and train id
         *
         * @exception InvalidTrainException if the train is not in the map (or the
         * requested resource is not in the map)
         *
         * @param trainId
         * @param resourceCategory
         */
        TrainStatePtr getTrainState( CommonTypes::TrainIdType trainId,
                                     CommonTypes::ETrainResource resourceCategory ) const;


        /**
         * Gets the corresponding core train state given a train id
         *
         * @exception InvalidTrainException if the train is not in the map
         *
         * @param trainId
         */
        CoreTrainStatePtr getCoreTrainState( CommonTypes::TrainIdType trainId ) const;


        /**
         * Gets all train states for a resource category - used to search all known states
         * for something.
         *
         * @param resource
         */
        TrainIdToStateMap getAllTrainStates( CommonTypes::ETrainResource resource ) const;


        /**
         * Register for updates to TrainState objects.
         *
         * @param resourceType
         * @param listener
         */
        void registerForStateChanges( CommonTypes::ETrainResource resourceType,
                                      ITrainStateUpdateListener& listener );


        /**
         * Cancel registration for state changes.
         *
         * @param resourceType
         * @param listener
         */
        void deregisterForStateChanges( CommonTypes::ETrainResource resourceType,
                                        ITrainStateUpdateListener& listener );


        /**
         * Tells the status manager that the train state has been updated.
         * This is only called by the TrainStateWriteLock object
         *
         * @param trainId
         * @param resource
         */
        void notifyStateChanged( CommonTypes::TrainIdType trainId,
                                 CommonTypes::ETrainResource resource ) const;

    private:

        /**
         * A map of listeners for train resource state changes
         */
        typedef std::set<ITrainStateUpdateListener*> TrainStateUpdateListenerSet;
        typedef std::map<CommonTypes::ETrainResource, TrainStateUpdateListenerSet> TrainStateUpdateListenerMap;


        /**
         * This is a map of train Ids to a map of train resources to associated states
         */
        TrainIdToResourceStateMap m_trainStateMap;


        /**
         * A map of listeners for train resource state changes
         */
        TrainStateUpdateListenerMap m_trainStatusListenerMap;


        /**
         * A lock for the train status map
         */
        mutable TA_Base_Core::NonReEntrantThreadLockable m_mapLock;


        /**
         * shared thread pool
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;

    };

}

#endif // !defined(TrainStatusManager_H)
