#ifndef QUAD_STATUS_COMMAND_H
#define QUAD_STATUS_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  *
  */

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"

// Forward declarations.

namespace TA_IRS_App
{
    class COEQuad;
}

namespace TA_IRS_App
{
    class QuadStatusCommand : public AbstractCOERequestCommand, public virtual IHardwareNotificationCommand
    {
    public:

        /** 
          * QuadStatusCommand
          *
          * description
          *
          */
        QuadStatusCommand( const std::vector< unsigned char >& notificationData );


        /** 
          * QuadStatusCommand
          *
          * description
          *
          * @param 
          *
          */
        QuadStatusCommand( COEQuad& quad );
       
        
        /**
          * ~QuadStatusCommand
          *
          * Standard destructor.
          */
        virtual ~QuadStatusCommand() {};


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

        QuadStatusCommand( const QuadStatusCommand& theQuadStatusCommand );
        QuadStatusCommand& operator=( const QuadStatusCommand& );

        //
        // Helper Functions
        //

        /** 
          * processQuadStatusData
          *
          * description
          *
          * @param 
          */
        void processQuadStatusData( const std::vector< unsigned char >& data );


        /** 
          * determineCameraKey
          *
          * description
          *
          * @param locationByte
          * @param 
          *
          * @return 
          *
          */
        unsigned long determineCameraKey( unsigned char locationByte, unsigned short idShort );

		unsigned short getCamIdFromByteIndex(const std::vector<unsigned char>& data, unsigned short index);


        //
        // Private Members
        //
        
        COEQuad* m_quad;

        unsigned short m_quadId;

        std::vector< unsigned char > m_notificationData;

        static const unsigned short NUM_QUAD_STATUS_REQUEST_DATA_BYTES;
        static const unsigned short QUAD_ID_INDEX;
        static const unsigned short FIRST_CAM_LOCATION_INDEX;
        static const unsigned short FIRST_CAM_ID_INDEX;
        static const unsigned short SECOND_CAM_LOCATION_INDEX;
        static const unsigned short SECOND_CAM_ID_INDEX;
        static const unsigned short THIRD_CAM_LOCATION_INDEX;
        static const unsigned short THIRD_CAM_ID_INDEX;
        static const unsigned short FOURTH_CAM_LOCATION_INDEX;
        static const unsigned short FOURTH_CAM_ID_INDEX;

        static const char* UNKNOWN_CAMERA_ASSIGNED;
        static const char* NOTIFICATION_FOR_UNKNOWN_QUAD;
        static const char* INSUFFICENT_LENGTH;
        static const char* BAD_NOTIFICATION_DATA;
    };
}

#endif // QUAD_STATUS_COMMAND_H
