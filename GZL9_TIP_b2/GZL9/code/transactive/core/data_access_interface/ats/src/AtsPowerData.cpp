/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsPowerData.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsPowerDataHelper is an object that is held by AtsPowerData and ConfigAtsPowerData objects. 
  * It contains all data used by AtsPowerDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPowerData and ConfigAtsPowerData.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsPowerData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
	AtsPowerData::AtsPowerData(const unsigned long key)
    {
        m_atsPowerDataHelper = new AtsPowerDataHelper(key);
    }

    AtsPowerData::AtsPowerData(unsigned long row, TA_Base_Core::IData& data)
    {
		m_atsPowerDataHelper = new AtsPowerDataHelper(row, data);
    }

    AtsPowerData::~AtsPowerData() 
	{
        delete m_atsPowerDataHelper;
        m_atsPowerDataHelper=NULL;
    }

	unsigned long AtsPowerData::getKey()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getKey();
	}

	unsigned long AtsPowerData::getTractionPowerDataPointEntityKey()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getTractionPowerDataPointEntityKey();
	}

	unsigned long AtsPowerData::getTractionPowerBitPosition()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getBitPosition();
	}

	unsigned long AtsPowerData::getElectricalSubSectionId()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getElectricalSubSectionId();
	}
	
	unsigned long AtsPowerData::getLocationId()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getLocationId();
	}
	
	std::string AtsPowerData::getMSSZone()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getMSSZone();
	}

	std::string AtsPowerData::getName()
    {
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        std::ostringstream name;
		name << m_atsPowerDataHelper->getLocationId();
        name << " - ";
		name << m_atsPowerDataHelper->getElectricalSubSectionId();

        return name.str();
    }

	void AtsPowerData::invalidate()
    {
        TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        m_atsPowerDataHelper->invalidate();
    }


} // TA_Core
