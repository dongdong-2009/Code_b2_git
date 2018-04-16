#include "app/cctv/video_display_manager/src/BarrieredTask.h"
#include "core/synchronisation/src/SingleThreadBarrier.h"

namespace TA_IRS_App
{
	BarrieredTask::BarrieredTask(boost::function<void ()> task, TA_Base_Core::SingleThreadBarrier& barrier)
	    : m_barrier(barrier),
	      m_task(task)
	{
	}

	BarrieredTask::~BarrieredTask()
	{
	}


	void BarrieredTask::executeWorkItem()
	{
		m_task();
		m_barrier.post();
	}
}