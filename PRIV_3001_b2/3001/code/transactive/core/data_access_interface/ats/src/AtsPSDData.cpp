/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsPSDData.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * AtsPSDDataHelper is an object that is held by AtsPSDData and ConfigAtsPSDData objects. 
  * It contains all data used by AtsPSDDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPSDData and ConfigAtsPSDData.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsPSDData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
	const std::string AtsPSDData::OUTPUT_DOOR_INHIBIT_STATE		 = "Door Inhibit State";
	const std::string AtsPSDData::OUTPUT_PRE_CLOSE_WARNING		 = "Pre-Close Warning";
	const std::string AtsPSDData::INPUT_DOOR_STATUS				 = "Door Status";

	const std::string AtsPSDData::INPUT_TYPE					 = "Input";
	const std::string AtsPSDData::OUTPUT_TYPE					 = "Output";


	AtsPSDData::AtsPSDData(const unsigned long key)
    {
        m_atsPSDDataHelper = new AtsPSDDataHelper(key);
    }

    AtsPSDData::AtsPSDData(unsigned long row, TA_Base_Core::IData& data)
    {
		m_atsPSDDataHelper = new AtsPSDDataHelper(row, data);
    }

    AtsPSDData::~AtsPSDData() 
	{
        delete m_atsPSDDataHelper;
        m_atsPSDDataHelper=NULL;
    }

	unsigned long AtsPSDData::getKey()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getKey();
	}

	unsigned long AtsPSDData::getPSDDataPointEntityKey()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDDataPointEntityKey();
	}
		
	std::string AtsPSDData::getPSDDataPointType()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDDataPointType();
	}
		
	std::string AtsPSDData::getPSDDataPointNameOfType()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDDataPointNameOfType();
	}

	unsigned long AtsPSDData::getStationId()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getStationId();
	}
		
	unsigned long AtsPSDData::getPlatformId()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPlatformId();
	}
		
	std::string AtsPSDData::getPlatformName()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPlatformName();
	}

    unsigned char AtsPSDData::getTisPlatformId()
    {
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getTisPlatformId();
    }
		
	int AtsPSDData::getPSDNumber()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDNumber();
	}

	std::string AtsPSDData::getMSSZone()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getMSSZone();
	}

	std::string AtsPSDData::getName()
    {
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");

        std::ostringstream name;
		name << m_atsPSDDataHelper->getPlatformId();
        name << " - ";
		name << m_atsPSDDataHelper->getPSDDataPointNameOfType();
		name << "/";
		name << m_atsPSDDataHelper->getPSDNumber();

        return name.str();
    }

	void AtsPSDData::invalidate()
    {
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        m_atsPSDDataHelper->invalidate();
    }

} // TA_Core
