 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/ECSZone.cpp $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2013/09/25 14:02:39 $
  * Last modified by: $Author: huangjian $
  * 
  * ECSZone is an implementation of IECSZone. It holds the data specific to an ECSZone entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/ecs/src/ECSZone.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseKey.h"

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
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSZoneHelper->getECSZoneId();
    }

    std::string ECSZone::getECSLocation()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return "";
		}
        return m_ECSZoneHelper->getECSLocation();
    }

    std::string ECSZone::getTrack()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return "";
		}
        return m_ECSZoneHelper->getTrack();
    }
    
    unsigned long ECSZone::getOccEquipmentEntityKey()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSZoneHelper->getOccEquipmentEntityKey();
    }

    unsigned long ECSZone::getStnEquipmentEntityKey()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSZoneHelper->getStnEquipmentEntityKey();
    }

    bool ECSZone::getIsIgnoringCongestion()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return false;
		}
        return m_ECSZoneHelper->getIsIgnoringCongestion();
    }

    std::string ECSZone::getName()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return "";
		}

        std::string name = m_ECSZoneHelper->getECSLocation();
        name += " - " + m_ECSZoneHelper->getTrack();

        return name;
    }
    

    void ECSZone::invalidate()
    {
		if (NULL == m_ECSZoneHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSZoneHelper pointer is null." );
			return;
		}
        m_ECSZoneHelper->invalidate();
    }

} // closes TA_Core
