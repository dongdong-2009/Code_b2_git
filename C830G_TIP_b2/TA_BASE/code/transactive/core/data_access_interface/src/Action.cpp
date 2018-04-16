/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/Action.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Action is an implementation of IAction. It holds the data specific to an action entry
  * in the database, and allows read-only access to that data.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/src/Action.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    Action::Action( const unsigned long key )
		: m_helper( new ActionHelper(key) )
    {
    }


    Action::Action( const unsigned long key, const std::string& name, const std::string& description )
        : m_helper( new ActionHelper(key,name,description) )
    {
    }

	Action::Action(const unsigned long row, TA_Base_Core::IData& data, std::vector<std::string>& actionGroups)
		: m_helper( new ActionHelper(row, data, actionGroups) )
	{
	}

	unsigned long Action::getKey()
    {
		TA_ASSERT( NULL != m_helper.get(), "m_helper is NULL" );

		return m_helper->getKey();
    }


    std::string Action::getName()
    {
		TA_ASSERT( NULL != m_helper.get(), "m_helper is NULL" );

		return m_helper->getName();
    }

    std::string Action::getDescription()
    {
		TA_ASSERT( NULL != m_helper.get(), "m_helper is NULL" );

		return m_helper->getDescription();
    }

    std::vector<std::string> Action::getActionGroups()
	{
		TA_ASSERT( NULL != m_helper.get(), "m_helper is NULL" );

		return m_helper->getActionGroups();
	}

    void Action::invalidate()
    {
		TA_ASSERT( NULL != m_helper.get(), "m_helper is NULL" );

		m_helper->invalidate();
    }



} // closes TA_Base_Core
