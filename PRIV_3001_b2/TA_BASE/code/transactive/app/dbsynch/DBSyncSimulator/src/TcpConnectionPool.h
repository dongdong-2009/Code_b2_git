#if !defined(__C_TCP_CONNECTION_POOL_HH__)
#define __C_TCP_CONNECTION_POOL_HH__

#include <string>
#include <set>
#include <list>
#include <map>
#include "CommonDef.h"
#include "CommonData.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/SocketSet.h"
#include "core/sockets/src/TcpServerSocket.h"


NS_BEGIN(TA_DB_Sync)

using namespace TA_Base_Core;

class TcpConnectionActor;

class CTcpConnectionPool : public Thread
{
private:
	typedef std::map<unsigned long, TcpConnectionActor*>              vecConPoolManagerConT;
	typedef std::map<unsigned long,TcpConnectionActor*>::iterator     vecConPoolManagerConIterT;
	
	typedef std::set<TcpSocket*>					   vecTcpSocketConT;
	typedef std::set<TcpSocket*>::iterator			   vecTcpSocketConIterT;

public:
	
	~CTcpConnectionPool();

public:

	/**
	* getInstance
	*
	* Public accessor to the singleton
	*
	* @return   CTcpConnectionPool    One and only CTcpConnectionPool
	*/
	static CTcpConnectionPool*  getInstance();

	virtual void run(); //Thread         virtual void run() = 0;
	virtual void terminate();//Thread    virtual void terminate() = 0;

public:	
	bool poolAcceptOneNewClient( TcpSocket* pNewClient);
	
    //release invaild action based on newActionID	
	void releaseInvaildAction(unsigned long newActionID);   
	void setDequeueStatusBad(unsigned long  actorID); //Dequeue meet problem, need to release TcpConnectionActor
    void setEnqueueStatusBad(unsigned long  actorID); //Enqueue meet problem, need to release TcpConnectionActor
private:
	CTcpConnectionPool();
	void _ThreadJob();
	bool _AcceptNewClient( TcpSocket* pNewClient);
 
private:
	void _JobCheckConnectedClients();
	bool _JobAcceptNewClient();
	void _ClearAll();
	void _ClearAllConnectedClients();
	void _ClearAllTcpSocket();
	unsigned long _getNextActorID();  //get next action id
private:
	bool	m_toTerminate;
	

private:
	ReEntrantThreadLockable	m_lockSetAllClients; 

    ReEntrantThreadLockable	m_lockActionClient; 

	vecConPoolManagerConT	m_AllConnectedClients;       //all connected actors
	int	m_nMaxClientNum;	
	int	m_nCurrentClientNum;

	vecTcpSocketConT    m_AllNewTCPClients;          //TCP clients

	/**
	* the one and only classInstance
	*/
	static CTcpConnectionPool* m_theClassInstance;

	/**
	* protect singleton creation
	*/
	static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

	

    TA_Base_Core::ReEntrantThreadLockable          m_seqIDLock;
	unsigned long                                  m_sequenceID; //used to create a unique id of TcpConnectionActor;
};
NS_END(TA_DB_Sync)




#endif//__C_TCP_CONNECTION_POOL_HH__
















