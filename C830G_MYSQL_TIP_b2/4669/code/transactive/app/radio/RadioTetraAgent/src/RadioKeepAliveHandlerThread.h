// RadioKeepAliveHandlerThread.h: interface for the RadioKeepAliveHandlerThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOKEEPALIVEHANDLERTHREAD_H__FC693178_5B92_4045_9003_934B820311B4__INCLUDED_)
#define AFX_RADIOKEEPALIVEHANDLERTHREAD_H__FC693178_5B92_4045_9003_934B820311B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/KeepAliveEventReader.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodCheckConnection.h"

#include "core/threads/src/Thread.h"
#include "core/timers/src/TimedWaitSemaphore.h"

namespace TA_IRS_App
{
	class RadioTCPConnection;

	class RadioKeepAliveHandlerThread : public TA_Base_Core::Thread
	{
	public:

		RadioKeepAliveHandlerThread( RadioTCPConnection * tcpConnection, unsigned long interval = 3, unsigned long expireTime = 10 );

		virtual ~RadioKeepAliveHandlerThread();

		virtual void run();
		virtual void terminate();

		void startCheckConnection();
		void stopCheckConnection();

		void setInterval( unsigned long interval );

		void setExpirePeriod( unsigned long expireTime );

		bool processEvent( TA_IRS_App::RadioEvent event );

	private:

		enum requestedState
		{
			NotRequeseted,
			RequestStartCheck,
			RequestStopCheck,
			KeepAliveStarted,
			KeepAliveStopped
		};

		bool								m_terminate;
		requestedState						m_checkConnProcessState;
		RadioTCPConnection *				m_tcpConnection;
		unsigned long						m_interval;
		unsigned long						m_expireTime;
		TA_Base_Core::TimedWaitSemaphore	m_semaphore;

	};
};

#endif // !defined(AFX_RADIOKEEPALIVEHANDLERTHREAD_H__FC693178_5B92_4045_9003_934B820311B4__INCLUDED_)
