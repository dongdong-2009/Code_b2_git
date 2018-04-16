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
#if !defined(ITimsDriverCallManager_H)
#define ITimsDriverCallManager_H

#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCallTypes.h"

#include <string>


namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:46 PM
     */
    class ITimsDriverCallManager
    {

    public:

        /**
         * FlexibleTimeoutNamedObject calls this method to calculate
         * the timeout value based on the current configuration
         */
        virtual unsigned short getFlexibleTimeOutValue() = 0;


        /**
         * This will retrieve all active OCC call requests.
         *
         * @return a sequence of OCC call requests
         */
        virtual TA_IRS_Bus::CallTypes::OccCallRequestList getAllOccCallRequests() = 0;


        /**
         * This will answer the outstanding OCC call request for the given train.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or there is no outstanding call request for this train
         * @exception TrainTransmissionFailureException if there was a failure sending the command to the train
         * @exception TrainRadioException if the radio call failed
         *
         * @param trainId    The train whos OCC call request we are going to answer.
         * @param sessionId    The session ID of the operator answering the call
         */
        virtual void answerOccCallRequest( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           const std::string& sessionId ) = 0;


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and make a DGNA containing the TSI
         * of each successfully set up train, and also the individual subscribers given.
         * It will then call the resulting group.
         *
         * If only one train is given, or only one train succeeds in the setup
         * (and there are no individual subscribers) a direct call will be made to the train.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param individualSubscribers    These are the non-train subscribers that will be included in the DGNA.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         *                    This can be overridden by the train agent if the given type does not
         *                    match the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        virtual TA_IRS_Bus::CallTypes::DriverCallResult makeDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                        const TA_IRS_Bus::CallTypes::ItsiList& individualSubscribers,
                                                                        bool override,
                                                                        TA_IRS_Bus::CallTypes::ECallType callType,
                                                                        TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                        const std::string& sessionId ) = 0;


        /**
         * Sets up a call to a train or a group of trains.
         * This will set up a call the given trains, and when successful call the group tsi given.
         *
         * @return the overall result of the operation, including the result of each specific train
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception GroupInUseException if the given group TSI is in use
         * @exception TrainRadioException if the DGNA or radio call failed
         *
         * @param trainList    A list of trains that the call will go to
         * @param groupTsi    This is the TSI of the group to call. If this is not specified,
         *                    and more than one train is given then a DGNA will be used.
         * @param override    Whether to send a request or an override
         * @param callType    The type of radio call to make (eg acknowledged/unacknowledged)
         *                    This can be overridden by the train agent if the given type does not
         *                    match the call that will be made (ie Single call is given and a group will be called)
         * @param callPriority    The call priority for the radio call
         * @param sessionId    The operator making the call
         */
        virtual TA_IRS_Bus::CallTypes::DriverCallResult makeDriverCall( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                        const std::string& groupTsi,
                                                                        bool override,
                                                                        TA_IRS_Bus::CallTypes::ECallType callType,
                                                                        TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                        const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITimsDriverCallManager_H)
