// RadioKeepAliveHandlerThread.cpp: implementation of the RadioKeepAliveHandlerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "RadioTCPConnection.h"
#include "RadioKeepAliveHandlerThread.h"
#include "RadioAgentConstants.h"

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/KeepAliveEventReader.h"

#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace TA_IRS_App;

RadioKeepAliveHandlerThread::RadioKeepAliveHandlerThread( TA_IRS_App::RadioTCPConnection * tcpConnection ,
														 unsigned long interval, unsigned long expireTime ):
m_terminate(false),
m_tcpConnection(tcpConnection),
m_interval(interval),
m_expireTime( expireTime ),
m_checkConnProcessState(NotRequeseted),
m_semaphore(0)
{
	FUNCTION_ENTRY("RadioKeepAliveHandlerThread");
	start();
	FUNCTION_EXIT;
}

RadioKeepAliveHandlerThread::~RadioKeepAliveHandlerThread()
{
	FUNCTION_ENTRY("~RadioKeepAliveHandlerThread");

	delete m_tcpConnection;
	m_tcpConnection = NULL;

	m_terminate = true;

	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::run()
{
	FUNCTION_ENTRY("run");

	while( !m_terminate )
	{
		if ( m_checkConnProcessState == RequestStartCheck )
		{
			startCheckConnection();
		}
		
		if ( m_semaphore.timedWait( m_expireTime * 1000 ) )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Received Event, reset timer." );
		} 
		else
		{
			if ( (m_checkConnProcessState == KeepAliveStarted) && !m_tcpConnection->getIgnoreIncomingData() )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[KeepAlive] Timeout, inform connection manager." );
				m_checkConnProcessState = NotRequeseted;
				m_tcpConnection->connectionLost();
			}
			else
			{
				m_semaphore.timedWait( m_expireTime * 1000 );
			}
		}
	}
	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::terminate()
{
	FUNCTION_ENTRY("terminate");

	m_terminate = true;
	
	m_semaphore.post();

	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::setInterval(unsigned long interval)
{
	m_interval = interval;
	m_checkConnProcessState = NotRequeseted;
}

void RadioKeepAliveHandlerThread::startCheckConnection()
{
	FUNCTION_ENTRY("startCheckConnection");

	if ( m_checkConnProcessState == RequestStartCheck ||
		m_checkConnProcessState == KeepAliveStarted )
	{
		FUNCTION_EXIT;
		return;
	}

	RadioMethodCheckConnection methodCheckConnection;
	methodCheckConnection.enableCheckConnection(true);
	methodCheckConnection.setCheckInterval( m_interval );

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startCheckConnection" );

	m_checkConnProcessState = RequestStartCheck;

	m_tcpConnection->Send( methodCheckConnection );

	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::stopCheckConnection() // Do not do anything now.
{
	FUNCTION_ENTRY("stopCheckConnection");

// 	if ( m_checkConnProcessState == RequestStopCheck ||
// 		m_checkConnProcessState == KeepAliveStopped )
// 	{
// 		FUNCTION_EXIT;
//		return; 
// 	}

// 	RadioMethodCheckConnection methodCheckConnection;
// 	methodCheckConnection.enableCheckConnection(false);
// 
// 	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopCheckConnection" );
// 
// 	m_checkConnProcessState = RequestStopCheck;
// 	m_tcpConnection->Send( methodCheckConnection );

	FUNCTION_EXIT;
}


void RadioKeepAliveHandlerThread::setExpirePeriod( unsigned long expireTime )
{
	FUNCTION_ENTRY("setExpirePeriod");
	m_expireTime = expireTime;
	FUNCTION_EXIT;
}

bool RadioKeepAliveHandlerThread::processEvent( TA_IRS_App::RadioEvent event )
{
	FUNCTION_ENTRY("processEvent");

	bool isKeepAlive = false;

	if ( m_checkConnProcessState == NotRequeseted )
	{
		FUNCTION_EXIT;
		return isKeepAlive;
	}
	else if ( event.getEventType() == EVENT_KEEPALIVE )
	{
		isKeepAlive = true;

		if ( TA_Base_Core::DebugUtil::getInstance().getLevel() >= TA_Base_Core::DebugUtil::DebugDebug )
		{
			RadioEventReader* eventReader = event.getEventReader();

			KeepAliveEventReader* keepAliveReader = dynamic_cast<KeepAliveEventReader*>(eventReader);

			if ( keepAliveReader != NULL )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"[KeepAlive] Received keep alive message: %d", keepAliveReader->getEventCounter() );
			}
		}
	}
	else if ( event.getEventType() == EVENT_CHECKCONNECTION )
	{
		isKeepAlive = true;

		std::ostringstream outStr;

		outStr << event.getStatus() << ", ";

		switch( event.getStatus() )
		{
		case STATUS_RESTARTED:
			m_checkConnProcessState = KeepAliveStarted;
			outStr << "KeepAliveRestarted ";
			break;
		case STATUS_STARTED:
			m_checkConnProcessState = KeepAliveStarted;
			outStr << "KeepAliveStarted ";
			break;
		case STATUS_STOPPED:
			m_checkConnProcessState = KeepAliveStopped;
			outStr << "KeepAliveStopped ";
			break;
		case STATUS_KO_NOT_ALLOWED:
			m_checkConnProcessState = NotRequeseted;
			{
				if ( m_interval <= 0 )
				{
					m_interval = TA_IRS_App::RadioAgentConstants::KEEP_ALIVE_DEFAULT_INTERVAL;
					startCheckConnection();
				}
			}
			outStr << "KeepAliveRequestFailed ";
			break;
		default:
			outStr << "Error " << event.getStatus();
			break;
		}

		outStr << "." << std::endl;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"[KeepAlive] Received check connection message, status: %s", outStr.str().c_str() );
	}

	m_semaphore.post();

	FUNCTION_EXIT;
	return isKeepAlive;
}
