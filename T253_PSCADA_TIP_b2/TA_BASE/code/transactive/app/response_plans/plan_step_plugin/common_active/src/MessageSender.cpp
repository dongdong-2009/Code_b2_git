#include "app/response_plans/plan_step_plugin/common_active/src/MessageSender.h"


namespace TA_Base_App
{
    IMessageSender* MessageSender::s_pMsgSender = NULL;

    MessageSender::MessageSender()
    {
    }

    MessageSender::~MessageSender()
    {
    }

    void MessageSender::init( IMessageSender& refMsgSender )
    {
        s_pMsgSender = &refMsgSender;
    }

    IMessageSender& MessageSender::instance()
    {
        if ( NULL == s_pMsgSender )
        {

        }

        return *s_pMsgSender;
    }

}