// IRadioPDSSender.h: interface for the IRadioPDSSender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IRADIOPDSSENDER_H__7B3291E6_25CF_4252_94BF_3623A47568E7__INCLUDED_)
#define AFX_IRADIOPDSSENDER_H__7B3291E6_25CF_4252_94BF_3623A47568E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtdFrame.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/ILibDownloadStatus.h"

typedef TA_IRS_App::TtisTypes::TtisFrameList FrameList;

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodSendData.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodCancel.h"

namespace TA_IRS_App
{
	class IRadioPDSSender  
	{
		public:
			virtual void Send (IPDSMethod * radioMethod, const long & lFrameSize, bool bSendMethod = true) = 0;
			virtual void stopSending() = 0;
	};

}

#endif // !defined(AFX_IRADIOPDSSENDER_H__7B3291E6_25CF_4252_94BF_3623A47568E7__INCLUDED_)
