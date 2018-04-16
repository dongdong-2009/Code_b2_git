#ifndef SEQUENCE_CONFIG_COMMAND_H
#define SEQUENCE_CONFIG_COMMAND_H

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
    class COESequence;
}

namespace TA_IRS_App
{
    class SequenceConfigCommand : public AbstractCOERequestCommand, public virtual IHardwareNotificationCommand
    {
    public:

        /** 
          * SequenceConfigCommand
          *
          * description
          *
          */
        SequenceConfigCommand( const std::vector< unsigned char >& notificationData );


        /** 
          * SequenceConfigCommand
          *
          * description
          *
          * @param 
          *
          */
        SequenceConfigCommand( COESequence& sequence );
       
        
        /**
          * ~SequenceConfigCommand
          *
          * Standard destructor.
          */
        virtual ~SequenceConfigCommand() {};


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

        SequenceConfigCommand( const SequenceConfigCommand& theSequenceConfigCommand );
        SequenceConfigCommand& operator=( const SequenceConfigCommand& );

        //
        // Helper Functions
        //

        /** 
          * processSequenceInfoData
          *
          * description
          *
          * @param 
          */
        void processSequenceInfoData( const std::vector< unsigned char >& data );


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
        unsigned long determineCameraKey( unsigned short idShort, unsigned char locationByte );


		unsigned short getCamIdFromByteIndex(const std::vector<unsigned char>& data, unsigned short index);
        //
        // Private Members
        //
        
        COESequence* m_sequence;

        unsigned short m_sequenceId;

        std::vector< unsigned char > m_notificationData;

        static const unsigned short NUM_SEQUENCE_INFO_REQUEST_DATA_BYTES;
        static const unsigned short SEQUENCE_ID_INDEX;
        static const unsigned short DWELL_TIME_INDEX;
        static const unsigned short FIRST_CAM_LOCATION_INDEX;
        static const unsigned short FIRST_CAM_ID_INDEX;
        static const unsigned short NUM_CAM_FIELDS;

        static const char* UNKNOWN_CAMERA_ASSIGNED;
        static const char* NOTIFICATION_FOR_UNKNOWN_SEQUENCE;
        static const char* BAD_NOTIFICATION_DATA;
        static const char* UNEXPECTED_NUM_CAM_BYTES;
    };
}

#endif // SEQUENCE_CONFIG_COMMAND_H