// PDSDatagramClient.h: interface for the CPDSDatagramClient class.
//
//////////////////////////////////////////////////////////////////////


/* The source code in this file is the property of 
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   PDSDatagramClient
* @author:  Dennis Abriol
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* PDSDatagramClient act as the actual client connecting to the server
* This module hold the socket information of the client so that
* when the request is already process and ready to send
* this module will just use the said information to send the datagram
*/


#if !defined(AFX_PDSDATAGRAMCLIENT_H__4316312F_F5BD_4F7D_971D_FBFB1FDAA73F__INCLUDED_)
#define AFX_PDSDATAGRAMCLIENT_H__4316312F_F5BD_4F7D_971D_FBFB1FDAA73F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/ISocketObserver.h"
#include "core/synchronisation/src/Semaphore.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/UDPSocketManager.h"

struct sockaddr;

namespace TA_IRS_App
{	
	class CPDSDatagramClientException : public TA_Base_Core::TransactiveException
	{
		public:
			CPDSDatagramClientException( const std::string& errorText ) throw();
			virtual ~CPDSDatagramClientException() throw();
	};


	class CPDSDatagramClient :  public TA_Base_Core::Thread 
	{
		public:
			void sendData (const std::vector<unsigned char>& data);
			CPDSDatagramClient (::sockaddr * clientAddress, TA_IRS_App::CUDPSocketManager * udpManager);
			
			virtual ~CPDSDatagramClient();
			std::string getClientName ();

		protected:
			
			virtual void run ();
			virtual void terminate ();

		private:
			struct sockaddr					* m_sockInfo;
			TA_Base_Core::SocketData		  m_socketData;
			TA_Base_Core::Semaphore			  m_commonSemaphore;
			TA_IRS_App::CUDPSocketManager	* m_udpManager;
			bool							  m_bTerminate;

	};
}
#endif // !defined(AFX_PDSDATAGRAMCLIENT_H__4316312F_F5BD_4F7D_971D_FBFB1FDAA73F__INCLUDED_)
