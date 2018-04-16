/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISApprovedRATISDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISApprovedRATISDisplayRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"

#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"

#include <iomanip>

namespace TA_IRS_App
{

	STISApprovedRATISDisplayRequest::STISApprovedRATISDisplayRequest()
        : STISMessage(),
        m_messageText(""),
        m_destination(""),
        m_RATISTag(""),
        m_startTime(""),
        m_endTime(""),
        m_priority(0),
        m_requiresApproval(true),
        m_overridable(false),
        m_clearMessage(false),
        m_messageLength(0),
        m_operatorName("")
	{
		m_messageIDString = "M12";
	}

	STISApprovedRATISDisplayRequest::~STISApprovedRATISDisplayRequest()
	{


	}


	std::vector<unsigned char> STISApprovedRATISDisplayRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M12

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// DESTINATION 'FIELD'
        dataStream << m_destination;

		// MESSAGE 'FIELDS' (MSGRXH)
		
		// Message Tag (12 bytes)
        dataStream << std::setw(12) << std::setfill(' ') << std::left << m_RATISTag;

		// Start time
		dataStream << m_startTime;

		// End time
		dataStream << m_endTime;

		// priority
		dataStream << m_priority;

        if (!m_isIncoming)
        {
            // if this is a message sent by the ISCS, then vetting and override will be 
            // filled with empty spaces and clearing be filled with 0
            dataStream << "  0";
        }
        else
        {
            dataStream << ( m_requiresApproval ? "1" : "0" ); // Vetting
            dataStream << ( m_overridable ? "1" : "0" ); // Overridable
            dataStream << ( m_clearMessage ? "1" : "0" ); // Is this a clearing message?
        }

        if (m_clearMessage) // don't send the message if clear is on
        {
            m_messageText = "";
        }

		dataStream << std::setw(3) << std::setfill('0') << std::right << m_messageText.size();

		dataStream << m_messageText;

		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();
        messageStream << std::setw(4) << std::setfill('0') << dataSize;

		// Add the data stream to the actual message stream after calculating its size
		messageStream << dataString;
		
		// No train data

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}

	void STISApprovedRATISDisplayRequest::log(bool sendSuccess)
	{
		TA_ASSERT(m_dataIsSet, "Log() called for Predefined display request that has no data");

		if( m_type == TA_Base_Core::RATIS_IN_CLEAR || m_type == TA_Base_Core::RATIS_OUT_CLEAR )
		{
			return;
		}

        TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();

        TA_IRS_Core::IDisplayRequest::EMessageType type;
        switch (m_type)
        {
        case TA_Base_Core::RATIS_IN_NEW:
            type = TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW;
            break;
        case TA_Base_Core::RATIS_OUT_NEW:
            type = TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW;
            break;
        case TA_Base_Core::RATIS_IN_UPDATE:
            type = TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE;
            break;
        case TA_Base_Core::RATIS_OUT_UPDATE:
            type = TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE;
            break;
        case TA_Base_Core::RATIS_IN_CLEAR:
            TA_ASSERT(false, "Attempting to log a RATIS clear");
            break;
        case TA_Base_Core::RATIS_OUT_CLEAR:
            TA_ASSERT(false, "Attempting to log a RATIS clear");
            break;
/*        case TA_IRS_Core::RATIS_APPROVED:
            TA_ASSERT(false, "Attempting to log a RATIS approved");
            break;
*/
        }
        displayRequest->setMessageType( type );
		displayRequest->setAdhocMessageContent( m_messageText );
		displayRequest->setRatisTag( m_RATISTag );
		displayRequest->setRatisDestination( m_destination );
		displayRequest->setRatisOverridable( m_overridable );
		displayRequest->setStartTime( m_startTime );
		displayRequest->setEndTime( m_endTime );
		displayRequest->setPriority( m_priority );
		displayRequest->setTimestamp( m_timestamp );
		//hongran++ TD17502
		displayRequest->setIfSendSuccess(sendSuccess ? TA_Base_Core::Pass : TA_Base_Core::Fail);
		//++hongran TD17502
		
        // check if its TTIS or STIS
        std::string system = m_destination.substr(3, 3);
        if ((system.compare("ATR") == 0) || (system.compare("TRN") == 0))
		{
            displayRequest->setDestinationSystem( "TTIS" );
		}
        else
		{
            displayRequest->setDestinationSystem( "STIS" );
		}

        // log the destination as an STIS destination for displaying in the log viewer
        std::string stisStation;
        std::string stisLevels;
        std::string stisPids;

        STISManager::getInstance()->getSTISDestinationFromRATISDestination(
            m_destination,
            stisStation,
            stisLevels,
            stisPids );

        displayRequest->setStisDestinationStation( stisStation );
        displayRequest->setStisDestinationLevels( stisLevels );
        displayRequest->setStisDestinationPids( stisPids );

        // operator name for tis log viewer
        displayRequest->setSubmittedBy( m_operatorName );

		// write this display request to the database
		displayRequest->applyChanges();

		delete displayRequest;
		displayRequest = NULL;

	}

	void STISApprovedRATISDisplayRequest::setData( 
        const std::string messageContent, 
        const unsigned short priority, 
        const std::string ratisTag, 
        const std::string destination, 
		const std::string startTime, 
        const std::string endTime, 
        const TA_Base_Core::ERATISMessageType type,
        const bool overridable,
		const bool requiresApproval,
        const std::string& operatorName )
	{
		m_messageText = messageContent;
		m_RATISTag = ratisTag;
		m_destination = destination;
		m_priority = priority;
		m_startTime = startTime;
		m_endTime = endTime;
		m_type = type;
		m_overridable = overridable;
		m_requiresApproval = requiresApproval;
        m_isIncoming = true;

        switch (m_type)
        {
        case TA_Base_Core::RATIS_IN_NEW:
        case TA_Base_Core::RATIS_OUT_NEW:
        case TA_Base_Core::RATIS_IN_UPDATE:
        case TA_Base_Core::RATIS_OUT_UPDATE:
            m_clearMessage = false;
            break;
        case TA_Base_Core::RATIS_IN_CLEAR:
        case TA_Base_Core::RATIS_OUT_CLEAR:
            m_clearMessage = true;
            break;
        }

        m_operatorName = operatorName;

		m_dataIsSet = true;
	}


	void STISApprovedRATISDisplayRequest::setData( 
        const std::string messageContent, 
        const unsigned short priority, 
        const std::string ratisTag, 
        const std::string destination, 
		const std::string startTime, 
        const std::string endTime,
        const TA_Base_Core::ERATISMessageType type,
        const std::string& operatorName )
	{
		m_messageText = messageContent;
		m_RATISTag = ratisTag;
		m_destination = destination;
		m_priority = priority;
		m_startTime = startTime;
		m_endTime = endTime;
		m_type = type;
        m_isIncoming = false;   // this is used by the ISCS to send RATIS to RXH
        m_operatorName = operatorName;
		m_dataIsSet = true;
	}
	
}