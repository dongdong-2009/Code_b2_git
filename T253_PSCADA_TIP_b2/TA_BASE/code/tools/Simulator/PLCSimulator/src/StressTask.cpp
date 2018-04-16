// Run.cpp: implementation for the Run class.
//
//////////////////////////////////////

#include "stdafx.h"
#include "StressTask.h"
#include "FileLoader.h"

StressTask::StressTask( RTUSimulator* sim, std::string & cfg):
m_sim(sim),
m_StressCfgFile(cfg),
m_bRunning(false),
m_handle(0)
{
	m_eventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	ASSERT(NULL != m_eventHandle);
	m_mulmapStress.clear();
}

StressTask::~StressTask( void )
{
	CloseHandle(m_eventHandle);
}

int StressTask::svc( void )
{
	ACE_DEBUG
		((LM_DEBUG, ACE_TEXT ("(%t) Handler Thread running\n")));
	m_bRunning = true;
	Stress();
	return 0;
}

void StressTask::Stress()
{
	GetStressConfig();
	while( 1 )
	{
		if (!m_bRunning)
		{
			break;
		}
		
		smap_type::iterator bgn = m_mulmapStress.begin() ;
		smap_type::iterator end = m_mulmapStress.end();
		
		if (bgn == end)
		{
			WaitForSingleObject(m_eventHandle, 2000);
			continue;
		}
		while( bgn != end )
		{
			if (!m_bRunning)
			{
				break;
			}
			int bit = -1;
			int addr = 0;
			int val = 0;
			
			std::string addrstr = bgn->first;
			std::string valstr = bgn->second;
			std::string bitstr("");
			
			if( addrstr.find('.') != std::string::npos )
			{
				bitstr = std::string(addrstr.begin() + addrstr.find('.') + 1, addrstr.end());
				addrstr.erase(addrstr.find('.'));
				
				if((bitstr.find("0X") != std::string::npos) || (bitstr.find("0x") != std::string::npos))
				{
					bitstr.erase(0,2);
					bit = _tcstoul(bitstr.c_str(), 0, 16);
				}
				else
				{
					bit = _tcstoul(bitstr.c_str(), 0, 10);
				}
			}
			
			if( (addrstr.find("0X") != std::string::npos) || (addrstr.find("0x") != std::string::npos) )
			{
				addrstr.erase(0,2);
				addr = _tcstoul(addrstr.c_str(), 0, 16);
			}
			else
			{
				addr = _tcstoul(addrstr.c_str(), 0, 10);
			}
			
			if( (valstr.find("0X") != std::string::npos) || (valstr.find("0x") != std::string::npos) )
			{
				valstr.erase(0,2);
				val = _tcstoul(valstr.c_str(), 0, 16);
			}
			else
			{
				val = _tcstoul(valstr.c_str(), 0, 10);
			}
			
			if( -1 == addr )
			{
				WaitForSingleObject(m_eventHandle, val);
			}
			else
			{
				m_sim->StressCallBack(addr, val, bit);
			}
			++bgn;
		}
	}
}

void StressTask::GetStressConfig()
{
	std::string file = m_StressCfgFile;
	if (m_StressCfgFile == "")
	{
		return;
	}
	std::string	pattern	="^([Xx0-9a-fA-F.]+)[[:space:]]*=[[:space:]]*([-Xx0-9a-fA-F]*)|^sleep[[:space:](]*([0-9]+)[[:space:])]*";
	
	int varpos = 1;
	int valpos = 2;
	int sleeppos = 3;
	
	std::ifstream is(file.c_str());
	std::string s;
	
	FileLoader()(s, is);
	
	boost::regex expression( pattern.c_str() );
	std::string::const_iterator start, end;
	
	start = s.begin();
	end = s.end();  
	m_mulmapStress.erase( m_mulmapStress.begin(), m_mulmapStress.end() );
	
	boost::match_results<std::string::const_iterator> what;
	boost::match_flag_type flags = boost::match_default;
	
	while(boost::regex_search(start, end, what, expression, flags))   
	{
		if(what[sleeppos].first != what[sleeppos].second)
		{
			m_mulmapStress.insert(smap_value("-1",std::string(what[sleeppos].first, what[sleeppos].second)));
		}
		else
		{
			m_mulmapStress.insert(smap_value(std::string(what[varpos].first, what[varpos].second),
				std::string(what[valpos].first, what[valpos].second)));
		}
		
		start = what[0].second;      
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}
}

void StressTask::SetHandle( ACE_hthread_t h )
{
	m_handle = h;
}

ACE_hthread_t StressTask::GetHandle()
{
	return m_handle;
}

void StressTask::CloseTask()
{
	m_bRunning = false;
	if (!SetEvent(m_eventHandle))
	{
		ASSERT(FALSE);
	}
}

