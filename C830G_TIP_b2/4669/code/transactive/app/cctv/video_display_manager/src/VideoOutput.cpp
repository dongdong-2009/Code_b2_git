/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/VideoOutput.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "BvsStage.h"
#include "DisplayItemManager.h"
#include "VideoDisplayManagerDlg.h"
#include "VideoInput.h"
#include "VideoOutput.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>
#include <sstream>


using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

using TA_IRS_Core::VideoTriggeringEventAccessFactory;
using TA_IRS_Core::IVideoTriggeringEvent;
using TA_IRS_Core::VideoStationIdAccessFactory;
typedef TA_IRS_Core::VideoStationIdAccessFactory::StationIdToLocationKeyMap StationIdToLocationKeyMap;

using TA_IRS_Bus::LocationCache;

namespace TA_IRS_App
{

    VideoOutput::VideoOutput( unsigned long entityKey,
                              const std::string& name,
                              const std::string& description )
      : DisplayItem(entityKey, name, description),
        m_currentInput(NULL),
        m_alarmStackEntries(),
        m_name(name)
    {
        FUNCTION_ENTRY( "VideoOutput" );
        FUNCTION_EXIT;
    }


    void VideoOutput::assignInput(VideoInput* videoInput)
    {
        FUNCTION_ENTRY( "assignInput" );

        ThreadGuard guard( m_lock );
        m_currentInput = videoInput;

        FUNCTION_EXIT;
    }


    VideoInput* VideoOutput::getInput()
    {
        FUNCTION_ENTRY( "getInput" );

        ThreadGuard guard( m_lock );

        FUNCTION_EXIT;
        return m_currentInput;
    }


    std::list<VideoOutput::CCTVTriggeredAlarm> VideoOutput::getAlarmStackItems()
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

        // clear the triggering events
        m_alarmStackEntries.clear();

        // clear any assignments
        m_currentInput = NULL;

        // load the video stations from the database
        StationIdToLocationKeyMap stationIdToLocationKeyMap;
        try
        {
            stationIdToLocationKeyMap =
                VideoStationIdAccessFactory::getInstance().getStationIdToLocationKeyMap();

        }
        catch (TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
        }

        // now make a map to location names from this map
        std::map<unsigned long, std::string> stationIdToLocationNameMap;
        for(StationIdToLocationKeyMap::iterator it = stationIdToLocationKeyMap.begin();
        it != stationIdToLocationKeyMap.end(); it++)
        {
            try
            {
                stationIdToLocationNameMap[it->first] =
                    LocationCache::getInstance().getLocationByKey(it->second);
            }
            catch (...)
            {
                // this is impossible - it is prevented by foreign key constriants
                // within the database.
            }
        }

        // load the triggering events from the database
        for (unsigned int i=0; i<alarmStack.length(); i++)
        {
            CCTVTriggeredAlarm alarm;

            // load the triggering event
            IVideoTriggeringEvent* event = NULL;

            alarm.stationId = alarmStack[i].stationId;

            if(alarmStack[i].eventId == 0)
            {
                alarm.locationName = "Train";

                alarm.eventId = 0;
                alarm.locationDesc = "Camera Alarm";
                std::stringstream desc;
                BvsStage* stage = DisplayItemManager::getInstance().getBvsStage(alarmStack[i].stationId);
                if( stage != NULL )
                {
                    desc << "Train: " << stage->getActiveTrain();

                    if(stage->getActiveVideoSource() == BvsStage::Single)
                    {
                        desc << "\r\n Single Camera: " << stage->getAssignment1();
                    }
                    else if(stage->getActiveVideoSource() == BvsStage::Quad)
                    {
                        desc << "\r\n 1: ";
                        if(stage->getAssignment1() != 0)
                        {
                            desc << "Camera " << stage->getAssignment1();
                        }
                        else
                        {
                            desc << "parked";
                        }
                        desc << "\r\n 2: ";
                        if(stage->getAssignment2() != 0)
                        {
                            desc << "Camera " << stage->getAssignment2();
                        }
                        else
                        {
                            desc << "parked";
                        }
                        desc << "\r\n 3: ";
                        if(stage->getAssignment3() != 0)
                        {
                            desc << "Camera " << stage->getAssignment3();
                        }
                        else
                        {
                            desc << "parked";
                        }
                        desc << "\r\n 4: ";
                        if(stage->getAssignment4() != 0)
                        {
                            desc << "Camera " << stage->getAssignment4();
                        }
                        else
                        {
                            desc << "parked";
                        }
                    }
                    else if(stage->getActiveVideoSource() == BvsStage::Sequence)
                    {
                        desc << "\r\n Sequence: " << stage->getAssignment1();
                    }
                    else
                    {
                        desc << "\r\n Error: train is not active" ;
                    }
                }
                else
                {
                    desc << "Invalid BVS Stage";
                }

                alarm.triggeringEventDesc = desc.str();
            }
            else
            {

                try
                {
                    event = VideoTriggeringEventAccessFactory::getInstance().getVideoTriggeringEvent(alarmStack[i].stationId, alarmStack[i].eventId);

                    alarm.locationName = stationIdToLocationNameMap[event->getStationId()];
                    // there should never be no match - but just in case
                    if (alarm.locationName == "")
                    {
                        alarm.locationName = "(Error resolving location name)";
                    }

                    alarm.eventId = event->getEventId();
                    alarm.locationDesc = event->getLocation();
                    alarm.triggeringEventDesc = event->getCctvTriggering();

                    delete event;
                    event = NULL;
                }
                catch (TA_Base_Core::TransactiveException& te)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());

                    if (event != NULL)
                    {
                        delete event;
                        event = NULL;
                    }

                    // insert unknown event details
                    alarm.locationName = "Unknown";
                    alarm.eventId = 0;
                    alarm.locationDesc = "Unknown Location";
                    alarm.triggeringEventDesc = "Unknown Triggering Event - Not Configured ";
                    alarm.triggeringEventDesc += te.what();
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
        return (m_alarmStackEntries.size() > 0);
    }


    bool VideoOutput::isBvsStageInAlarmStack(BvsStage* stage)
    {
        FUNCTION_ENTRY( "isBvsStageInAlarmStack" );

        std::list<CCTVTriggeredAlarm>::iterator itr;
        for(itr=m_alarmStackEntries.begin(); itr!=m_alarmStackEntries.end(); itr++)
        {
            if((*itr).eventId == 0)
            {
                BvsStage* alarmStage = DisplayItemManager::getInstance().getBvsStage((*itr).stationId);
                if(alarmStage == stage)
                {
                    FUNCTION_EXIT;
                    return true;
                }
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    std::vector<VideoOutput::TrainCamera> VideoOutput::getTrainCamerasInAlarmStack()
    {
        FUNCTION_ENTRY( "getTrainCamerasInAlarmStack" );

        std::vector<VideoOutput::TrainCamera> stages;

        std::list<CCTVTriggeredAlarm>::iterator itr;

        for(itr=m_alarmStackEntries.begin(); itr!=m_alarmStackEntries.end(); itr++)
        {
            if((*itr).eventId == 0)
            {
                BvsStage* alarmStage = DisplayItemManager::getInstance().getBvsStage((*itr).stationId);
                if(alarmStage!=NULL)
                {
                    //finally set the service num so the operator know  hats were on about
                    //all this just so they can see the right service num??
                    std::string serviceNum="";
                    CWnd* wnd = AfxGetMainWnd();
                    VideoDisplayManagerDlg* mainDialog = dynamic_cast<VideoDisplayManagerDlg*>(wnd);
                    VideoSourcesListCtrl::TrainDetails* trainDetails = NULL;

                    if (mainDialog != NULL)
                    {
                        TrainsPage* trainsPage = mainDialog->getTrainsPage();
                        if (trainsPage != NULL)
                        {
                            trainDetails = trainsPage->getTrainDetails(alarmStage->getActiveTrain());
                        }
                    }
                    if (trainDetails != NULL)
                    {
                        serviceNum = trainDetails->serviceNumber;
                    }

                    if(alarmStage->getActiveVideoSource() == BvsStage::Single)
                    {
                        TrainCamera camera;
                        camera.trainId = alarmStage->getActiveTrain();
                        camera.cameraId = alarmStage->getAssignment1();
                        camera.serviceNum = serviceNum;
                        stages.push_back(camera);
                    }
                    else if(alarmStage->getActiveVideoSource() == BvsStage::Quad)
                    {
                        TrainCamera camera;
                        camera.trainId = alarmStage->getActiveTrain();
                        camera.cameraId = alarmStage->getAssignment1();
                        camera.serviceNum = serviceNum;
                        if(camera.cameraId !=0)
                        {
                            stages.push_back(camera);
                        }
                        camera.cameraId = alarmStage->getAssignment2();
                        if(camera.cameraId !=0)
                        {
                            stages.push_back(camera);
                        }
                        camera.cameraId = alarmStage->getAssignment3();
                        if(camera.cameraId !=0)
                        {
                            stages.push_back(camera);
                        }
                        camera.cameraId = alarmStage->getAssignment4();
                        if(camera.cameraId !=0)
                        {
                            stages.push_back(camera);
                        }
                    }
                }
            }
        }

        FUNCTION_EXIT;
        return stages;
    }


    std::vector<BvsStage*> VideoOutput::getUnassignedStagesInAlarmStack()
    {
        FUNCTION_ENTRY( "getUnassignedStagesInAlarmStack" );

        std::vector<BvsStage*> stages;

        std::list<CCTVTriggeredAlarm>::iterator itr;

        for(itr=m_alarmStackEntries.begin(); itr!=m_alarmStackEntries.end(); itr++)
        {
            if((*itr).eventId == 0)
            {
                BvsStage* alarmStage = DisplayItemManager::getInstance().getBvsStage((*itr).stationId);
                if(alarmStage!=NULL)
                {
                    if (alarmStage->getActiveTrain() == 0)
                    {
                        // only add unique stages
                        if ( std::find(stages.begin(), stages.end(), alarmStage) == stages.end() )
                        {
                            stages.push_back(alarmStage);
                        }
                    }
                }
            }
        }

        FUNCTION_EXIT;
        return stages;
    }


    void VideoOutput::updateBvsStageInAlarmStack(BvsStage* stage)
    {
        FUNCTION_ENTRY( "updateBvsStageInAlarmStack" );

        std::list<CCTVTriggeredAlarm>::iterator itr;
        for(itr=m_alarmStackEntries.begin(); itr!=m_alarmStackEntries.end(); itr++)
        {
            if((*itr).eventId == 0)
            {
                BvsStage* alarmStage = DisplayItemManager::getInstance().getBvsStage((*itr).stationId);

                if(alarmStage == stage && alarmStage!=NULL)
                {
                    std::stringstream desc;
                    desc << "Train: " << alarmStage->getActiveTrain();
                    if(alarmStage->getActiveVideoSource() == BvsStage::Single)
                    {
                        desc << "\r\n Single Camera: " << alarmStage->getAssignment1();
                    }
                    else if(alarmStage->getActiveVideoSource() == BvsStage::Quad)
                    {
                        desc << "\r\n 1: ";
                        if(stage->getAssignment1() != 0)
                        {
                            desc << "Camera " << alarmStage->getAssignment1();
                        }
                        else
                        {
                            desc << "parked";
                        }
                        desc << "\r\n 2: ";
                        if(stage->getAssignment2() != 0)
                        {
                            desc << "Camera " << alarmStage->getAssignment2();
                        }
                        else
                        {
                            desc << "parked";
                        }
                        desc << "\r\n 3: ";
                        if(stage->getAssignment3() != 0)
                        {
                            desc << "Camera " << alarmStage->getAssignment3();
                        }
                        else
                        {
                            desc << "parked";
                        }
                        desc << "\r\n 4: ";
                        if(stage->getAssignment4() != 0)
                        {
                            desc << "Camera " << alarmStage->getAssignment4();
                        }
                        else
                        {
                            desc << "parked";
                        }
                    }
                    else if(alarmStage->getActiveVideoSource() == BvsStage::Sequence)
                    {
                        desc << "\r\n Sequence: " << alarmStage->getAssignment1();
                    }
                    else
                    {
                        desc << "\r\n Error: train is not active" ;
                    }
                    (*itr).triggeringEventDesc = desc.str();
                }
            }
        }

        FUNCTION_EXIT;
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
            for (std::list<CCTVTriggeredAlarm>::iterator iter = m_alarmStackEntries.begin();
            iter != m_alarmStackEntries.end(); iter++, i++)
            {
                detailsStr << i << ": " ;
                if(iter->triggeringEventDesc != "Unknown Triggering Event - Not Configured " )
                {
                    detailsStr << iter->locationName
                    //<< " " << iter->locationDesc
                    << ", " ;
                }
                detailsStr << iter->triggeringEventDesc << "\r\n";
            }
        }
        // details of current assignment - if any
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
