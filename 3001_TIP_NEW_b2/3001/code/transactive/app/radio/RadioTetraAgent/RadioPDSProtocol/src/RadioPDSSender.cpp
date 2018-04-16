// RadioPDSSender.cpp: implementation of the CRadioPDSSender class.
//
//////////////////////////////////////////////////////////////////////


#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtisTypes.h"

// testing only
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/PDSMessageCreator.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSSender.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
	
	CRadioPDSSenderException::CRadioPDSSenderException(const std::string &errorText) throw ()
		: TA_Base_Core::TransactiveException(errorText)
	{
		FUNCTION_ENTRY( "CRadioPDSSenderException" );		
		FUNCTION_EXIT;
	}
	
	CRadioPDSSenderException::~CRadioPDSSenderException() throw()
	{
		FUNCTION_ENTRY( "~CRadioPDSSenderException" );
		FUNCTION_EXIT;
	}
	
	CRadioPDSSender::CRadioPDSSender(IPDSDatagramServer * pdsDatagramServer) 
		: m_bTerminate(false),
		  m_bSendMethod(true),
		  m_libDownloadStat (NULL),
		  m_pdsMethodGeneric (NULL),
		  m_nFrameSize(0),
		  m_pdsDatagramServer (pdsDatagramServer) 

	{
	}

	CRadioPDSSender::~CRadioPDSSender()
	{
		m_bTerminate = true;
		terminateAndWait();
	}

	void CRadioPDSSender::Send(IPDSMethod * radioMethod,const long & lFrameSize,bool bSendMethod)
	{	
		
		if (0 == m_nFrameSize && !bSendMethod)
		{
			m_libDownloadStat->notifyDownloadStatus(TA_IRS_App::TtisTypes::PDSLibDownloadCancelled);
		}
	
		terminateAndWait();
		m_bTerminate = false;
		m_pdsMethodGeneric = radioMethod;
		m_nFrameSize = lFrameSize;
		
		m_bSendMethod = bSendMethod;
		
		start ();
	}

	void CRadioPDSSender::run()
	{
		FUNCTION_ENTRY("run()");

		SendFrameData ();
		
		FUNCTION_EXIT;
	}

	void CRadioPDSSender::terminate ()
	{
		// cancelled or die
		m_bTerminate = true;
	}
	
	void CRadioPDSSender::attach (ILibDownloadStatus * libDownloadObserver)
	{
		FUNCTION_ENTRY ("attachObserver ()");
		
		if (!libDownloadObserver)
		{
			TA_THROW(CRadioPDSSenderException("Observer has not been attached, cannot proceed on sending"));
		}

		m_libDownloadStat = libDownloadObserver;

		FUNCTION_EXIT;
	}

	void CRadioPDSSender::SendFrameData ()
	{
		// send this one to class that handle sockets;
		// notify test dialog for notification
		
		FUNCTION_ENTRY ("SendFrameData");

		long lCounter = 0;
		
		while (m_nFrameSize != lCounter)
		{
			if (m_bTerminate)
			{
				LOG_GENERIC (SourceInfo,
							TA_Base_Core::DebugUtil::DebugInfo,
							"Sending terminated due to cancellation or disconnection"
							);
				
				m_libDownloadStat->notifyDownloadStatus(TA_IRS_App::TtisTypes::PDSLibDownloadCancelled);
				break;	
			}
			
			if (m_bSendMethod)
			{
				CRadioPDSMethodSendData * methodSend = (CRadioPDSMethodSendData *)m_pdsMethodGeneric;
				methodSend->setCurrentFrameIndex(lCounter);
			}
			else
			{
				CRadioPDSMethodCancel * methodCancel = (CRadioPDSMethodCancel *)m_pdsMethodGeneric;
				methodCancel->setCurrentFrameIndex();
			}

			// send data here
			m_pdsDatagramServer->Send(m_pdsMethodGeneric);
			// send data here
			
			TA_Base_Core::Thread::sleep(500);
			lCounter++;
		}
		
		m_nFrameSize = 0;
		if (!m_bTerminate)
		{
			m_libDownloadStat->notifyDownloadStatus(TA_IRS_App::TtisTypes::PDSLibDownloadComplete);
		}

		FUNCTION_EXIT;
	}

	void CRadioPDSSender::stopSending ()
	{
		terminateAndWait();
	}

}