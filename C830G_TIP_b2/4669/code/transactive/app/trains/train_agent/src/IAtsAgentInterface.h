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
#if !defined(IAtsAgentInterface_H)
#define IAtsAgentInterface_H

#include "app/trains/train_agent/src/AtsDataTypes.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainTransactionModel/src/CommonDataTypes.h"

namespace TA_IRS_App
{

    /**
     * This is used to provide access to the local ATS agent
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:38 PM
     */
    class IAtsAgentInterface
    {

    public:

        /**
         * This method is used to access the full list of Train information and will typlically used to synchronise at startup of the client application
         *
         * @return A list of AtsTrainInformation for all trains known to the ATS agent.
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         * @exception OperationNotSupportedException if the operation is not supported
         */
        virtual AtsTrainInformationList getTrainInformationList() = 0;


        /**
         * This method is used to send OA1 tables to ATS
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         * @exception OperationNotSupportedException if the operation is not supported
         * @exception InvalidParameterException if there is an error with the data
         *
         * @param physicalTrainNumber    The identifier of the train that originated the OA1 table
         * @param oa1    A 24 byte OA1 table to send to ATS
         */
        virtual void sendOa1TableToAts( TA_IRS_Bus::CommonTypes::TrainIdType physicalTrainNumber,
                                        const TA_IRS_Bus::TrainMessageData& oa1 ) = 0;


        /**
         * This method is used to send Atc Alarm tables to ATS
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         * @exception OperationNotSupportedException if the operation is not supported
         * @exception InvalidParameterException if there is an error with the data
         *
         * @param physicalTrainNumber    The identifier of the train that originated the OA1 table
         * @param atcTrainStatus    A 24 byte Atc data table to send to ATS
         */
        virtual void sendAtcTrainStatusReportToAts( TA_IRS_Bus::CommonTypes::TrainIdType physicalTrainNumber,
                                                    const TA_IRS_Bus::TrainMessageData& atcTrainStatus ) = 0;


        /**
         * This method is used to retrieve the double ATS failure state of the agent
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         */
        virtual bool inOccDoubleAtsFailure() = 0;


        /**
         * This method is used to send Radio Fallback state to ATS
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         *
         * @param radioFallback    True if radio is in fallback, false otherwise
         */
        virtual void radioInFallback( bool radioFallback ) = 0;


        /**
         * This method is used to reset ATS after some form of degraded mode
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         */
        virtual void resetAts() = 0;


        /**
         * Gets the entity key of the local ATS agent object
         *
         * @return the ATS agent entity key
         */
        virtual unsigned long getAtsEntityKey() = 0;


        /**
         * Gets the entity name of the local ATS agent object
         *
         * @return the ATS agent entity name
         */
        virtual std::string getAtsAgentName() = 0;

    };

} // TA_IRS_App

#endif // !defined(IAtsAgentInterface_H)
