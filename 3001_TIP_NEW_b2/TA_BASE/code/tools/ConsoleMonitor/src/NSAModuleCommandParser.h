#ifndef NSA_MODULE_COMMAND_PARSE_H__
#define NSA_MODULE_COMMAND_PARSE_H__

#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"
#include "core/message/src/TA_CosUtility.h"

#include "omniNotify.h"
#include "COS/CosNotification.hh"

#include "CommonModuleCommandParser.h"

namespace TA_Base_Core
{
    class NSAModuleCommandParser : public CommonModuleCommandParser
    {
    public:
        NSAModuleCommandParser(void);
        virtual ~NSAModuleCommandParser(void);
        virtual void postInit();
        virtual void displayUsage();
        virtual std::string getStateDesc();
    protected:
        virtual bool processCommand(const std::vector<std::string> & vtCommandArgs);
    private:
        void listChannelCommand(const std::vector<std::string> & vtCommandArgs);
        void setChannelCommand(const std::vector<std::string> & vtCommandArgs);
        void channelCommand(const std::vector<std::string> & vtCommandArgs, FILE * pFile);
        void attachEventChannelCommand(const std::vector<std::string> & vtCommandArgs);
        void processFlagCommand(const std::vector<std::string> & vtCommandArgs);
        void processLoopCommand(const std::vector<std::string> & vtCommandArgs);
    private:
        TA_Base_Core::IChannelLocator_var m_currChannelLocator; 

        CosNotifyChannelAdmin::EventChannel_var m_currChannel;
        AttNotification::Interactive_var m_currTargetObj;
        AttNotification::Interactive_var m_currentChannelObj;
        std::string m_channelName;
        int m_currentClassLevel;  
    };

}
#endif