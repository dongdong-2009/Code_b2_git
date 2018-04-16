#pragma once

#include "core/threads/src/Thread.h"
#include "boost/function.hpp"

class ThreadedMemberFunc : public TA_Base_Core::Thread
{
public:
	ThreadedMemberFunc(const boost::function<void ()>& func):m_func(func)
	{
		start();
	}
	ThreadedMemberFunc()
	{
	}
	void setThreadFunc(const boost::function<void ()>& func)
	{
		m_func = func;
	}
	virtual ~ThreadedMemberFunc()
	{
	}
	virtual void terminate()
	{
	}
	virtual void run()
	{
		if(false == m_func.empty())
		{
			m_func();
		}
	}
private:
	boost::function<void ()> m_func;
};