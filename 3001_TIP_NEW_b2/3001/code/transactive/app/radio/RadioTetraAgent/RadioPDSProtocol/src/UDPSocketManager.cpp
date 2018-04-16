// UDPSocketManager.cpp: implementation of the CUDPSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/UDPSocketManager.h"
#include <algorithm>
#include "core/utilities/src/DebugUtil.h"

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
			FUNCTION_ENTRY ("CUDPSocketManager");

			LOG_GENERIC (SourceInfo,
						 TA_Base_Core::DebugUtil::DebugInfo,
						 "\t\t\n== UDPSocketManager =="
						 /*"\t\t\nHost   : %s" */
						 "\t\t\nPort   : %s",
						 /*strHost.c_str(),*/strService.c_str());
			start ();
			FUNCTION_EXIT;
	}

	CUDPSocketManager::~CUDPSocketManager()
	{
			FUNCTION_ENTRY ("~CUDPSocketManager");
			terminateAndWait();
			FUNCTION_EXIT;
	}



	void CUDPSocketManager::run ()
	{
		FUNCTION_ENTRY ("run");
		while (!m_bTerminate)
		{
			getIncomingData();
		}
		FUNCTION_EXIT;
	}


	void CUDPSocketManager::terminate ()
	{
		FUNCTION_ENTRY ("terminate");
		m_bTerminate = true;
		FUNCTION_EXIT;
	}
	void CUDPSocketManager::getIncomingData()
	{
		FUNCTION_ENTRY ("getIncomingData");
		char * pszData = new char [1024];
		memset(pszData, 0, 1024);
		unsigned int nRetLen;
		
		struct sockaddr_in * ptrClientAddress = new struct sockaddr_in;
		struct sockaddr_in clientAddress;
		
		TA_Base_Core::Thread::sleep(1);
		
		try
		{
			 int nResult = read (pszData,&nRetLen,1024,(struct sockaddr *)& clientAddress);
			
			if( 0 <= nResult )
			{
				
				std::vector <unsigned char> vctrSocketdata;
				vctrSocketdata.resize(nResult);

				std::copy ((pszData), (pszData + nResult),vctrSocketdata.begin());

				LOG_GENERIC(SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Read File Content, Family Protocol : %d",clientAddress.sin_family );
				
				ptrClientAddress->sin_addr = clientAddress.sin_addr;
				ptrClientAddress->sin_family = clientAddress.sin_family;
				ptrClientAddress->sin_port = clientAddress.sin_port;

				for (int nCounter = 0; nCounter < 7; nCounter++)
				{
					* (ptrClientAddress->sin_zero + nCounter) = clientAddress.sin_zero[nCounter];
				}


				m_clientConnected->connectionEstablish(getSocketId(),(struct sockaddr *)ptrClientAddress,vctrSocketdata);
			}
			else
			{	
					delete ptrClientAddress;
					ptrClientAddress = NULL;
			}
			
			delete [] pszData;
			pszData = NULL;
		}
		catch (...)
		{
			// error log
		}	
		FUNCTION_EXIT;
	}


	int CUDPSocketManager::writeData (const char  * pBuffer,const int pLength,sockaddr* addrClient)
	{
		FUNCTION_ENTRY ("writeData");
		int nDataWrite = write(pBuffer,pLength,addrClient);
		FUNCTION_EXIT;
		return nDataWrite;
			
	}
}