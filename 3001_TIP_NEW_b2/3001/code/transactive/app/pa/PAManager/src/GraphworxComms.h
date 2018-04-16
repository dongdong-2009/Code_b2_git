/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/GraphworxComms.h $
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

namespace TA_App
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
         * This should be called one PA Manager has finished loading up
         * 
         * @param displayId the identifier of the graphworx display we want to activate
         *      this is typically taken from the runparams <STN>.<DisplayID> identifier
         *      that is sent through in the head of every runparam message
         *
         * @return true if operation was successful
         *
         */
        bool   activateGraphworxDisplay(const std::string& displayId);

    private:
            
        /**
         * dispatchCommandToGraphworx
         *
         * Sends through the input identifier / command pair to graphworx display
         *
         * @param displayIdentifier the identifier of the display to dispatch to, -1 to send to all
         * 
         * @param command the command tag to append to the string sent to the graphworx display
         *
         * @return true if operation was successful
         *
         */
        bool dispatchCommandToGraphworx(const std::string displayIdentifier, const std::string command);

        // Singleton
        GraphworxComms() {}
        
        static GraphworxComms s_instance;
    };

}   // end TA_App