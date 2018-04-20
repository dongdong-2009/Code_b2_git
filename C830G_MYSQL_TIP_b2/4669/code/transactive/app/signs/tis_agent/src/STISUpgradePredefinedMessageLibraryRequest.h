#ifndef STISUPGRADEPREDEFINEDMESSAGELIBRARYREQUEST_H_INCLUDED
#define STISUPGRADEPREDEFINEDMESSAGELIBRARYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISUpgradePredefinedMessageLibraryRequest.h $
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

	class STISUpgradePredefinedMessageLibraryRequest : public STISMessage
	{
	public:

		STISUpgradePredefinedMessageLibraryRequest();

		~STISUpgradePredefinedMessageLibraryRequest();

		void setData( int libraryVersion );

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

	private:

		int m_libraryVersion;

	};

}

#endif
