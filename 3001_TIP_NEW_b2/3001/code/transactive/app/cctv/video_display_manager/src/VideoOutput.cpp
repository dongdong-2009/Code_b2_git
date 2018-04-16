/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/VideoOutput.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/05/14 17:41:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "BvsStage.h"
#include "DisplayItemManager.h"
#include "VideoDisplayManagerDlg.h"
#include "VideoInput.h"
#include "Camera.h"
#include "VideoOutput.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/cctv/video_display_manager/src/TriggeringEventManager.h"
#include <algorithm>
#include <sstream>


using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_IRS_Core::IVideoTriggeringEvent;

namespace TA_IRS_App
{

    VideoOutput::VideoOutput( unsigned long entityKey,
                              const std::string& name,
                              const std::string& description )
      : DisplayItem(entityKey, name, description),
        m_currentInput(NULL),
        m_alarmStackEntries(),
		m_bQuadInput(false),
		m_bShowingAckAlarm(false),
		m_bTriggeringAlarm(false)
    {
        FUNCTION_ENTRY( "VideoOutput" );
		m_quadCameraList.clear();
		m_quadCameraList.push_back(NULL);
		m_quadCameraList.push_back(NULL);
		m_quadCameraList.push_back(NULL);
		m_quadCameraList.push_back(NULL);
        FUNCTION_EXIT;
    }

	VideoOutput::~VideoOutput()
	{
		FUNCTION_ENTRY( "~VideoOutput" );
		m_quadCameraList.clear();
		FUNCTION_EXIT;
	}

    void VideoOutput::assignInput(VideoInput* videoInput)
    {
        FUNCTION_ENTRY( "assignInput" );

        ThreadGuard guard( m_lock );
        m_currentInput = videoInput;
		m_quadCameraList.clear();
		m_quadCameraList.push_back(NULL);
		m_quadCameraList.push_back(NULL);
		m_quadCameraList.push_back(NULL);
		m_quadCameraList.push_back(NULL);
		m_bQuadInput = false;
        FUNCTION_EXIT;
    }

	void VideoOutput::assignQuad(Camera* cameraW,Camera* cameraX,Camera* cameraY,Camera* cameraZ)
	{
		FUNCTION_ENTRY( "assignQuad" );

		ThreadGuard guard( m_lock );
		m_quadCameraList.clear();
		m_quadCameraList.push_back(cameraW);
		m_quadCameraList.push_back(cameraX);
		m_quadCameraList.push_back(cameraY);
		m_quadCameraList.push_back(cameraZ);
		m_currentInput = NULL;
		m_bQuadInput = true;
		FUNCTION_EXIT;
	}

    VideoInput* VideoOutput::getInput()
    {
        FUNCTION_ENTRY( "getInput" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_currentInput;
    }

	bool VideoOutput::isQuadInput()
	{
		FUNCTION_ENTRY( "getInput" );

		ThreadGuard guard( m_lock );

		FUNCTION_EXIT;
		return m_bQuadInput;
	}

	std::vector<Camera*> VideoOutput::getQuadCameras()
	{
		FUNCTION_ENTRY( "getInput" );
		ThreadGuard guard( m_lock );
		FUNCTION_EXIT;
		return m_quadCameraList;
	}

    std::vector<VideoOutput::CCTVTriggeredAlarm> VideoOutput::getAlarmStackItems()
    {
        FUNCTION_ENTRY( "getAlarmStackItems" );
        ThreadGuard guard( m_lock );
        FUNCTION_EXIT;
        return m_alarmStackEntries;
    }


    void VideoOutput::setAlarmStackItems(TA_Base_Bus::VideoOutputCorbaDef::MonitorAlarmStack alarmStack)
    {
        FUNCTION_ENTRY( "setAlarmStackItems" );

        ThreadGuard guard( m_lock );
		m_alarmStackEntries.clear();
        m_currentInput = NULL;
        // load the triggering events from the database
        for (unsigned int i=0; i<alarmStack.length(); ++i)
        {
            CCTVTriggeredAlarm alarm;
            alarm.stationId = alarmStack[i].stationId;
            if(alarmStack[i].eventId == 0)
            {
                alarm.locationName = "Train";
                alarm.eventId = 0;
                alarm.locationDesc = "Camera Alarm";
                std::stringstream desc;
            }
            else
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Process event %lu:%lu",alarmStack[i].stationId, alarmStack[i].eventId);
				IVideoTriggeringEvent* event = TriggeringEventManager::instance().getVideoTriggeringEvent(alarmStack[i].stationId, alarmStack[i].eventId);
				if(NULL != event)
                {
					alarm.locationName = TriggeringEventManager::instance().getLocationNameByStationId(alarmStack[i].stationId);
                    // there should never be no match - but just in case
                    if (alarm.locationName == "")
                    {
                        alarm.locationName = "(Error resolving location name)";
                    }
					unsigned long stationId = event->getStationId();
                    alarm.eventId = event->getEventId();
                    alarm.locationDesc = event->getLocation();
                    alarm.triggeringEventDesc = event->getCctvTriggering();
                }
				else
                {
                    // insert unknown event details
                    alarm.locationName = "Unknown";
                    alarm.eventId = 0;
                    alarm.locationDesc = "Unknown Location";
                    alarm.triggeringEventDesc = "Unknown Triggering Event - Not Configured";
                }
            }
			m_alarmStackEntries.push_back(alarm);
        }

        FUNCTION_EXIT;
    }

    bool VideoOutput::isInAlarmMode()
    {
        FUNCTION_ENTRY( "isInAlarmMode" );
        FUNCTION_EXIT;
        ThreadGuard guard( m_lock );
        return (m_alarmStackEntries.size() > 0);
    }


    void VideoOutput::clearAlarmStack()
    {
        FUNCTION_ENTRY( "clearAlarmStack" );

        ThreadGuard guard( m_lock );

        // clear the triggering events
        m_alarmStackEntries.clear();

        FUNCTION_EXIT;
    }


    std::string VideoOutput::getDetails()
    {
        FUNCTION_ENTRY( "getDetails" );

        // reentrant lock - parent class uses the same lock
        ThreadGuard guard( m_lock );

        // get the item details
        std::ostringstream detailsStr;
        detailsStr << DisplayItem::getDetails() << ":" << "\r\n";

        // details of alarm stack - if any
        if (m_alarmStackEntries.size() > 0)
        {
            detailsStr << "Monitor Alarm Stack:" << "\r\n";

            // details of current alarm stack
            int i = 1;
            for (std::vector<CCTVTriggeredAlarm>::iterator iter = m_alarmStackEntries.begin(); iter != m_alarmStackEntries.end(); iter++, i++)
            {
                detailsStr << i << ": " ;
                if(iter->triggeringEventDesc != "Unknown Triggering Event - Not Configured " )
                {
                    detailsStr << iter->locationName
                    << ", " ;
                }
                detailsStr << iter->triggeringEventDesc << "\r\n";
            }
        }
        // details of current assignment - if any
		else if( m_bQuadInput )
		{
			detailsStr << "QUAD \r\n";
			detailsStr << "1: " <<(m_quadCameraList[0]!=NULL?m_quadCameraList[0]->getDetails():" ") << "\r\n";
			detailsStr << "2: " <<(m_quadCameraList[1]!=NULL?m_quadCameraList[1]->getDetails():" ") << "\r\n";
			detailsStr << "3: " <<(m_quadCameraList[2]!=NULL?m_quadCameraList[2]->getDetails():" ") << "\r\n";
			detailsStr << "4: " <<(m_quadCameraList[3]!=NULL?m_quadCameraList[3]->getDetails():" ") << "\r\n";
			
		}
        else if (m_currentInput != NULL)
        {
            detailsStr << m_currentInput->getDetails();
        }
        else
        {
            detailsStr << "No current assignment";
        }

        FUNCTION_EXIT;
        return detailsStr.str();
    }


} // TA_App
