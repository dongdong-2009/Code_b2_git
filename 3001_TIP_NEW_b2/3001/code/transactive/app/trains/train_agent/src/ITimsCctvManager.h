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
#if !defined(ITimsCctvManager_H)
#define ITimsCctvManager_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:45 PM
     */
    class ITimsCctvManager
    {

    public:

        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timeout value based on the current configuration
         */
        virtual unsigned short getFlexibleTimeOutValue() = 0;


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
                                   const std::string& sessionId ) = 0;


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
                                                   const std::string& sessionId ) = 0;


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
                                    const std::string& sessionId ) = 0;


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera.
         * If there are no conflicts, this will activate the train, and set the default broadcast state for the train.
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
         */
        virtual void setSingleScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera,
                                      const std::string& sessionId,
                                      unsigned long trainLocation = 0 ) = 0;


        /**
         * Activate the given train CCTV broadcast and set the image to the given quad.
         * If there are no conflicts, this will activate the train, and set the default broadcast state for the train.
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
         * @param cameraW    Top left camera
         * @param cameraX    Top right camera
         * @param cameraY    Bottom left camera
         * @param cameraZ    Bottom right camera
         * @param sessionId    The operator setting the quad
         */
        virtual void setQuadScreen( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,
                                    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ,
                                    const std::string& sessionId,
                                    unsigned long trainLocation = 0 ) = 0;


        /**
         * Activate the given train CCTV broadcast and set the image to the given sequence.
         * If there are no conflicts, this will activate the train, and set the default broadcast state for the train.
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
                                  const std::string& sessionId ) = 0;


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera list.
         * If there are no conflicts, this will activate the train, and set the default broadcast state for the train.
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
                                    const std::string& sessionId ) = 0;


        /**
         * Activate the given train CCTV broadcast and set the image to the given camera list.
         * If there are no conflicts, this will activate the train, and set the default broadcast state for the train.
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
                                                    const std::string& sessionId ) = 0;


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
                                       const std::string& sessionId ) = 0;


        /**
         * Given a stage ID, get the train that is active for this stage, and its CCTV details.
         * This is usually called be the video switch agent to determine the current state for each stage.
         *
         * @return the train cctv details for the active train for the given stage. If none, the train ID will be 0.
         *
         * @param stageID
         */
        virtual TA_IRS_Bus::CctvTypes::CctvSwitchState getActiveTrainForStage( unsigned long stageId ) = 0;

		virtual TA_IRS_Bus::CctvTypes::CctvSwitchState	getCctvState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsCctvManager_H)
