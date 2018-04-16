#ifndef TTISPREDEFINEDDISPLAYREQUEST_H_INCLUDED
#define TTISPREDEFINEDDISPLAYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISPredefinedDisplayRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/TTISMessage.h"

// IDL compiler-produced header file
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

namespace TA_IRS_App
{

	class TTISPredefinedDisplayRequest : public TTISMessage
	{
	public:

		TTISPredefinedDisplayRequest();		
		
		~TTISPredefinedDisplayRequest();
		
		virtual void log(bool sendSuccess);
		
        virtual void setData( CORBA::UShort trainNumber, const TA_Base_Core::TrainPIDList& pidList, TA_Base_Core::ELibrarySection librarySection,
			CORBA::UShort libraryVersion, CORBA::UShort messageID, CORBA::UShort priority, const char* startTime, const char* endTime, const std::string& operatorName);	
		
		
	private:
		
		int m_libraryVersion;
		
		TA_Base_Core::ELibrarySection m_librarySection;
		
		int m_messageTag;
		
		int m_priority;
		
		std::string m_startTime;
		
		std::string m_endTime;
		
		// List of PIDs this display request is intended for.
        std::vector<TA_Base_Core::EPIDSelection> m_pidSelectionList;
		
		std::string m_submittedBy;

	};

}

#endif
