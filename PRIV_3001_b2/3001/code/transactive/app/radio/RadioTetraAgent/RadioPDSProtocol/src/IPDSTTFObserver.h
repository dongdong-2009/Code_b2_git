// IPDSTTFObserver.h: interface for the IPDSTTFObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPDSTTFOBSERVER_H__A4FCB49A_FF1C_4BB1_9023_14B9EA4C3143__INCLUDED_)
#define AFX_IPDSTTFOBSERVER_H__A4FCB49A_FF1C_4BB1_9023_14B9EA4C3143__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

namespace TA_IRS_App
{
	class IPDSTTFObserver  
	{
		public:
			virtual void NotifyTTSStatus (const long & lTrainID, bool success, const std::string& failReason) = 0;
			virtual void NotifyEndOfTtisDownloadStatus( const unsigned long& lTrainID, unsigned short& status ) = 0;
	};
}

#endif // !defined(AFX_IPDSTTFOBSERVER_H__A4FCB49A_FF1C_4BB1_9023_14B9EA4C3143__INCLUDED_)
