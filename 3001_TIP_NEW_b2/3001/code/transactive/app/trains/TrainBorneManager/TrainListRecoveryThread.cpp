

#include "core/utilities/src/DebugUtil.h"
#include "TrainListRecoveryThread.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_App
{
	using TA_Base_Core::DebugUtil;

	TrainListRecoveryThread::TrainListRecoveryThread(TA_IRS_Bus::TrainSelector * trainSelector)
							:m_trainSelector(trainSelector),
							m_bIsTerminate(false),
							m_trainlistRecoveryDelay(30),
							m_semaphore(0)
	{
		FUNCTION_ENTRY("TrainListRecoveryThread");

		std::string initialiseRetryDelayStr = TA_Base_Core::RunParams::getInstance().get("TraInfoRecoverTime");

		if ( !initialiseRetryDelayStr.empty() )
		{
			long temp = atol(initialiseRetryDelayStr.c_str());

			if(temp <= 0) //disable the thread
			{
				 m_bIsTerminate = true;
			}
			else if (temp < 10) //min train list recovery interval time is 10 sec
			{
				m_trainlistRecoveryDelay = 10;
			}
			else if (temp > 10)
			{
				m_trainlistRecoveryDelay = temp;
			}
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "trainlistRecoveryDelay set to %d", m_trainlistRecoveryDelay );
		FUNCTION_EXIT;
	}

	TrainListRecoveryThread::~TrainListRecoveryThread(void)
	{
		FUNCTION_ENTRY("~TrainListRecoveryThread");
		FUNCTION_EXIT;
	}

	void TrainListRecoveryThread::run()
	{
		FUNCTION_ENTRY("run");	

		while(false == m_bIsTerminate)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Begin to check if radio session is Login");

			m_semaphore.timedWait(m_trainlistRecoveryDelay * 1000);

			if (m_bIsTerminate)
			{
				return;
			}

			try
			{
				m_trainSelector->recoveryTrainList();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Train list is recovered");
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "TrainListRecoveryThread unknown exception");
			}	
		}
		FUNCTION_EXIT;
	}

	void TrainListRecoveryThread::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bIsTerminate = true;
		// unblock the thread
		m_semaphore.post();
		FUNCTION_EXIT;
	}

}