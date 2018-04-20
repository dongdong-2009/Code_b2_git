/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISUpgradePredefinedMessageLibraryRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISUpgradePredefinedMessageLibraryRequest.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/STISManager.h"

#include <iomanip>

namespace TA_IRS_App
{

	STISUpgradePredefinedMessageLibraryRequest::STISUpgradePredefinedMessageLibraryRequest()
	{
		m_messageIDString = "M70";
	}


	STISUpgradePredefinedMessageLibraryRequest::~STISUpgradePredefinedMessageLibraryRequest()
	{
	}


	std::vector<unsigned char> STISUpgradePredefinedMessageLibraryRequest::createProtocolMessage()
	{
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number

		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M70

		// Data length - always 3 bytes
		messageStream << "0003";
		
		// The library version we are upgrading to
		messageStream << std::setw(3) << std::setfill('0') << m_libraryVersion;
		
		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++ )
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
	}


	void STISUpgradePredefinedMessageLibraryRequest::setData( int libraryVersion )
	{
		m_libraryVersion = libraryVersion;
	}


	void STISUpgradePredefinedMessageLibraryRequest::log(bool sendSuccess)
	{
	}
	
}