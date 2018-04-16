#ifndef CCTOCCSTATUSREQUEST_H_INCLUDED
#define CCTOCCSTATUSREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  */
#include "STISStatusRequest.h"

namespace TA_IRS_App
{

	class CCTLibraryReceivedNotifyRequest : public STISStatusRequest
	{
	public:
		CCTLibraryReceivedNotifyRequest(unsigned long libraryVersion);
		~CCTLibraryReceivedNotifyRequest();
		virtual std::vector<unsigned char> createProtocolMessage();
		virtual void log(bool sendSuccess);
		virtual void processResponse();
	private:
		unsigned long m_libraryVersion;
	};

	class CCTOCCStatusRequest : public STISStatusRequest
	{
	public:

		CCTOCCStatusRequest();

		~CCTOCCStatusRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

    	virtual void processResponse();

	private:

		static int m_previousRXHMessageAck;

	};

}

#endif
