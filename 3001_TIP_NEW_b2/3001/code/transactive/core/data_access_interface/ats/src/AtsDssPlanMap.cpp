 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsDssPlanMap.cpp $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * AtsDssPlanMap is an implementation of IAtsDssPlanMap. It holds the data specific to an AtsDssPlanMap entry
  * in the database, and allows read-only access to that data.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsDssPlanMap.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    AtsDssPlanMap::AtsDssPlanMap(const unsigned long key)
    {
        m_atsDssPlanMapHelper = new AtsDssPlanMapHelper(key);
    }

    AtsDssPlanMap::AtsDssPlanMap(unsigned long row, TA_Base_Core::IData& data)
     : m_atsDssPlanMapHelper( new AtsDssPlanMapHelper(row, data))
    {
    }

    AtsDssPlanMap::~AtsDssPlanMap() 
	{
        delete m_atsDssPlanMapHelper;
        m_atsDssPlanMapHelper=NULL;
    }


	unsigned long AtsDssPlanMap::getDssEvent()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getDssEvent();
    }


	std::string AtsDssPlanMap::getPlanPath()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getPlanPath();
    }


	unsigned long AtsDssPlanMap::getKey()
	{
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getKey();
    }

	std::string AtsDssPlanMap::getName()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getName();
    }


    void AtsDssPlanMap::invalidate()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        m_atsDssPlanMapHelper->invalidate();
    }

} // closes TA_Core
