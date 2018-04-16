// IPDSClientConnectionObserver.h: interface for the IPDSClientConnectionObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPDSCLIENTCONNECTIONOBSERVER_H__5D9ADCA8_C28E_4963_891D_00C11609AF37__INCLUDED_)
#define AFX_IPDSCLIENTCONNECTIONOBSERVER_H__5D9ADCA8_C28E_4963_891D_00C11609AF37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#if !defined( WIN32 )
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
//TD 14774
//zhou yuan++
#define WIN32_LEAN_AND_MEAN 
//++zhou yuan
#include <winsock2.h>
#endif // !WIN32

namespace TA_IRS_App
{
	class IPDSClientConnectionObserver  
	{
	public:
		virtual void connectionEstablish (const int & socketID, ::sockaddr* clientAddress, std::vector<unsigned char> socketData) = 0;
	};
}

#endif // !defined(AFX_IPDSCLIENTCONNECTIONOBSERVER_H__5D9ADCA8_C28E_4963_891D_00C11609AF37__INCLUDED_)
