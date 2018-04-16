 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ECSMasterMode.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ECSMasterMode is an implementation of IECSMasterMode. It holds the data specific to an ECSMasterMode entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif

#include "core/data_access_interface/ecs/src/ECSMasterMode.h"
#include "core/data_access_interface/ecs/src/IECSStationMode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Core
{

    ECSMasterMode::ECSMasterMode(const unsigned long key)
    {
        m_ECSMasterModeHelper = new ECSMasterModeHelper(key);
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSMasterModeHelper pointer is null." ));
		}
	}

    ECSMasterMode::ECSMasterMode(unsigned long row, TA_Base_Core::IData& data)
    {
		m_ECSMasterModeHelper = new ECSMasterModeHelper(row, data);
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSMasterModeHelper pointer is null." ));
		}
    }


    ECSMasterMode::~ECSMasterMode() 
	{
        delete m_ECSMasterModeHelper;
        m_ECSMasterModeHelper=NULL;
    }

    std::string ECSMasterMode::getName()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}

        std::ostringstream name;
		name << "ECS Master Mode " << m_ECSMasterModeHelper->getKey();

        return name.str();
    }

    unsigned long ECSMasterMode::getKey()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSMasterModeHelper->getKey();
    }

    unsigned long ECSMasterMode::getMasterMode()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSMasterModeHelper->getMasterMode();
    }

    std::string ECSMasterMode::getCondition()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getCondition();
    }   

    int ECSMasterMode::getECSZoneId()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSMasterModeHelper->getECSZoneId();
    }
    
    std::string ECSMasterMode::getEvacuationDirection()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getEvacuationDirection();
    }    
    
    std::string ECSMasterMode::getAirflowDirection()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getAirflowDirection();
    }

	std::string ECSMasterMode::getSituation()
	{
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
		return m_ECSMasterModeHelper->getSituation();
	}

    std::string ECSMasterMode::getDisplayDirection()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getDisplayDirection();
    }

	std::string ECSMasterMode::getVerificationMessage()
	{
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
		return m_ECSMasterModeHelper->getVerificationMessage();

	}

    std::vector<IECSStationMode*> ECSMasterMode::getAssociatedStationModes()
	{
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );

			std::vector<IECSStationMode*> vectStationMode;
			vectStationMode.clear();
			return vectStationMode;
		}
		return m_ECSMasterModeHelper->getAssociatedStationModes();
	}

    std::vector<unsigned long> ECSMasterMode::getAssociatedStationModeKeys()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );

            std::vector<unsigned long> vectStationModeKey;
			vectStationModeKey.clear();
			return vectStationModeKey;
		}
		return m_ECSMasterModeHelper->getAssociatedStationModeKeys();
	}

    void ECSMasterMode::invalidate()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return ;
		}
        m_ECSMasterModeHelper->invalidate();
    }

	void ECSMasterMode::setAssociatedStationMode( unsigned long stationModeKey, bool isAssociated)
    {
        FUNCTION_ENTRY("setAssociatedStationMode" );
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return ;
		}

		m_ECSMasterModeHelper->setAssociatedStationMode(stationModeKey, isAssociated);
		
		FUNCTION_EXIT;
    }


} // closes TA_Core
