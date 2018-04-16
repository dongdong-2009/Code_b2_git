// RadioPDSHandler.cpp: implementation of the CRadioPDSHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSHandler.h"
#include <algorithm>

// testing only
#include "PDSMessageCreator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
	
	const unsigned long CRadioPDSHandler::TIS7TIMER = 1;
	const unsigned long CRadioPDSHandler::TIS5TIMER = 2;
	const unsigned long CRadioPDSHandler::TIMEREXPIRED = 0;
	const unsigned long CRadioPDSHandler::TIMERSTOPPED = 1;
	
	CCRadioPDSHandlerException::CCRadioPDSHandlerException(const std::string &errorText) throw ()
		: TA_Base_Core::TransactiveException(errorText)
	{
		FUNCTION_ENTRY( "CCRadioPDSHandlerException" );
		FUNCTION_EXIT;
	}
	
	CCRadioPDSHandlerException::~CCRadioPDSHandlerException() throw()
	{
		FUNCTION_ENTRY( "~CCRadioPDSHandlerException" );
		FUNCTION_EXIT;
	}
	

	CRadioPDSHandler::~CRadioPDSHandler()
	{
		FUNCTION_ENTRY("~CRadioPDSHandler");
		
		terminateTimer();
		terminateAndWait();

		m_vctrDataLib.clear();
		m_frameListBuff.clear();
		m_bCancelled = false;
		

		m_bTimerExpireToFailed = false;

		if (m_methodSend)
		{
			delete m_methodSend;
			m_methodSend = NULL;
		}
		
		if (m_methodCancel)	
		{
			delete m_methodCancel;
			m_methodCancel = NULL;
		}
		
		m_bSendLibrary = false;
		m_bIsSending = false;
		FUNCTION_EXIT;
	}


	CRadioPDSHandler::CRadioPDSHandler (IEventRecordRemover * pdsEventRemover,
										IRadioPDSSender * pdsSender, 
										IPDSTTFObserver * pdsObserver, 
										std::vector <unsigned char> vctrLibData, 
										const long & lTrainID,
										const TA_IRS_App::CommonTypes::SRadioPDSTimer& radioPDSTimer )
										: 
										m_bTimerStat (false),
										m_bCancelled(false),
										m_bTimerExpireToFailed (false),
										m_bSendLibrary (false),
										m_bIsSending(false),
										m_lTimerOption (0),
										m_radioPDSSender(pdsSender),
										m_pdsObserver(pdsObserver),
										m_lTrainID(lTrainID),
										m_methodSend (0),
										m_methodCancel(0),
										m_radioPDSTimer(radioPDSTimer),
										m_pdsEventRemover (pdsEventRemover),
										m_semaTimer (0)
										
	{
		
		FUNCTION_ENTRY("CRadioPDSHandler");
		
		
		if (!pdsSender || !pdsObserver || !pdsEventRemover)
		{
			TA_THROW(CCRadioPDSHandlerException("Invalid pointers has been provided, cannot proceed on Handler Instantiation"));
			//return; unpassble code
		}
		
		m_vctrDataLib.erase (m_vctrDataLib.begin(),m_vctrDataLib.end ());
		m_vctrDataLib.resize(vctrLibData.size ());

		std::copy (vctrLibData.begin(),vctrLibData.end(),m_vctrDataLib.begin());
		
		// data is delivered start timer for 
		// TIMS sending of command
		startTimer (TIS5TIMER);

		FUNCTION_EXIT;
	}

	void CRadioPDSHandler::run() 
	{
		
		FUNCTION_ENTRY (  "run () " );

		// timer will pass here on any post or timer expire
		long lTimerStatus = 0;

		// testing purpose
		if (!m_bTimerExpireToFailed)
		{
			lTimerStatus = TA_IRS_App::TtisTypes::PDSLibDownloadTimerEventWait;
		}
		else
		{
			lTimerStatus = TA_IRS_App::TtisTypes::PDSLibDownloadTimerSuccessWait;
		}
		
		LOG_GENERIC (
						SourceInfo,
						TA_Base_Core::DebugUtil::DebugInfo,
						"%s",
						CPDSMessageCreator::GetNotificationMessage(lTimerStatus,"HANDLER",m_lTrainID).c_str()
					);

		// start timer
		switch (m_lTimerOption)
		{
			case TIS5TIMER:
				waitTIS5Timer();
				break;
			case TIS7TIMER:
				waitTIS7Timer();
				break;
			default:;
		}
		
		if (!m_bTimerExpireToFailed)
		{
			if (TIMEREXPIRED == m_bTimerStat)
				lTimerStatus = TA_IRS_App::TtisTypes::PDSLibDownloadTimerExpireFailed;
		}
		else
		{
			if (TIMEREXPIRED == m_bTimerStat)
				lTimerStatus = TA_IRS_App::TtisTypes::PDSLibDownloadTimerExpireSuccess;
			else
				lTimerStatus = TA_IRS_App::TtisTypes::PDSLibDownloadTimerStopEventRecv;
		}
		
		LOG_GENERIC (
						SourceInfo,
						TA_Base_Core::DebugUtil::DebugInfo,
						"%s",
						CPDSMessageCreator::GetNotificationMessage(lTimerStatus,"HANDLER",m_lTrainID).c_str()
					);

		notifyDownloadStatus(lTimerStatus);

		FUNCTION_EXIT;
		
	}

	void CRadioPDSHandler::SendPDS(TA_IRS_App::IPDSEvent * pdsEvent)
	{
		
		FUNCTION_ENTRY ( "SendPDS ()" );
		
		
		
		TA_IRS_App::RadioPDSEvent * radioPdsEvent = (TA_IRS_App::RadioPDSEvent *)(pdsEvent);
		radioPdsEvent->getEventReader();	
		
		m_radioPDSBuffer = radioPdsEvent->getRadioPDSBuffer();
		if (TA_IRS_App::TtdFrame::FrameRepition == radioPdsEvent->getEventType())
		{
			if (!m_methodSend)
			{
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No Begin Yet Cannot Resend");
				m_pdsEventRemover->removeResendRecordForTrain(m_lTrainID);
				
				delete pdsEvent;
				pdsEvent = NULL;

				return;
			}
			
			if (m_bIsSending)
			{
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Cannot Resend, Operation not allowed while still sending");
				m_pdsEventRemover->removeResendRecordForTrain(m_lTrainID);
				
				delete pdsEvent;
				pdsEvent = NULL;
				
				return;
			}

		 	if (0 == m_methodSend->SetupFrameForResend(m_radioPDSBuffer))
			{
				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No Valid Frame Recieve");
				m_pdsEventRemover->removeResendRecordForTrain(m_lTrainID);
				
				delete pdsEvent;
				pdsEvent = NULL;

				return;
			}
		}
		else
		{
			m_methodSend = new CRadioPDSMethodSendData (m_vctrDataLib,radioPdsEvent->getClientName(),m_radioPDSBuffer);
		}
		
		terminateTimer();
		terminateAndWait();		
		m_bTimerExpireToFailed = true;		
		
		m_bIsSending = true;
		m_radioPDSSender->Send(m_methodSend, m_methodSend->getFrameContainerSize() );
		
		m_bSendLibrary = true;
		
		delete pdsEvent;
		pdsEvent = NULL;

		FUNCTION_EXIT;
	}
	
	void CRadioPDSHandler::waitTIS5Timer()
	{
		FUNCTION_ENTRY (" waitTIS5Timer () ");
		
		m_bTimerStat = m_semaTimer.timedWait(m_radioPDSTimer.lTimer5);	
		
		FUNCTION_EXIT;
	}

	void CRadioPDSHandler::waitTIS7Timer ()
	{
		FUNCTION_ENTRY (" waitTIS7Timer () ");

		m_bTimerStat = m_semaTimer.timedWait(m_radioPDSTimer.lTimer7);	

		FUNCTION_EXIT;
	}

	void CRadioPDSHandler::terminateTimer()
	{
		FUNCTION_ENTRY (" terminateTimer () ");

		m_semaTimer.post ();

		FUNCTION_EXIT;
	}

	void CRadioPDSHandler::cancelPDS()
	{
		FUNCTION_ENTRY (" cancelPDS () ");
		
		// only cancel if there is something to be cancel
		// stop current sending of frame
		if (!m_radioPDSSender)
		{
			// throw exception here cannot proceed no valid sender pointer
			return;
		}

		if (!m_bSendLibrary)
		{
			LOG_GENERIC (SourceInfo,
				TA_Base_Core::DebugUtil::DebugInfo,
				"There is nothing to be cancel"
				);
			return;
		}

		m_bCancelled = true;
		
		terminateTimer();
		terminateAndWait();	
		m_methodCancel = new CRadioPDSMethodCancel (m_methodSend->getClientName(),m_radioPDSBuffer);
		m_radioPDSSender->Send(m_methodCancel,m_methodCancel->getFrameContainerSize(),false);
		
		FUNCTION_EXIT;
	}

	void CRadioPDSHandler::notifyDownloadStatus (const long & lDownloadStatus)
	{		
		
		FUNCTION_ENTRY ("notifyDownloadStatus ()");

		std::string strReason;
		bool bSuccess = true;

		switch (lDownloadStatus)
		{
			case TA_IRS_App::TtisTypes::PDSLibDownloadComplete:
				
				if (!m_bCancelled)		// notify Sending of library packet is done
				{
					
					if (TA_IRS_App::TtdFrame::FrameRepition == m_methodSend->getEventType())
					{
						// remove to allow resend again
						m_pdsEventRemover->removeResendRecordForTrain(m_lTrainID);
					}
					
					m_bIsSending = false;
					startTimer (TIS7TIMER);
				}
				else	// notify sending of cancel packet is done
				{
					strReason = "Download Cancelled";
					bSuccess = false;
					m_pdsEventRemover->removeAllRecordForTrain(m_lTrainID);
					m_pdsObserver->NotifyTTSStatus(m_lTrainID,bSuccess,strReason);
					
					m_bCancelled = false;
					m_bIsSending = false;
				}
				break;
			
			case TA_IRS_App::TtisTypes::PDSLibDownloadCancelled:
				m_bCancelled = true;
				break;

			case TA_IRS_App::TtisTypes::PDSLibDownloadTimerExpireFailed:
				strReason = "Failure, No event recieve on given time";
				bSuccess = false;
				m_pdsObserver->NotifyTTSStatus(m_lTrainID,bSuccess,strReason);
				break;
			
			case TA_IRS_App::TtisTypes::PDSLibDownloadCallDisconnection:
				strReason = "Transfer Disconnection";
				bSuccess = false;
				m_pdsEventRemover->removeAllRecordForTrain(m_lTrainID);
				m_pdsObserver->NotifyTTSStatus(m_lTrainID,bSuccess,strReason);
				m_bIsSending = false;
				break;
			
			case TA_IRS_App::TtisTypes::PDSLibDownloadTimerExpireSuccess:
				strReason = "Successfully transfer library";
				bSuccess = true;
				m_pdsEventRemover->removeAllRecordForTrain(m_lTrainID);
				m_pdsObserver->NotifyTTSStatus(m_lTrainID,bSuccess,strReason);
				m_bIsSending = false;
				break;	
			default:;

		}

		FUNCTION_EXIT;
	}

	void CRadioPDSHandler::startTimer(long lTimerOption)
	{
		FUNCTION_ENTRY ("startTimer ()");

		m_lTimerOption = lTimerOption;
		start ();

		FUNCTION_EXIT;
	}

	void CRadioPDSHandler::callDisconnection()
	{	
		FUNCTION_ENTRY (" callDisconnection () ");
		
		m_radioPDSSender->stopSending();
		notifyDownloadStatus (TA_IRS_App::TtisTypes::PDSLibDownloadCallDisconnection);
		
		FUNCTION_EXIT;
	}

}
