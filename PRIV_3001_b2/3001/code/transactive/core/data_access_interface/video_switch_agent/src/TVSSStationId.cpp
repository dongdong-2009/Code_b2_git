 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/video_switch_agent/src/TVSSStationId.cpp $
  * @author Zhang hongzhi
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/02/5 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * TVSSStationId is an implementation of ITVSSStationId. It holds the data specific to an TVSSStationId entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/video_switch_agent/src/TVSSStationId.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
	TVSSStationId::TVSSStationId(const unsigned long key)
    {
        m_TVSSStationIdHelper = new TVSSStationIdHelper(key);
    }

    TVSSStationId::TVSSStationId(unsigned long row, TA_Base_Core::IData& data)
     : m_TVSSStationIdHelper( new TVSSStationIdHelper(row, data))
    {
    }

    TVSSStationId::~TVSSStationId() 
	{
        delete m_TVSSStationIdHelper;
        m_TVSSStationIdHelper=NULL;
    }

    unsigned long TVSSStationId::getKey()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
        return m_TVSSStationIdHelper->getKey();
    }

    std::string TVSSStationId::getName()
    {
        // just return the station id
        std::ostringstream name;
        name << getStationId();
        return name.str();
    }

	unsigned long TVSSStationId::getStationId()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
        return m_TVSSStationIdHelper->getStationId();
    }


    unsigned long TVSSStationId::getTaLocation()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
        return m_TVSSStationIdHelper->getTaLocation();
    }

	void TVSSStationId::invalidate()
    {
        TA_ASSERT(m_TVSSStationIdHelper != NULL,"The TVSSStationIdHelper pointer is null.");
        m_TVSSStationIdHelper->invalidate();
    }

} // closes TA_IRS_Core
