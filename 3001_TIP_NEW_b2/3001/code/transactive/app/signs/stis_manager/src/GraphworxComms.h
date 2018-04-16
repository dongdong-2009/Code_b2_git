/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/GraphworxComms.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Provides interface to graphworx displays
  *
  */

#pragma once

#include <string>

namespace TA_IRS_App
{
    class GraphworxComms
    {
    public:

        static GraphworxComms getInstance();

        /**
         * resetGraphworxDisplays
         *
         * Resets all the graphworx displays to an inactive state
         * This should be called on shutdown, and immediately on startup
         *  the call on startup is generally redundant (unless PA Manager crashed)
         *
         * @return true if operation was successful
         *
         */
        bool    resetGraphworxDisplaysToInactive();
    
        /**
         * activateGraphworxDisplay
         *
         * Activates the specified graphworx display - this will
         *      automatically de-active all other displays
         * This should be called one STIS Manager has finished loading up
         * 
         * @param displayIdentifier the identifier of the graphworx display we want to activate
         *      this is typically taken from the runparams <STN>.<displayIdentifier> identifier
         *      that is sent through in the head of every runparam message
         *
         * @return true if operation was successful
         *
         */
        bool   activateGraphworxDisplay(const long displayIdentifier);

        /** 
          * displayCurrentMessage
          *
          * Displays the current message that is currently being displayed by the specified PID.
          *
          * @param cmd The display info
          * @param message The message that is currently being displayed by the PID
          * @param priority The priority level
          * @param displayIdentifier An x position on the screen that will display this message
          *
          * @return true if the operation is successful
          *
          */
        bool displayCurrentMessage(const std::string& cmd, const std::string& message, const std::string& priority, const long displayIdentifier, const std::string& startTime, const std::string& endTime);

    private:
            
        /**
         * dispatchCommandToGraphworx
         *
         * Sends through the input identifier / command pair to graphworx display
         *
         * @param command the command tag to append to the string sent to the graphworx display
         *
         * @param displayIdentifier the identifier of the display to dispatch to, default is -1 to send to all
         * 
         * @return true if operation was successful
         *
         */
        bool dispatchCommandToGraphworx(const std::string& command, const long displayIdentifier = -1);

        // Singleton
        GraphworxComms() {}
        
        static GraphworxComms s_instance;
    };

}   // end TA_IRS_App
