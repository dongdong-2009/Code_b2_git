

//#include"StdAfx.h"
#include "stdafx.h"
#include<list>
#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Condition_Thread_Mutex.h"
#include "ace/Condition_T.h"
#include <fstream>

struct SReactionNode
{
	int address;
	int data;
	SETTYPE type;
	int bit;
	int milisec;
};



class CReactionControl: public ACE_Task_Base
{
public:
	CReactionControl(void );
	~CReactionControl( void );
	virtual int svc( void );
	int queueNode(SReactionNode &);
	void terminate();

private:
	bool                     m_isRuning;
	std::list<SReactionNode> m_list;
	ACE_Thread_Mutex         m_mutex;
	ACE_Thread_Mutex         m_conMutex;
	ACE_Condition<ACE_Thread_Mutex>	m_condition;
	std::ofstream     m_logFile;
	
};



