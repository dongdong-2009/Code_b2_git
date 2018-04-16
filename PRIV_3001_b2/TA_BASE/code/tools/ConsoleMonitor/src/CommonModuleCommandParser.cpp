#include <assert.h>

#include "CommonModuleCommandParser.h"
#include "MonitorCommonDataType.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/StdPubUtility.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#if defined( SOLARIS )
#define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif

namespace TA_Base_Core
{

    AbstractModuleCommandParser::AbstractModuleCommandParser() 
        : m_isCatchInput(false)
        , m_moduleType(TA_Base_Core::InnerMonitorConsts::MODULE_TYPE_BASE)
    {

    }
    void AbstractModuleCommandParser::parseCommand(const std::vector<std::string> & vtCommandArgs)
    {
        if ( ! processCommand(vtCommandArgs) )
        {
            std::cout << "wrong command name: " << vtCommandArgs[0] << std::endl;  
        }

    }


    CommonModuleCommandParser::CommonModuleCommandParser()
        : m_managedProcessCorba(TA_Base_Core::IManagedProcessCorbaDef::_nil())
        , m_currIscsInteractiveObj(TA_Base_Core::ISCSInteractive::_nil())
    {
        m_moduleType = TA_Base_Core::InnerMonitorConsts::MODUME_TYPE_COMMON;
    }

    CommonModuleCommandParser::~CommonModuleCommandParser(void)
    {
        m_managedProcessCorba = TA_Base_Core::IManagedProcessCorbaDef::_nil();
        m_currIscsInteractiveObj = TA_Base_Core::ISCSInteractive::_nil();
    }

    bool CommonModuleCommandParser::initModuleParser(const std::string & entityName, const std::string & endPoint)
    {
        char agentLocBuffer[1024];
        agentLocBuffer[0] = 0x0;
        sprintf(agentLocBuffer, "corbaloc::%s/%s_ManagedProcess", endPoint.c_str(), entityName.c_str());

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CommonModuleCommandParser::setRunParam: corbaLocString <%s>", agentLocBuffer); 

        try
        {
            //for old app, it do not support name resolution for managedProcess corba object on specific port
            if ( !TA_Base_Core::RunParams::getInstance().isSet("IsOldManagedProcess") )  
            {
                CORBA::Object_var tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(agentLocBuffer);
                TA_Base_Core::IManagedProcessCorbaDef_ptr objRep = TA_Base_Core::IManagedProcessCorbaDef::_narrow(tmpVar);
                m_managedProcessCorba = objRep;
            }
            else
            {
                m_managedProcessCorba = TA_Base_Core::IManagedProcessCorbaDef::_nil();
            }
            m_processedAgentEntityName = entityName;
            m_processedAgentEndPoint = endPoint;
            postInit();

            return true;
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "Corba Exception: " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            std::cout << msg.c_str() << std::endl;
        }
        catch(std::exception & e)
        {
            std::string msg( "Exception: " );
            msg.append( e.what() );
            std::cout << msg.c_str() << std::endl;
        }
        catch(...)
        {
            std::string msg( "Unknown exception" );
            std::cout << msg.c_str() << std::endl;

        }
        return false;
    }

    void CommonModuleCommandParser::displayUsage()
    {
        std::cout << " setparam param=value" << std::endl;
        std::cout << " setcorbaparam param=value" << std::endl;
        std::cout << " conninter endpoint interactiveServantKey" << std::endl;
        std::cout << " inter command argument" << std::endl;
        std::cout << " state" << std::endl;
        std::cout << " dumpcorbaref" << std::endl;

    }

    std::string CommonModuleCommandParser::getStateDesc()
    {
        std::ostringstream ostr;
        ostr << "module name: "<< m_moduleType << ", entity name: " << m_processedAgentEntityName <<", endpoint: " << m_processedAgentEndPoint;
        return ostr.str();
    }

    bool CommonModuleCommandParser::processCommand(const std::vector<std::string> & vtCommandArgs)
    {
        bool fProcessed = false;
        if (_stricmp("state", vtCommandArgs[0].c_str() ) == 0)
        {   
            fProcessed = true;
            std::cout << " " << getStateDesc().c_str() << std::endl;
        }
        else if (_stricmp(InnerMonitorConsts::SET_NORMAL_PARAM, vtCommandArgs[0].c_str() ) == 0
            ||   _stricmp(InnerMonitorConsts::SET_CORBA_PARAM, vtCommandArgs[0].c_str() ) == 0 )
        {
            fProcessed = true;
            char tmpBuffer[256];
            strcpy(tmpBuffer, vtCommandArgs[1].c_str() );
            char * paramValue = tmpBuffer;
            char * paramName = TA_Base_Core::CStdPubUtility::seperateText(&paramValue, '=');

            if( TA_Base_Core::CStdPubUtility::isEmptyString(paramValue) )
            {
                std::cout << " wrong parameter value, should not empty" << std::endl;
                return false;
            }

            if( TA_Base_Core::CStdPubUtility::isEmptyString(paramName) )
            {
                std::cout << " wrong parameter name, should not empty" << std::endl;
                return false;
            }

            if ( _stricmp(InnerMonitorConsts::SET_NORMAL_PARAM, vtCommandArgs[0].c_str() ) == 0 )
            {
                setRunParam(paramName, paramValue);
            }
            else
            {
                setRunParam("corba", vtCommandArgs[1].c_str());
            }
        }
        else if ( _stricmp("inter", vtCommandArgs[0].c_str() ) == 0 )
        {
            fProcessed = true;
            processInteractiveCommand(vtCommandArgs);
        }
        else if ( _stricmp("conninter", vtCommandArgs[0].c_str() ) == 0 )
        {
            fProcessed = true;
            connectInteractiveObj(vtCommandArgs);
        }
        else if ( _stricmp("dumpcorbaref", vtCommandArgs[0].c_str() ) == 0 )
        {
            fProcessed = true;
            dumpCorbaReference();
        }
        return fProcessed;
    }


    void CommonModuleCommandParser::setRunParam(const char * paramName, const char * paramValue)
    {
        if ( CORBA::is_nil(m_managedProcessCorba) )
        {
            std::cout << " ManangedProcess Corba interface could not be resolved" << std::endl;
            return;
        }

        try
        {
            if ( paramName == 0x0 || *paramName == 0x0 )
            {
                throw "Parameter name should not be empty";
            }

            if ( paramValue == 0x0 || *paramValue == 0x0 )
            {
                throw "Parameter value should not be empty";
            }

            TA_Base_Core::RunParam runParamCorbaDef;
            runParamCorbaDef.name = (const char*) paramName;
            runParamCorbaDef.value = (const char*) paramValue;

            m_managedProcessCorba->setParam(runParamCorbaDef);
            std::string msg(" set parameter successful!");
            std::cout << msg.c_str() << std::endl;
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "Corba Exception: " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            std::cout << msg.c_str() << std::endl;
        }
        catch(std::exception & e)
        {
            std::string msg( "Exception: " );
            msg.append( e.what() );
            std::cout << msg.c_str() << std::endl;
        }
        catch(...)
        {
            std::string msg( "Exception: wrong command parameter" );
            std::cout << msg.c_str() << std::endl;

        }

    } //end function


    void CommonModuleCommandParser::processInteractiveCommand(const std::vector<std::string> & vtCommandArgs)
    {
        if ( CORBA::is_nil( m_currIscsInteractiveObj ) )
        {
            std::cout << " please use command <conninter> to set interactive object first"  << std::endl;
            return;
        }

        size_t argCount = vtCommandArgs.size(); 

        if ( argCount < 2)
        {
            std::cout << " wrong command of [inter]"  << std::endl;
            return;
        }

        CORBA::Boolean fResult = false;
        CORBA::Boolean targetChanged = false;
        TA_Base_Core::ISCSInteractive_ptr newTargetObj;

        char argBuffer[1024];
        argBuffer[0] = 0x0;
        for(int i=2; i<argCount; i++)
        {
            strcat(argBuffer, vtCommandArgs[i].c_str());
            if (i!=argCount-1)
            {
                strcat(argBuffer, " ");
            }
        }

        char * response = m_currIscsInteractiveObj->processCommand(vtCommandArgs[1].c_str(), argBuffer, fResult,
            targetChanged, newTargetObj);

        std::cout << response << std::endl;
        CORBA::string_free(response);
    }

    void CommonModuleCommandParser::connectInteractiveObj(const std::vector<std::string> & vtCommandArgs)
    {
        if ( vtCommandArgs.size() != 3)
        {
            std::cout << " wrong command of [conninter]" << std::endl;
            return;
        }

        char agentLocBuffer[1024];
        agentLocBuffer[0] = 0x0;
        sprintf(agentLocBuffer, "corbaloc::%s/%s", vtCommandArgs[1].c_str(), vtCommandArgs[2].c_str());

        LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CommonModuleCommandParser::connectInteractiveObj: corbaLocString <%s>", agentLocBuffer); 

        try
        {
            CORBA::Object_var tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(agentLocBuffer);
            TA_Base_Core::ISCSInteractive_ptr objRep = TA_Base_Core::ISCSInteractive::_narrow(tmpVar);
            m_currIscsInteractiveObj = objRep;
            std::cout << " set interactive object successfully" << std::endl;
        }
        catch (CORBA::Exception& e)
        {
            std::string msg( "Corba Exception: " );
            msg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e) );
            std::cout << msg.c_str() << std::endl;
        }
        catch(std::exception & e)
        {
            std::string msg( "Exception: " );
            msg.append( e.what() );
            std::cout << msg.c_str() << std::endl;
        }
        catch(...)
        {
            std::string msg( "Unknown exception" );
            std::cout << msg.c_str() << std::endl;

        }
    }

    void CommonModuleCommandParser::dumpCorbaReference()
    {
        if ( !CORBA::is_nil(m_managedProcessCorba) )
        {
            CORBA::String_var objToString( (CorbaUtil::getInstance().getOrb()).object_to_string(m_managedProcessCorba) );
            std::cout << "Agent managed process corba string: " << objToString.in() << std::endl;
        }

        if (! CORBA::is_nil(m_currIscsInteractiveObj))
        {
            CORBA::String_var objToString( (CorbaUtil::getInstance().getOrb()).object_to_string(m_currIscsInteractiveObj) );
            std::cout << "ISCS interactive corba string: " << objToString.in() << std::endl;

        }
        
    
    }

}