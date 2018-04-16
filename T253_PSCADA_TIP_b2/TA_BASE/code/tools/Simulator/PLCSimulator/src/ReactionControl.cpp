
#include "ReactionControl.h"
#include "RTUSimulator.h"

CReactionControl::CReactionControl(void )
:m_mutex(),
m_conMutex(),
m_condition(m_conMutex)
{
	m_isRuning=false;
	m_logFile.open("simulator.log",ios::in|ios::out|ios::app);
}

CReactionControl::~CReactionControl(void )
{
	m_isRuning=false;
	m_condition.signal();
}

int CReactionControl::queueNode(SReactionNode &rn)
{
	ACE_Guard<ACE_Thread_Mutex> guard(m_mutex);

	m_list.push_back(rn);
	return m_list.size();
}


void CReactionControl::terminate()
{
	m_isRuning=false;
}

int CReactionControl::svc()
{
	ACE_Time_Value tvInternal(1,500);
	ACE_Time_Value tv;
	m_isRuning=true;
	while(m_isRuning)
	{
		{
			ACE_Guard<ACE_Thread_Mutex> guard(m_mutex);
			std::list<SReactionNode>::iterator it=m_list.begin();
			for(;it!=m_list.end();)
			{
				it->milisec =  it->milisec - 1000;
				if( it->milisec < 0 )
				{
					RTUSimulator::getInstance().SetData(it->address,it->data,it->type, it->bit, true);
					
					SYSTEMTIME systime;
					GetLocalTime(&systime);
					m_logFile<<systime.wDay<<" ";
					m_logFile<<systime.wHour<<" ";
					m_logFile<<systime.wMinute<<" ";
					m_logFile<<systime.wSecond<<"::";
					m_logFile<<"Address:"<<it->address;
					m_logFile<<"bit:"<<it->bit;
					m_logFile<<"data:"<<it->data;
					m_logFile<<"type"<<it->type<<std::endl;

					it=m_list.erase(it);
					continue;
				}
				++it;
			}
		}
		//tv=ACE_OS::gettimeofday() + tvInternal;
		//m_condition.wait(&tv);
		ACE_OS::sleep(1);
	}
	return -1;
}
