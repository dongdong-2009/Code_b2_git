 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsMssZone.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * AtsMssZone is an implementation of IAtsMssZone. It holds the data specific to an AtsMssZone entry
  * in the database, and allows read-only access to that data.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsMssZone.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    AtsMssZone::AtsMssZone(const unsigned long key)
    {
        m_atsMssZoneHelper = new AtsMssZoneHelper(key);
    }

    AtsMssZone::AtsMssZone(unsigned long row, TA_Base_Core::IData& data)
     : m_atsMssZoneHelper( new AtsMssZoneHelper(row, data))
    {
    }

    AtsMssZone::~AtsMssZone() 
	{
        delete m_atsMssZoneHelper;
        m_atsMssZoneHelper=NULL;
    }


	unsigned long AtsMssZone::getStationId()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        return m_atsMssZoneHelper->getStationId();
    }


    std::string AtsMssZone::getMssZone()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        return m_atsMssZoneHelper->getMssZone();
    }

	unsigned long AtsMssZone::getKey()
	{
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        return m_atsMssZoneHelper->getKey();
    }

	std::string AtsMssZone::getName()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");

        std::ostringstream name;
		name << m_atsMssZoneHelper->getStationId();
        name << " - ";
		name << m_atsMssZoneHelper->getMssZone();

        return name.str();
    }


    void AtsMssZone::invalidate()
    {
        TA_ASSERT(m_atsMssZoneHelper != NULL,"The AtsMssZoneHelper pointer is null.");
        m_atsMssZoneHelper->invalidate();
    }

} // closes TA_Core
