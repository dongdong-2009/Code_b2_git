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
#if !defined(TrainPaState_H)
#define TrainPaState_H

#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{

    /**
     * Public Address state of the train
     *
     * This is a local state and is only synchronised between peer agents.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:31 PM
     */
    class TrainPaState : public TA_IRS_Bus::TrainState
    {

    public:

        /**
         * Creates the object with a train ID
         *
         * @param trainId
         */
        TrainPaState( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Destructor
         */
        virtual ~TrainPaState();


        /**
         * Returns the train resource this state represents.
         *
         * @return the resource category this state represents.
         */
        virtual TA_IRS_Bus::CommonTypes::ETrainResource getResourceCategory() const;


        /**
         * This gets the lock used for sending PA commands.
         * This lock is acquired for the duration of a PA operation to prevent
         * multiple PA commands being sent to the same train at the same time.
         *
         * @return the lock to acquire while sending commands to this train
         */
        TA_Base_Core::IThreadLockable& getCommandLock();


        /**
         * Gets the PA live announcement ID
         *
         * @return the announcement ID of the ongoing announcement (0 if none)
         */
        TA_IRS_Bus::PaTypes::AnnouncementId getAnnouncementId() const;


        /**
         * Gets the PA live static talkgroup to use
         *
         * @return the group, or "" if no group to be used
         */
        std::string getAnnouncementStaticGroup() const;


        /**
         * Gets the operators session ID for the live announcement
         *
         * @return the operator performing the live announcement (empty if none)
         */
        const std::string& getSessionId() const;


        /**
         * Gets the console Id of an ongoing call
         *
         * @return the console id (or 0 if none in progress)
         */
        unsigned long getConsoleId() const;


        /**
         * Gets the radio call ID for the live announcement
         *
         * @return the radio call ID for the live announcement if it is ongoing and has been made
         */
        unsigned long getCallId() const;


        /**
         * Gets the the number of times the PA continue has been automatically sent for this live announcement.
         *
         * @return the number of auto acks sent
         */
        unsigned char getAutoContinueCount() const;


        /**
         * Checks whether an operator continue is required for the live announcement on this train.
         *
         * @return true if a continue is required.
         */
        bool isContinueRequired();


        /**
         * Sets the PA live announcement ID, and the operators session ID
         * Also resets the auto continue counter to 0.
         *
         * @param announcementId    The announcement ID for the live announcement
         * @param sessionId    The operator performing the live announcement
         * @param timestamp    The timestamp on the incoming TIMS message
         * @param staticGroup    The optional static group to use to make the announcement
         */
        void setAnnouncementDetails( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                     const std::string& sessionId,
                                     time_t timestamp,
                                     const std::string& staticGroup = "" );


        /**
         * The announcement has ended, clears the announcement id, session id, call id and continue count.
         *
         * @param timestamp    The timestamp , only specify if there was a preceding TIMs event to take the time from
         */
        void resetAnnouncementDetails( time_t timestamp = 0 );


        /**
         * Sets the call ID
         *
         * @param sessionId    The operator who made the call
         * @param consoleId    The console of the operator who made the call
         * @param callId    The call that was made
         * @param timestamp    The time the call was made
         */
        void setCallDetails( const std::string& sessionId,
                             unsigned long consoleId,
                             unsigned long callId,
                             time_t timestamp );


        /**
         * Clears the call ID
         */
        void clearCall();


        /**
         * Increments the auto continue ack count.
         */
        const void incrementAutoContinueCount();


        /**
         * Sets that a request for reset has been received when the continue count has been exceeded
         *
         * @param timestamp
         */
        void setContinueRequired( time_t timestamp );


        /**
         * Resets the auto continue count
         *
         * @param timestamp    The time the state was reset, only specify if there was a preceding TIMs event to take the time from
         */
        const void resetAutoContinueCount( time_t timestamp = 0 );


        /**
         * Gets the state for synchronisation purposes
         *
         * @return the state needing to be synchronised
         */
        TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState getSyncState() const;


        /**
         * Sets the state - for synchronisation
         *
         * @param state    The new internal state
         */
        void setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState& state );


    private:

        /**
         * Private default constructor
         */
        TrainPaState();


        /**
         * The session ID of the operator performing a live broadcast.
         */
        std::string m_sessionId;


        /**
         * The console ID so the call ID can be uniquely identified in the system
         */
        unsigned long m_consoleId;


        /**
         * The Radio call ID of the live announcement
         */
        unsigned long m_liveAnnouncementCallId;


        /**
         * The ID of the ongoing live announcement on the train.
         */
        TA_IRS_Bus::PaTypes::AnnouncementId m_liveAnnouncementId;


        /**
         * If a static talkgroup is to be used for the live announcement, it is stored here.
         */
        std::string m_liveAnnouncementStaticGroup;


        /**
         * This is the number of times a PA live announcement has been auto acknowledged.
         */
        unsigned char m_autoContinueCounter;


        /**
         * This is true if auto continues have all been sent, and the operator's continue is required.
         */
        bool m_continueRequired;


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
     * @created 01-Apr-2008 2:48:31 PM
     */
    typedef boost::shared_ptr< TrainPaState > TrainPaStatePtr;

} // TA_IRS_App

#endif // !defined(TrainPaState_H)
