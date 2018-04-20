/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(SetStageSwitchModeCommand_H)
#define SetStageSwitchModeCommand_H

#include "app/cctv/video_switch_agent/protocols/COE/src/AbstractCOERequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"


namespace TA_IRS_App
{
   /**
    * See linked document for command format.
    * @author oliverk
    * @version 1.0
    * @updated 05-Feb-2008 2:44:17 PM
    */
    class SetStageSwitchModeCommand : public virtual AbstractCOERequestCommand, 
                                      public virtual IHardwareNotificationCommand
    {
    public:

       /**
        * 
        * @param originatingStage
        * @param destinationStage
        * @param allowSwitch
        */
        SetStageSwitchModeCommand( unsigned long originatingStage,
                                   unsigned long destinationStage,
                                   bool allowSwitch );


        virtual ~SetStageSwitchModeCommand() {};


       /**
        * processResponse  Processes the response to this command that has been received from the switching hardware. 
        * @return     bool True if the response was valid. False otherwise. 
        * @param      DataVector& data The COE to process.  This will have the checksum removed.
        * 
        * @param data
        */
        bool processResponse( const std::vector<unsigned char>& data );


         /**
          * processNotification
          *
          * Processes the notification data that has been received from the hardware.
          */
        virtual void processNotification() {};//LIMIN TODO, here is a stub, how to implement this function ?


        /**
          * getResponseDataToSend
          *
          * Returns the response data to be sent to the hardware..
          *
          * @return     std::string
          *             The data to be sent to the hardware or
          *             "" if no response is required.
          */
        virtual std::string getResponseDataToSend() { return ""; };//LIMIN TODO, here is a stub, how to implement this function ?

    private:

        SetStageSwitchModeCommand& operator=( const SetStageSwitchModeCommand& );
        SetStageSwitchModeCommand( const SetStageSwitchModeCommand& );


        /**
         * Holds the value of the command until the table with the values is updated.
         */
        static const char m_commandValue;

    };

}


#endif // !defined(SetStageSwitchModeCommand_H)
