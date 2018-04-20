/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISConfigurationDataRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISConfigurationDataRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"

namespace TA_IRS_App
{

	STISConfigurationDataRequest::STISConfigurationDataRequest()
        : STISMessage()
	{
		m_messageIDString = "M00";
	}


	STISConfigurationDataRequest::~STISConfigurationDataRequest()
	{
	}


	std::vector<unsigned char> STISConfigurationDataRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M00 cow

		// Now the data length - in this case it will always be 3 bytes
		messageStream << "0003";

		// Now the actual data
		// This message only contains the 3-character station ID
		messageStream << STISManager::getInstance()->getLocationName();

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISConfigurationDataRequest::log(bool sendSuccess)
	{
	}


	void STISConfigurationDataRequest::processResponse()
	{
	}
	
}