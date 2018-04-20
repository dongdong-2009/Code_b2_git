/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/radio/src/ITSIGroup.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * ITSIGroup is an implementation of IITSIGroup. It holds the data specific to an ITSIGroup entry
  * in the database, and allows read-only access to that data.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/radio/src/ITSIGroup.h"

namespace TA_Base_Core
{
	const std::string ITSIGroup::ITSI_GROUP = "ITSI";

    ITSIGroup::ITSIGroup( const unsigned long key )
		: m_key( key),
        m_name("")
    {
		m_ITSIGroupHelper = new ProfileGroupHelper(ITSIGroup::ITSI_GROUP, m_key);
    }


    ITSIGroup::ITSIGroup(const unsigned long key, const std::string& name )
        : m_key( key ),
        m_name( name )
    {
		m_ITSIGroupHelper = new ProfileGroupHelper(ITSIGroup::ITSI_GROUP, m_key);
    }

	ITSIGroup::~ITSIGroup()
	{
		if ( m_ITSIGroupHelper != NULL )
		{
			delete m_ITSIGroupHelper;
			m_ITSIGroupHelper = NULL;
		}
	}

	unsigned long ITSIGroup::getKey()
    {
		return m_key;
    }


    std::string ITSIGroup::getName()
    {
        if (m_name.empty()) m_name = m_ITSIGroupHelper->getName(); 
		return m_name;
    }

    void ITSIGroup::invalidate()
    {
		m_ITSIGroupHelper->invalidate();
		m_name = "";
    }


    bool ITSIGroup::isGroupUsedInProfile(unsigned long key)
    {
		return m_ITSIGroupHelper->isGroupUsedInProfile(key);
    }

} // closes TA_Base_Core
