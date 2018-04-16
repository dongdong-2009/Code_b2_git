#ifndef STISSTATIONSTATUSREQUEST_H_INCLUDED
#define STISSTATIONSTATUSREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISStationStatusRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/19 12:35:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include "app/signs/tis_agent/src/STISStatusRequest.h"


namespace TA_IRS_App
{

	class STISStationStatusRequest : public STISStatusRequest
	{
	public:

		STISStationStatusRequest(const std::string& location);

		~STISStationStatusRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

		virtual void processResponse();
	};

}

#endif
