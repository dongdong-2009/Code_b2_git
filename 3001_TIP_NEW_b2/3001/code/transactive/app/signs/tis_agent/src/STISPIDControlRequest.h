#ifndef STISPIDCONTROLREQUEST_H_INCLUDED
#define STISPIDCONTROLREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISPIDControlRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
