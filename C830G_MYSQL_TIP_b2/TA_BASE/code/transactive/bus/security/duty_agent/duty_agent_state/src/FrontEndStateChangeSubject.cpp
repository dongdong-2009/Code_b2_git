/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  Implements FrontEndStateChangeSubject.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	void FrontEndStateChangeSubjectImpl::update( const DutyState & state )
	{
		FUNCTION_ENTRY("update");
		if( state.isEmpty() )
		{
			FUNCTION_EXIT;
			return;
		}

 		boost::shared_ptr<DutyState> pState(new DutyState(state));
 		boost::shared_ptr<IWorkItem> workItem(new FrontEndStateChangeSubjectWorkItem(*this, pState));
		m_threadPool->queueWorkItem(workItem);

		//this->notify(*this, state);

		FUNCTION_EXIT;
	}
	FrontEndStateChangeSubjectImpl::FrontEndStateChangeSubjectImpl():
	m_threadPool(0)
	{
		m_threadPool = TA_Base_Core::ThreadPoolSingletonManager::getInstance();
	}
	FrontEndStateChangeSubjectImpl::~FrontEndStateChangeSubjectImpl()
	{
		TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPool);
		m_threadPool = 0;
	}
	FrontEndStateChangeSubjectWorkItem::FrontEndStateChangeSubjectWorkItem(FrontEndStateChangeSubjectImpl& parent,boost::shared_ptr<DutyState> state):
		parent_(parent), state_(state)
	{

	}
	FrontEndStateChangeSubjectWorkItem::~FrontEndStateChangeSubjectWorkItem()
	{

	}
	void FrontEndStateChangeSubjectWorkItem::executeWorkItem()
	{
		parent_.notify(parent_, *state_);
	}

}
