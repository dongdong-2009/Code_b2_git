#ifndef _COMMAND_MONITOR_H__
#define _COMMAND_MONITOR_H__

#include <vector>
#include "core/corba/src/CorbaUtil.h"
#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/corba/IDL/src/IChannelLocatorCorbaDef.h"

#include "MonitorCommonDataType.h"

namespace TA_Base_Core
{
    class AbstractModuleCommandParser
    {
    public:
        AbstractModuleCommandParser();
        virtual ~AbstractModuleCommandParser(void) {};
        
        virtual bool initModuleParser(const std::string & entityName, const std::string & endPoint) = 0;
        
        virtual void postInit() = 0;
        
        virtual void displayUsage() = 0;
        
        virtual std::string getStateDesc() = 0;

    protected:
        virtual bool processCommand(const std::vector<std::string> & vtCommandArgs) = 0;
    
    public:
        const char * getModuleType()
        {
            return m_moduleType.c_str();
        }

        void parseCommand(const std::vector<std::string> & vtCommandArgs);
        
        bool getCatchInput()
        {
            return m_isCatchInput;
        }
    protected:
        std::string m_processedAgentEntityName;
        std::string m_processedAgentEndPoint;
        std::string m_moduleType;
        bool m_isCatchInput; //catch input internally
    };

    //class NamingModuleCommandParser: 
    class CommonModuleCommandParser: public AbstractModuleCommandParser
    {
    public:
        CommonModuleCommandParser();
	    virtual ~CommonModuleCommandParser(void);

        //begin virtual method ---------------------------
        virtual void postInit() {};
        virtual void displayUsage();
        virtual std::string getStateDesc();

        virtual bool initModuleParser(const std::string & entityName, const std::string & endPoint);

        void connectInteractiveObj(const std::vector<std::string> & vtCommandArgs);
        void dumpCorbaReference();


    protected:
        void setRunParam(const char * paramName, const char * paramValue);
        
        virtual bool processCommand(const std::vector<std::string> & vtCommandArgs);

    private:
        CORBA::Object_ptr resolveManangedProcess(const char * entityName);
        void processInteractiveCommand(const std::vector<std::string> & vtCommandArgs);
    
    protected:
        TA_Base_Core::IManagedProcessCorbaDef_var m_managedProcessCorba;
        TA_Base_Core::ISCSInteractive_var m_currIscsInteractiveObj;

    };
}

#endif