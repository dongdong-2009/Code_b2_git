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
#if !defined(TrainPecState_H)
#define TrainPecState_H

#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{

    /**
     * Stores the state relating to PEC calls and PEC call requests.
     *
     * This is a local state and is only synchronised between peer agents.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:32 PM
     */
    class TrainPecState : public TA_IRS_Bus::TrainState
    {

    public:

        /**
         * Creates the object with a train ID
         *
         * @param trainId
         */
        TrainPecState( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Destructor
         */
        virtual ~TrainPecState();


        /**
         * Returns the train resource this state represents.
         *
         * @return the resource category this state represents.
         */
        virtual TA_IRS_Bus::CommonTypes::ETrainResource getResourceCategory() const;


        /**
         * This gets the lock used for sending PEC commands.
         * This lock is acquired for the duration of a PEC operation to prevent multiple
         * PEC commands being sent to the same train at the same time.
         *
         * @return the lock to acquire while sending commands to this train
         */
        TA_Base_Core::IThreadLockable& getCommandLock();


        /**
         * Gets the operators session ID for the currently answered PEC.
         *
         * @return the session id or empty if none
         */
        const std::string& getSessionId() const;


        /**
         * Gets the console Id of an ongoing call
         *
         * @return the console id (or 0 if none in progress)
         */
        unsigned long getConsoleId() const;


        /**
         * Gets the radio call ID for the currently answered PEC
         *
         * @return the call ID or 0 if none
         */
        unsigned long getCallId() const;


        /**
         * Checks if the given operator can answer the given PEC
         *
         * @return true if there is no other ongoing PEC
         *
         * @exception InvalidParameterException if no matching active PEC point is found for this train
         *
         * @param car    The car number
         * @param pec    The pec number
         */
        bool canAnswerPec( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                           TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec ) const;


        /**
         * Gets the current PEC point in progress for this train.
         *
         * @exception InvalidParameterException if no active PEC point is found for this train
         */
        TA_IRS_Bus::PecTypes::PecPoint getConnectedPecPoint() const;


        /**
         * Gets the current active PEC points for this train.
         *
         * @return the list of active PEC points for this train
         */
        TA_IRS_Bus::PecTypes::ActivePecPointList getActivePecPoints() const;


        /**
         * Gets the the number of times the PEC continue has been automatically sent for this PEC.
         *
         * @return the number of auto acks sent
         */
        unsigned char getAutoContinueCount() const;


        /**
         * Checks whether an operator continue is required for the PEC on this train.
         *
         * @return true if a continue is required.
         */
        bool isContinueRequired();


        /**
         * A new PEC has been activated (or an old one has been re-activated)
         *
         * @return the final saved data (to send in a message) by reference - copy if needed.
         *
         * @param car    The car the pec was activated in
         * @param pec    The pec number within the car
         * @param timestamp    The timestamp on the incoming TIMS message
		 * @param pecTrainSource The PEC Train Source
         */
        TA_IRS_Bus::PecTypes::PecPoint pecActivated( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                     TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
													 time_t timestamp, TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource );


        /**
         * Sets the operators session ID - when answering a PEC.
         *
         * @param sessionId    The operator who is answering/has answered the active pec
         */
        void setSessionId( const std::string& sessionId );


        /**
         * Resets the operators session ID - when answering a PEC fails
         */
        void resetSessionId();


        /**
         * A PEC has been answered by the operator
         *
         * @return the final saved data (to send in a message) by reference - copy if needed.
         *
         * @param car    The car the pec was activated in
         * @param pec    The pec number within the car
         * @param timestamp    The timestamp on the incoming TIMS message
         * @param sessionId    The operator who answered the PEC
         * @param consoleId    The console of the operator who made the call
         * @param callId    The call that was made for the active PEC
		 * @param pecTrainSource The PEC Train Source
         */
        TA_IRS_Bus::PecTypes::PecPoint pecAnswered( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                    time_t timestamp,
                                                    const std::string& sessionId,
                                                    unsigned long consoleId,
                                                    unsigned long callId,
													TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource );


        /**
         * A PEC has been answered by the driver.
         *
         * @return the list of PEC that were affected by the driver answer (those that need client updates)
         *
         * @param car    The car the pec was activated in
         * @param pec    The pec number within the car
         * @param timestamp    The timestamp on the incoming TIMS message
         */
        TA_IRS_Bus::PecTypes::ActivePecPointList pecAnsweredByDriver( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                      TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                      time_t timestamp,
																	  TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource );


        /**
         * clears all details for any ongoing call.
         *
         * @param timestamp    The time the call was ended, if this was triggered by a TIMS event, use the TIMS event timestamp
         */
        void clearCall( time_t timestamp = 0 );


        /**
         * Sets that a manual continue is required
         *
         * @param timestamp    The time the call was ended, if this was triggered by a TIMS event, use the TIMS event timestamp
         */
        void setContinueRequired( time_t timestamp = 0 );


        /**
         * Increments the auto continue ack count.
         */
        void incrementAutoContinueCount();


        /**
         * A PEC (or all PEC) have been reset
         * If this was the active PEC, the call and session IDs will also be reset
         *
         * @return the PEC that were reset. If no PEC were reset, return nothing.
         *
         * @param car    The car the pec was activated in
         * @param pec    The pec number within the car
         * @param timestamp    The timestamp on the incoming TIMS message
		 * @param pecTrainSource The PEC Train Source
         */
        TA_IRS_Bus::PecTypes::ActivePecPointList pecReset( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                           TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                           time_t timestamp,
														   TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource );

		/**
		 * A PEC has been hold
		 *
		 * @return the PEC that were reset. If no PEC were reset, return nothing.
		 *
		 * @param car    The car the pec was activated in
		 * @param pec    The pec number within the car
		 * @param timestamp    The timestamp on the incoming TIMS message
		 * @param pecTrainSource The PEC Train Source
		 */
		TA_IRS_Bus::PecTypes::ActivePecPointList pecHold( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
														  TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
														  time_t timestamp,
														  TA_IRS_Bus::PecTypes::EPecTrainSource pecTrainSource );


        /**
         * This will remove all active PEC for this train.
         *
         * @param timestamp    The time the PEC was reset
         */
        TA_IRS_Bus::PecTypes::ActivePecPointList resetAllPec( time_t timestamp );


        /**
         * Resets the auto continue count
         *
         * @param timestamp    The time the state was reset, only specify if there was a preceding TIMs event to take the time from
         */
        const void resetAutoContinueCount( time_t timestamp = 0 );


        /**
         * This will invalidate all active PEC for this train.
         *
         * @param timestamp    The time the PEC was reset
         */
        TA_IRS_Bus::PecTypes::ActivePecPointList invalidateAllPec( time_t timestamp );


        /**
         * Gets the state for synchronisation purposes
         *
         * @return the state needing to be synchronised
         */
        TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState getSyncState() const;


        /**
         * Sets the bulk state - for synchronisation
         *
         * @param state    The new internal state
         */
        void setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState& state );


    private:

        /**
         * Private default constructor
         */
        TrainPecState();


        /**
         * Updates an existing PEC point or adds a new one.
         *
         * @return the final stored PEC point
         *
         * @param pec    The new/updated PEC point to store
         * @param timestamp    The time on the incoming TIMS event
         */
        TA_IRS_Bus::PecTypes::PecPoint updatePec( const TA_IRS_Bus::PecTypes::PecPoint& pec,
                                                  time_t timestamp );


        /**
         * A set of activated PEC on the train.
         * Only one can be in progress, all the rest must be 'PecSourceUnassigned'
         */
        TA_IRS_Bus::PecTypes::ActivePecPointList m_activePecPoints;


        /**
         * The session ID of the operator who answered the PEC
         */
        std::string m_sessionId;


        /**
         * The console ID so the call ID can be uniquely identified in the system
         */
        unsigned long m_consoleId;


        /**
         * The Radio call ID of the pec call
         */
        unsigned long m_callId;


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
         * This lock is acquired for the duration of an operation to prevent multiple
         * commands of the same type being sent to the same train at the same time.
         */
        TA_Base_Core::NonReEntrantThreadLockable m_commandLock;

    };

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:33 PM
     */
    typedef boost::shared_ptr< TrainPecState > TrainPecStatePtr;

} // TA_IRS_App

#endif // !defined(TrainPecState_H)
