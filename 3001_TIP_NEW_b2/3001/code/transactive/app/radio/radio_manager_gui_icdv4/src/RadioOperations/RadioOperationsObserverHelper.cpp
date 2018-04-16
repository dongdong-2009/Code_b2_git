/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperationsObserverHelper.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperationsObserverHelper.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioOperationsObserverHelper::RadioOperationsObserverHelper()
{
}

RadioOperationsObserverHelper::~RadioOperationsObserverHelper()
{
}

void RadioOperationsObserverHelper::addChildObserver(CWnd* pObserver, unsigned long message)
{
    FUNCTION_ENTRY("addChildObserver");
    
	// check if key(message) exists in map
	ChildCollection::iterator child = m_ChildObservers.find(message);

	if ( child != m_ChildObservers.end())
	{
		boost::shared_ptr<CWndVec> cWndList = (*child).second;

		if (cWndList.use_count() != 0)
		{
			CWndVec* childlist = cWndList.get();
			CWndVec::iterator pos = std::find(childlist->begin(), childlist->end(), pObserver);
			
			// data not found, add new observer
			if(pos == childlist->end()) 
			{
				childlist->push_back(pObserver);
			}
		}
	}
	else
	{
		// if key(message) does not exist in map yet, 
		// create a new vector instance with one element
		boost::shared_ptr<CWndVec> cWndList(new CWndVec(1,pObserver));

		m_ChildObservers[message] = cWndList;
	}
	FUNCTION_EXIT;
}


void RadioOperationsObserverHelper::removeChildObserver(CWnd* pObserver, unsigned long message)
{
    FUNCTION_ENTRY("removeChildObserver");
    
	ChildCollection::iterator child = m_ChildObservers.find(message);

	if ( child != m_ChildObservers.end() )
	{
		boost::shared_ptr<CWndVec> cWndList = (*child).second;

		CWndVec* childlist = cWndList.get();
		CWndVec::iterator pos = std::find(childlist->begin(), childlist->end(), pObserver);
        if (pos != childlist->end())
        {
    		childlist->erase(pos);
        }
	}

	FUNCTION_EXIT;
}

void RadioOperationsObserverHelper::notifyChildWindows(UINT message, WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("notifyChildWindows");

	ChildCollection::iterator child = m_ChildObservers.find(message);

	if ( child != m_ChildObservers.end() )
	{
		boost::shared_ptr<CWndVec> cWndList = (*child).second;

		CWndVec::iterator iter;

		for ( iter = cWndList.get()->begin(); iter != cWndList.get()->end(); iter++ )
		{
			PostMessage( (*iter)->m_hWnd, message, wParam,lParam);
		}	
	}
	
	FUNCTION_EXIT;
}