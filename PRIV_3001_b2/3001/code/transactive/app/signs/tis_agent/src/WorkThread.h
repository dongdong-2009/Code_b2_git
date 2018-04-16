#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include "core/threads/src/Thread.h"
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace TA_IRS_App
{
	class WorkThread : public TA_Base_Core::Thread, boost::noncopyable
	{
	public:
	    typedef boost::function<void ()> ThreadFunc;
		WorkThread(const ThreadFunc& workFunc);
		void start();
		virtual void run();
		virtual void terminate();
		bool shouldRunning()
		{
			return m_shouldRunning;
		}
	private:
		bool m_shouldRunning;
	    ThreadFunc m_workFunc;
	};
}

#endif
