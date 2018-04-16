#ifndef LIBRARYSYNCFAILEDMANAGER_H
#define LIBRARYSYNCFAILEDMANAGER_H

#include <boost/noncopyable.hpp>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <string>

namespace TA_IRS_App
{
	class LibrarySyncFailedManager : boost::noncopyable
	{
		public:
			LibrarySyncFailedManager(void) {}
			virtual ~LibrarySyncFailedManager(void) {}
			virtual void startAllProcessors() {}
			virtual void stopAllProcessors() {}
			virtual void startSTISSynchronise() {}
			virtual void stopSTISSynchronise() {}
			virtual void startSTISUpgrade() {}
			virtual void stopSTISUpgrade() {}
			virtual void startTTISSynchronise() {}
			virtual void stopTTISSynchronise() {}
			virtual void startTTISUpgrade() {}
			virtual void stopTTISUpgrade() {}
			virtual void setSessionId(std::string sessionId) {}
	};

	class LibrarySyncFailedProcessor;
	class OccLibrarySyncFailedManager : public LibrarySyncFailedManager
	{
	public:
		OccLibrarySyncFailedManager(unsigned short prosessRepeatInterval = 5000);
		virtual ~OccLibrarySyncFailedManager(void);
		virtual void startAllProcessors();
		virtual void stopAllProcessors();
		virtual void startSTISSynchronise();
		virtual void stopSTISSynchronise();
		virtual void startSTISUpgrade();
		virtual void stopSTISUpgrade();
		virtual void startTTISSynchronise();
		virtual void stopTTISSynchronise();
		virtual void startTTISUpgrade();
		virtual void stopTTISUpgrade();
		virtual void setSessionId(std::string sessionId);
	private:
		TA_Base_Core::ReEntrantThreadLockable m_stisSynchroniseProcessorLock;
		TA_Base_Core::ReEntrantThreadLockable m_stisUpgradeProcessorLock;
		TA_Base_Core::ReEntrantThreadLockable m_ttisSynchroniseProcessorLock;
		TA_Base_Core::ReEntrantThreadLockable m_ttisUpgradeProcessorLock;
		LibrarySyncFailedProcessor* m_stisSynchroniseProcessor;
		LibrarySyncFailedProcessor* m_stisUpgradeProcessor;
		LibrarySyncFailedProcessor* m_ttisSynchroniseProcessor;
		LibrarySyncFailedProcessor* m_ttisUpgradeProcessor;
	};
}

#endif
