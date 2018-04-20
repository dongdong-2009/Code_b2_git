#ifndef BVS_STATUS_COMMAND_H
#define BVS_STATUS_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/BVSStatusCommand.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Developed to handle BVS Status notification.  
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"


// Forward declarations.

namespace TA_IRS_App
{
    class COEBVSStage;
}

namespace TA_IRS_App
{
    class BVSStatusCommand : public AbstractCOERequestCommand, public virtual IHardwareNotificationCommand
    {
    public:

        /** 
          * BVSStatusCommand
          *
          * description
          *
          */
        BVSStatusCommand( const std::vector< unsigned char >& notificationData );

        BVSStatusCommand( COEBVSStage& BVSStage );        
        
        virtual ~BVSStatusCommand() {};

        /**
          * processNotification
          *
          * Processes the notification data that has been received from the hardware.
          */
        virtual void processNotification();


        virtual bool processResponse( const std::vector< unsigned char >& data );

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

        BVSStatusCommand( const BVSStatusCommand& theBVSStatusCommand );
        BVSStatusCommand& operator=( const BVSStatusCommand& );

        std::vector< unsigned char > m_notificationData;

        static const unsigned short NUM_BVS_STATUS_REQUEST_DATA_BYTES;
        static const unsigned short BVS_STATION_ID_INDEX;
        static const unsigned short BVS_VIDEO_STATUS_INDEX;

        static const unsigned char WITH_VIDEO;
        static const unsigned char WITHOUT_VIDEO;

        static const char* BVS_VIDEO_NOT_PRESENT;
        static const char* BVS_VIDEO_PRESENT;
        static const char* INVALID_BVS_STATUS;

        unsigned short m_station_id;

        COEBVSStage* m_BVSStage;
    };
}
#endif // BVS_STATUS_COMMAND_H
