#ifndef IHARDWARE_NOTIFICATION_COMMAND_H
#define IHARDWARE_NOTIFICATION_COMMAND_H

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h $
 * @author:  Katherine Thomson
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * This interface is used by the communication handler to handle
 * notifications from the hardware for the agent.
 *
 * This interface may be used by a protocol library,
 * but it is not mandatory.
 */

#include <string>
#include <vector>

namespace TA_IRS_App
{
    class IHardwareNotificationCommand
    {
    public:

        /**
          * ~IHardwareNotificationCommand
          *
          * Standard destructor.
          */

        virtual ~IHardwareNotificationCommand() {};

        /**
          * processNotification
          *
          * Processes the notification data that has been received from the hardware.
          *
          */

        virtual void processNotification() = 0;

        /**
          * getResponseDataToSend
          *
          * Returns the response data to be sent to the hardware.
          *
          * @return     std::string
          *             The data to be sent to the hardware or
          *             "" if no response is required.
          */

        virtual std::string getResponseDataToSend() = 0;

    }; // IHardwareNotificationCommand

} // TA_IRS_App

#endif // IHARDWARE_NOTIFICATION_COMMAND_H