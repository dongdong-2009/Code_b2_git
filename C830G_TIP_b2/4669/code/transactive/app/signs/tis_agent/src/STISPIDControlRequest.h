#ifndef STISPIDCONTROLREQUEST_H_INCLUDED
#define STISPIDCONTROLREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISPIDControlRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISMessage.h"


namespace TA_IRS_App
{

	class STISPIDControlRequest : public STISMessage
	{
	public:

		STISPIDControlRequest();

		~STISPIDControlRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

		void setData( const std::string& destination, TA_Base_Core::EPIDControl controlCommand );

	private:

		TA_Base_Core::EPIDControl m_controlCommand;
		std::string m_destination;

	};

}

#endif
