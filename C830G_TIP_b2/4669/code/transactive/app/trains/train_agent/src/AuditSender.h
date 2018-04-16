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
#if !defined(AuditSender_H)
#define AuditSender_H


#include "app/trains/train_agent/src/IAuditSender.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"

#include "core/types/public_types/TypeDefs.h"

#include <ctime>


namespace TA_Base_Core
{
    class ItaAuditing;
    class MessageType;
}

namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ITrainStatusManager;
}


namespace TA_IRS_App
{

    /**
     * This is the implementation of the audit message sender. It simply builds and raises audit messages.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:19 PM
     */
    class AuditSender : public IAuditSender
    {

    public:

        /**
         * Constructor
         *
         * @param configuration    Agent configuration
         * @param statusManager    Stores train state
         * @param taAuditing    Used to submit audit messages
         */
        AuditSender( ITrainAgentConfiguration& configuration,
                     ITrainStatusManager& statusManager,
                     TA_Base_Core::ItaAuditing& taAuditing );


        /**
         * Destructor
         */
        virtual ~AuditSender();


        /**
         * This will audit an operator taking control of a train manually
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditOperatorTakeTrainControl( const std::string& sessionId,
                                                    TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                    time_t timeStamp );


        /**
         * This will audit a train timeout event for a given operation on a given train
         *
         * @param sessionId    The operator associated with the event if any
         * @param trainId    The train that the event relates to
         * @param operation    The command that actually timed out
         */
        virtual void auditTrainTimeout( const std::string& sessionId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        const std::string& operation );


        /**
         * This will audit a train call being made
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditTrainCallStarted( const std::string& sessionId,
                                            TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            time_t timeStamp );


        /**
         * This will audit a train call being ended
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditTrainCallEnded( const std::string& sessionId,
                                          TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                          time_t timeStamp );


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
                                       bool carrierOn );


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
                                            TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera );


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
                                        TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode mode );


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
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ );


        /**
         * This will audit a train cctv carrier off due to conflict
         *
         * @param offendingTrainId    The offending train (that had its carrier dropped)
         * @param owningTrainId    The train still broadcasting
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditCctvConflict( TA_IRS_Bus::CommonTypes::TrainIdType offendingTrainId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId,
                                        time_t timeStamp );


        /**
         * This will audit a train selection list being deleted
         *
         * @param sessionId    The operator associated with the event
         * @param selectionName    The selection list
         */
        virtual void auditDeleteTrainSelection( const std::string& sessionId,
                                                const std::string& selectionName );


        /**
         * This will audit a train selection list being saved
         *
         * @param sessionId    The operator associated with the event
         * @param selectionName    The selection list
         * @param trainList    The list of trains in the list
         */
        virtual void auditSaveTrainSelection( const std::string& sessionId,
                                              const std::string& selectionName,
                                              const TA_IRS_Bus::CommonTypes::TrainIdList& trainList );


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
                                        TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


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
                                     TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


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
                                          TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


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
                                          TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


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
                                            TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera );


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
                                    TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


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
                                                 TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


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
                                       TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


        /**
         * This will audit a PEC reset all
         *
         * @param sessionId    The operator associated with the event
         * @param trainId    The train that the event relates to
         * @param timeStamp    The time the event occurred (from the incoming train message)
         */
        virtual void auditPecResetAll( const std::string& sessionId,
                                       TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       time_t timeStamp );


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
                                          TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber );


    private:


        /**
         * Constructor
         */
        AuditSender();


        /**
         * This will audit an event for a given operation on a given train
         *
         * @param messageType    The message type to log
         * @param sessionId    The operator associated with the event if any
         * @param descriptionParameters    The custom name value pairs for the message type. Common name value pairs will be created as part of this function and appended to these.
         * @param trainId    The train that the event relates to if any
         * @param timeStamp    The time the event occurred, if not specified the current system time will be used.
         */
        void auditEvent( const TA_Base_Core::MessageType& messageType,
                         const std::string& sessionId,
                         TA_Base_Core::NameValuePairs& descriptionParameters,
                         TA_IRS_Bus::CommonTypes::TrainIdType trainId = 0,
                         time_t timeStamp = 0 );


        /**
         * This will build the common name value pairs for train audit messages. The common pairs that will be added are:
         * [Train] = "PV/Service"
         * [PV] = "Train ID"
         * [Service] = "Service Number"
         * [Location] = "Current Location"
         *
         * @param descriptionParameters    The name value pairs to add the common name value pairs to
         * @param trainId    The train that the event relates to
         */
        void buildCommonTrainParameters( TA_Base_Core::NameValuePairs& descriptionParameters,
                                         TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Used to access agent configuration
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * Used to access train states
         */
        ITrainStatusManager& m_statusManager;


        /**
         * Used to submit audit messages
         */
        TA_Base_Core::ItaAuditing& m_taAuditing;

    };

} // TA_IRS_App

#endif // !defined(AuditSender_H)
