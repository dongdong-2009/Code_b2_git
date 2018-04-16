/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng
  * 1: delete the star and stop calls
  * 2: add the getReceiver() function
*/

#ifndef __CONSOLE_STATE_MANAGER_H
#define __CONSOLE_STATE_MANAGER_H

#include "ace/Task.h"

namespace TA_IRS_App {

		class ConsoleStateManager
		{
        public:
            ConsoleStateManager();
            virtual ~ConsoleStateManager ();
			
			void setClientID(unsigned char);
			/*
			 * The application is given it's client id when it registers 
			 * with the Console Dispatch Interface using the McdiRegisterClient()
			 */
			unsigned char getClientID();

		private: 
            //disable methods
            ConsoleStateManager(const ConsoleStateManager&);
            void operator=(const ConsoleStateManager&);
            //the singleton pattern
            unsigned char m_clientID;
		};

        typedef ACE_Singleton<ConsoleStateManager, ACE_Null_Mutex> TheConsoleStateManager;
};

#endif
