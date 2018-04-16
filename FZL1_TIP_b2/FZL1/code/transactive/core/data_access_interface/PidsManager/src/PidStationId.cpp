 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/PidStationId.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * PidStationId is an implementation of IPidStationId. It holds the data specific to an PidStationId entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/PidsManager/src/PidStationId.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
	PidStationId::PidStationId(const unsigned long key)
    {
        m_PidStationIdHelper = new PidStationIdHelper(key);
    }

    PidStationId::PidStationId(unsigned long row, TA_Base_Core::IData& data)
     : m_PidStationIdHelper( new PidStationIdHelper(row, data))
    {
    }

    PidStationId::~PidStationId() 
	{
        delete m_PidStationIdHelper;
        m_PidStationIdHelper=NULL;
    }

    unsigned long PidStationId::getKey()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
        return m_PidStationIdHelper->getKey();
    }

    std::string PidStationId::getName()
    {
        // just return the station id
        std::ostringstream name;
        name << getStationId();
        return name.str();
    }

	unsigned long PidStationId::getStationId()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
        return m_PidStationIdHelper->getStationId();
    }


    unsigned long PidStationId::getTaLocation()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
        return m_PidStationIdHelper->getTaLocation();
    }

	void PidStationId::invalidate()
    {
        TA_ASSERT(m_PidStationIdHelper != NULL,"The PidStationIdHelper pointer is null.");
        m_PidStationIdHelper->invalidate();
    }

} // closes TA_IRS_Core
