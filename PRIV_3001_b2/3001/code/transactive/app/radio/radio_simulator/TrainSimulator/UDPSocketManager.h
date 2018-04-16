// UDPSocketManager.h: interface for the CUDPSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSOCKETMANAGER_H__D949226D_6086_44EA_9C72_E6727CB46D7F__INCLUDED_)
#define AFX_UDPSOCKETMANAGER_H__D949226D_6086_44EA_9C72_E6727CB46D7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "UdpSocket.h"
#include "IPDSClientConnectionObserver.h"

#include <string>
#include <vector>

// customize so that client and server apps can use the module

namespace TA_IRS_App
{
class CUDPSocketManager : public TA_Base_Core::UdpSocket, public TA_Base_Core::Thread
{
public:
	CUDPSocketManager(const std::string & strHost, const std::string & strService, IPDSClientConnectionObserver * clientObserver);
	CUDPSocketManager(const std::string & strHost, const std::string & strService, IPDSClientConnectionObserver * clientObserver, ::sockaddr_in * sockAdddress);
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
