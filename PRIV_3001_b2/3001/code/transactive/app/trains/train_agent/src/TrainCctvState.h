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
#if !defined(TrainCctvState_H)
#define TrainCctvState_H

#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// better for using and returning fixed size arrays
#include <boost/array.hpp>

namespace TA_IRS_App
{

    /**
     * Stores the CCTV state for a train
     *
     * This is a global state and is synchronised across all train agents
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:26 PM
     */
    class TrainCctvState : public TA_IRS_Bus::TrainState
    {

    public:

        /**
         * Creates the object with a train ID
         *
         * @param trainId
         */
        TrainCctvState( TA_IRS_Bus::CommonTypes::TrainIdType trainId );
        

        /**
         * Destructor
         */
        virtual ~TrainCctvState();


        /**
         * Returns the train resource this state represents.
         *
         * @return the resource category this state represents.
         */
        virtual TA_IRS_Bus::CommonTypes::ETrainResource getResourceCategory() const;


        /**
         * This gets the lock used for sending CCTV commands.
         * This lock is acquired for the duration of a CCTV operation
         * to prevent multiple CCTV commands being sent to the same train at the same time.
         *
         * @return the lock to acquire while sending commands to this train
         */
        TA_Base_Core::IThreadLockable& getCommandLock();


        /**
         * Gets whether the train's carrier is on
         *
         * @return whether train cctv is active
         */
        bool isCctvActivated() const;


        /**
         * Gets the Train ID this train will conflict with
         *
         * @return the conflict train id if it exists
         */
        TA_IRS_Bus::CommonTypes::TrainIdType getPreConflictTrain() const;


        /**
         * Gets whether this train is in pre conflict
         *
         * @return whether pre conflict exists
         */
        bool isPreConflict() const;


        /**
         * If this train is on, what mode is it in (sequence, camera, quad)
         *
         * @return the mode
         */
        TA_IRS_Bus::CctvTypes::ECctvSwitchMode getCctvMode() const;


        /**
         * This returns the state of the CCTV, the fields mean different things depending on the return value of getCctvMode
         *
         * @return the mode-dependent state of the train (unsigned char[4])
         */
        boost::array<unsigned char, 4> getCctvState() const;


        /**
         * Gets the current CCTV state for this train, for sending to client applications.
         *
         * @return the current switch state of this train
         */
        TA_IRS_Bus::CctvTypes::CctvSwitchState getCctvSwitchState();


        /**
         * Sets the CCTV to active/inactive, and sets the default CCTV state for the train.
         * This is the result of a carrier on command.
         *
         * @param activated    whether the cctv is activated or not
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setActivated( bool activated,
                           time_t timestamp );


        /**
         * Sets the train this train is in pre conflict with
         *
         * @param conflictTrain    The train this train will soon conflict with
         */
        void setPreConflict( TA_IRS_Bus::CommonTypes::TrainIdType conflictTrain );


        /**
         * This will clear any pre-conflicts
         */
        void clearPreConflict();


        /**
         * Sets the CCTV to active, and the camera to the given camera number
         *
         * @param camera    Sets the CCTV to active, and the camera to the given camera number
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setSingleCamera( TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera,
                              time_t timestamp );


        /**
         * Sets the CCTV to active, and the sequence to the given sequence number
         *
         * @param sequence    The sequence number currently playing on the train
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setSequence( TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequence,
                          time_t timestamp );


        /**
         * Sets the CCTV to active, and the quad to the given configuration.
         *
         * @param cameraOne    Top left
         * @param cameraTwo    top right
         * @param cameraThree    bottom left
         * @param cameraFour    bottom right
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setQuad( TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraOne,
                      TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraTwo,
                      TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraThree,
                      TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraFour,
                      time_t timestamp );


        /**
         * Gets the state for synchronisation purposes
         *
         * @return the state needing to be synchronised
         */
        TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState getSyncState() const;


        /**
         * Sets the sync state for synchronisation purposes
         *
         * @param state    The new internal state
         */
        void setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState& state );


    private:
        
        /**
         * Private default constructor
         */
        TrainCctvState();


        /**
         * Resets the CCTV state to defaults.
         *
         * @param activated    true if the train carrier is on
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void setToDefaultCctvState( bool activated,
                                    time_t timestamp );


        /**
         * The current state of the cctv
         */
        TA_IRS_Bus::CctvTypes::ECctvSwitchMode m_cctvState;


        /**
         * The train ID this train is in pre conflict with
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_preConflictTrain;


        /**
         * This is the switch state
         */
        boost::array<unsigned char, 4> m_items;


        /**
         * This is the lock used for sending commands.
         * This lock is acquired for the duration of an operation to prevent
         * multiple commands of the same type being sent to the same train at the same time.
         */
        TA_Base_Core::NonReEntrantThreadLockable m_commandLock;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:26 PM
     */
    typedef boost::shared_ptr< TrainCctvState > TrainCctvStatePtr;

} // TA_IRS_App
#endif // !defined(TrainCctvState_H)
