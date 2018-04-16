// UDPSocketManager.cpp: implementation of the CUDPSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#include "UDPSocketManager.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
	CUDPSocketManager::CUDPSocketManager(const std::string & strHost, 
										 const std::string & strService, 
										 IPDSClientConnectionObserver * clientObserver) :
										 m_bTerminate (false),
										 m_clientConnected (clientObserver),
										 UdpSocket (strHost,strService)
										 
	{
		start ();
	}
	
	CUDPSocketManager::CUDPSocketManager (const std::string & strHost, 
										  const std::string & strService,
										  IPDSClientConnectionObserver * clientObserver,
										  ::sockaddr_in * sockAdddress) :
										  UdpSocket (strHost,strService,sockAdddress),
										  m_clientConnected (clientObserver),
									      m_bTerminate (false)
	{	
		
		// start listening
		start ();
	}	

	CUDPSocketManager::~CUDPSocketManager()
	{
		terminateAndWait();
	}



	void CUDPSocketManager::run ()
	{
		while (!m_bTerminate)
		{
			getIncomingData();
		}
	}


	void CUDPSocketManager::terminate ()
	{
		m_bTerminate = true;
	}

	void CUDPSocketManager::getIncomingData()
	{
		char * pszData = new char [1024];
		unsigned int nRetLen;
		struct sockaddr * clientAddress = new struct sockaddr;

		TA_Base_Core::Thread::sleep(1);
		
		try
		{
			int nResult = read (pszData,&nRetLen,1024,clientAddress);
		
			if( 0 <= nResult )
			{	
				std::vector <unsigned char> vctrSocketdata;
				vctrSocketdata.resize(nResult);

				std::copy ((pszData), (pszData + nResult),vctrSocketdata.begin());
				
				m_clientConnected->connectionEstablish(clientAddress,vctrSocketdata);
			}
			else
			{	
				delete clientAddress;
			}
			
			delete [] pszData;
		}
		catch (...)
		{
			// error log
		}	
	}


	int CUDPSocketManager::writeData (const char  * pBuffer,const int pLength,sockaddr* addrClient)
	{
		int nDataWrite = write(pBuffer,pLength,addrClient);
		return nDataWrite;
	}
}