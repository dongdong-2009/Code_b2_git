#ifndef TRIGGERING_EVENT_COMMAND_H
#define TRIGGERING_EVENT_COMMAND_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
    class TriggeringEventCommand : public AbstractCOERequestCommand, public virtual IHardwareNotificationCommand
    {
		static const unsigned short LSB_NUMBER_OF_ALARMS_INDEX;
		static const unsigned short MSB_NUMBER_OF_ALARMS_INDEX;
		static const unsigned short FIRST_ALARM_INDEX;
		static const char* INSUFFICENT_LENGTH;    
		static const char* BAD_NOTIFICATION_DATA;    

    public:

        /** 
          * TriggeringEventCommand
          *
          * description
          *
          */
        TriggeringEventCommand( const std::vector< unsigned char >& notificationData );

        /** 
          * TriggeringEventCommand
          *
          * description
          *
          * @param communicationsChannelIndex
          * @param 
          *
          */
        TriggeringEventCommand();
       
        
        /**
          * ~TriggeringEventCommand
          *
          * Standard destructor.
          */
        virtual ~TriggeringEventCommand() {};


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

        TriggeringEventCommand( const TriggeringEventCommand& theTriggeringEventCommand );
        TriggeringEventCommand& operator=( const TriggeringEventCommand& );

        //
        // Helper Functions
        //
		std::vector< unsigned char > m_notificationData;

    };
}

#endif // TRIGGERING_EVENT_COMMAND_H

