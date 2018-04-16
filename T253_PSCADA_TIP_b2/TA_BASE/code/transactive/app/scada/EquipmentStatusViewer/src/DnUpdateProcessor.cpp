#include "app/scada/EquipmentStatusViewer/src/DnUpdateProcessor.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_App
{
	DnUpdateProcessor::DnUpdateProcessor(DnUpdateProcessCallBack& callBack)
		: m_callBack(callBack),
		  m_semaphore(0),
		  m_keepRunning(true)
	{
	}

	DnUpdateProcessor::~DnUpdateProcessor()
	{
		terminateAndWait();

		TA_ASSERT(THREAD_STATE_RUNNING != getCurrentState(), "QueueProcessor thread not terminated before destruction");
	}

	void DnUpdateProcessor::run()
	{
		//support restart
		m_keepRunning = true;

		while(m_keepRunning)
		{
			boost::shared_ptr<DnUpdateRequest> newItem = blockForItem();

			if (newItem.get() != NULL && m_keepRunning)
			{
				m_callBack.processorCallBack(newItem);
			}
		}
	}

	void DnUpdateProcessor::terminate()
	{
		m_keepRunning = false;

		m_semaphore.post();
	}

	void DnUpdateProcessor::clearQueue()
	{
		TA_THREADGUARD( m_queueLock );

		m_queue.clear();
	}

	void DnUpdateProcessor::insert(boost::shared_ptr<DnUpdateRequest>& newRequest)
	{
		TA_THREADGUARD( m_queueLock );

		boost::shared_ptr<DnUpdateRequest> newPtr( newRequest );

		m_queue.push_back( newPtr );

		m_semaphore.post();
	}

	void DnUpdateProcessor::insertUnique(boost::shared_ptr<DnUpdateRequest>& newRequest)
	{
		TA_THREADGUARD( m_queueLock );

		boost::shared_ptr<DnUpdateRequest> newPtr( newRequest );

		bool itemExist = false;

		DnUpdateQueue::iterator iter = m_queue.begin();
		for (; iter != m_queue.end(); ++iter)
		{
			if ( *(iter->get()) == *(newPtr.get()) )
			{
				itemExist = true;
				break;
			}
		}

		if ( !itemExist )
		{
			m_queue.push_back( newPtr );

			m_semaphore.post();
		}
	}

	boost::shared_ptr<DnUpdateRequest> DnUpdateProcessor::blockForItem()
	{
		m_semaphore.wait();

		TA_THREADGUARD( m_queueLock );

		boost::shared_ptr<DnUpdateRequest> item;
		item.reset();
		
		if ( !m_queue.empty() )
		{
			item = m_queue.front();

			m_queue.pop_front();
		}

		return item;
	}
}

