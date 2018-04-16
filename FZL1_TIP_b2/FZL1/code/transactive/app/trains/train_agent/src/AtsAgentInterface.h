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
#if !defined(AtsAgentInterface_H)
#define AtsAgentInterface_H

#include "app/trains/train_agent/src/IAtsAgentInterface.h"

#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"

#include "core/naming/src/NamedObject.h"


namespace TA_IRS_App
{
    class ITrainAgentConfiguration;
}


namespace TA_IRS_App
{

    /**
     * This implements the communication with the ATS Agent. It is used as an interface to the ATS agent.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:15 PM
     */
    class AtsAgentInterface : public IAtsAgentInterface
    {

    public:

        /**
         * Constructor with minimum required information
         *
         * @param configuration    The agent configuration
         */
        AtsAgentInterface( ITrainAgentConfiguration& configuration );


        /**
         * Destructor
         */
        virtual ~AtsAgentInterface();


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
                                        const TA_IRS_Bus::TrainMessageData& oa1 );


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
                                                    const TA_IRS_Bus::TrainMessageData& atcTrainStatus );


        /**
         * This method is used to access the full list of Train information and will
         * typically used to synchronise at startup of the client application
         *
         * @return A list of AtsTrainInformation for all trains known to the ATS agent.
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         * @exception OperationNotSupportedException if the operation is not supported
         */
        virtual AtsTrainInformationList getTrainInformationList();


        /**
         * This method is used to retrieve the double ATS failure state of the agent
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         */
        virtual bool inOccDoubleAtsFailure();


        /**
         * This method is used to send Radio Fallback state to ATS
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         *
         * @param radioFallback    True if radio is in fallback, false otherwise
         */
        virtual void radioInFallback( bool radioFallback );


        /**
         * This method is used to reset ATS after some form of degraded mode
         *
         * @exception AgentCommunicationException if there is an error contacting the agent
         */
        virtual void resetAts();


        /**
         * Gets the entity key of the local ATS agent object
         *
         * @return the ATS agent entity key
         */
        unsigned long getAtsEntityKey();


        /**
         * Gets the entity name of the local ATS agent object
         *
         * @return the ATS agent entity name
         */
        virtual std::string getAtsAgentName();


    private:


        /**
         * Private default constructor
         */
        AtsAgentInterface();


        /**
         * Loads the ats named object details from the database and sets up m_atsAgent
         *
         * @param configuration    The agent configuration
         */
        void loadAtsObject( ITrainAgentConfiguration& configuration );


        /**
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:16 PM
         */
        typedef TA_Base_Core::NamedObject < TA_IRS_Bus::IAtsTrainCorbaDef,
                                            TA_IRS_Bus::IAtsTrainCorbaDef_ptr,
                                            TA_IRS_Bus::IAtsTrainCorbaDef_var > AtsAgentObject;


        /**
         * The named object used to call the ATS agent
         */
        AtsAgentObject m_atsAgent;


        /**
         * Used for subscribing to ats comms messages
         */
        unsigned long m_atsEntityKey;


        /**
         * Used to raise alarms when comms errors occur
         */
        std::string m_atsEntityName;

    };

} // TA_IRS_App

#endif // !defined(AtsAgentInterface_H)
