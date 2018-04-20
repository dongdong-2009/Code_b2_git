/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_list_control/src/TrainAlarms.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(TrainAlarms_H)
#define TrainAlarms_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/trains/TrainAgentCorba/idl/src/ITrainCctvCorba.h"
#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"
#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"

#include <ACE/Singleton.h>

namespace TA_Base_Bus
{
    class LocationSelectionDialog;
}

namespace TA_Base_Bus
{
	class TrainAlarms  
	{

        friend class ACE_Singleton<TrainAlarms, ACE_Recursive_Thread_Mutex>;

	public:

        static TrainAlarms& getInstance();


        /**
         * This is used to activate the triggered cameras on alarm acknowledgement
         * 
         * @param details
         */
        void acknowledgeAlarm(TA_Base_Bus::AlarmDataItem* details );
    
    
    private:

        /**
         * Used by ACE singleton
         */
        TrainAlarms();
        ~TrainAlarms();

        /*
         * This is used to activate trainborne video for the given cameras
         * 
         * @return true on success, false on failure
         * 
         * @param trainId    The train to activate
         * @param cameraIds    The cameras to activate
         * @param sessionId    The session ID activating the train
         */
        bool activateTrain( unsigned char trainId,
                            TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence cameraIds,
                            std::string sessionId );


        /**
         * This is used to add the alarm image to the video monitor once successfully activated on the train
         * 
         * @param location    The alarm location
         * @param trainId    The train ID
         * @param cameraIds    The cameras to add to the alarm stack
         * @param consoleKey    The current console key
         */
        void switchMonitor( unsigned long location,
                            unsigned char trainId,
                            TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence cameraIds,
                            unsigned long consoleKey );

        
        /**
         * The dialog used to select a location for delocalised trains
         */
        LocationSelectionDialog* m_locationSelectionDialog;

	};

    /**
     * @author adamr
     * @version 1.0
     * @created 13-Mar-2008 6:38:03 PM
     */
    typedef ACE_Singleton<TrainAlarms, ACE_Recursive_Thread_Mutex> TrainAlarmsSingleton;

};

#endif // !defined(TrainAlarms_H)

