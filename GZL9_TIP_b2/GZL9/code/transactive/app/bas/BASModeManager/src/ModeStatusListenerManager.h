#pragma once
#include "ace/Singleton.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <vector>

namespace TA_IRS_App
{

	// The interface which dialog want to update mode status,
	// need inherit from this interface
	class IModeStatusListenerInterface
	{
	public:
		virtual void updateModeStatus(unsigned long modeKey, int status) = 0;
	};

	class BasManagerSubscriber;
	class ModeStatusManager;
	class ModeStatusListenerManager
	{
	public:
		friend BasManagerSubscriber;
		friend ModeStatusManager;
		ModeStatusListenerManager(void);
		~ModeStatusListenerManager(void);
	public:
		// add a listener to manager
		// listener will be call back when mode updated
		unsigned long addListener(IModeStatusListenerInterface* listener);
		
		// remove a listener from manager,
		// then this listener will not be call back to update the mode status
		unsigned long removeListener(IModeStatusListenerInterface* listener);

	protected:
		// we declare this function as a protected one, 
		// only call by class BasManagerSubscriber
		void updateModeStatus(unsigned long modeKey, int status);
	private:
		// The listener lock and listener cache
		TA_Base_Core::NonReEntrantThreadLockable m_listenerLock;
		std::vector<IModeStatusListenerInterface*> m_vctLister;
	};

	typedef ACE_Singleton<ModeStatusListenerManager,ACE_Recursive_Thread_Mutex> ModeStatusListenerManagerInstance;

}
