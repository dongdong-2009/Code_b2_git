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
#if !defined(IAuditSender_H)
#define IAuditSender_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"

#include <string>
#include <ctime>

namespace TA_IRS_App
{

    /**
     * This is the interface used by components within the train agent to send audit messages
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:39 PM
     */
    class IAuditSender
    {

    public:

        /**
         * This will audit a train timeout event for a given operation on a given train
         *
         * @param sessionId    The operator associated with the event if any
         * @param trainId    The train that the event relates to
         * @param operation    The command that actually timed out
         */
        virtual void auditTrainTimeout( const std::string& sessionId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        const std::string& operation ) = 0;


        /**
         * This will audit an operator taking control of a train manually
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditOperatorTakeTrainControl( const std::string& sessionId,
                                                    TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                    time_t timeStamp ) = 0;


        /**
         * This will audit a train call being made
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditTrainCallStarted( const std::string& sessionId,
                                            TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            time_t timeStamp ) = 0;


        /**
         * This will audit a train call being ended
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditTrainCallEnded( const std::string& sessionId,
                                          TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          time_t timeStamp ) = 0;


        /**
         * This will audit a train cctv being enabled or disabled.
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carrierOn    whether the carrier was turned on or off
         */
        virtual void auditCctvCarrier( const std::string& sessionId,
                                       TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       time_t timeStamp,
                                       bool carrierOn ) = 0;


        /**
         * This will audit a train cctv single screen
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param camera    The selected camera
         */
        virtual void auditCctvSingleScreen( const std::string& sessionId,
                                            TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            time_t timeStamp,
                                            TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera ) = 0;


        /**
         * This will audit a train cctv sequence
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param sequence    The selected sequence
         * @param mode    the sequence mode
         */
        virtual void auditCctvSequence( const std::string& sessionId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        time_t timeStamp,
                                        TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequence,
                                        TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode mode ) = 0;


        /**
         * This will audit a train cctv quad
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param cameraW    One of the quad cameras
         * @param cameraX    One of the quad cameras
         * @param cameraY    One of the quad cameras
         * @param cameraZ    One of the quad cameras
         */
        virtual void auditCctvQuad( const std::string& sessionId,
                                    TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    time_t timeStamp,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ ) = 0;


        /**
         * This will audit a train cctv carrier off due to conflict
         *
         * @param offendingTrainId    The offending train (that had its carrier dropped)
         * @param owningTrainId    The train still broadcasting
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditCctvConflict( TA_IRS_Bus::CommonTypes::TrainIdType offendingTrainId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId,
                                        time_t timeStamp ) = 0;


        /**
         * This will audit a train selection list being deleted
         *
         * @param sessionId    The operator associated with the event
         * @param selectionName    The selection list
         */
        virtual void auditDeleteTrainSelection( const std::string& sessionId,
                                                const std::string& selectionName ) = 0;


        /**
         * This will audit a train selection list being saved
         *
         * @param sessionId    The operator associated with the event
         * @param selectionName    The selection list
         * @param trainList    The list of trains in the list
         */
        virtual void auditSaveTrainSelection( const std::string& sessionId,
                                              const std::string& selectionName,
                                              const TA_IRS_Bus::CommonTypes::TrainIdList& trainList ) = 0;


        /**
         * This will audit a PEC activation
         *
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecActivated( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        time_t timeStamp,
                                        TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                        TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;


        /**
         * This will audit a PEC answer
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecAnswer( const std::string& sessionId,
                                     TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     time_t timeStamp,
                                     TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                     TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;


        /**
         * This will audit a PEC invalidation (train moves out of control area)
         *
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecCallInvalid( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          time_t timeStamp,
                                          TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                          TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;


        /**
         * This will audit a PEC expiration (a train has disappeared with an active PEC)
         *
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecNotAnswered( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          time_t timeStamp,
                                          TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                          TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;


        /**
         * This will audit a PEC cctv image activation
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param camera    The camera that was activated
         */
        virtual void auditPecCctvActivated( const std::string& sessionId,
                                            TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            time_t timeStamp,
                                            TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera ) = 0;


        /**
         * This will audit a PEC reset
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecReset( const std::string& sessionId,
                                    TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    time_t timeStamp,
                                    TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;


        /**
         * This will audit a PEC reset for an unanswered PEC
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecResetWithoutAnswer( const std::string& sessionId,
                                                 TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                 time_t timeStamp,
                                                 TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                                 TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;


        /**
         * This will audit a PEC reset initiated by the train
         *
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecResetByTims( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          time_t timeStamp,
                                          TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                          TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;


        /**
         * This will audit a PEC reset all
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditPecResetAll( const std::string& sessionId,
                                       TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       time_t timeStamp ) = 0;


        /**
         * This will audit a PEC call disconnection
         *
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecCallLost( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       time_t timeStamp,
                                       TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                       TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;
		
		/**
         * This will audit a PEC hold
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         * @param carNumber    The PEC car number
         * @param pecNumber    The PEC number within the car
         */
        virtual void auditPecHold( const std::string& sessionId,
                                    TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    time_t timeStamp,
                                    TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IAuditSender_H)
