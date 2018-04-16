// UDPSocketManager.h: interface for the CUDPSocketManager class.
//
//////////////////////////////////////////////////////////////////////

/* The source code in this file is the property of an
* organization and is not for redistribution
* in any form.
* Source:   $Source$
* @author:  Dennis Abriol
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* Udp Socket Manager module handles the setting up of UdpSockt by calling low level
* UdpSocket module. This module also manage the or store server instance for this module
* to callback if there is\are clients requesting to recieve frames.
*/


#if !defined(AFX_UDPSOCKETMANAGER_H__D949226D_6086_44EA_9C72_E6727CB46D7F__INCLUDED_)
#define AFX_UDPSOCKETMANAGER_H__D949226D_6086_44EA_9C72_E6727CB46D7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/UdpSocket.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSClientConnectionObserver.h"

#include <string>
#include <vector>

namespace TA_IRS_App
{

	class CUDPSocketManager  : public TA_Base_Core::UdpSocket, public TA_Base_Core::Thread
	{
		public:
			CUDPSocketManager(const std::string & strHost, const std::string & strService, IPDSClientConnectionObserver * clientObserver);
			virtual ~CUDPSocketManager();
			int writeData (const char  * pBuffer,const int pLength,sockaddr* addrClient);
		protected:
			virtual void run ();
			virtual void terminate (); 
			
		private:
			void getIncomingData();
			
			bool m_bTerminate;
			IPDSClientConnectionObserver * m_clientConnected;
	};
}

#endif // !defined(AFX_UDPSOCKETMANAGER_H__D949226D_6086_44EA_9C72_E6727CB46D7F__INCLUDED_)
