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
#if !defined(TrainPecCorbaServant_H)
#define TrainPecCorbaServant_H


#include "bus/trains/TrainAgentCorba/idl/src/ITrainPecCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
   class ITimsPecManager;
}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainPecCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:32 PM
     */
    class TrainPecCorbaServant : public TA_Base_Core::ServantBase,
                                 public POA_TA_Base_Bus::ITrainPecCorba
    {

    public:

        /**
         *
         * @param timsPecManager    to delegate calls to
         */
        TrainPecCorbaServant( ITimsPecManager& timsPecManager );


        /**
         * Destructor
         */
        virtual ~TrainPecCorbaServant();


        /**
         * Gets all active PEC for all trains, and their status.
         *
         * @return all active PEC for all trains managed by this agent
         */
        TA_Base_Bus::ITrainPecCorba::PecUpdateList* getAllActivePec();


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception ITrainExceptionsCorba::TransmissionFailureException if there was a failure sending the command to the train
         * @exception ITrainExceptionsCorba::TransactionTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception ITrainExceptionsCorba::TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception ITrainExceptionsCorba::MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception ITrainExceptionsCorba::BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception ITrainExceptionsCorba::TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception ITrainExceptionsCorba::TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param sessionId    The operator performing the action
         */
        TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence* activatePecCamera( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                                              TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                                                                              TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                                                                              const char* sessionId );


        /**
         * This will activate the correct CCTV image for a PEC on a train.
         * It follows all the standard CCTV conflict management rules.
         * If this train is delocalised, it allows the operator to select the location of the train manually.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception ITrainExceptionsCorba::TransmissionFailureException if there was a failure sending the command to the train
         * @exception ITrainExceptionsCorba::TransactionTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception ITrainExceptionsCorba::TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception ITrainExceptionsCorba::MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception ITrainExceptionsCorba::BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception ITrainExceptionsCorba::TrainCctvConflictException if the train can not be activated due to conflict management rules
         * @exception ITrainExceptionsCorba::TrainCctvException if there was an error performing a CCTV operation
         *
         * @param trainId    The train that the PEC is in progress on
         * @param car    The car the PEC is active in
         * @param pec    The PEC that is active
         * @param trainLocation    Than manually determined location of the train
         * @param sessionId    The operator performing the action
         */
        TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence* activatePecCameraDelocalisedTrain( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                                                                              TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                                                                                              TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                                                                                              CORBA::ULong trainLocation,
                                                                                              const char* sessionId );


        /**
         * This will start the PEC answer sequence, and make the audio call.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception ITrainExceptionsCorba::TransmissionFailureException if there was a failure sending the command to the train
         * @exception ITrainExceptionsCorba::TransactionTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception ITrainExceptionsCorba::TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception ITrainExceptionsCorba::MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception ITrainExceptionsCorba::BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception ITrainExceptionsCorba::TrainPecException if there was an error performing a PEC operation
         * @exception ITrainExceptionsCorba::TrainRadioException if the radio call failed
         *
         * @param trainId    The train the PEC is on
         * @param car    The PEC car
         * @param pec    The PEC within the car
         * @param sessionId    The operator answering the PEC
         */
        void answerPec( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                        TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                        TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                        const char* sessionId );


        /**
         * Resets the PEC on the train.
         * If this PEC is in progress, the PEC call will be ended and the PEC reset. The PEC will be considered reset once the command executes successfully.
         * If this PEC is not valid, then the actual reset will not be sent. The PEC will simply be removed from the list of active PEC.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous (ie if the given PEC is not active or not valid)
         * @exception ITrainExceptionsCorba::TransmissionFailureException if there was a failure sending the command to the train
         * @exception ITrainExceptionsCorba::TransactionTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception ITrainExceptionsCorba::TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception ITrainExceptionsCorba::MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception ITrainExceptionsCorba::BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception ITrainExceptionsCorba::TrainPecException if there was an error performing a PEC operation
         *
         * @param trainId    The train to reset the PEC on
         * @param car    The car
         * @param pec    The PEC number
         * @param sessionId    The operator performing the reset
         */
        void resetPec( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                       TA_Base_Bus::ITrainPecCorba::EPecCarNumber car,
                       TA_Base_Bus::ITrainPecCorba::EPecNumber pec,
                       const char* sessionId );


        /**
         * This is the same as resetPec except it applies to all PEC on a particular train.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception ITrainExceptionsCorba::TransmissionFailureException if there was a failure sending the command to the train
         * @exception ITrainExceptionsCorba::TransactionTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception ITrainExceptionsCorba::TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception ITrainExceptionsCorba::MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception ITrainExceptionsCorba::BadCommandException if instead of a valid response, the train responded with a bad command message
         * @exception ITrainExceptionsCorba::TrainPecException if there was an error performing a PEC operation
         *
         * @param trainId    The train to reset the PEC on
         * @param sessionId    The operator doing the reset
         */
        void resetAllPec( TA_Base_Bus::ITrainCommonCorba::TrainId trainId, const char* sessionId );


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        CORBA::UShort getFlexibleTimeOutValue();


    private:


        /**
         * Private default constructor
         */
        TrainPecCorbaServant();


        /**
         * Used to delegate calls to
         */
        ITimsPecManager& m_timsPecManager;

    };

} // TA_IRS_App

#endif // !defined(TrainPecCorbaServant_H)
