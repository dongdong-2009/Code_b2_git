#ifndef RADIO_RESET_SESSION_TASK_H
#define RADIO_RESET_SESSION_TASK_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/src/RadioResetSessionTask.h $
  * @author 
  * @version $Revision: #1 $
  * Last modification : $DateTime: $
  * Last modified by : $Author: $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioTask.h"

namespace TA_IRS_App
{
    class RadioTcpServer;

    class RadioResetSessionTask
        : public RadioTask
    {
        public:
            /**
             * 
             */
            RadioResetSessionTask(RadioTcpServer * radio, unsigned long sessionRef);


            /**
             *
             */
            ~RadioResetSessionTask();

        protected:
            virtual void perform();

        private:
			unsigned long m_sessionRef;


    }; // class RadioResetSessionTask
} // namespace TA_IRS_App

#endif // #ifndef RADIO_RESET_SESSION_TASK_H
