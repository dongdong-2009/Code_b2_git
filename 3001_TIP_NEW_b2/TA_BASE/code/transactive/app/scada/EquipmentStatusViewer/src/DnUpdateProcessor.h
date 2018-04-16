#ifndef EDSV_DN_UPDATE_PROCESSOR_H
#define EDSV_DN_UPDATE_PROCESSOR_H

#include "app/scada/EquipmentStatusViewer/src/DnUpdateRequest.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include <string>
#include <deque>
#include "boost/shared_ptr.hpp"

namespace TA_Base_App
{
	class DnUpdateProcessCallBack
	{
	public:
		
		virtual ~DnUpdateProcessCallBack() {};

		virtual void processorCallBack(boost::shared_ptr<DnUpdateRequest> newRequest) = 0;
	};

	class DnUpdateProcessor : public TA_Base_Core::Thread
	{
	private:
		typedef std::deque<boost::shared_ptr<DnUpdateRequest> > DnUpdateQueue;

	public:
		DnUpdateProcessor(DnUpdateProcessCallBack& callBack);
		~DnUpdateProcessor();
		void run();
		void terminate();
		void clearQueue();
		void insert(boost::shared_ptr<DnUpdateRequest>& newRequest);
		void insertUnique(boost::shared_ptr<DnUpdateRequest>& newRequest);
		boost::shared_ptr<DnUpdateRequest> blockForItem();
	private:
		DnUpdateProcessCallBack&					m_callBack;
		DnUpdateQueue								m_queue;
		TA_Base_Core::NonReEntrantThreadLockable	m_queueLock;
		TA_Base_Core::Semaphore						m_semaphore;
		bool										m_keepRunning;
	};
}
#endif	
