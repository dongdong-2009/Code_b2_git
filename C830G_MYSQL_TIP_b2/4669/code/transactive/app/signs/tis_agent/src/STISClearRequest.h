#ifndef STISCLEARREQUEST_H_INCLUDED
#define STISCLEARREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISClearRequest.h $
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

	class STISClearRequest : public STISMessage
	{
	public:

		STISClearRequest();

		~STISClearRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

        void setData( const TA_Base_Core::STISDestination& destination, const unsigned short lowerPriority, 
									const unsigned short upperPriority, const TA_Base_Core::ERATISMessageType type, const bool m_overridable, const std::string& operatorName );

        void setData( const std::string& destination, const unsigned short lowerPriority, 
            const std::string& messageTag, const TA_Base_Core::ERATISMessageType type, const bool m_overridable, const std::string& operatorName );

	private:

		// PID(s) the message is to be cleared from
        std::string m_destination;

		// Identifier of the previously sent message
		std::string m_tag;

		TA_Base_Core::ERATISMessageType m_type;
		
		bool m_overridable;

		unsigned short m_upperPriority;

		unsigned short m_lowerPriority;

        std::string m_operatorName;

	};

}

#endif
