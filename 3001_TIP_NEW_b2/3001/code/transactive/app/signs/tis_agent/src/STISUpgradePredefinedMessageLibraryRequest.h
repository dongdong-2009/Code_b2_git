#ifndef STISUPGRADEPREDEFINEDMESSAGELIBRARYREQUEST_H_INCLUDED
#define STISUPGRADEPREDEFINEDMESSAGELIBRARYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/STISUpgradePredefinedMessageLibraryRequest.h $
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

	class STISUpgradePredefinedMessageLibraryRequest : public STISMessage
	{
	public:

		STISUpgradePredefinedMessageLibraryRequest();

		~STISUpgradePredefinedMessageLibraryRequest();

		void setData( unsigned long libraryVersion );

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

	private:

		unsigned long m_libraryVersion;

	};

}

#endif
