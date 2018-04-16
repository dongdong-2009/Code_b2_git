/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/Subsystem.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/06/07 16:40:53 $
  * Last modified by:  $Author: hui.wang $
  * 
  * Subsystem is an implementation of ISubsystem. It holds the data specific to an Subsystem entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/Subsystem.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

	Subsystem::Subsystem( const unsigned long key )
        : m_helper(new SubsystemHelper(key))
	{
	}


	Subsystem::Subsystem(const unsigned long row, TA_Base_Core::IData& data)
		: m_helper( new SubsystemHelper(row, data) )
	{
	}


    Subsystem::~Subsystem()
    {
        try
        {
            if (m_helper != NULL)
            {
                delete m_helper;
                m_helper = NULL;
            }
        }
		//exception
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    unsigned long Subsystem::getKey()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getKey();
    }

	unsigned long Subsystem::getSystemKey()
	{
		TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

		return m_helper->getSystemKey();
	}

    std::string Subsystem::getName()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getName();
    }

    bool Subsystem::isPhysical()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->isPhysical();
    }

	bool Subsystem::isExclusive()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->isExclusive();
    }

    time_t Subsystem::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getDateCreated();
    }

    
    time_t Subsystem::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getDateModified();
    }


    void Subsystem::invalidate()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        m_helper->invalidate();
    }



} // closes TA_Base_Core
