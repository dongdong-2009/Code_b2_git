#ifndef TTISLIBRARYDOWNLOADREQUEST_H_INCLUDED
#define TTISLIBRARYDOWNLOADREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISLibraryDownloadRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/TTISMessage.h"


namespace TA_IRS_App
{

	class TTISLibraryDownloadRequest : public TTISMessage
	{
	public:

		TTISLibraryDownloadRequest();

		~TTISLibraryDownloadRequest();

		virtual void log(bool sendSuccess);

	private:

		int m_downloadCommand;

		std::string data;

	};

}

#endif
