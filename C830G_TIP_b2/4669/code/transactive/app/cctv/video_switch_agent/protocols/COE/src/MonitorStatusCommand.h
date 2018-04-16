#ifndef MONITOR_STATUS_COMMAND_H
#define MONITOR_STATUS_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/MonitorStatusCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COEVideoOutput;
}

namespace TA_IRS_App
{
	class MonitorStatusCommand : public AbstractCOERequestCommand, public virtual IHardwareNotificationCommand
    {
    public:

        /** 
          * MonitorStatusCommand
          *
          * description
          *
          */
        MonitorStatusCommand( const std::vector< unsigned char >& notificationData );

        /** 
          * MonitorStatusCommand
          *
          * description
          *
          * @param communicationsChannelIndex
          * @param 
          *
          */
        MonitorStatusCommand( COEVideoOutput& videoOutput );
       
        
        /**
          * ~MonitorStatusCommand
          *
          * Standard destructor.
          */
        virtual ~MonitorStatusCommand() {};


        /**
          * processResponse
          *
          * Processes the response to this command that has been received from
          * the switching hardware.
          *
          * @return     bool
          *             True if the response was valid.
          *             False otherwise.
          *
          * @param      DataVector& data
          *             The COE to process.  This will have the checksum removed.
          */
        virtual bool processResponse( const std::vector< unsigned char >& data );


        /**
          * processNotification
          *
          * Processes the notification data that has been received from the hardware.
          */
        virtual void processNotification();


        /**
          * getResponseDataToSend
          *
          * Returns the response data to be sent to the hardware..
          *
          * @return     std::string
          *             The data to be sent to the hardware or
          *             "" if no response is required.
          */
        virtual std::string getResponseDataToSend(){ return ""; }

    private:

        //
        // Disable copy constructor and assignment operator.
        //

        MonitorStatusCommand( const MonitorStatusCommand& theMonitorStatusCommand );
        MonitorStatusCommand& operator=( const MonitorStatusCommand& );

        //
        // Helper Functions
        //

        /** 
          * processMonitorStatusData
          *
          * description
          *
          * @param 
          */
        void processMonitorStatusData( const std::vector< unsigned char >& data );


        /** 
          * processCameraMode
          *
          * description
          *
          * @param 
          */
        void processCameraMode( const std::vector< unsigned char >& data );


        /** 
          * processSequenceMode
          *
          * description
          *
          * @param 
          */
        void processSequenceMode( const std::vector< unsigned char >& data );


        /** 
          * processQuadMode
          *
          * description
          *
          * @param 
          */
        void processQuadMode( const std::vector< unsigned char >& data );


        /** 
          * processTrainMode
          *
          * description
          *
          * @param 
          */
        void processTrainMode( const std::vector< unsigned char >& data );


        /** 
          * processAlarmMode
          *
          * description
          *
          * @param 
          */
        void processAlarmMode( const std::vector< unsigned char >& data );


        /** 
          * processParkedMode
          *
          * description
          *
          * @param 
          */
        void processParkedMode( const std::vector< unsigned char >& data );

        //
        // Private Members
        //
        
        COEVideoOutput* m_videoOutput;

        unsigned short m_monitorId;

        std::vector< unsigned char > m_notificationData;

        static const unsigned short NUM_MONITOR_STATUS_REQUEST_DATA_BYTES;
        static const unsigned short MONITOR_ID_INDEX;
        static const unsigned short MODE_INDEX;
        static const unsigned short DEVICE_ID_INDEX;
        static const unsigned short DEVICE_LOCATION_INDEX;
        static const unsigned char CAMERA_MODE;
        static const unsigned char SEQUENCE_MODE;
        static const unsigned char QUAD_MODE;
        static const unsigned char TRAIN_MODE;
        static const unsigned char ALARM_MODE;
        static const unsigned char PARKED_MODE;
        
        static const char* UNKNOWN_CAMERA_ASSIGNED;
        static const char* UNKNOWN_SEQUENCE_ASSIGNED;
        static const char* UNKNOWN_QUAD_ASSIGNED;       
        static const char* UNKNOWN_BVS_STAGE_ASSIGNED; 
        static const char* NOTIFICATION_FOR_UNKNOWN_MONITOR;
        static const char* INSUFFICENT_LENGTH;  
        static const char* INVALID_ASSIGNMENT_TYPE;
        static const char* BAD_NOTIFICATION_DATA;         
    };
}

#endif // MONITOR_STATUS_COMMAND_H

