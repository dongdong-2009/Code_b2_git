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
#if !defined(TimsCctvManager_H)
#define TimsCctvManager_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "app/trains/train_agent/src/IAtsChangeProcessor.h"
#include "app/trains/train_agent/src/IOperationModeObserver.h"
#include "app/trains/train_agent/src/ITimsCctvManager.h"
#include "app/trains/train_agent/src/ITimsCctvManagerTaskCallback.h"
#include "app/trains/train_agent/src/CctvTasks.h"
#include "app/trains/train_agent/src/TrainCctvState.h"
#include "app/trains/train_agent/src/TrainCommsState.h"

//#include "bus/managed_agent/src/IOperationStateCallback.h"
#include "bus/trains/TrainTransactionModel/src/ITrainEventProcessor.h"
#include "bus/trains/TrainTransactionModel/src/ITrainStateUpdateListener.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvCarrierOnCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvSingleCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvSequenceCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvQuadCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

// forward declarations
namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
    class ITrainStatusManager;
    class IOperationModeManager;
    class IAlarmUtility;
    class IRightsChecker;
    class IMessageSender;
    class IAuditSender;
    class AtsMessageProcessor;
    class ICctvAgentInterface;
}

namespace TA_IRS_Bus
{
    class ITrainTransactionManager;
}

namespace TA_Base_Core
{
    class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
{

    /**
     * This is the TIMS CCTV manager, it handles all CCTV related logic. It provides the logic used on the mainline.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:06 PM
     */
    class TimsCctvManager : public IAtsChangeProcessor,
                            public IOperationModeObserver,
                            public ITimsCctvManager,
                            public TA_IRS_Bus::ITrainStateUpdateListener,
                            public TA_IRS_Bus::ITrainEventProcessor,
                            public ITimsCctvManagerTaskCallback
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
        TimsCctvManager( ITrainAgentConfiguration& configuration,
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
        virtual ~TimsCctvManager();


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        unsigned short getFlexibleTimeOutValue();


        /**
         * Activate the given train CCTV broadcast.
         * If there are no conflicts, this will activate the train, and set the default broadcast state for the train.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    The operator performing the action
         */
        virtual void setCarrierOn( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast.
         * If there are no conflicts, this will activate the train, and set the default broadcast state for the train.
         *
         * This is to be used only on delocalised trains, if it is used on localised trains, it will cause an exception.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
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
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    The operator performing the action
         */
        virtual void setCarrierOff( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera.
         * If there are no conflicts, this will activate the train, and set the camera broadcast state for the train.
         *
         * Note that a carrier on message will not be sent to the train, just sending the camera will implicitly activate the train.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to set the single camera on
         * @param camera    The camera to set
         * @param sessionId    The operator performing the action
         * @param trainLocation    Only specified from addAlarmImageInternal
         */
        virtual void setSingleScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera,
                                      const std::string& sessionId,
                                      unsigned long trainLocation = 0 );


        /**
         * Activate the given train CCTV broadcast and set the image to the given quad.
         * If there are no conflicts, this will activate the train, and set the quad broadcast state for the train.
         *
         * Note that a carrier on message will not be sent to the train, just sending the quad will implicitly activate the train.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train ID to set the quad screen on
         * @param cameraW   Top left cam
         * @param cameraX   Top right cam
         * @param cameraY   bottom left cam
         * @param cameraZ   bottom right cam
         * @param sessionId    The operator setting the quad
         * @param trainLocation    Only specified from addAlarmImageInternal
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
         * If there are no conflicts, this will activate the train, and set the sequence broadcast state for the train.
         *
         * Note that a carrier on message will not be sent to the train, just sending the sequence will implicitly activate the train.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
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
         * If there are no conflicts, this will activate the train.
         *
         * This builds a single or quad with the given alarm images, it adds to the already displayed images on the train if any.
         *
         * Note that a carrier on message will not be sent to the train, just sending the images will implicitly activate the train.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
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
         * If there are no conflicts, this will activate the train.
         *
         * This builds a single or quad with the given alarm images, it adds to the already displayed images on the train if any.
         *
         * Note that a carrier on message will not be sent to the train, just sending the images will implicitly activate the train.
         *
         * This is to be used only on delocalised trains, if it is used on localised trains, it will cause an exception.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
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
         * It will result in a new quad, single, or a carrier off being sent to the train (depending on what is already displaying)
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train ID
         * @param cameraId    The camera to remove
         * @param sessionId    The operator performing the action
         */
        virtual void removeAlarmImage( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                       TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraId,
                                       const std::string& sessionId );


        /**
         * Given a stage ID, get the train that is active for this stage, and its CCTV details.
         * This is usually called be the video switch agent to determine the current state for each stage.
         *
         * @return the train cctv details for the active train for the given stage. If none, the train ID will be 0.
         *
         * @param stageID
         */
        virtual TA_IRS_Bus::CctvTypes::CctvSwitchState getActiveTrainForStage( unsigned long stageId );


		virtual TA_IRS_Bus::CctvTypes::CctvSwitchState	getCctvState( TA_IRS_Bus::CommonTypes::TrainIdType trainId );

        /**
         * The operation mode of the agent has changed.
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


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
         * Implemented by the concrete class to handle state changes.
         *
         * @param state
         */
        virtual void processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state );
        
        
        /**
         * This will process the events from trains that this object is subscribed for.
         *
         * @param event
         */
        virtual void processEvent( TA_IRS_Bus::TrainEventPtr event );


    protected:


        /**
         * This is the result of conflict checking for a train.
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:06 PM
         */

        struct TrainConflict
        {
            /**
             * The type of conflict the train is in
             */
            CctvZone::ECctvZoneType conflictType;


            /**
             * The train that was originally broadcasting. The train that the queried train will conflict with
             */
            TA_IRS_Bus::CommonTypes::TrainIdType owningTrain;
        };


        /**
         * Deactivate the given train CCTV broadcast.
         * This is the internal logic. Note it is not checked whether the CCTV is active or not,
         * because this command can be sent regardless, it also doesnt check if this agent is in control of this train,
         * because commands are sent automatically at station exit.
         *
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    Optional session ID in case the operator is performing this action
         */
        void setCarrierOffInternal( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    const std::string& sessionId = "" );


        /**
         * This will schedule a task to send a carrier off command to the given train.
         * Optionally it will wait to execute this task until the train starts to communicate with this agent.
         * There are 2 cases:
         * 1. When a train is leaving an isolated station, the task must be executed immediately,
         *    no validity check should be performed
         * 2. When the train is entering a station, there may be a change mode command that must
         *    be executed before this task can be, it is necessary to wait for this command to complete first.
         *
         * @param trainId    The train that must be deactivated
         * @param owningTrainId    If the carrier off is due to conflict, which train is the owning train
         * @param waitForCommunication    Whether to wait for the train to communicate with this agent
         */
        void scheduleCarrierOffTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId,
                                     bool waitForCommunication );


        /**
         * This will cancel all previously scheduled carrier off tasks
         */
        void cancelAllCarrierOffTasks();


        /**
         * This will cancel a previously scheduled task to send a carrier off command to the given train.
         *
         * @param trainId    The train that must be deactivated
         */
        void cancelCarrierOffTask( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * This will cancel a previously scheduled task to send a carrier off command to the given train.
         *
         * @param locationKey    The location to cancel tasks for
         */
        void cancelCarrierOffTasksForLocation( unsigned long locationKey );


        /**
         * Used to access agent configuration
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * Used to set and get train state
         */
        ITrainStatusManager& m_statusManager;


        /**
         * Used to query and register for operation mode changes
         */
        IOperationModeManager& m_operationModeManager;


        /**
         * Used to send client updates
         */
        IMessageSender& m_messageSender;


        /**
         * Flag to indicate if the agent has not gone into control or monitor mode yet
         */
        bool m_firstRun;


    private:


        /**
         * Private default constructor
         */
        TimsCctvManager();


        /**
         * This will process an MPU changeover from a train, sent when the primary MPU fails and the secondary takes over.
         *
         * @param changeoverEvent    The MPU changover event
         */
        void processChangeoverStatusEvent( TA_IRS_Bus::ChangeoverStatusEventPtr changeoverEvent );
        
        
        /**
         * This will actually send the carrier off to the train. It is called from the carrier off work item.
         *
         * This can be declared private here so only those with a pointer to the interface declaring this public can call this.
         *
         * @param trainId    The train to carrier off
         * @param owningTrainId    If the carrier off is due to conflict, which train is the owning train
         */
        virtual void performCarrierOff( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId );


        /**
         * Given a station location, get the train that is active for this location, and its CCTV details.
         *
         * @return the train cctv details for the active train for the given station. If none, the train ID will be 0.
         *
         * @param locationKey    The station location to check
         */
        TA_IRS_Bus::CctvTypes::CctvSwitchState getActiveTrainForStation( unsigned long locationKey );


        /**
         * This is the common logic used to activate CCTV carrier.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    The operator performing the action
         * @param delocalisedTrainLocation    This is the operator specified train location
         */
        void setCarrierOnInternal( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   const std::string& sessionId,
                                   unsigned long delocalisedTrainLocation = 0 );


        /**
         * This is the common logic to add a cctv alarm image
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train id to add the alarm image to
         * @param cameraIds    The cameras to add
         * @param sessionId    The operator performing the action
         * @param trainLocation    The manually specified train location
         */
        void addAlarmImageInternal( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds,
                                    const std::string& sessionId,
                                    unsigned long trainLocation = 0 );


        /**
         * Given a CCTV state, and some cameras to add to the alarm stack, return the new cameras for the CCTV state.
         *
         * @return a 4 element list of cameras
         *
         * @param switchState    The original CCTV state
         * @param cameraIds    The cameras to add to the alarm stack
         */
        TA_IRS_Bus::CctvTypes::TrainCameraSequence addCamerasToAlarmStack( const TA_IRS_Bus::CctvTypes::CctvSwitchState& switchState,
                                                                           const TA_IRS_Bus::CctvTypes::TrainCameraSequence& cameraIds );


        /**
         * This is a generic function to set the mode of CCTV to default, single screen,
         * sequence, or quad. It is used by setCarrierOnInternal, setSingleScreen, setQuadScreen, and setSequence.
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train ID
         * @param sessionId    The operator setting the state
         * @param command    the specific command to send
         * @param operationDescription  the description for log messages
         * @param trainLocation    Only specified from addAlarmImageInternal
         */
        template <typename CommandTypePtr>
        void genericSetCctvMode( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                 const std::string& sessionId,
                                 CommandTypePtr command,
                                 const std::string& operationDescription,
                                 unsigned long trainLocation = 0 );


        /**
         * Each specific type of command has some post processing on a successful result.
         * This is called from genericSetCctvMode for each type of command sent
         *
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param command    the specific command sent (not used, only allows polymorphism)
         * @param commandReceived    the result received
         */
        void handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  const std::string& sessionId,
                                  TrainCctvStatePtr cctvState,
                                  const TrainConflict& trainConflict,
                                  TA_IRS_Bus::CctvCarrierOnCommandPtr command,
                                  TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived );


        void handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  const std::string& sessionId,
                                  TrainCctvStatePtr cctvState,
                                  const TrainConflict& trainConflict,
                                  TA_IRS_Bus::CctvSingleCommandPtr command,
                                  TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived );


        void handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  const std::string& sessionId,
                                  TrainCctvStatePtr cctvState,
                                  const TrainConflict& trainConflict,
                                  TA_IRS_Bus::CctvQuadCommandPtr command,
                                  TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived );


        void handleCommandResult( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  const std::string& sessionId,
                                  TrainCctvStatePtr cctvState,
                                  const TrainConflict& trainConflict,
                                  TA_IRS_Bus::CctvSequenceCommandPtr command,
                                  TA_IRS_Bus::CctvCommandReceivedEventPtr commandReceived );


        /**
         * This is called before activating a train or changing its CCTV state.
         * It will check the given operator session has rights to activate this train, and also check this agent is in control of this train.
         * It will check activity and do optional location override and conflict checking.
         *
         * @return the conflict calculations if this train is active
         *
         * @exception AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if the train ID given is not known or not valid for this operation
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         *
         * @param trainId    The train ID that is activated, or will be activated
         * @param operatorSession    The operator sending the command
         * @param delocalisedLocation    The operator manually overridden location in case of delocalisation
         */
        TrainConflict preActivateTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        const std::string& operatorSession,
                                        unsigned long delocalisedLocation = 0 );


        /**
         * This is called after activating a train or changing its CCTV state.
         * Based on the previously calculated conflict state, it will update the cctv state and raise or close alarms.
         *
         * @param trainCctvState    The cctv state of the train that was just activated
         * @param trainConflict    The previously calculated train conflict
         */
        void postActivateTrain( TrainCctvStatePtr trainCctvState,
                                const TrainConflict& trainConflict );


        /**
         * This is called after deactivating a train
         *
         * @param trainCctvState    The cctv state of the train that was just deactivated
         * @param timeStamp    The time the train was deactivated, if 0 the current time will be used
         */
        void onTrainDeactivation( TrainCctvStatePtr trainCctvState,
                                  time_t timeStamp = 0 );


        /**
         * Handles train comms state changes
         *
         * @param state    The train comms state that changed
         */
        virtual void processTrainCommsStateChanged( TrainCommsStatePtr state );


        /**
         * Handles train cctv state changes
         *
         * @param state    The train cctv state that changed
         */
        virtual void processTrainCctvStateChanged( TrainCctvStatePtr state );


        /**
         * This is used to re-evalute the train conflicts.
         * This is done in the case a train was activated or deactivated, there may be other affected trains that are now in pre conflict or conflict, or previously were and are not any longer. These need to be processed.
         *
         * Also, if the agent gains control of an area, it must ensure it sorts out the conflict for the trains it is newly responsible for.
         */
        void reEvaluateConflicts();


        /**
         * This checks conflicts for a given train that is to be activated, or will be activated.
         * It will take into account degraded vs normal mode etc
         *
         * @return the conflict state of the train
         *
         * @param commsState    The comms state for the train that will be/is activated
         */
        TrainConflict checkConflicts( TrainCommsStatePtr commsState );


        /**
         * This will open a pre conflict alarm for the given train if one is not already open
         *
         * @param trainId    the train to raise the alarm for
         * @param conflictingTrainId    The train this train is in conflict with
         */
        void raisePreConflictAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    TA_IRS_Bus::CommonTypes::TrainIdType conflictingTrainId );


        /**
         * This will close any open pre conflict alarms for the given train
         *
         * @param trainId    the train to close the alarm for
         */
        void closePreConflictAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Used to send and register for messages from trains
         */
        TA_IRS_Bus::ITrainTransactionManager& m_transactionManager;


        /**
         * Used to raise and close alarms
         */
        IAlarmUtility& m_alarms;


        /**
         * Used to check rights for operator actions
         */
        IRightsChecker& m_rightsChecker;


        /**
         * Used to send audit messages for operator actions
         */
        IAuditSender& m_auditSender;


        /**
         * Used to subscribe for ATS messages
         */
        AtsMessageProcessor& m_atsMessageProcessor;


        /**
         * This is the interface used to communicate with the video switch agent
         */
        ICctvAgentInterface& m_switchAgent;


        /**
         * Used to schedule work items
         */
        TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;


        /**
         * The map of carrier off tasks indexed by train ID
         */
        CarrierOffTaskMap m_carrierOffTasks;


        /**
         * The lock for m_carrierOffTasks
         */
        TA_Base_Core::NonReEntrantThreadLockable m_carrierOffTaskLock;

    };

}

#endif // !defined(TimsCctvManager_H)
