#ifndef MONITOR_COMMAND_HELPER_H__
#define MONITOR_COMMAND_HELPER_H__

#include "core/timers/src/ITimeoutCallback.h"
#include "core/message/src/TA_CosNotifyInterfaces.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/SubscriptionManager.h"

#include "MonitorMessageSender.h"
#include "MonitorMessageProcessor.h"

#include "CommonModuleCommandParser.h"
#include "MonitorCommonDataType.h"


namespace TA_Base_Core
{

    class MonitorMessageHelper: public TA_Base_Core::IEventHandler,
        public TA_Base_Core::ITimeoutCallback
    {
    public:
        MonitorMessageHelper(bool isAgentSide);
        ~MonitorMessageHelper(void);
        virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events);

        void init();
        void doSmokeTest();

        virtual void configTimer();
        /**
        *
        * timerExpired
        *
        * This operation will be called when the registered timeout period has expired.
        *
        */
        virtual void timerExpired(long timerId, void* userData);
    private:
        void processMessage( const std::string& header, const CORBA::Any& message, CommandStateInfo & cmdInfo );
        void sendMonitorMessage(CommandStateInfo & info);
        void createFilter();
        void subscribeMessage();
        void unsubscriberMessage();

    private:
        MonitorMessageProcessor m_msgProcessor;
        MonitorMessageSender m_msgsender;

        //so far, we don't need filter info
        bool m_isAgentSide;
        TA_Base_Core::FilterData m_commonFilter; 
        SubscriptionFilterSet m_subscriptions;

        std::string m_locationKey;
        std::string m_entityName;
        std::string m_contextName;
        std::string m_channelname;

    };

}

#endif