/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainCctvCorbaProxy_H)
#define TrainCctvCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainCctvUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{

    /**
     * Interface for commands used by Trainborne CCTV operations.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:46:52 PM
     */

    class TrainCctvCorbaProxy
                : public TA_IRS_Bus::GenericObserverManager< ITrainCctvUpdateObserver >,
                  public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
    {

    public:

        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentName    The entity name of the train agent this proxy will contact
         * @param agentKey    The train agent entity key
         * @param locationKey    The agent location key
         */
        TrainCctvCorbaProxy( const std::string& agentName,
                             unsigned long agentKey,
                             unsigned long locationKey );


        /**
         * Destructor
         */
        ~TrainCctvCorbaProxy();


        /**
         * Activate the given train CCTV broadcast. If there are no conflicts, this will
         * activate the train, and set the default broadcast state for the train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    The operator performing the action
         */
        void setCarrierOn( CommonTypes::TrainIdType trainId,
                           const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast. If there are no conflicts, this will
         * activate the train, and set the default broadcast state for the train.
         *
         * This is to be used only on delocalised trains,
         * if it is used on localised trains, it will cause an exception.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to activate/deactivate
         * @param trainLocation    This is the operator specified train location
         * @param sessionId    The operator performing the action
         */
        void setCarrierOnDelocalisedTrain( CommonTypes::TrainIdType trainId,
                                           unsigned long trainLocation,
                                           const std::string& sessionId );


        /**
         * Deactivate the given train CCTV broadcast.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to activate/deactivate
         * @param sessionId    The operator performing the action
         */
        void setCarrierOff( CommonTypes::TrainIdType trainId,
                            const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera.
         * If there are no conflicts, this will activate the train, and set the default
         * broadcast state for the train.
         *
         * Note that a carrier on message will not be sent to the train, just sending the
         * camera will implicitly activate the train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to set the single camera on
         * @param camera    The camera to set
         * @param sessionId    The operator performing the action
         */
        void setSingleScreen( CommonTypes::TrainIdType trainId,
                              ProtocolCctvTypes::ECctvCamera camera,
                              const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given quad. If
         * there are no conflicts, this will activate the train, and set the default
         * broadcast state for the train.
         *
         * Note that a carrier on message will not be sent to the train, just sending the
         * quad will implicitly activate the train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train ID to set the quad screen on
         * @param cameraW    Top left cam
         * @param cameraX    Top right cam
         * @param cameraY    Bottom left cam
         * @param cameraZ    Bottom right cam
         * @param sessionId    The operator setting the quad
         */
        void setQuadScreen( CommonTypes::TrainIdType trainId,
                            ProtocolCctvTypes::ECctvCamera cameraW,
                            ProtocolCctvTypes::ECctvCamera cameraX,
                            ProtocolCctvTypes::ECctvCamera cameraY,
                            ProtocolCctvTypes::ECctvCamera cameraZ,
                            const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given sequence.
         * If there are no conflicts, this will activate the train, and set the default
         * broadcast state for the train.
         *
         * Note that a carrier on message will not be sent to the train, just sending the
         * sequence will implicitly activate the train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train to set the sequence on
         * @param sequenceNo    The predefined sequence to set
         * @param sequenceMode    The mode of the sequence (SequenceForward should be used)
         * @param sessionId    The operator performing the action
         */
        void setSequence( CommonTypes::TrainIdType trainId,
                          ProtocolCctvTypes::ECctvSequenceNumber sequenceNo,
                          ProtocolCctvTypes::ECctvSequenceMode sequenceMode,
                          const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera
         * list. If there are no conflicts, this will activate the train, and set the
         * default broadcast state for the train.
         *
         * This builds a single or quad with the given alarm images, it adds to the
         * already displayed images on the train if any.
         *
         * Note that a carrier on message will not be sent to the train, just sending the
         * images will implicitly activate the train.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train id to add the alarm image to
         * @param cameraIds    The cameras to add
         * @param sessionId    The operator performing the action
         */
        void addAlarmImage( CommonTypes::TrainIdType trainId,
                            const CctvTypes::TrainCameraSequence& cameraIds,
                            const std::string& sessionId );


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera
         * list. If there are no conflicts, this will activate the train, and set the
         * default broadcast state for the train.
         *
         * This builds a single or quad with the given alarm images, it adds to the
         * already displayed images on the train if any.
         *
         * Note that a carrier on message will not be sent to the train, just sending the
         * images will implicitly activate the train.
         *
         * This is to be used only on delocalised trains, if it is used on localised
         * trains, it will cause an exception.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train id to add the alarm image to
         * @param cameraIds    The cameras to add
         * @param trainLocation    The manually specified train location
         * @param sessionId    The operator performing the action
         */
        void addAlarmImageDelocalisedTrain( CommonTypes::TrainIdType trainId,
                                            const CctvTypes::TrainCameraSequence& cameraIds,
                                            unsigned long trainLocation,
                                            const std::string& sessionId );


        /**
         * This will remove the given camera from the alarm images on the given train.
         *
         * It will result in a new quad, single, or a carrier off being sent to the train
         * (depending on what is already displaying)
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train ID
         * @param cameraId    The camera to remove
         * @param sessionId    The operator performing the action
         */
        void removeAlarmImage( CommonTypes::TrainIdType trainId,
                               ProtocolCctvTypes::ECctvCamera cameraId,
                               const std::string& sessionId );


        /**
         * Given a stage ID, get the train that is active for this stage, and its CCTV
         * details.
         * This is usually called be the video switch agent to determine the current state
         * for each stage.
         *
         * @return the train cctv details for the active train for the given stage.
         *         If none, the train ID will be 0.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         *
         * @param stageID the stage to check
         */
        CctvTypes::CctvSwitchState getActiveTrainForStage( unsigned long stageID );


        /**
         *
         * @param payload
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );

    protected:

        /**
         * This is a virtual method, which by default does nothing.
         * It is called when the first observer is added, and can be overridden to perform
         * some specific operation (eg subscribe to messages)
         */
        virtual void firstObserverAdded();


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when last first observer is removed, and can be overridden to
         * perform some specific operation (eg unsubscribe to messages)
         */
        virtual void lastObserverRemoved();

    private:

        // to avoid including any CORBA in the proxy class header files
        // we are now using the 'pimpl' idiom
        // ive just pimpled this class out
        struct CctvCorbaObject;


        /**
         * Private Constructor
         */
        TrainCctvCorbaProxy();


        /**
         * Called to subscribe to updates from the agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the agent
         */
        void unsubscribeToMessages();


        /**
         * object for access to the CORBA servant.
         */
        CctvCorbaObject* m_cctvCorbaObject;


        /**
         * flag to indicate message subscription
         */
        bool m_subscribedToMessages;


        /**
         * Lock to protect the subscription process
         */
        TA_Base_Core::NonReEntrantThreadLockable m_subscriptionLock;


        /**
         * This is the entity name of the train agent.
         */
        std::string m_trainAgentEntityName;


        /**
         * This is the entity key of the train agent, used to subscribe to comms messages
         * and state update messages.
         */
        unsigned long m_trainAgentEntityKey;


        /**
         * This is the location key of the train agent entity.
         */
        unsigned long m_trainAgentLocationKey;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:53 PM
     */
    typedef boost::shared_ptr< TrainCctvCorbaProxy > TrainCctvCorbaProxyPtr;

}

#endif // !defined(TrainCctvCorbaProxy_H)
