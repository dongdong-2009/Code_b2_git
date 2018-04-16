#ifndef STISCLEARREQUEST_H_INCLUDED
#define STISCLEARREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISClearRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/06 19:23:10 $
  * Last modified by:  $Author: huang.wenbo $
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

        void setData( const TA_Base_Core::STISDestination& destination,
			          const unsigned short highPriority,
					  const unsigned short lowPriority );

        void setData( const std::string& destination,
			          const unsigned short priority,
                      const std::string& messageTag );
	private:

		// PID(s) the message is to be cleared from
        std::string m_destination;

		// Identifier of the previously sent message
		std::string m_tag;
		unsigned short m_highPriority;

		unsigned short m_lowPriority;
	};

}

#endif
