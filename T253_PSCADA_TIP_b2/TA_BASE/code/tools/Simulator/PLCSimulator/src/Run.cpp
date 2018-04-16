// Run.cpp: implementation for the Run class.
//
//////////////////////////////////////

#include "stdafx.h"
#include "Run.h"
#include "RTUSimulator.h"

Run::Run( RTUSimulator& simulator ) : 
m_sim( simulator ),
m_portID( 0 )
{

}

Run::~Run( void )
{

}

int Run::svc( void )
{
	ACE_DEBUG
      ((LM_DEBUG, ACE_TEXT ("(%t) Handler Thread running\n")));
	m_sim.StartSvr( m_portID );
	return 0;
}

void Run::SetID( const int& i )
{
	m_portID = i;
}
ClientRun::ClientRun(ACE_SOCK_Stream* peer, std::string& clientAddr, RTUSimulator& sim, int listenPort)
	: m_sim(sim),
	  m_peer(peer),
	  m_listenPort(listenPort),
	  m_clientAddr(clientAddr)
{

}

ClientRun::~ClientRun()
{
	delete m_peer;
}

int ClientRun::svc( void )
{
	std::string logStart("client: ");
	logStart += m_clientAddr;
	logStart += std::string(" start.");
	m_sim.writeToLogLn(logStart);
	ACE_SOCK_Stream& peer = *m_peer;
	
	unsigned char buffer[ 122880 ];
	ssize_t bytes_received;
	while ((bytes_received = peer.recv (buffer, sizeof(buffer))) != -1)
	{
		if( bytes_received != 0)
		{
			int cmdType = m_sim.ParseCmdClass(peer, buffer, bytes_received, m_listenPort);
			switch( cmdType )
			{
			case READNW:
				{
					m_sim.QueryAndSend(peer, buffer);
					break;
				}
			case WRITENW:
				{
					m_sim.WriteAndSend(peer, buffer);
					break;
				}
			case WRITENB:
				{
					m_sim.WriteAndSend(peer, buffer, true);
					break;
				}
			case READNB:
				{
					m_sim.QueryAndSend(peer, buffer, true);
					break;
				}
			case READCOIL:
				{
					m_sim.QueryAndSend(peer, buffer, true);
					break;
				}
			case READ_HOLD_REGISTERS:
				{
					m_sim.QueryAndSend(peer, buffer);
					break;
				}
			case WRITE1W:
				{
					m_sim.Write1AndSend(peer, buffer);
					break;
				}
			case WRITE1B:
				{
					m_sim.Write1AndSend(peer, buffer, true);
					break;
				}
			default:
				{
					std::string logs("error on parse command class");
					m_sim.writeToLogLn(logs);
				}
			}
		}
		else
		{
			break;
		}
	}
	peer.close ();
	std::string logs("clients: ");
	logs += m_clientAddr;
	logs += std::string(" exit");
	m_sim.writeToLogLn(logs);
	return 0;
}
int ClientRun::getListenPort()
{
	return m_listenPort;
}