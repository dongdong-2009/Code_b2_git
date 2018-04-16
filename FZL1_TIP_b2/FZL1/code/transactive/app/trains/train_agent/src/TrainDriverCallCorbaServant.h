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
#if !defined(TrainDriverCallCorbaServant_H)
#define TrainDriverCallCorbaServant_H

#include "bus/trains/TrainAgentCorba/idl/src/ITrainDriverCallCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
    class ITimsDriverCallManager;
}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainDriverCallCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:27 PM
     */
    class TrainDriverCallCorbaServant : public TA_Base_Core::ServantBase, 
                                        public POA_TA_Base_Bus::ITrainDriverCallCorba
    {

    public:

        /**
         *
         * @param timsDriverCallManager    Used to delegate calls to
         */
        TrainDriverCallCorbaServant( ITimsDriverCallManager& timsDriverCallManager );


        /**
         * Destructor
         */
        virtual ~TrainDriverCallCorbaServant();


        /**
         * This will retrieve all active OCC call requests.
         *
         * @return a sequence of OCC call requests
         */
        TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList* getAllOccCallRequests();


        /**
         * This will answer the outstanding OCC call request for the given train.
         *
         * @exception ITrainExceptionsCorba::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or there is no outstanding call request for this train
         * @exception ITrainExceptionsCorba::TransmissionFailureException if there was a failure sending the command to the train
         * @exception ITrainExceptionsCorba::TrainRadioException if the radio call failed
         *
         * @param trainId    The train whos OCC call request we are going to answer.
         * @param sessionId    The session ID of the operator answering the call
         */
        void answerOccCallRequest( TA_Base_Bus::ITrainCommonCorba::TrainId trainId,
                                   const char* sessionId );


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and when successful call the group tsi given.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         * @exception ITrainExceptionsCorba::TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param groupTsi    This is the TSI of the group to call. If this is not specified, and more than one train is given then a DGNA will be used.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         * This can be overridden by the train agent if the given type does not match the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult* makeStaticGroupDriverCall( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainList,
                                                                                         const char* groupTsi,
                                                                                         CORBA::Boolean override,
                                                                                         TA_Base_Bus::IRadioSessionCorbaDef::ECallType callType,
                                                                                         TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority callPriority,
                                                                                         const char* sessionId );


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and make a DGNA containing the TSI of each successfully set up train, and also the individual subscribers given.
         * It will then call the resulting group.
         *
         * If only one train is given, or only one train succeeds in the setup (and there are no individual subscribers) a direct call will be made to the train.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         * @exception ITrainExceptionsCorba::TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param individualSubscribers    These are the non-train subscribers that will be included in the DGNA.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         * This can be overridden by the train agent if the given type does not match the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult* makeDgnaDriverCall( const TA_Base_Bus::ITrainCommonCorba::TrainList& trainList,
                                                                                  const TA_Base_Bus::ITrainDriverCallCorba::ItsiList& individualSubscribers,
                                                                                  CORBA::Boolean override,
                                                                                  TA_Base_Bus::IRadioSessionCorbaDef::ECallType callType,
                                                                                  TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority callPriority,
                                                                                  const char* sessionId );


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        CORBA::UShort getFlexibleTimeOutValue();


    private:


        /**
         * Private default constructor
         */
        TrainDriverCallCorbaServant();


        /**
         * Used to delegate calls to
         */
        ITimsDriverCallManager& m_timsDriverCallManager;

    };

} // TA_IRS_App

#endif // !defined(TrainDriverCallCorbaServant_H)
