#ifndef __MESSAGE_SENDER_HELPER_INCLUDE_H
#define __MESSAGE_SENDER_HELPER_INCLUDE_H

#include "IMessageSender.h"

namespace TA_Base_App
{
    class MessageSender
    {
    public:
        static IMessageSender& instance();
        static void init( IMessageSender& refMsgSender );

    private:
        MessageSender();
        MessageSender( const MessageSender& fromHelper );
        MessageSender& operator=(const MessageSender& fromHelper);
        ~MessageSender();

    private:
        static IMessageSender* s_pMsgSender;
    };
}


#endif