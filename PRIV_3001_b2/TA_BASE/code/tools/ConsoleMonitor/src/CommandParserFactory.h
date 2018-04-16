#ifndef COMMAND_PARSER_FACTORY_H__
#define COMMAND_PARSER_FACTORY_H__

#include <map>
#include "CommonModuleCommandParser.h" 

namespace TA_Base_Core
{
    class CommandParserFactory
    {
    public:
        ~CommandParserFactory(void);
        void init();
        void cleanup();

        CommonModuleCommandParser * getCurrentCommandParser()
        {
            return m_currentCommandParser;
        }

        static CommandParserFactory & getInstance();
        
        void processModuleCommand(const std::vector<std::string> & vtCommandArg);

        
        CommonModuleCommandParser * getCommandParser(const std::string & moduleName)
        {
            return m_commandParserMap[moduleName];
        }

        CommonModuleCommandParser * setCurrentCommandParser(const std::string & moduleName)
        {
            m_currentCommandParser = m_commandParserMap[moduleName];
            return m_currentCommandParser;
        }

        bool connectModule(const std::string & moduleName, const std::string & entityName, const std::string & endPoint );

    private:
        CommandParserFactory(void);
        bool isCorrectModuleName(const std::string & moduleName);
        std::string listAllModules();

        static CommandParserFactory * m_theClassInstance;
        std::map<std::string, CommonModuleCommandParser *> m_commandParserMap;
        CommonModuleCommandParser* m_currentCommandParser;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;  // Guard the singleton

    };

}
#endif