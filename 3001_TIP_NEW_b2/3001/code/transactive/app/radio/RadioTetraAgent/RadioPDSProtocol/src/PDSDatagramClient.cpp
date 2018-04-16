// PDSDatagramClient.cpp: implementation of the CPDSDatagramClient class.
//
//////////////////////////////////////////////////////////////////////
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/PDSDatagramClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
	
	CPDSDatagramClientException::CPDSDatagramClientException(const std::string &errorText) throw ()
	: TA_Base_Core::TransactiveException(errorText)
	{
		FUNCTION_ENTRY( "CPDSDatagramClientException" );
		FUNCTION_EXIT;
	}

	CPDSDatagramClientException::~CPDSDatagramClientException() throw()
	{
		FUNCTION_ENTRY( "~CPDSDatagramClientException" );
		FUNCTION_EXIT;
	}


	CPDSDatagramClient::CPDSDatagramClient (::sockaddr * clientAddress, 
											TA_IRS_App::CUDPSocketManager * udpManager) : 
											m_sockInfo (clientAddress),
											m_udpManager (udpManager),
											m_bTerminate (false)
	{
		FUNCTION_ENTRY ("CPDSDatagramClient");
		start ();
		FUNCTION_EXIT;
	}

	CPDSDatagramClient::~CPDSDatagramClient()
	{
		FUNCTION_ENTRY ("~CPDSDatagramClient");
		if (m_sockInfo)
		{
			delete m_sockInfo;
			m_sockInfo = NULL;
		}
	
		terminate();
		m_commonSemaphore.post ();
		terminateAndWait();
		FUNCTION_EXIT;
		
	}

	void CPDSDatagramClient::sendData (const std::vector<unsigned char>& data)
	{

		FUNCTION_ENTRY ("sendData");
	
		m_socketData.erase(m_socketData.begin(),m_socketData.end ());
		m_socketData.append(data.begin(),data.end());
		m_commonSemaphore.post ();

		FUNCTION_EXIT;
	}

	void CPDSDatagramClient::run ()
	{
		
		FUNCTION_ENTRY ("RUN");
		
		//while (!m_bTerminate)
		while (true)
		{
			TA_Base_Core::Thread::sleep(1);
			
			m_commonSemaphore.wait();
			
			if (m_bTerminate)
				break;

			int nDataSize = m_socketData.size();
			
			char * pszData = new char [nDataSize] ;
			
			memset (pszData,0,nDataSize);
			memcpy (pszData,m_socketData.c_str(),nDataSize);
			
			
			if (m_socketData.empty())
			{
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Run () - Data Empty - Client Name : %s", getClientName().c_str());
				continue;
			}
			
			int nSendResult = m_udpManager->writeData(pszData,nDataSize,m_sockInfo);

			if (nSendResult == -1)
			{
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Send Error : %d",nSendResult);
			}
		
			if (pszData)
			{
				delete [] pszData;
				pszData = NULL;
			}
		}

		FUNCTION_EXIT;
	}

	void CPDSDatagramClient::terminate ()
	{
		FUNCTION_ENTRY ("terminate");
		m_bTerminate = true;
		FUNCTION_EXIT;
	}

	std::string CPDSDatagramClient::getClientName ()
	{
		FUNCTION_ENTRY ("getClientName");
		struct sockaddr_in * clientInAddr = (sockaddr_in *)m_sockInfo;
		std::string ip_address = inet_ntoa(clientInAddr->sin_addr);
		FUNCTION_EXIT;
		return ip_address;
	}
}