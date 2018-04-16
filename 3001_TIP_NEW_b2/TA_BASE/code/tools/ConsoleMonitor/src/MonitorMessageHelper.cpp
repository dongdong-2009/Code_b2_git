#include "MonitorMessageHelper.h"
#include "MonitorCommonDataType.h"

#include <assert.h>
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/TransactiveMessageSubscription.h"



#include "core/timers/src/SingletonTimerUtil.h"


namespace TA_Base_Core
{

    MonitorMessageHelper::MonitorMessageHelper(bool isAgentSide)
        :m_isAgentSide(isAgentSide)
        ,m_channelname(TA_Base_Core::InnerMonitorMessage::Context.getChannelName())
        ,m_contextName(TA_Base_Core::InnerMonitorMessage::Context.getContextName())
        ,m_msgProcessor(isAgentSide)
    {

        m_entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        m_locationKey = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
        TA_ASSERT(m_entityName.length()>0, "EntityName should not be empty");
        TA_ASSERT(m_locationKey.length()>0, "locationKey should be larger than 0");

    }

    MonitorMessageHelper::~MonitorMessageHelper(void)
    {
        unsubscriberMessage();
    }

    void MonitorMessageHelper::init()
    {
        createFilter();

        subscribeMessage();

        configTimer();
    }



    bool MonitorMessageHelper::onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events)
    {
        //no need to add lock, since bacth queue make sure its sequence
        //TA_Base_Core::ThreadGuard guard(m_lock); 

        // Forward framework call to old method name for each event.
        for (unsigned int i = 0; i < events.size(); ++i)
        {
            TA_Assert(events[i] != NULL);

            std::string event_header = gStringFromEventHeader( *events[i] );
            CommandStateInfo cmdInfo;
            processMessage( event_header, events[i]->remainder_of_body, cmdInfo);

            if (cmdInfo.nextStep == NEXTSTEP_SENDMESSAGE)
            {
                cmdInfo.nextStep = NEXTSTEP_NONE;
                sendMonitorMessage(cmdInfo);
            }

        }

        return true;
    }

    void MonitorMessageHelper::sendMonitorMessage(CommandStateInfo & cmdInfo)
    {
        TA_ASSERT(cmdInfo.nextStep == NEXTSTEP_NONE, "Next STEP should be set to NONE");
    }

    void MonitorMessageHelper::processMessage( const std::string& header, const CORBA::Any& message, CommandStateInfo & cmdInfo)
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        TA_ASSERT(cmdInfo.nextStep == NEXTSTEP_NONE, "Next STEP should be set to NONE");

        TA_Base_Core::MonitorResponseCorbaDef* responsePayload = 0;
        TA_Base_Core::MonitorCommandCorbaDef * commandPayLoad = 0;
        if ( ( message >>= commandPayLoad) != 0 )
        {
            m_msgProcessor.processRequestMessage(commandPayLoad, cmdInfo);
        }
        else if ( ( message >>= responsePayload) != 0 )
        {
            m_msgProcessor.processResponseMessage(responsePayload, cmdInfo); 
        }
        else
        {
            LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "wrong message received [%s]", header.c_str());
        }

        FUNCTION_EXIT;
    }

    void MonitorMessageHelper::subscribeMessage()
    {
        FUNCTION_ENTRY( "subscriber_message" );

        std::vector<std::string> typeKeys;

        if(m_isAgentSide)
        {
            typeKeys.push_back( TA_Base_Core::InnerMonitorMessage::RequestMonitor.getTypeKey());
        }
        else
        {   
            //typeKeys.push_back( TA_Base_Core::InnerMonitorMessage::AllMessages.getTypeKey());
            typeKeys.push_back( TA_Base_Core::InnerMonitorMessage::ResponseMonitor.getTypeKey());
        }

        TA_ASSERT(m_commonFilter.size()>0,"Subscription Filter should not be empty!");
        SubscriptionFilter* sf = gCreateSubscriptionFilter( m_contextName,"", typeKeys, &m_commonFilter );

        m_subscriptions.addSubscription( &gGetSubscriptionManager(),m_channelname, sf, atoi(m_locationKey.c_str()), this );

        FUNCTION_EXIT;
    }


    void MonitorMessageHelper::unsubscriberMessage()
    {
        FUNCTION_ENTRY( "unsubscriber_message" );

        m_subscriptions.removeAllSubscriptions();

        FUNCTION_EXIT;
    }


    void MonitorMessageHelper::createFilter()
    {
        NameValuePair * pLocationPair = new TA_Base_Core::NameValuePair("Location", m_locationKey);
        m_commonFilter.push_back(pLocationPair);
        if (m_isAgentSide)
        {
            NameValuePair * pEntityNamePair = new TA_Base_Core::NameValuePair("EntityName", TA_Base_Core::InnerMonitorConsts::MONITOR_ENTITYNAME);
            m_commonFilter.push_back(pEntityNamePair);
        }
    }

    void MonitorMessageHelper::doSmokeTest()
    {
        m_msgsender.sendRequestMessage(TA_Base_Core::InnerMonitorMessage::RequestMonitor.getTypeKey(), "help", 
            TA_Base_Core::InnerMonitorMessage::IM_REQUEST_SETPARAM, "");

    }

    void MonitorMessageHelper::configTimer()
    {
        if (!m_isAgentSide)
        {
           //expire per 30 senconds
           //TODO:
           //TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, 30000, false, NULL);
        }
    }

    void MonitorMessageHelper::timerExpired(long timerId, void* userData)
    {
        if (!m_isAgentSide)
        {
            //doSmokeTest();
        }
    }

}