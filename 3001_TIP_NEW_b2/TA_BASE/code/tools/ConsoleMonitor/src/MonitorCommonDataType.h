#ifndef MONITOR_MESSAGE_DATA_HEADER_H__
#define MONITOR_MESSAGE_DATA_HEADER_H__

#include <string>
#include <vector>
#include "core/message/types/MessageTypes.h"


namespace TA_Base_Core
{
    enum NextStep 
    {
        NEXTSTEP_NONE,
        NEXTSTEP_SENDMESSAGE
    };

    struct MonitorCommand
    {
        std::string commandName;
        std::string param;
        int timeToLive;
    };

    struct CommandStateInfo  
    {
        NextStep nextStep;
        int Lock; //maybe need bring a lock to keep synchronize
        std::string commandName;
        std::string optionInfo;
            
       void init()
       {
           nextStep = NEXTSTEP_NONE;
           commandName = "";
           optionInfo = "";
       }

       CommandStateInfo()
       {
           init();
       }

    };

    class InnerMonitorConsts
    {
    public:
        static char * SET_NORMAL_PARAM;
        static char * SET_CORBA_PARAM;
        static char * MONITOR_ENTITYNAME;
        static char * MODULE_TYPE_BASE;
        static char * MODUME_TYPE_COMMON;
        static char * MODULE_TYPE_NSA;
    };

    class MonitorStaticLib
    {
    public:
        static void outputDebugString(const char * str);
        static void splitCommand(const char * commandLine, std::vector<std::string> & vtArgs);
    };


    namespace InnerMonitorMessage
    {
        enum CommandType
        {
            IM_REQUEST_SETPARAM, 
            IM_REPLY_SETPARAM,
            IM_REPLY_STATEUPDATE,
            IM_REQUEST_DUMPCONFIG
        };

        // Root context  NSAMonitor
        static MessageContext Context("InnerMonitor", "InnerMonitor", CommsMessage);  //MessageCategory is not used

        // Message Types for context NamingService
        static const MessageType RequestMonitor(Context, "RequestMonitor");  
        static const MessageType ResponseMonitor(Context, "ResponseMonitor"); 
        static const MessageType AllMessages(Context, "*");
    };
};  

#endif