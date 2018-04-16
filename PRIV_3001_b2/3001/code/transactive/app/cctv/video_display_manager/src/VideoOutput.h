/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/VideoOutput.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(VideoOutput_H)
#define VideoOutput_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DisplayItem.h"
#include "bus\cctv\video_switch_agent\IDL\src\VideoOutputCorbaDef.h"
#include <vector>

namespace TA_IRS_App
{
    // video inputs can be assigned to video outputs
    class VideoInput;
    //class BvsStage;
	class Camera;
	class VideoOutput : public DisplayItem
    {
    public:

        // This is used for alarm stacks
        struct CCTVTriggeredAlarm
        {
            std::string locationName;           // TA location name (from Video Station Id)
            unsigned int eventId;               // CCTV event id
            unsigned int stationId;
            std::string locationDesc;           // event location description
            std::string triggeringEventDesc;    // The triggering event description
        };

        struct TrainCamera
        {
            unsigned long trainId;
            unsigned long cameraId;
            std::string serviceNum;
        };

        VideoOutput( unsigned long entityKey,
                     const std::string& name,
                     const std::string& description );


        virtual ~VideoOutput() ;

        /**
          * getDetails
          *
          * Gets a more detailed description of the item.
          * This is formatted for printing in a text box.
          * This includes the details of any assigned item.
          *
          * @return The detailed description.
          */
        virtual std::string getDetails();


        /**
          * assignInput
          *
          * Sets the current input for this output
          *
          * @param videoInput a pointer to the input - or NULL to clear the assignment.
          */
        virtual void assignInput(VideoInput* videoInput);

		virtual void assignQuad(Camera* cameraW,Camera* cameraX,Camera* cameraY,Camera* cameraZ);
        /**
          * getInput
          *
          * Gets the current input for this output
          *
          * @param a pointer to the input - or NULL if none.
          */
        virtual VideoInput* getInput();

		virtual bool isQuadInput();

		virtual std::vector<Camera*>  getQuadCameras();
        /**
          * getAlarmStackItems
          *
          * Gets the current alarm stack items
          *
          * @return a list of items
          */
        virtual std::vector<CCTVTriggeredAlarm> getAlarmStackItems();

        /**
          * setAlarmStackItems
          *
          * Set the alarm stack items via a list of keys into the database.
          * If errors are encountered looking these up the details will reflect this.
          *
          * @param triggeringEventKeys  The keys of the events in the database.
          */
        virtual void setAlarmStackItems(TA_Base_Bus::VideoOutputCorbaDef::MonitorAlarmStack alarmStack);
        virtual bool isInAlarmMode();
		
		virtual inline bool getTriggeringAlarmMode() {return m_bTriggeringAlarm;};
		virtual inline bool getShowingAckAlarm() {return m_bShowingAckAlarm;};
		virtual inline void setTriggeringAlarm(bool alarm){m_bTriggeringAlarm = alarm;};
		virtual inline void setShowingAckAlarm(bool alarm) {m_bShowingAckAlarm = alarm;};

        /**
          * clearAlarmStack
          *
          * Clears the alarm stack for this output
          */
        virtual void clearAlarmStack();

    private:
        VideoInput* m_currentInput;  // current input item (or NULL)
        std::vector<CCTVTriggeredAlarm> m_alarmStackEntries;
		bool m_bQuadInput;
		bool m_bTriggeringAlarm,m_bShowingAckAlarm;
		std::vector<Camera*> m_quadCameraList;
        // These arent needed
        VideoOutput();
        VideoOutput( const VideoOutput& );
        VideoOutput& operator=( const VideoOutput& );
    };
}

#endif // !defined(VideoOutput_H)

