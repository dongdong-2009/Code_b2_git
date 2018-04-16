#if !defined(TA_Base_Utilits_TASingleton_H)
#define TA_Base_Utilits_TASingleton_H
#if defined _MSC_VER
#pragma once
#endif //_MSC_VER > 1000

// the implement class need have a public default constructor or
// declare the template is a friend class
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


namespace TA_Base_Core
{
	template<typename className, typename lock>
	class TASingleton
	{
	public:
		virtual ~TASingleton(void);
	private:
		TASingleton(void);
		TASingleton(const TASingleton&);
		TASingleton& operator=(const TASingleton&);
	public:
		static className& getInstance();
	protected:
		static lock m_lock;
		static className* m_instance;
	};

	template<typename className, typename lock>
	lock TASingleton<className, lock>::m_lock;

	template<typename className, typename lock> 
	className* TASingleton<className, lock>::m_instance = NULL;

	template<typename className, typename lock>
	TASingleton<className, lock>::TASingleton(void)
	{			
	}

	template<typename className, typename lock>
	TASingleton<className, lock>::~TASingleton(void)
	{
		if (NULL != m_instance)
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

	template<typename className, typename lock>
	className& TASingleton<className, lock>::getInstance()
	{
		TA_THREADGUARD(m_lock);

		if (NULL == m_instance)
		{
			m_instance = new className;
		}
		return *m_instance;
	}
	typedef TA_Base_Core::NonReEntrantThreadLockable TASingletonLock;
}

#define TASINGLETONLOCK TA_Base_Core::TASingletonLock

#endif //TA_Base_Utilits_TASingleton_H