 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ecs/src/ECSStationMode.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ECSStationMode is an implementation of IECSStationMode. It holds the data specific to an ECSStationMode entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/ecs/src/ECSStationMode.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    ECSStationMode::ECSStationMode(const unsigned long key)
     : m_ECSStationModeHelper ( new ECSStationModeHelper(key))
    {}

    ECSStationMode::ECSStationMode(unsigned long row, TA_Base_Core::IData& data)
     : m_ECSStationModeHelper( new ECSStationModeHelper(row, data))
    {
    }


    ECSStationMode::~ECSStationMode() 
	{
        delete m_ECSStationModeHelper;
        m_ECSStationModeHelper=NULL;
    }

    std::string ECSStationMode::getName()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The m_ECSStationModeHelper pointer is null.");

        std::ostringstream name;
		name << "ECS Station Mode " << m_ECSStationModeHelper->getKey();

        return name.str();
    }

    unsigned long ECSStationMode::getKey()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getKey();
    }

    unsigned long ECSStationMode::getStationMode()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getStationMode();
    }

    unsigned long ECSStationMode::getLocationKey()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getLocationKey();
    }

    std::string ECSStationMode::getOperation()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getOperation();
    }    
    
    std::string ECSStationMode::getTrack()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getTrack();
    }    
    
    std::string ECSStationMode::getCondition()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getCondition();
    }
    
    std::string ECSStationMode::getCongestionLogic()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        return m_ECSStationModeHelper->getCongestionLogic();
    }

    void ECSStationMode::invalidate()
    {
        TA_ASSERT(m_ECSStationModeHelper != NULL,"The ECSStationModeHelper pointer is null.");
        m_ECSStationModeHelper->invalidate();
    }

} // closes TA_Core
