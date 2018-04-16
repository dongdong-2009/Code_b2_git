#ifndef STISOCCSTATUSREQUEST_H_INCLUDED
#define STISOCCSTATUSREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISOCCStatusRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISStatusRequest.h"


namespace TA_IRS_App
{

	class STISOCCStatusRequest : public STISStatusRequest
	{
	public:

		STISOCCStatusRequest();

		~STISOCCStatusRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

    	virtual void processResponse();

	private:

		static int m_previousRXHMessageAck;

	};

}

#endif
