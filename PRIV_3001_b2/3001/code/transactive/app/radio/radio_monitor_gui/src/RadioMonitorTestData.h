/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorTestData.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Responsible for generating test data, for purposes of testing Radio Monitor
  *
  */

#pragma once

#include "RadioMonitorTypes.h"
#include "RadioMonitorUtils.h"
#include "bus/pa/messagesubscriber/src/SimpleMessageSubscriber.h"

namespace TA_IRS_App
{
    namespace RadioMonitor
    {
    
        class RadioMonitorTestData
        {
        public:
            
            typedef TA_IRS_App::RadioMonitorUtils::CorbaMonitorUpdate CorbaMonitorUpdate;
            typedef TA_Base_Bus::IRadioCorbaDef::SubscriberDetailsNotification SubscriberNotification;

            RadioMonitorTestData();
            static RadioMonitorTestData& getInstance();

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
            const TA_IRS_App::RadioMonitorUtils::CorbaMonitorUpdate generateRandomUpdate(EUpdateType type);
           
            // Begins worker thread posting data to subscribers
            void beginThread(TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* pMonitor,
                              TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* pListen,
                              TA_IRS_Bus::SimpleMessageSubscriber<SubscriberNotification>* pSearchPanel);
            // Cancels thread, waiting for it to complete before returning from function
            void cancelThread();

            // Internal helper functions
            void flagThreadComplete() { m_threadActive = false; }
            TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* getMonitorSubscriber() { return m_pMonitor; }
            TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* getListenSubscriber() { return m_pListen; }
            TA_IRS_Bus::SimpleMessageSubscriber<SubscriberNotification>* getSearchPanelSubscriber() { return m_pSearchPanel; }

        private:

            /**
             * generates an update, whose unique parameters match existingItem (if provided) 
             */
            const TA_IRS_App::RadioMonitorUtils::CorbaMonitorUpdate 
                generateUpdate(const TA_IRS_App::RadioMonitorUtils::CorbaMonitorUpdate* existingItem);

            
            std::list<TA_IRS_App::RadioMonitorUtils::CorbaMonitorUpdate> m_updates;
            
            TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* m_pMonitor;
            TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* m_pListen;
            TA_IRS_Bus::SimpleMessageSubscriber<SubscriberNotification>*    m_pSearchPanel;
            volatile bool m_threadActive;
        };
    
    }   // end namespace RadioMonitor

}   // end namespace TA_IRS_App
