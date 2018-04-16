// IPDSDatagramServer.h: interface for the IPDSDatagramServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPDSDATAGRAMSERVER_H__2C1738B5_6D9C_4D91_83A3_816DBEDE1F4B__INCLUDED_)
#define AFX_IPDSDATAGRAMSERVER_H__2C1738B5_6D9C_4D91_83A3_816DBEDE1F4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSMethod.h"

namespace TA_IRS_App
{
	class IPDSDatagramServer  
	{
		public:
			virtual void Send (IPDSMethod * pdsMethod) = 0;
	};
}


#endif // !defined(AFX_IPDSDATAGRAMSERVER_H__2C1738B5_6D9C_4D91_83A3_816DBEDE1F4B__INCLUDED_)
