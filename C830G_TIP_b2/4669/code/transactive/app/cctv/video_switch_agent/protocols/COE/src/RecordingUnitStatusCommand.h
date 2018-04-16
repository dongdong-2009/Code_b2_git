#ifndef RECORDING_UNIT_STATUS_COMMAND_H
#define RECORDING_UNIT_STATUS_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/RecordingUnitStatusCommand.h $
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
    class COERecordingUnit;
}

namespace TA_IRS_App
{
    class RecordingUnitStatusCommand : public AbstractCOERequestCommand, public virtual IHardwareNotificationCommand
    {
    public:

        /** 
          * RecordingUnitStatusCommand
          *
          * description
          *
          */
        RecordingUnitStatusCommand( const std::vector< unsigned char >& notificationData );


        /** 
          * RecordingUnitStatusCommand
          *
          * description
          *
          * @param 
          *
          */
        RecordingUnitStatusCommand( COERecordingUnit& recordingUnit );
       
        
        /**
          * ~RecordingUnitStatusCommand
          *
          * Standard destructor.
          */
        virtual ~RecordingUnitStatusCommand() {};


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

        RecordingUnitStatusCommand( const RecordingUnitStatusCommand& theRecordingUnitStatusCommand );
        RecordingUnitStatusCommand& operator=( const RecordingUnitStatusCommand& );

        //
        // Helper Functions
        //

        /** 
          * processRecordingUnitStatusData
          *
          * description
          *
          * @param 
          */
        void processRecordingUnitStatusData( const std::vector< unsigned char >& data );

        //
        // Private Members
        //
        
        COERecordingUnit* m_recordingUnit;

        unsigned short m_recordingUnitId;

        std::vector< unsigned char > m_notificationData;

        static const unsigned short NUM_RECORDING_UNIT_STATUS_REQUEST_DATA_BYTES;
        static const unsigned short RECORDING_UNIT_ID_INDEX;
        static const unsigned short RECORDING_UNIT_STATUS_BYTE_INDEX;
        static const unsigned char NOT_RECORDING_CODE;
        static const unsigned char REAL_TIME_RECORDING_CODE;
        static const unsigned char TIME_LAPSE_RECORDING_CODE;

        static const char* NOTIFICATION_FOR_UNKNOWN_RECORDING_UNIT;
        static const char* INVALID_RECORDING_UNIT_STATUS;
        static const char* BAD_NOTIFICATION_DATA;
        static const char* INSUFFICENT_LENGTH;
    };
}

#endif // RECORDING_UNIT_STATUS_COMMAND_H