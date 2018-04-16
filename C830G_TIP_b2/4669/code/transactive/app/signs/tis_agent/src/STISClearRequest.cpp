/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISClearRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISClearRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"

#include <iomanip>

#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"

namespace TA_IRS_App
{


	STISClearRequest::STISClearRequest()
        : STISMessage(), 
          m_destination(""),
          m_tag("")
	{
		m_messageIDString = "M20";
	}


	STISClearRequest::~STISClearRequest()
	{
	}


	std::vector<unsigned char> STISClearRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M20

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// DESTINATION (MSGDEST) 'FIELDS'

        // Message destination (MSGDEST)
        dataStream << m_destination;

		// Message Tag - this is actually the timestamp for M20 messages
		// When this message is originated at the STIS Manager,
		// the timestamp is unknown and is filled with 12 space characters
        dataStream << std::setw(12) << std::setfill(' ') << std::left << m_tag << std::right;

		// Priority clearing info

		// Priority 1
		bool priorityRange[8];

		// set all priorities to 'Do Not Clear' - i.e '0'
        int i = 0;
		for(i=0;i<8;i++)
		{
			priorityRange[i] = false;
		}

		for(i = (m_lowerPriority-1) ; i < m_upperPriority ; i++)
		{
			priorityRange[i] = true;
		}

        // only clear all (1 to 8) and clear RATIS (6 to 6) should set priority 6
        if ( ((m_lowerPriority == 1) && (m_upperPriority == 8)) ||
             ((m_lowerPriority == 6) && (m_upperPriority == 6)) )
        {
            priorityRange[5] = true;
        }
        else
        {
            priorityRange[5] = false;
        }

		for(i=0;i<8;i++)
		{
			if(priorityRange[i] == true)
			{
				dataStream << "1";
			}
			else
			{
				dataStream << "0";
			}
		}

		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();
        messageStream << std::setw(4) << std::setfill('0') << dataSize;
		
		// Add the data stream to the actual message stream after calculating it
		messageStream << dataString;

		// No train data

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int z=0;z < messageString.size() ; z++)
		{
			messageVector.push_back( messageString[z] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISClearRequest::log(bool sendSuccess)
	{
		TA_ASSERT(m_dataIsSet, "Log() called for Predefined display request that has no data");

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			" STISClearRequest::log() is called");

        TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();

        TA_IRS_Core::IDisplayRequest::EMessageType type;
        switch (m_type)
        {
        case TA_Base_Core::RATIS_IN_CLEAR:
            type = TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR;
            break;
        case TA_Base_Core::RATIS_OUT_CLEAR:
            type = TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR;
     	    break;
        case TA_Base_Core::RATIS_IN_NEW:
            TA_ASSERT(false, "Attempting to log a  no RATIS approved");
            break;
        case TA_Base_Core::RATIS_OUT_NEW:
            TA_ASSERT(false, "Attempting to log a no RATIS approved");
            break;
        case TA_Base_Core::RATIS_IN_UPDATE:
            TA_ASSERT(false, "Attempting to log a no RATIS approved");
            break;
        case TA_Base_Core::RATIS_OUT_UPDATE:
            TA_ASSERT(false, "Attempting to log a no RATIS approved");
            break;
/*        case TA_IRS_Core::RATIS_APPROVED:
            TA_ASSERT(false, "Attempting to log a RATIS approved");
            break;
*/
        }
        displayRequest->setMessageType( type );
		//defectID(17927) AVteam++
		//displayRequest->setAdhocMessageContent("Ratis Clear Message");
		displayRequest->setAdhocMessageContent( " " );
		//defectID(17927) ++AVteam
		displayRequest->setRatisTag( m_tag );
		displayRequest->setRatisDestination( m_destination );
		displayRequest->setRatisOverridable( m_overridable );
		displayRequest->setStartTime( "" );
		displayRequest->setEndTime( "" );
		displayRequest->setTimestamp( m_timestamp );
		//hongran++ TD17502
		displayRequest->setIfSendSuccess(sendSuccess ? TA_Base_Core::Pass : TA_Base_Core::Fail);
		//++hongran TD17502
		
        displayRequest->setDestinationSystem( "STIS" );

        // operator name for tis log viewer
        displayRequest->setSubmittedBy( m_operatorName );

		// write this display request to the database
		displayRequest->applyChanges();

		delete displayRequest;
		displayRequest = NULL;
	}


    void STISClearRequest::setData( const TA_Base_Core::STISDestination& destination, const unsigned short lowerPriority, 
									const unsigned short upperPriority ,const TA_Base_Core::ERATISMessageType type, const bool overridable, const std::string& operatorName)
	{
		m_destination = getMessageDestinationString(destination);
		m_lowerPriority = lowerPriority;
		m_upperPriority = upperPriority;
		m_type = type;
		m_overridable = overridable;
		m_operatorName = operatorName;
		m_dataIsSet = true;
	}


    void STISClearRequest::setData( const std::string& destination, const unsigned short priority, const std::string& messageTag, const TA_Base_Core::ERATISMessageType type, const bool overridable, const std::string& operatorName)
	{
        m_tag = messageTag;
		m_destination = destination;
		m_lowerPriority = priority;
		m_upperPriority = priority;
		m_type = type;
		m_overridable = overridable;
		m_operatorName = operatorName;
		m_dataIsSet = true;
	}
	
}