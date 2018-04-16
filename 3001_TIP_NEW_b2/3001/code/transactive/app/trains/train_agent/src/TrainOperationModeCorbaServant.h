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
#if !defined(TrainOperationModeCorbaServant_H)
#define TrainOperationModeCorbaServant_H

#include "bus/trains/TrainAgentCorba/idl/src/ITrainOperationModeCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{

    class IOperationModeManagerServantInterface;
    class ITimsModeManager;

}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainOperationModeCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:29 PM
     */
    class TrainOperationModeCorbaServant : public TA_Base_Core::ServantBase, 
                                           public POA_TA_Base_Bus::ITrainOperationModeCorba
    {

    public:

        /**
         *
         * @param operationModeManager    The operation mode manager
         * @param timsModeManager    The TIMS mode manager
         */
        TrainOperationModeCorbaServant( IOperationModeManagerServantInterface& operationModeManager,
                                        ITimsModeManager& timsModeManager );


        /**
         * Destructor
         */
        virtual ~TrainOperationModeCorbaServant();


        /**
         * Called by the radio agent to notify the local train agent that it has entered or left radio fallback mode. If not already in local duty mode, entering fallback will trigger local duty mode.
         *
         * @param fallback    Whether the radio system is in fallback or not (true is fallback, false is normal)
         */
        void inFallback( CORBA::Boolean fallback );


        /**
         * Called by the ATS agent to notify the local train agent that it has entered or left double OCC ATS failure mode. If not already in local duty mode, entering fallback will trigger local duty mode.
         *
         * @param doubleAtsFailure
         */
        void inDoubleAtsFailure( CORBA::Boolean doubleAtsFailure );


        /**
         * Checks whether the agent is in local control due to radio fallback or local duty. This is only applicable at station locations.
         *
         * This is called by the ATS agent to determine if it should be exchanging ATS data locally, or using OCC ATS to do the data exchange.
         *
         * @return true if ATS should be in local mode
         */
        bool radioInFallback();


        /**
         * This will manually force train communications over to this train agent.
         * It is only to be used in failure scenarios, and incorrect use could result in the situation automatically normalising (or attempting to normalise)
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception ITrainExceptionsCorba::TransmissionFailureException if there was a failure sending the command to the train
         * @exception ITrainExceptionsCorba::TransactionTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception ITrainExceptionsCorba::TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception ITrainExceptionsCorba::MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception ITrainExceptionsCorba::BadCommandException if instead of a valid response, the train responded with a bad command message
         *
         * @param trainId    The train ID to manually take
         * @param sessionId    The operator taking control
         */
        void manuallyTakeTrainControl( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                       const char* sessionId );


    private:


        /**
         * Private default constructor
         */
        TrainOperationModeCorbaServant();


        /**
         * This is used to delegate operation mode calls to
         */
        IOperationModeManagerServantInterface& m_operationModeManager;


        /**
         * Used to take control of trains manually
         */
        ITimsModeManager& m_timsModeManager;

    };

} // TA_IRS_App

#endif // !defined(TrainOperationModeCorbaServant_H)
