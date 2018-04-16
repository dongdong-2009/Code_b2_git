#ifndef STISSTATIONSTATUSREQUEST_H_INCLUDED
#define STISSTATIONSTATUSREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISStationStatusRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
