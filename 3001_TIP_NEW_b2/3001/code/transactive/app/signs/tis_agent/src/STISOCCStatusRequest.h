#ifndef STISOCCSTATUSREQUEST_H_INCLUDED
#define STISOCCSTATUSREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISOCCStatusRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#include "app/signs/tis_agent/src/STISStatusRequest.h"


namespace TA_IRS_App
{

	class STISOCCStatusRequest : public STISStatusRequest
	{
	public:

		STISOCCStatusRequest(bool isTimeScheduleValid = false);

		~STISOCCStatusRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

    	virtual void processResponse();

	private:

		static int m_previousRXHMessageAck;
		bool m_isTimeScheduleValid;

	};

}

#endif
