 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaZone.cpp $
  * @author Jade Lee
  * @version $Revision: #5 $
  * Last modification: $DateTime: 2013/09/26 14:59:27 $
  * Last modified by: $Author: huangjian $
  * 
  * PaZone is an implementation of IPaZone. It holds the data specific to an PaZone entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/pa_4669/src/PaZone.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    PaZone::PaZone(const unsigned long key)
     : m_paZoneHelper ( new PaZoneHelper(key))
    {}


    PaZone::PaZone(unsigned long row, TA_Base_Core::IData& data)
     : m_paZoneHelper( new PaZoneHelper(row, data))
    {
    }


    PaZone::~PaZone() 
	{
        delete m_paZoneHelper;
        m_paZoneHelper=NULL;
    }

    unsigned long PaZone::getKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getKey();
    }

    unsigned long PaZone::getLocationKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getLocationKey();
    }

    unsigned long PaZone::getId()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getId();
    }

    std::string PaZone::getLabel()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getLabel();
    }    
    
    std::string PaZone::getStnEntityName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getStnEntityName();
    }    
    
    std::string PaZone::getOccEntityName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getOccEntityName();
    }    

/*
    unsigned long PaZone::getStatusEntityKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getStatusEntityKey();
    }
*/

    //std::list<std::string> PaZone::getAssociatedEquipmentEntities()
    //{
    //    TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
    //    return m_paZoneHelper->getAssociatedEquipmentEntities();
    //}

    void PaZone::invalidate()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        m_paZoneHelper->invalidate();
    }

    std::string PaZone::getName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The m_paZoneHelper pointer is null.");

        std::ostringstream name;
		name << "PA Zone " << m_paZoneHelper->getKey();

        return name.str();
    }
} // closes TA_Base_Core
