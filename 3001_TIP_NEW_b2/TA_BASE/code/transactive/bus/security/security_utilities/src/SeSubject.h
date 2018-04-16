#ifndef SESUBJECT_H_INCLUDED
#define SESUBJECT_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/security_utilities/src/SeSubject.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/06/11 14:34:19 $
  * Last modified by:  $Author: liwei.gong $
  *
  * An implementation of the subject part of the observer pattern.
  * This implementation allows multiple subjects to be observerved.
  *
  */

namespace TA_Base_Bus
{
	template<class Sub>
	class SeSubject
	{
	public:
		typedef SeObserver<Sub> ObserverType;

		SeSubject() 
		{
		}

		virtual ~SeSubject() 
		{
		}

        // The attach/detach methods must be named differently to
        // their SeObserver counterparts in order to avoid ambiguity
        // in situations where a subject is also an observer.
		void attachObserver(ObserverType& observer); 

		// detachObserver
		void detachObserver(ObserverType& observer);

	protected:
		void notify(Sub& subject);
        
	private:
		//SeSubject(const SeSubject& seSubject);

		TA_Base_Core::NonReEntrantThreadLockable m_observerLock;

		typedef std::set<ObserverType* > ObserverSet;
		ObserverSet m_observers;
	};

	template<class Sub>
	void SeSubject<Sub>::attachObserver( ObserverType& observer )
	{
		FUNCTION_ENTRY("attachObserver");

		TA_Base_Core::ThreadGuard guard(m_observerLock);
		m_observers.insert(&observer);
		FUNCTION_EXIT;
	}

	template<class Sub>
	void SeSubject<Sub>::detachObserver( ObserverType& observer )
	{
		FUNCTION_ENTRY("detachObserver");

		TA_Base_Core::ThreadGuard guard(m_observerLock);
		m_observers.erase(&observer);
		FUNCTION_EXIT;
	}

	template<class Sub>
	void SeSubject<Sub>::notify( Sub& subject )
	{
		FUNCTION_ENTRY("notify");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"begin notify %s", typeid(*this).name());
		m_observerLock.acquire();
		ObserverSet localObservers(m_observers);
		m_observerLock.release();

		typename ObserverSet::iterator it;
		
		// nofity duty update
		for( it = localObservers.begin(); it != localObservers.end(); ++it )
		{       
			(*it)->update(subject);
		}
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"end notify %s", typeid(*this).name());
		FUNCTION_EXIT;
	}
}

#endif // SESUBJECT_H_INCLUDED
