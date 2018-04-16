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
#if !defined(TrainState_H)
#define TrainState_H


#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <boost/shared_ptr.hpp>


// forward declarations
namespace TA_IRS_Bus
{
    class TrainStatusManager;
}


namespace TA_IRS_Bus
{
    /**
     * This represents the state of a train resource.
     *
     * It will define data and methods for updating and synchronising this data.
     *
     * TrainState objects are owned by the TrainStatusManager, a reference to a
     * TrainState object can be obtained from the status manager, and any reads/writes
     * on that object should be protected by a TrainStateLock.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:51 PM
     */

    class TrainState
    {

    public:

        /**
         * Creates the object with a train ID
         *
         * @param trainId
         */
        TrainState( CommonTypes::TrainIdType trainId );


        /**
         * virtual destructor for base class
         */
        virtual ~TrainState();


        /**
         * Returns the train resource this state represents.
         */
        virtual CommonTypes::ETrainResource getResourceCategory() const = 0;


        /**
         * Gets the train Id for this state object
         */
        CommonTypes::TrainIdType getTrainId() const;


        /**
         * Gets the last modification time of the train state
         *
         * @return the time of the last data change
         */
        time_t getTimestamp() const;


        /**
         * Gets the lock for this object, to be used in a threadlock.
         * This is only called by the TrainStateWriteLock object.
         */
        TA_Base_Core::IThreadLockable& getLock() const;


        /**
         * Used by the state lock to determine if changes were made.
         *
         * @return whether this state object was modified.
         */
        bool hasDataChanged() const;


        /**
         * Used to determine if a state update is required
         *
         * @return whether this state object was modified.
         */
        bool isSynchronisationRequired() const;


        /**
         * Returns the owning status manager.
         * setTrainStatusManager must have been called first or this will ASSERT
         */
        TrainStatusManager& getTrainStatusManager() const;


        /**
         * Gets the last modification time of the train state
         *
         * @param timestamp    Sets the modification time of the train state
         */
        void setTimestamp( time_t timestamp );


        /**
         * Clears the data changed flag.
         *
         * This is used by the state lock.
         */
        void resetDataChanged();


        /**
         * Clears the synchronisation required flag.
         *
         * This is used by anything sending synchronisation updates for this train
         */
        void setSynchronisationComplete();


        /**
         * Sets the owning object. This is called by TrainStatusManager::addTrainState()
         *
         * @param owner
         */
        void setTrainStatusManager( TrainStatusManager& owner );


    protected:

        /**
         * Sets the data changed flag.
         *
         * This is used internally by concrete state objects.
         *
         * @param synchronisationRequired    Optionally set whether this is synchronisable state or not
         */
        void setDataChanged( bool synchronisationRequired = true );



        /**
         * Sets the data changed flag, also sets the timestamp.
         *
         * This is used internally by concrete state objects.
         *
         * @param timestamp    timestamp to set, if 0 the current system time will be used
         * @param synchronisationRequired    Optionally set whether this is synchronisable state or not
         */
        void setDataChangedWithTimestamp( time_t timestamp = 0, bool synchronisationRequired = true );


    private:

        /**
         * The train this state is for
         */
        CommonTypes::TrainIdType m_trainId;


        /**
         * This is used for locking the state for reading/updating
         */
        mutable TA_Base_Core::ReEntrantThreadLockable m_lock;


        /**
         * The owning object
         */
        TrainStatusManager* m_trainStatusManager;


        /**
         * this represents whether there are changes to this state requiring
         * synchronisation/observer notification.
         * It is used by the train state lock to determine whether to notify observers.
         */
        bool m_dataChanged;


        /**
         * This flag is set for the sole purpose of differentiating actual changes with sync updates.
         * It will be used by anything that wants to sync train state to know if it is an update that requires synchronisation.
         */
        bool m_synchronisationRequired;


        /**
         * The timestamp of the last change
         */
        time_t m_timeStamp;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:51 PM
     */
    typedef  boost::shared_ptr< TrainState > TrainStatePtr;

}

#endif // !defined(TrainState_H)
