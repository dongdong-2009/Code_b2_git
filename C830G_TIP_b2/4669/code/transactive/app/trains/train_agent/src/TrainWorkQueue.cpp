/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainWorkQueue.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */


#include <sstream>

#include "TrainWorkQueue.h"

#include "core/utilities/src/DebugUtil.h"
#include "TrainException.h"

namespace TA_IRS_App
{
	//
	// TrainWorkQueueManager
	//

	TrainWorkQueueManager::TrainWorkQueueManager() : m_threadPoolManager (NULL)
	{
		m_threadPoolManager = TA_Base_Core::ThreadPoolSingletonManager::getInstance();
	};

	TrainWorkQueueManager::~TrainWorkQueueManager()
	{
		TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPoolManager);
		m_threadPoolManager = NULL;
	};

	int TrainWorkQueueManager::perform(AbstractTrainWorkQueueItem * item)
	{
		// take ownership of the task
		TA_Base_Core::IWorkItemPtr newOwner(item);

		if (m_threadPoolManager != NULL)
		{
			m_threadPoolManager->queueWorkItem(newOwner);
		}

		return 0;
	}

	//
	// AbstractTrainWorkQueueItem
	//

	AbstractTrainWorkQueueItem::AbstractTrainWorkQueueItem()
	{
	}

	AbstractTrainWorkQueueItem::~AbstractTrainWorkQueueItem()
	{
	}

	std::string AbstractTrainWorkQueueItem::info()
	{
		return std::string("Unknown");
	}

	void AbstractTrainWorkQueueItem::executeWorkItem()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			        "Started WorkQueueItem: %s",
					info().c_str());

		try
		{
			call();
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Unhandled exception in WorkQueueItem call()");
		}
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			        "Finished WorkQueueItem: %s",
					info().c_str());
	}


	//
	// TrainTIMSNotification
	//

	std::string TrainTIMSNotification::info()
	{
		std::ostringstream s;
		s << "TrainTIMSNotification [Train=" << static_cast<int>(m_event.getTrainID()) << "]";
		return s.str();
	}

	int TrainTIMSNotification::call()
	{
		FUNCTION_ENTRY("TrainTIMSNotification::call()");

		try
		{
			m_trainManager.onTrainEvent(m_event);
		}
		catch (TrainException &e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in TIMS worker thread");
		}
		
		FUNCTION_EXIT;
		return 0;
	}

	//
	// TrainATSNotification
	//

	std::string TrainATSNotification::info()
	{
		std::ostringstream s;
		s << "TrainATSNotification [Train=" << static_cast<int>(m_event.trainID) << "]";
		return s.str();
	}

	int TrainATSNotification::call()
	{
		try
		{
			m_trainManager.onATSEvent(m_event);
		}
		catch (TrainException &e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in ATS worker thread");
		}

		return 0;
	}

	//
	// TrainDutyStateUpdate
	//

	std::string TrainDutyStateUpdate::info()
	{
		return std::string("TrainDutyStateUpdate");
	}

	int TrainDutyStateUpdate::call()
	{
		try
		{
			m_trainManager.onDutyEvent(m_event);
		}
		catch (TrainException &e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in Duty worker thread");
		}

		return 0;
	}

	//
	// TrainDutyStateUpdate
	//

    std::string TrainLibraryDownloadUpdate::info()
	{
		return std::string("TrainLibraryDownloadUpdate");
	}

	int TrainLibraryDownloadUpdate::call()
	{
		try
		{
			m_trainManager.onTrainLibraryDownloadEvent(m_trainId, m_success, m_failReason);
		}
		catch (TrainException &e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TrainException", e.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "<unknown exception>", "Error in TTIS worker thread");
		}

		return 0;
	}
}
