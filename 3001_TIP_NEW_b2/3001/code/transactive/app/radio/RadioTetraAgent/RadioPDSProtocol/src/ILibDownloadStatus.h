// ILibDownloadStatus.h: interface for the ILibDownloadStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILIBDOWNLOADSTATUS_H__A6677563_CA36_4C5E_BEEC_65F71ABA4422__INCLUDED_)
#define AFX_ILIBDOWNLOADSTATUS_H__A6677563_CA36_4C5E_BEEC_65F71ABA4422__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_IRS_App
{
	class ILibDownloadStatus  
	{
		public:
			virtual void notifyDownloadStatus (const long &lDownloadStatus) = 0;
	};
}


#endif // !defined(AFX_ILIBDOWNLOADSTATUS_H__A6677563_CA36_4C5E_BEEC_65F71ABA4422__INCLUDED_)
