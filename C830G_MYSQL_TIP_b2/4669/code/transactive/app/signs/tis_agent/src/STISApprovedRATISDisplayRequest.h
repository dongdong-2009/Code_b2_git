#ifndef STISAPPROVEDRATISDISPLAYREQUEST_H_INCLUDED
#define STISAPPROVEDRATISDISPLAYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISApprovedRATISDisplayRequest.h $
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

	class STISApprovedRATISDisplayRequest : public STISMessage
	{
	public:

		STISApprovedRATISDisplayRequest();

		~STISApprovedRATISDisplayRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

	    void setData( 
            const std::string messageContent, 
            const unsigned short priority, 
            const std::string ratisTag, 
            const std::string destination, 
		    const std::string startTime, 
            const std::string endTime, 
            const TA_Base_Core::ERATISMessageType type,
            const bool overridable,
		    const bool requiresApproval,
            const std::string& operatorName );

	    void setData( 
            const std::string messageContent, 
            const unsigned short priority, 
            const std::string ratisTag, 
            const std::string destination, 
		    const std::string startTime, 
            const std::string endTime,
            const TA_Base_Core::ERATISMessageType type,
            const std::string& operatorName );


	private:

		std::string m_messageText;

		// Destination defined in RATIS protocol - if this is an outgoing RATIS message
		std::string m_destination;

		std::string m_RATISTag;

		std::string m_startTime;

		std::string m_endTime;

		unsigned short m_priority;

		bool m_requiresApproval;

		bool m_overridable;

		bool m_clearMessage;

		int m_messageLength;

		TA_Base_Core::ERATISMessageType m_type;

        bool m_isIncoming;

        std::string m_operatorName;
	};

}

#endif
