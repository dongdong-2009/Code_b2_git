#ifndef BARRIEREDTASK_H
#define BARRIEREDTASK_H

#include "core/threads/src/IWorkItem.h"
#include <boost/function.hpp>

namespace TA_Base_Core
{
    class SingleThreadBarrier;
}

namespace TA_IRS_App
{
	class BarrieredTask : public TA_Base_Core::IWorkItem
	{
	public:
		BarrieredTask(boost::function<void ()> task, TA_Base_Core::SingleThreadBarrier& barrier);
		virtual void executeWorkItem();
		virtual ~BarrieredTask();
	private:
		boost::function<void ()> m_task;
		TA_Base_Core::SingleThreadBarrier& m_barrier;
	};
}
#endif