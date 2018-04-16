 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsPlatform.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * AtsPlatform is an implementation of IAtsPlatform. It holds the data specific to an AtsPlatform entry
  * in the database, and allows read-only access to that data.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsPlatform.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
    AtsPlatform::AtsPlatform(const unsigned long key)
    {
        m_atsPlatformHelper = new AtsPlatformHelper(key);
    }

    AtsPlatform::AtsPlatform(unsigned long row, TA_Base_Core::IData& data)
     : m_atsPlatformHelper( new AtsPlatformHelper(row, data))
    {
    }

    AtsPlatform::~AtsPlatform() 
	{
        delete m_atsPlatformHelper;
        m_atsPlatformHelper=NULL;
    }


	unsigned long AtsPlatform::getStationId()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getStationId();
    }


    unsigned long AtsPlatform::getPlatformId()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getPlatformId();
    }


    unsigned char AtsPlatform::getTisPlatformId()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getTisPlatformId();
    }


	unsigned long AtsPlatform::getKey()
	{
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getKey();
    }

	std::string AtsPlatform::getName()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        return m_atsPlatformHelper->getName();
    }


    void AtsPlatform::invalidate()
    {
        TA_ASSERT(m_atsPlatformHelper != NULL,"The AtsPlatformHelper pointer is null.");
        m_atsPlatformHelper->invalidate();
    }

} // closes TA_Core
