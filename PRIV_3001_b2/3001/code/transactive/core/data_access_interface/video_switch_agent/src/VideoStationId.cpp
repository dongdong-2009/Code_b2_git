 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoStationId.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * VideoStationId is an implementation of IVideoStationId. It holds the data specific to an VideoStationId entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/video_switch_agent/src/VideoStationId.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
	VideoStationId::VideoStationId(const unsigned long key)
    {
        m_VideoStationIdHelper = new VideoStationIdHelper(key);
    }

    VideoStationId::VideoStationId(unsigned long row, TA_Base_Core::IData& data)
     : m_VideoStationIdHelper( new VideoStationIdHelper(row, data))
    {
    }

    VideoStationId::~VideoStationId() 
	{
        delete m_VideoStationIdHelper;
        m_VideoStationIdHelper=NULL;
    }

    unsigned long VideoStationId::getKey()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
        return m_VideoStationIdHelper->getKey();
    }

    std::string VideoStationId::getName()
    {
        // just return the station id
        std::ostringstream name;
        name << getStationId();
        return name.str();
    }

	unsigned long VideoStationId::getStationId()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
        return m_VideoStationIdHelper->getStationId();
    }


    unsigned long VideoStationId::getTaLocation()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
        return m_VideoStationIdHelper->getTaLocation();
    }

	void VideoStationId::invalidate()
    {
        TA_ASSERT(m_VideoStationIdHelper != NULL,"The VideoStationIdHelper pointer is null.");
        m_VideoStationIdHelper->invalidate();
    }

} // closes TA_IRS_Core
