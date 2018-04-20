#ifndef STISPREDEFINEDDISPLAYREQUEST_H_INCLUDED
#define STISPREDEFINEDDISPLAYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISPredefinedDisplayRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISMessage.h"

// IDL compiler-produced header file
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

namespace TA_IRS_App
{

	class STISPredefinedDisplayRequest : public STISMessage
	{
	public:

		STISPredefinedDisplayRequest();

		~STISPredefinedDisplayRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

        void setData( const TA_Base_Core::STISDestination& destination, TA_Base_Core::ELibrarySection librarySection, unsigned short libraryVersion, unsigned short messageTag, const std::string& startTime, 
			const std::string& endTime, int priority, const std::string& operatorName );


	private:

		// Where the message is to be displayed
        TA_Base_Core::STISDestination m_destination;

		// Index into the predefined message library
		int m_tag;

        TA_Base_Core::ELibrarySection m_librarySection;

		unsigned short m_libraryVersion;

		std::string m_startTime;

		std::string m_endTime;

		unsigned short m_priority;

		std::string m_submittedBy;


	};

}

#endif
