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
#if !defined(TrainCallState_H)
#define TrainCallState_H

#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{

    /**
     * This holds the state for driver calls occ, and occ calls driver.
     *
     * This is a local state and is only synchronised between peer agents.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:24 PM
     */
    class TrainCallState : public TA_IRS_Bus::TrainState
    {

    public:

        /**
         * Creates the object with a train ID
         *
         * @param trainId
         */
        TrainCallState( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Destructor
         */
        virtual ~TrainCallState();


        /**
         * Returns the train resource this state represents.
         *
         * @return the resource category this state represents.
         */
        virtual TA_IRS_Bus::CommonTypes::ETrainResource getResourceCategory() const;


        /**
         * This gets the lock used for sending Call commands.
         * This lock is acquired for the duration of a Call operation
         * to prevent multiple Call commands being sent to the same train at the same time.
         *
         * @return the lock to acquire while sending commands to this train
         */
        TA_Base_Core::IThreadLockable& getCommandLock();


        /**
         * checks if there is an incoming call occ from the train
         *
         * @returns whether there is an incoming call
         */
        bool hasIncomingCall() const;


        /**
         * If there is an incoming call, get the time stamp, 0 otherwise.
         *
         * @returns the incoming call time
         */
        time_t getIncomingCallTime() const;


        /**
         * Gets the call type of an ongoing call
         *
         * @return the call type in progress
         */
        TA_IRS_Bus::CallTypes::ETrainCallType getCallType() const;


        /**
         * Gets the session Id of an operator making the call
         *
         * @return the session id (or "" if none in progress)
         */
        const std::string& getSessionId() const;


        /**
         * Gets the call Id of an ongoing call
         *
         * @return the call id (or 0 if none in progress)
         */
        unsigned long getCallId() const;


        /**
         * Gets the console Id of an ongoing call
         *
         * @return the console id (or 0 if none in progress)
         */
        unsigned long getConsoleId() const;


        /**
         * A call OCC request was received, this sets it.
         *
         * @param timestamp    The time on the incoming TIMS event
         */
        void setIncomingCall( time_t timestamp );


        /**
         * A call OCC request was answered or cancelled
         *
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        void clearIncomingCall( time_t timestamp );


        /**
         * An operator is making a call, store the details
         *
         * @param sessionId    The operator making/answering the call
         * @param callType    the type of call the operator is trying to make
         * @param timestamp    The timestamp on the incoming TIMS message, or if there was
         *                     no preceding TIMS message while making the call, the current time will be used
         * @param consoleId    Optional console ID if the call is made straight away
         * @param callId    Optional call ID if the call is made straight away
         */
        void setCallDetails( const std::string& sessionId,
                             TA_IRS_Bus::CallTypes::ETrainCallType callType,
                             time_t timestamp = 0,
                             unsigned long consoleId = 0,
                             unsigned long callId = 0 );


        /**
         * clears all details for any ongoing call.
         *
         * @param timestamp    The time the call was ended, if this was triggered by a TIMS event, use the TIMS event timestamp
         */
        void clearCall( time_t timestamp = 0 );


        /**
         * Gets the state for synchronisation purposes
         *
         * @return the state needing to be synchronised
         */
        TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState getSyncState() const;


        /**
         * Sets the state for synchronisation purposes
         *
         * @param state    The new internal state
         */
        void setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState& state );


    private:

        /**
         * Private default constructor
         */
        TrainCallState();


        /**
         * If there is an incoming call
         */
        bool m_incomingCall;


        /**
         * If there is an incoming call, the time it came in.
         */
        time_t m_incomingCallTime;


        /**
         * the session id of the operator calling this train, "" if none
         */
        std::string m_sessionId;


        /**
         * The console ID so the call ID can be uniquely identified in the system
         */
        unsigned long m_consoleId;


        /**
         * the call in progress (0 if none)
         */
        unsigned long m_callId;


        /**
         * If a call is in progress (established) and its type
         */
        TA_IRS_Bus::CallTypes::ETrainCallType m_callType;


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
     * @created 01-Apr-2008 2:48:24 PM
     */
    typedef boost::shared_ptr< TrainCallState > TrainCallStatePtr;

} // TA_IRS_App

#endif // !defined(TrainCallState_H)
