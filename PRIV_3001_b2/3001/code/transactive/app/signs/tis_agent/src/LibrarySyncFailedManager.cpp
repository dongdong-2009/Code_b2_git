#include "LibrarySyncFailedManager.h"
#include "LibrarySyncFailedProcessor.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	using TA_Base_Core::DebugUtil;

	OccLibrarySyncFailedManager::OccLibrarySyncFailedManager(unsigned short prosessRepeatInterval)
		: m_stisSynchroniseProcessor(NULL),
		  m_stisUpgradeProcessor(NULL),
		  m_ttisSynchroniseProcessor(NULL),
		  m_ttisUpgradeProcessor(NULL)
	{
		try
		{
			m_stisSynchroniseProcessor =
				new LibrarySyncFailedProcessor(LibrarySyncFailedProcessor::STIS_SYNCHRONISE, prosessRepeatInterval);
			m_stisUpgradeProcessor =
				new LibrarySyncFailedProcessor(LibrarySyncFailedProcessor::STIS_UPGRADE, prosessRepeatInterval);
			m_ttisSynchroniseProcessor =
				new LibrarySyncFailedProcessor(LibrarySyncFailedProcessor::TTIS_SYNCHRONISE, prosessRepeatInterval);
			m_ttisUpgradeProcessor =
				new LibrarySyncFailedProcessor(LibrarySyncFailedProcessor::TTIS_UPGRADE, prosessRepeatInterval);
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception occur when create library processors");
		}
	}

	OccLibrarySyncFailedManager::~OccLibrarySyncFailedManager(void)
	{
		delete m_stisSynchroniseProcessor;
		delete m_stisUpgradeProcessor;
		delete m_ttisSynchroniseProcessor;
		delete m_ttisUpgradeProcessor;
	}

	void OccLibrarySyncFailedManager::startAllProcessors()
	{
		startSTISSynchronise();
		startSTISUpgrade();
		startTTISSynchronise();
		startTTISUpgrade();
	}

	void OccLibrarySyncFailedManager::stopAllProcessors()
	{
		stopSTISSynchronise();
		stopSTISUpgrade();
		stopTTISSynchronise();
		stopTTISUpgrade();
	}

	void OccLibrarySyncFailedManager::startSTISSynchronise()
	{
		TA_Base_Core::ThreadGuard guard(m_stisSynchroniseProcessorLock);
		if(NULL != m_stisSynchroniseProcessor)
		{
			m_stisSynchroniseProcessor->startProcess();
		}
	}

	void OccLibrarySyncFailedManager::stopSTISSynchronise()
	{
		TA_Base_Core::ThreadGuard guard(m_stisSynchroniseProcessorLock);
		if(NULL != m_stisSynchroniseProcessor)
		{
			m_stisSynchroniseProcessor->stopProcess();
		}
	}

	void OccLibrarySyncFailedManager::startSTISUpgrade()
	{
		TA_Base_Core::ThreadGuard guard(m_stisUpgradeProcessorLock);
		if(NULL != m_stisUpgradeProcessor)
		{
			m_stisUpgradeProcessor->startProcess();
		}
	}

	void OccLibrarySyncFailedManager::stopSTISUpgrade()
	{
		TA_Base_Core::ThreadGuard guard(m_stisUpgradeProcessorLock);
		if(NULL != m_stisUpgradeProcessor)
		{
			m_stisUpgradeProcessor->stopProcess();
		}
	}

	void OccLibrarySyncFailedManager::startTTISSynchronise()
	{
		TA_Base_Core::ThreadGuard guard(m_ttisSynchroniseProcessorLock);
		if(NULL != m_ttisSynchroniseProcessor)
		{
			m_ttisSynchroniseProcessor->startProcess();
		}
	}

	void OccLibrarySyncFailedManager::stopTTISSynchronise()
	{
		TA_Base_Core::ThreadGuard guard(m_ttisSynchroniseProcessorLock);
		if(NULL != m_ttisSynchroniseProcessor)
		{
			m_ttisSynchroniseProcessor->stopProcess();
		}
	}

	void OccLibrarySyncFailedManager::startTTISUpgrade()
	{
		TA_Base_Core::ThreadGuard guard(m_ttisUpgradeProcessorLock);
		if(NULL != m_stisUpgradeProcessor)
		{
			m_ttisUpgradeProcessor->startProcess();
		}
	}

	void OccLibrarySyncFailedManager::stopTTISUpgrade()
	{
		TA_Base_Core::ThreadGuard guard(m_ttisUpgradeProcessorLock);
		if(NULL != m_ttisUpgradeProcessor)
		{
			m_ttisUpgradeProcessor->stopProcess();
		}
	}

	void OccLibrarySyncFailedManager::setSessionId(std::string sessionId)
	{
		m_stisSynchroniseProcessor->setSessionId(sessionId);
		m_stisUpgradeProcessor->setSessionId(sessionId);
		m_ttisSynchroniseProcessor->setSessionId(sessionId);
		m_ttisUpgradeProcessor->setSessionId(sessionId);
	}
}
