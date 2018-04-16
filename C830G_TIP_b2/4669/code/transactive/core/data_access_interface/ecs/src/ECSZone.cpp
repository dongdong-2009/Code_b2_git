 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ecs/src/ECSZone.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ECSZone is an implementation of IECSZone. It holds the data specific to an ECSZone entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/ecs/src/ECSZone.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    ECSZone::ECSZone(const unsigned long key)
    {
        m_ECSZoneHelper = new ECSZoneHelper(key);
    }

    ECSZone::ECSZone(unsigned long row, TA_Base_Core::IData& data)
     : m_ECSZoneHelper( new ECSZoneHelper(row, data))
    {
    }


    ECSZone::~ECSZone() 
	{
        delete m_ECSZoneHelper;
        m_ECSZoneHelper=NULL;
    }


    int ECSZone::getECSZoneId()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");
        return m_ECSZoneHelper->getECSZoneId();
    }

    std::string ECSZone::getECSLocation()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");
        return m_ECSZoneHelper->getECSLocation();
    }

    std::string ECSZone::getTrack()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");
        return m_ECSZoneHelper->getTrack();
    }
    
    unsigned long ECSZone::getOccEquipmentEntityKey()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");
        return m_ECSZoneHelper->getOccEquipmentEntityKey();
    }

    unsigned long ECSZone::getStnEquipmentEntityKey()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");
        return m_ECSZoneHelper->getStnEquipmentEntityKey();
    }

    bool ECSZone::getIsIgnoringCongestion()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");
        return m_ECSZoneHelper->getIsIgnoringCongestion();
    }

    std::string ECSZone::getName()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");

        std::string name = m_ECSZoneHelper->getECSLocation();
        name += " - " + m_ECSZoneHelper->getTrack();

        return name;
    }
    

    void ECSZone::invalidate()
    {
        TA_ASSERT(m_ECSZoneHelper != NULL,"The ECSZoneHelper pointer is null.");
        m_ECSZoneHelper->invalidate();
    }

} // closes TA_Core
