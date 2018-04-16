 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoTriggeringEvent.cpp $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by: $Author: weikun.lin $
  * 
  * VideoTriggeringEvent is an implementation of IVideoTriggeringEvent. It holds the data specific to an VideoTriggeringEvent entry
  * in the database, and allows read-only access to that data.
  */

#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEvent.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{
	VideoTriggeringEvent::VideoTriggeringEvent(const unsigned long key)
    {
        m_VideoTriggeringEventHelper = new VideoTriggeringEventHelper(key);
    }

    VideoTriggeringEvent::VideoTriggeringEvent(unsigned long row, TA_Base_Core::IData& data)
     : m_VideoTriggeringEventHelper( new VideoTriggeringEventHelper(row, data))
    {
    }

    VideoTriggeringEvent::~VideoTriggeringEvent() 
	{
        delete m_VideoTriggeringEventHelper;
        m_VideoTriggeringEventHelper=NULL;
    }

    unsigned long VideoTriggeringEvent::getKey()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getKey();
    }

    std::string VideoTriggeringEvent::getName()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getName();
    }

	unsigned long VideoTriggeringEvent::getStationId()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getStationId();
    }


    unsigned long VideoTriggeringEvent::getEventId()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getEventId();
    }

	std::string VideoTriggeringEvent::getLocation()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getLocation();
    }


	std::string VideoTriggeringEvent::getCctvTriggering()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getTriggeringEvent();
    }

    bool VideoTriggeringEvent::getManualClosing()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getManualClosing();
    }

	// TD 15200
	unsigned long VideoTriggeringEvent::getEntityKey()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        return m_VideoTriggeringEventHelper->getEntityKey();
    }
	// TD 15200

	void VideoTriggeringEvent::invalidate()
    {
        TA_ASSERT(m_VideoTriggeringEventHelper != NULL,"The VideoTriggeringEventHelper pointer is null.");
        m_VideoTriggeringEventHelper->invalidate();
    }

} // closes TA_IRS_Core
