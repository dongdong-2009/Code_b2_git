

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventSubject.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventSubject.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventObserver.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>



namespace TA_IRS_App {


	CADIEventSubject::CADIEventSubject()
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	CADIEventSubject::~CADIEventSubject()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	void CADIEventSubject::registerObserver(CADIEventObserver& o)
	{
		FUNCTION_ENTRY("registerObserver");

		std::vector<CADIEventObserver*>::iterator pos;

		// find in the observer list (m_observers)
		pos = std::find(m_observers.begin(), m_observers.begin()+m_observers.size(), &o);

		// data not found, add new observer
		if(pos == m_observers.end()) m_observers.push_back(&o);

		FUNCTION_EXIT;
	}

	void CADIEventSubject::deregisterObserver(const CADIEventObserver& o)
	{
		FUNCTION_ENTRY("deregisterObserver");

		// find in the observer list (m_observers)
		std::vector<CADIEventObserver*>::iterator pos;
		pos = std::find(m_observers.begin(), m_observers.begin()+m_observers.size(), &o);

		// if found, remove from list (m_observers)
		if(pos != m_observers.end()) m_observers.erase(pos);

		FUNCTION_EXIT;
	}

	void CADIEventSubject::notifyObservers(CADIEvent& e)
	{
		FUNCTION_ENTRY("notifyObservers");

		// send event to all observers
		std::vector<CADIEventObserver*>::iterator pos;
		for(pos=m_observers.begin(); pos!=m_observers.end(); pos++)
		{
			(*pos)->notify(e);
		}

		FUNCTION_EXIT;
	}
}