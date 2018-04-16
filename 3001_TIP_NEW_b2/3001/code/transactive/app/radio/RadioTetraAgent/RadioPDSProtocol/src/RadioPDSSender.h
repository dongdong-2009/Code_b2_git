// RadioPDSSender.h: interface for the CRadioPDSSender class.
//
//////////////////////////////////////////////////////////////////////

/* The source code in this file is the property of
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   RadioPDSSender.h
* @author:  Dennis Abriol
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* This class handles the sending of the raw data coming from
* RadioPDSManager (arrange by RadioPDSHandler of course) going thru
* the UDPDatagramServer
*/

#if !defined(AFX_RADIOPDSSENDER_H__A33FC7E0_D33C_42EB_824A_3E601AA351B2__INCLUDED_)
#define AFX_RADIOPDSSENDER_H__A33FC7E0_D33C_42EB_824A_3E601AA351B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IRadioPDSSender.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/ILibDownloadStatus.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodSendData.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodCancel.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSDatagramServer.h"

typedef TA_IRS_App::TtisTypes::TtisFrameList FrameList;

namespace TA_IRS_App
{
	
	class CRadioPDSSenderException : public TA_Base_Core::TransactiveException
	{
		public:
			CRadioPDSSenderException( const std::string& errorText ) throw();
			virtual ~CRadioPDSSenderException() throw();
	};

	class CRadioPDSSender : public IRadioPDSSender , public TA_Base_Core::Thread
	{
		public:
			CRadioPDSSender(IPDSDatagramServer * pdsDatagramServer);
			virtual ~CRadioPDSSender();
			
			virtual void Send (IPDSMethod * radioMethod, const long & lFrameSize, bool bSendMethod = true);

			virtual void stopSending ();

			void attach (ILibDownloadStatus * libDownloadObserver);
		
		protected:
			virtual void run ();
			virtual void terminate ();

		private:

			void SendFrameData ();
			
			bool m_bTerminate;
			bool m_bSendMethod;
			FrameList m_frameLibData;
			ILibDownloadStatus * m_libDownloadStat;
			IPDSMethod		   * m_pdsMethodGeneric;

			int	m_nFrameSize;
			IPDSDatagramServer * m_pdsDatagramServer;
	};
}

#endif // !defined(AFX_RADIOPDSSENDER_H__A33FC7E0_D33C_42EB_824A_3E601AA351B2__INCLUDED_)
