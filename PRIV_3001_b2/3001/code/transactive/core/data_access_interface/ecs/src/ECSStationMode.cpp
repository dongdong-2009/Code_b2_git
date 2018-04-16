 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ECSStationMode.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ECSStationMode is an implementation of IECSStationMode. It holds the data specific to an ECSStationMode entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/ecs/src/ECSStationMode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Core
{

    ECSStationMode::ECSStationMode(const unsigned long key)
    {
		m_ECSStationModeHelper = new ECSStationModeHelper(key);
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSStationModeHelper pointer is null." ));
		}
	}

    ECSStationMode::ECSStationMode(unsigned long row, TA_Base_Core::IData& data)
    {
		m_ECSStationModeHelper = new ECSStationModeHelper(row, data);
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSStationModeHelper pointer is null." ));
		}
    }


    ECSStationMode::~ECSStationMode() 
	{
        delete m_ECSStationModeHelper;
        m_ECSStationModeHelper=NULL;
    }

    std::string ECSStationMode::getName()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return "";
		}

        std::ostringstream name;
		name << "ECS Station Mode " << m_ECSStationModeHelper->getKey();

        return name.str();
    }

    unsigned long ECSStationMode::getKey()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSStationModeHelper->getKey();
    }

    unsigned long ECSStationMode::getStationMode()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSStationModeHelper->getStationMode();
    }

    unsigned long ECSStationMode::getLocationKey()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSStationModeHelper->getLocationKey();
    }

    std::string ECSStationMode::getOperation()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return "";
		}
        return m_ECSStationModeHelper->getOperation();
    }    
    
    std::string ECSStationMode::getTrack()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return "";
		}
        return m_ECSStationModeHelper->getTrack();
    }    
    
    std::string ECSStationMode::getCondition()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return "";
		}
        return m_ECSStationModeHelper->getCondition();
    }
    
    std::string ECSStationMode::getCongestionLogic()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return "";
		}
        return m_ECSStationModeHelper->getCongestionLogic();
    }

    void ECSStationMode::invalidate()
    {
		if (NULL == m_ECSStationModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSStationModeHelper pointer is null." );
			return ;
		}
        m_ECSStationModeHelper->invalidate();
    }

} // closes TA_Core
