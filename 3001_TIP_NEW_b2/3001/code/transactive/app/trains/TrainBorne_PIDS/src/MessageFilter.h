#pragma once
#include <vector>
#include <string>
#include "TTISPredefinedMessages.h"

namespace TA_IRS_App
{
    class MessageFilter
    {
    public:
        MessageFilter( const std::string& filter );
        bool operator()( const TA_Base_Core::PredefinedMessage& message );
    public:
        ~MessageFilter( void ) {};
    private:
        bool upperCaseCmp( const std::string& beFilter );
        std::string m_filterString;
    };
}

