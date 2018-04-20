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
#if !defined(IRadioAgentInterface_H)
#define IRadioAgentInterface_H

#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainTransactionModel/src/ICommandSender.h"
#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * Used to perform radio functions.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:42 PM
     */
    class IRadioAgentInterface : public TA_IRS_Bus::ICommandSender
    {

    public:

        /**
         * Given a list of TSIs, create a DGNA.
         *
         * @return the group TSI
         *
         * @exception TrainRadioException if making the group fails
         *
         * @param tsiList    The list of TSIs to put in the DGNA
         * @param sessionId    The operator who will use the DGNA
         */
        virtual std::string createDynamicGroup( const TA_IRS_Bus::CallTypes::ItsiList& tsiList,
                                                const std::string& sessionId ) = 0;


        /**
         * This will make an audio call to the given TSI, suitable for a PEC.
         * It is a:
         * single call
         * half duplex
         * emergency priority
         * direct (hook)
         * invisible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi    The TSI to make the call to
         * @param sessionId    The session of the operator making the call
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makePecCall( const std::string& trainItsi,
                                                                            const std::string& sessionId ) = 0;


        /**
         * This will make an audio call to the given TSI, suitable for answering an OCC Call request.
         * It is a:
         * single call
         * full duplex
         * normal priority
         * direct (hook)
         * visible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi    The TSI to make the call to
         * @param sessionId    The session of the operator making the call
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeOccCall( const std::string& trainItsi,
                                                                            const std::string& sessionId ) = 0;


        /**
         * This will make an audio call to the given TSI, suitable for making an outgoing Driver Call.
         * It is a:
         * single call
         * full duplex
         * normal priority
         * direct (hook)
         * visible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi    The TSI of the train to call
         * @param sessionId    The session of the operator making the call
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeSingleDriverCall( const std::string& trainItsi,
                                                                                     const std::string& sessionId,
                                                                                     TA_IRS_Bus::CallTypes::ECallPriority callPriority ) = 0;


        /**
         * This will make an audio call to the given group TSI, suitable for making an outgoing Driver Call to a group (DGNA or GOA).
         * It is a:
         * group call (type specified by parameters)
         * half duplex
         * priority specified by parameters
         * direct (hook)
         * visible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param groupTsi    The TSI of the group to call
         * @param sessionId    The session of the operator making the call
         * @param callType    The type of call to make. Can not be single call, if a single call is given it will be overridden to a group unacknowledged call.
         * @param callPriority    The priority of the call to make
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeGroupDriverCall( const std::string& groupTsi,
                                                                                    const std::string& sessionId,
                                                                                    TA_IRS_Bus::CallTypes::ECallType callType,
                                                                                    TA_IRS_Bus::CallTypes::ECallPriority callPriority ) = 0;


        /**
         * This will make an audio call to the given TSI, suitable for making a PA Live Announcement Call.
         * It is a:
         * single call
         * half duplex
         * normal priority
         * direct (hook)
         * invisible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi
         * @param sessionId
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeSinglePaCall( const std::string& trainItsi,
                                                                                 const std::string& sessionId ) = 0;


        /**
         * This will make an audio call to the given group TSI, suitable for making a PA Live Announcement Call to a group of trains.
         * It is a:
         * group broadcast call
         * half duplex
         * normal priority
         * direct (hook)
         * invisible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param groupTsi
         * @param sessionId
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeGroupPaCall( const std::string& groupTsi,
                                                                                const std::string& sessionId ) = 0;


        /**
         * This will end a call that is in progress. It will also remove the call from the ongoing calls list in the status manager.
         *
         * It will not throw any exceptions, just log an error if ending the call failed.
         *
         * @param callId
         * @param sessionId
         */
        virtual void endCall( unsigned long callId,
                              const std::string& sessionId ) = 0;


        /**
         * This sets up a data call to transfer the given data to the given ITSI.
         *
         * @exception TrainRadioException if making the call fails, or there is already a transfer in progress.
         *
         * @param trainItsi    The TSI to send the data to
         * @param data    The raw bytes to send
         * @param sessionId    The operator sending the data
         */
        virtual void makeDataCall( const std::string& trainItsi,
                                   const TA_IRS_Bus::TrainMessageData& data,
                                   const std::string& sessionId ) = 0;


        /**
         * This ends a data call in progress to the given ITSI.
         *
         * No exceptions will be thrown. Errors will be logged in the event of failure.
         *
         * @param trainItsi    The TSI to send the data to
         * @param sessionId    The operator sending the data
         */
        virtual void cancelDataCall( const std::string& trainItsi,
                                     const std::string& sessionId ) = 0;


        /**
         * This checks the current fallback status of the radio system.
         *
         * @return true if the radio system is in fallback
         *
         * @exception AgentCommunicationException if there was an error asking the radio agent
         */
        virtual bool inFallback() = 0;


        /**
         * This will tell the radio agent (at a station) to reset its connection to the radio system. THis is done to leave radio fallback mode.
         *
         * @exception AgentCommunicationException if there was an error communicating with the radio agent
         */
        virtual void resetConnection() = 0;


        /**
         * This will get the local radio entity key - for receiving call stack updates
         *
         * @return the entity key to use for message subscription
         */
        virtual unsigned long getRadioAgentEntityKey() = 0;

    };

} // TA_IRS_App

#endif // !defined(IRadioAgentInterface_H)
