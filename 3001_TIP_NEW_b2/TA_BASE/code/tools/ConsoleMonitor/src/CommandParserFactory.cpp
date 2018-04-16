#include <sstream>
#include <assert.h>


#include "CommandParserFactory.h"
#include "NSAModuleCommandParser.h"
#include "CommonModuleCommandParser.h"

#if defined( SOLARIS )
#include "core/utilities/src/StdPubUtility.h"
#define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Core
{
    CommandParserFactory * CommandParserFactory::m_theClassInstance = NULL;
    TA_Base_Core::NonReEntrantThreadLockable CommandParserFactory::m_singletonLock;

    CommandParserFactory::CommandParserFactory(void)
    {
        init();
    }

    CommandParserFactory::~CommandParserFactory(void)
    {
        cleanup();
    }

    CommandParserFactory & CommandParserFactory::getInstance()
    {
        TA_Base_Core::ThreadGuard guard(m_singletonLock);
        if ( m_theClassInstance == NULL )
        {
            m_theClassInstance = new CommandParserFactory();
        }
        return *m_theClassInstance;
    }


    void CommandParserFactory::cleanup()
    {
        std::map<std::string, CommonModuleCommandParser *>::iterator it;
        for(it = m_commandParserMap.begin(); it != m_commandParserMap.end(); it++)
        {
            CommonModuleCommandParser * parser = it->second;
            assert(parser != NULL);
            delete parser;
        }
        m_commandParserMap.clear();
    }

    void CommandParserFactory::init()
    {
        CommonModuleCommandParser * parser = new CommonModuleCommandParser();
        m_commandParserMap[parser->getModuleType()] = parser;

        parser = new NSAModuleCommandParser();
        m_commandParserMap[parser->getModuleType()] = parser;

        m_currentCommandParser = parser;

    }

    std::string CommandParserFactory::listAllModules()
    {
        std::ostringstream ostr;
        ostr << " all modules:  ";
        std::map<std::string, CommonModuleCommandParser *>::iterator it;
        for(it = m_commandParserMap.begin(); it != m_commandParserMap.end(); it++)
        {
            std::string name = it->first;
            ostr << name.c_str() << " ";
        }

        return ostr.str();
    }

    bool CommandParserFactory::isCorrectModuleName(const std::string & moduleName)
    {
        return (m_commandParserMap.find(moduleName) != m_commandParserMap.end());
    }

    void CommandParserFactory::processModuleCommand(const std::vector<std::string> & vtCommandArg)
    {
        if( vtCommandArg.size() == 1 )
        {
            std::string moduleList = listAllModules();
            std::cout << " " << moduleList.c_str() << std::endl;
        }
        else if( vtCommandArg.size() == 2 && _stricmp( "current", vtCommandArg[1].c_str() ) == 0 )
        {
            std::cout << m_currentCommandParser->getModuleType() << std::endl;
        }
        else if( vtCommandArg.size() == 4 ) 
        {
            if( connectModule(vtCommandArg[1], vtCommandArg[2], vtCommandArg[3] ) )
            {
                std::cout << " switch module successfully!"  << std::endl;        
            }
        }
        else
        {
            std::cout <<" invalid module command" << std::endl; 
        }
    }


    bool CommandParserFactory::connectModule(const std::string & moduleName, const std::string & entityName, const std::string & endPoint )
    {
        if (! isCorrectModuleName(moduleName) )
        {
            std::cout <<  "invalid module name: " << moduleName.c_str() << std::endl;
            return false;
        }
        CommonModuleCommandParser * parser = getCommandParser(moduleName);
        assert( parser!=NULL );

        if ( parser->initModuleParser(entityName, endPoint) )
        {
            m_currentCommandParser = parser;
            std::cout << " " << parser->getStateDesc().c_str() << std::endl;
            return true;
        }
        return false;
    }

}