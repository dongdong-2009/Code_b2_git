// RadioPDSHandler.h: interface for the CRadioPDSHandler class.
//
//////////////////////////////////////////////////////////////////////
/* The source code in this file is the property of 
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   $RadioPDSHandler$
* @author:  Dennis Abriol
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* RadioPDSHandler manage the timer for every request of client to 
* send a frame. This module notify the RadioPDSManager what is the
* status of the sending of data to client, aside from that the said
* module will take care the time out of every client request.
*/


#if !defined(AFX_RADIOPDSHANDLER_H__24FD2A20_CB00_4ACC_9A9B_028BC902874B__INCLUDED_)
#define AFX_RADIOPDSHANDLER_H__24FD2A20_CB00_4ACC_9A9B_028BC902874B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/threads/src/Thread.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/FrameCheckSequence.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtdFrame.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/CommonTypes.h"
#include "core/timers/src/TimedWaitSemaphore.h"
#include "core/utilities/src/DebugUtil.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IRadioPDSSender.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSTTFObserver.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/ILibDownloadStatus.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethod.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodCancel.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodSendData.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IEventRecordRemover.h"


#include <vector>

namespace TA_IRS_App
{
	
	class CCRadioPDSHandlerException : public TA_Base_Core::TransactiveException
	{
	public:
		CCRadioPDSHandlerException( const std::string& errorText ) throw();
		virtual ~CCRadioPDSHandlerException() throw();
	};

	
	class CRadioPDSHandler : public TA_Base_Core::Thread , public ILibDownloadStatus
	{
		public:
			void callDisconnection();
			void cancelPDS();
		
			CRadioPDSHandler (IEventRecordRemover * pdsEventRemover,IRadioPDSSender * pdsSender, IPDSTTFObserver * pdsObserver,
				std::vector <unsigned char> vctrLibData, const long & lTrainID, const TA_IRS_App::CommonTypes::SRadioPDSTimer& radioPDSTimer );
			virtual ~CRadioPDSHandler();
			
			// Called by the manager to start sending data frame
			void SendPDS(TA_IRS_App::IPDSEvent * pdsEvent);
			
			// Called by the publisher (sender) to know the status of download
			virtual void notifyDownloadStatus (const long & lDownloadStatus);
			
		protected:
			//thread operations
			virtual void run ();
			virtual void terminate () {}

		private:
			
			void startTimer (long lTimerOption);
			void waitTIS5Timer ();
			void waitTIS7Timer ();
			void terminateTimer ();
			CRadioPDSHandler(){};
			
			// Types
			typedef TA_IRS_App::TtisTypes::TtisFrameList FrameList;
			typedef	TA_IRS_App::TtdFrame TrainFrameType;
			
			// attributes
			std::vector <unsigned char> m_vctrDataLib;
			
			// class flags
			bool m_bTimerStat;
			bool m_bCancelled;
			bool m_bTimerExpireToFailed;
			bool m_bSendLibrary;
			bool m_bIsSending;

			// class timer option variable
			long m_lTimerOption;
			
			// frame storage
			FrameList m_frameListBuff;
			FrameList m_frameListToBeSend;
			
			TA_Base_Core::TimedWaitSemaphore m_semaTimer;
			TA_IRS_App::SRadioPDSBuffer		m_radioPDSBuffer;
			IRadioPDSSender * m_radioPDSSender;
			IPDSTTFObserver * m_pdsObserver;
			long			  m_lTrainID;
			
			CRadioPDSMethodSendData * m_methodSend;
			CRadioPDSMethodCancel   * m_methodCancel;
			IEventRecordRemover		* m_pdsEventRemover;

			//PDSTimer
			TA_IRS_App::CommonTypes::SRadioPDSTimer m_radioPDSTimer;

			// To replace macro definitions
			static const unsigned long TIS7TIMER;
			static const unsigned long TIS5TIMER;
			static const unsigned long TIMEREXPIRED;
			static const unsigned long TIMERSTOPPED;
	};
}

#endif // !defined(AFX_RADIOPDSHANDLER_H__24FD2A20_CB00_4ACC_9A9B_028BC902874B__INCLUDED_)
