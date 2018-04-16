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
#if !defined(DepotTimsCctvManager_H)
#define DepotTimsCctvManager_H

#include "app/trains/train_agent/src/TimsCctvManager.h"

namespace TA_IRS_App
{

    /**
     * This is the Depot specific CCTV object.
     * There is no CCTV equipment or conflict management in the depot so this is simple,
     * it just makes sure that the only thing that can be done is disabling trains as they enter the depot.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:32 PM
     */
    class DepotTimsCctvManager : public TimsCctvManager
    {

    public:

        /**
         * Constructor
         *
         * @param configuration
         * @param statusManager
         * @param transactionManager
         * @param operationModeManager
         * @param alarms
         * @param rightsChecker
         * @param messageSender    Used to send messages to clients of this agent
         * @param auditSender
         * @param atsMessageProcessor
         * @param switchAgent    The interface to the video switch agent
         */
        DepotTimsCctvManager( ITrainAgentConfiguration& configuration,
                              ITrainStatusManager& statusManager,
                              TA_IRS_Bus::ITrainTransactionManager& transactionManager,
                              IOperationModeManager& operationModeManager,
                              IAlarmUtility& alarms,
                              IRightsChecker& rightsChecker,
                              IMessageSender& messageSender,
                              IAuditSender& auditSender,
                              AtsMessageProcessor& atsMessageProcessor,
                              ICctvAgentInterface& switchAgent );


        /**
         * Destructor
         */
        virtual ~DepotTimsCctvManager();


        /**
         * Activate the given train CCTV broadcast.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    The operator performing the action
         */
        virtual void setCarrierOn( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train to activate/deactivate
         * @param trainLocation    This is the operator specified train location
         * @param sessionId    The operator performing the action
         */
        virtual void setCarrierOnDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                   unsigned long trainLocation,
                                                   const std::string& sessionId );


        /**
         * Deactivate the given train CCTV broadcast.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    The operator performing the action
         */
        virtual void setCarrierOff( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train to set the single camera on
         * @param camera    The camera to set
         * @param sessionId    The operator performing the action
         */
        virtual void setSingleScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera,
                                      const std::string& sessionId,
                                      unsigned long trainLocation = 0 );


        /**
         * Activate the given train CCTV broadcast and set the image to the given quad.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train ID to set the quad screen on
         * @param cameraW
         * @param cameraX
         * @param cameraY
         * @param cameraZ
         * @param sessionId    The operator setting the quad
         */
        virtual void setQuadScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ,
                                    const std::string& sessionId,
                                    unsigned long trainLocation = 0 );


        /**
         * Activate the given train CCTV broadcast and set the image to the given sequence.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train to set the sequence on
         * @param sequenceNo    The predefined sequence to set
         * @param sequenceMode    The mode of the sequence (SequenceForward should be used)
         * @param sessionId    The operator performing the action
         */
        virtual void setSequence( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequenceNo,
                                  TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode sequenceMode,
                                  const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera list.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train id to add the alarm image to
         * @param cameraIds    The cameras to add
         * @param sessionId    The operator performing the action
         */
        virtual void addAlarmImage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                    const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera list.
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train id to add the alarm image to
         * @param cameraIds    The cameras to add
         * @param trainLocation    The manually specified train location
         * @param sessionId    The operator performing the action
         */
        virtual void addAlarmImageDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                    const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                                    unsigned long trainLocation,
                                                    const std::string& sessionId );


        /**
         * This will remove the given camera from the alarm images on the given train.
         *
         * This is not supported in the depot, it will throw an exception
         *
         * @exception CctvException "Operation not supported in the depot"
         *
         * @param trainId    The train ID
         * @param cameraId    The camera to remove
         * @param sessionId    The operator performing the action
         */
        virtual void removeAlarmImage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraId,
                                       const std::string& sessionId );


        /**
         * This is called when the train agent has acquired the control of a location.
         * All trains within the location are now under this agent's control.
         *
         * @param locationKey    This is the location key of the newly acquired location
         */
        virtual void locationControlAcquired( unsigned long locationKey );


        /**
         * This is called when the train agent has released the control of a location.
         * All trains within the location are now under another agent's control.
         *
         * @param locationKey    This is the location key of the recently released location
         */
        virtual void locationControlReleased( unsigned long locationKey );


        /**
         * This will process the changes between the new and old ats information
         *
         * @param oldInformation    The previous information
         * @param newInformation    The latest updated information
         */
        virtual void processAtsUpdate( const AtsTrainInformation& oldInformation,
                                       const AtsTrainInformation& newInformation );


        /**
         * The operation mode of the agent has changed.
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


    private:

        
        /**
         * Private default constructor
         */
        DepotTimsCctvManager();

    };

}

#endif // !defined(DepotTimsCctvManager_H)
