/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioManagerTestData.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * [Taken from Radio Monitor, and adapted]
  * Responsible for generating test data, for purposes of testing Radio Manager
  *
  */

#pragma once

#include <list>

#include "core\message\src\SpecialisedMessageSubscriber.h"
#include "CommsMessageCorbaDef.h"
#include "core\synchronisation\src\NonReEntrantThreadLockable.h"
#include "bus\radio\RadioAgentCorbaDef_4669\idl\src\IRadioCorbaDef.h"

namespace TA_IRS_App
{
    namespace RadioManager
    {
    
        class RadioManagerTestData
        {
        public:
            
            typedef TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef> SubscriberDef;
            typedef TA_Base_Bus::IRadioCorbaDef::CallStatusEvent CurrentCallNotification;
            typedef TA_Base_Bus::IRadioCorbaDef::CallDetailsType CallDetails;

            RadioManagerTestData();
            static RadioManagerTestData& getInstance();

            enum EUpdateType
            {
                NEW_UPDATE,
                UPDATE_UPDATE,
                REMOVE_UPDATE
            };
            
            /**
             * generateRandomUpdate
             *
             * Generates (and returns) an update as defined by input type
             * @param type the type of update to generate - if unable to satisfy
             *          an update or remove request, automatically become type NEW_UPDATE
             */
            const CallDetails generateRandomCurrentCallUpdate(EUpdateType type);
           
            // Begins worker thread posting data to subscribers
            void beginThread(SubscriberDef& currentCallSubscriber, long sessionRef);

            // Cancels thread, waiting for it to complete before returning from function
            void cancelThread();

            // @return new copy of specified call details
            CallDetails* getCopyCurrentCallDetails(long callID);
        
           
        private:

            /**
             * @return an existing conference call (out of current call pool) - if none exist, returns null
             */
            const CallDetails* getRandomConferenceCall();

            /**
             * Entry point for background thread that generates test data
             */
            static DWORD WINAPI RadioManagerTestData_Thread(void* val);
            static DWORD WINAPI RadioManagerTestData_Thread2(void* val);

            // Internal helper functions
            void flagThreadComplete() { m_activeThreads --; }

            /**
             * generates an update, whose unique parameters match existingItem (if provided) 
             */
            const CallDetails generateCurrentCallUpdate(const CallDetails* existingItem);
            
            std::list<CallDetails> m_currentCallUpdates;
            
            SubscriberDef* m_currentCallSubscriber;
            
            TA_Base_Core::NonReEntrantThreadLockable m_currentCallLock;
            long m_sessionRef;

            volatile int m_activeThreads;
        };
    
    }   // end namespace RadioManager

}   // end namespace TA_IRS_App
