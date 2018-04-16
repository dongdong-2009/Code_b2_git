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
#if !defined(ITimsPecManager_H)
#define ITimsPecManager_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:49 PM
     */
    class ITimsPecManager
    {

    public:

        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        virtual unsigned short getFlexibleTimeOutValue() = 0;


        /**
         * Gets all active PEC for all trains, and their status.
         *
         * @return all active PEC for all trains managed by this agent
         */
        virtual TA_IRS_Bus::PecTypes::PecUpdateList getAllActivePec() = 0;


        /**
         * This will start the PEC answer sequence, and make the audio call.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operation
         * @exception TrainRadioException if the radio call failed
         *
         * @param trainId    The train the PEC is on
         * @param car    The PEC car
         * @param pec    The PEC within the car
         * @param sessionId    The operator answering the PEC
         */
        virtual void answerPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                const std::string& sessionId ) = 0;


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param sessionId    The operator performing the action
         */
        virtual TA_IRS_Bus::CctvTypes::TrainCameraSequence activatePecCamera( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                              TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                              TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                              const std::string& sessionId ) = 0;


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         * If this train is delocalised, it allows the operator to select the location of the train manually.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception CctvConflictException if the train can not be activated due to conflict management rules
         * @exception CctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param trainLocation    Than manually determined location of the train
         * @param sessionId    The operator performing the action
         */
        virtual TA_IRS_Bus::CctvTypes::TrainCameraSequence activatePecCameraDelocalisedTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                unsigned long trainLocation,
                const std::string& sessionId ) = 0;


        /**
         * Resets the PEC on the train.
         * If this PEC is in progress, the PEC call will be ended and the PEC reset. The PEC will be considered reset once the command executes successfully.
         * If this PEC is not valid, then the actual reset will not be sent. The PEC will simply be removed from the list of active PEC.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operatio
         *
         * @param trainId    The train to reset the PEC on
         * @param car    The car
         * @param pec    The PEC number
         * @param sessionId    The operator performing the reset
         */
        virtual void resetPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                               TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                               TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                               const std::string& sessionId ) = 0;


        /**
         * This is the same as resetPec except it applies to all PEC on a particular train.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception PecException if there was an error performing a PEC operatio
         *
         * @param trainId    The train to reset the PEC on
         * @param sessionId    The operator doing the reset
         */
        virtual void resetAllPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsPecManager_H)
