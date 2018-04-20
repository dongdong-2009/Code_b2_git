#ifndef TTISCLEARREQUEST_H_INCLUDED
#define TTISCLEARREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISClearRequest.h $
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

	class TTISClearRequest : public TTISMessage
	{
	public:

		TTISClearRequest();

		~TTISClearRequest();

		virtual std::vector<char> createProtocolMessage();

		virtual void log(bool sendSuccess);

	private:

		int m_PIDAddress;

        TA_Base_Core::ETTISClearType m_type;

	};

}

#endif
