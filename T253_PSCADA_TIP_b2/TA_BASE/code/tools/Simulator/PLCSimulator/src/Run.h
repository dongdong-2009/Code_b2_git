// Run.h: interface for the Run class.
//
//////////////////////////////////////

#pragma once

class RTUSimulator;

class Run : public ACE_Task_Base
{
public:
	Run( RTUSimulator& );
	~Run( void );
	virtual int svc( void );
	void SetID( const int& );

private:
	RTUSimulator & m_sim;
	int m_portID;
};
class ClientRun : public ACE_Task_Base
{
public:
	ClientRun(ACE_SOCK_Stream* peer, std::string& clientAddr, RTUSimulator& sim, int listenPort);
	~ClientRun();
	virtual int svc( void );
	int getListenPort();
private:
	RTUSimulator&		m_sim;
	ACE_SOCK_Stream*	m_peer;
	int					m_listenPort;
	std::string         m_clientAddr;
};
