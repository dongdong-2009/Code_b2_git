 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ECSMasterMode.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ECSMasterMode is an implementation of IECSMasterMode. It holds the data specific to an ECSMasterMode entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif

#include "core/data_access_interface/ecs/src/ECSMasterMode.h"
#include "core/data_access_interface/ecs/src/IECSStationMode.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    ECSMasterMode::ECSMasterMode(const unsigned long key)
     : m_ECSMasterModeHelper ( new ECSMasterModeHelper(key))
    {}

    ECSMasterMode::ECSMasterMode(unsigned long row, TA_Base_Core::IData& data)
     : m_ECSMasterModeHelper( new ECSMasterModeHelper(row, data))
    {
    }


    ECSMasterMode::~ECSMasterMode() 
	{
        delete m_ECSMasterModeHelper;
        m_ECSMasterModeHelper=NULL;
    }

    std::string ECSMasterMode::getName()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The m_ECSMasterModeHelper pointer is null.");

        std::ostringstream name;
		name << "ECS Master Mode " << m_ECSMasterModeHelper->getKey();

        return name.str();
    }

    unsigned long ECSMasterMode::getKey()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getKey();
    }

    unsigned long ECSMasterMode::getMasterMode()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getMasterMode();
    }

    std::string ECSMasterMode::getCondition()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getCondition();
    }    

    int ECSMasterMode::getECSZoneId()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getECSZoneId();
    }
    
    std::string ECSMasterMode::getEvacuationDirection()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getEvacuationDirection();
    }    
    
    std::string ECSMasterMode::getAirflowDirection()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getAirflowDirection();
    }

	std::string ECSMasterMode::getSituation()
	{
		TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getSituation();
	}
    std::string ECSMasterMode::getDisplayDirection()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getDisplayDirection();
    }

	std::string ECSMasterMode::getVerificationMessage()
	{
		TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getVerificationMessage();

	}

    std::vector<IECSStationMode*> ECSMasterMode::getAssociatedStationModes()
	{
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getAssociatedStationModes();
	}

    std::vector<unsigned long> ECSMasterMode::getAssociatedStationModeKeys()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getAssociatedStationModeKeys();
	}

    void ECSMasterMode::invalidate()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        m_ECSMasterModeHelper->invalidate();
    }

	void ECSMasterMode::setAssociatedStationMode( unsigned long stationModeKey, bool isAssociated)
    {
        FUNCTION_ENTRY("setAssociatedStationMode" );
	TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");

		m_ECSMasterModeHelper->setAssociatedStationMode(stationModeKey, isAssociated);
		
		FUNCTION_EXIT;
    }


} // closes TA_Core
