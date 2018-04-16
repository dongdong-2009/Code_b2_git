#ifndef SEOBSERVER_H_INCLUDED
#define SEOBSERVER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/security_utilities/src/SeObserver.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * An implementation of the observer part of the observer pattern.
  * This implementation allows multiple subjects to be observerved.
  *
  */

namespace TA_Base_Bus
{
	template<class Sub>
	class SeObserver
	{
	public:
		SeObserver( Sub& subject );
		virtual ~SeObserver();

		virtual void update( Sub& subject ) = 0;
	
	protected:
		void attach();
		// allow the observer to detach BEFORE it is destructed
		void detach();

	private:
		//SeObserver(const SeObserver& seObserver);

		Sub& m_subject;
		bool m_detached;
	};

	template<class Sub>
	SeObserver<Sub>::SeObserver( Sub& subject ) :
		m_subject(subject),
		m_detached(true)
	{
		attach();
	}

	template<class Sub>
	void SeObserver<Sub>::attach()
	{
		// attach
		if( m_detached )
		{
			m_subject.attachObserver(*this);
			m_detached = false;
		}
	}

	template<class Sub>
	void SeObserver<Sub>::detach()
	{
		// detach
		if( !m_detached )
		{
			m_subject.detachObserver(*this);
			m_detached = true;
		}
	}

	template<class Sub>
	SeObserver<Sub>::~SeObserver()
	{
		detach();
	}
}
#endif // SEOBSERVER_H_INCLUDED
