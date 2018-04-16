#include "StdAfx.h"
#include "InteractiveLoop.h"
#include "ProcessManager.h"
#include "SystemControllerAdmin.h"
#include "ControlStationCorbaImpl.h"
#include "Utilities.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;
static const char* DEFAULT_PASSWORD = "<empty>";

InteractiveLoop::InteractiveLoop()
{
    initCommands();
    initHelpString();
}

void InteractiveLoop::initCommands()
{
    addCommand(&InteractiveLoop::help, GeneralCommand, "help", "help", "Print this message");
    addCommand(&InteractiveLoop::exit, GeneralCommand, "exit", "exit", "This will close the Console Manager");
    addCommand(&InteractiveLoop::cls, GeneralCommand, "cls", "cls", "Clear the screen");
    addCommand(&InteractiveLoop::list, GeneralCommand, "list", "list", "Lists all currently running processes");
    addCommand(&InteractiveLoop::terminate, GeneralCommand, "terminate", "terminate <id>", "Terminates the specified application");
    addCommand(&InteractiveLoop::chprofile, GeneralCommand, "chprofile", "chprofile <profile id> <location id> [password]", str(boost::format("Changes current session profile(default password: %s)") % DEFAULT_PASSWORD));
    addCommand(&InteractiveLoop::set, AgentCommand, "set", "set <control|monitor> <id>", "This will tell the agent to switch to control/monitor mode");
    addCommand(&InteractiveLoop::systemcontroller, AgentCommand, "systemcontroller", "systemcontroller <start|stop>", "Activate/Deactivate SystemController");
    addCommand(&InteractiveLoop::controlstation, AgentCommand, "controlstation", "controlstation <start|stop>", "Activate/Deactivate ControlStation");
    addCommand(&InteractiveLoop::maximise, GuiCommand, "maximise", "maximise <id>", "This will tell the GUI to maximise");
    addCommand(&InteractiveLoop::minimise, GuiCommand, "minimise", "minimise <id>", "This will tell the GUI to minimise");
    addCommand(&InteractiveLoop::restore, GuiCommand, "restore", "restore <id>", "This will tell the GUI to restore and gain the focus");
    addCommand(&InteractiveLoop::server, GuiCommand, "server", "server <up|down> <id>", "This will tell the GUI the server has come up/has gone down");
    addCommand(&InteractiveLoop::setparam, GuiCommand, "setparam", "setparam <id> <name> [value]", "Set a runparam (and its optional value) for the specified Managed Process");
    addCommand(&InteractiveLoop::setparamthis, GuiCommand, "setparamthis", "setparamthis <id> <name> [value]", "Set a runparam (and its optional value) for this (ConsoleManager)");;
}

void InteractiveLoop::run()
{
    std::cout << m_helpString << std::endl;

    for (std::string line; std::getline(std::cin, line);)
    {
        try
        {
            processCommand(line);
        }
        catch (const std::exception& ex)
        {
            if (::strlen(ex.what()))
            {
                std::cout << ex.what() << std::endl;
            }
        }
        catch (const ExitLoopException&)
        {
            break;
        }
        catch (...)
        {
            std::cout << "unknown exception." << std::endl;
        }
    }
}

void InteractiveLoop::initHelpString()
{
    static const char* groupNames[] = { "General Commands", "Agent Commands", "GUI Commands" };
    std::stringstream strm;

    BOOST_FOREACH(Group2CommandListMap::value_type& v, m_group2Commands)
    {
        strm << groupNames[v.first] << ":\n";

        BOOST_FOREACH(CommandPtr command, v.second)
        {
            strm << boost::format("\t%s - %s") % command->m_format % command->m_description << std::endl;
        }
    }

    m_helpString = strm.str();
}

unsigned long InteractiveLoop::getId(const std::string& idStr)
{
    try
    {
        return boost::lexical_cast<unsigned long>(idStr);
    }
    catch (...)
    {
        throw std::exception(str(boost::format("bad id: %d") % idStr).c_str());
    }
}

void InteractiveLoop::addCommand(Function f, ECommandGroup group, const std::string& command, const std::string& format, const std::string& description)
{
    CommandPtr cmd(new Command(f, command, format, description));
    m_commands[command] = cmd;
    m_group2Commands[group].push_back(cmd);
}

CommandPtr InteractiveLoop::getCommand(const std::string& command)
{
    if (m_commands.find(command) == m_commands.end())
    {
        throw std::exception(str(boost::format("'%s' is not a command") % command).c_str());
    }

    return m_commands[command];
}

void InteractiveLoop::processCommand(const std::string& line)
{
    std::string command;
    std::vector<std::string> args;

    boost::split(args, line, boost::is_space(), boost::token_compress_on);
    args.erase(std::remove_if(args.begin(), args.end(), Utilities::IsEmptyString()), args.end());

    if (args.empty())
    {
        throw std::exception("");
    }

    command = args[0];
    args.erase(args.begin());
    getCommand(command)->execute(args);
}

void InteractiveLoop::exit(const std::vector<std::string>& args)
{
    throw ExitLoopException();
}

void InteractiveLoop::help(const std::vector<std::string>& args)
{
    std::cout << InteractiveLoop::getInstance().m_helpString << std::endl;
}

void InteractiveLoop::cls(const std::vector<std::string>& args)
{
    system("cls");
}

void InteractiveLoop::list(const std::vector<std::string>& args)
{
    std::cout << ProcessManager::getInstance().getRunningApps() << std::endl;
}

void InteractiveLoop::terminate(const std::vector<std::string>& args)
{
    unsigned long id = getId(args[0]);
    ProcessManager::getInstance().terminateApp(id);
}

void InteractiveLoop::chprofile(const std::vector<std::string>& args)
{
    const std::string& profileIdParam = args[0];
    const std::string& locationIdParam = args[1];
    std::string passwordParam = DEFAULT_PASSWORD;

    if (2 < args.size())
    {
        passwordParam = args[2];
    }

    Utilities::changeProfile(profileIdParam, locationIdParam, passwordParam);
}

void InteractiveLoop::set(const std::vector<std::string>& args)
{
    const std::string& mode = args[0];
    unsigned long id = getId(args[1]);

    if (boost::iequals(mode, "Control"))
    {
        ProcessManager::getInstance().changeMode(true, id);
    }
    else if (boost::iequals(mode, "Monitor"))
    {
        ProcessManager::getInstance().changeMode(false, id);
    }
    else
    {
        std::cout << "Invalid mode specified" << std::endl;
    }
}

void InteractiveLoop::systemcontroller(const std::vector<std::string>& args)
{
    const std::string& mode = args[0];

    if (boost::iequals(mode, "start"))
    {
        SystemControllerAdmin::getInstance().activate();
    }
    else if (boost::iequals(mode, "stop"))
    {
        SystemControllerAdmin::getInstance().deactivate();
    }
    else
    {
        std::cout << "Invalid state specified" << std::endl;
    }
}

void InteractiveLoop::controlstation(const std::vector<std::string>& args)
{
    const std::string& mode = args[0];

    if (boost::iequals(mode, "start"))
    {
        ControlStationCorbaImpl::getInstance().activate();
    }
    else if (boost::iequals(mode, "stop"))
    {
        ControlStationCorbaImpl::getInstance().deactivate();
    }
    else
    {
        std::cout << "Invalid state specified" << std::endl;
    }
}

void InteractiveLoop::maximise(const std::vector<std::string>& args)
{
    unsigned long id = getId(args[0]);
    ProcessManager::getInstance().changeGUI(TA_Base_Core::Maximise, id);
}

void InteractiveLoop::minimise(const std::vector<std::string>& args)
{
    unsigned long id = getId(args[0]);
    ProcessManager::getInstance().changeGUI(TA_Base_Core::Minimise, id);
}

void InteractiveLoop::restore(const std::vector<std::string>& args)
{
    unsigned long id = getId(args[0]);
    ProcessManager::getInstance().changeGUI(TA_Base_Core::GetFocus, id);
}

void InteractiveLoop::server(const std::vector<std::string>& args)
{
    const std::string& mode = args[0];
    unsigned long id = getId(args[1]);

    if (boost::iequals(mode, "up"))
    {
        ProcessManager::getInstance().serverChange(true, id);
    }
    else if (boost::iequals(mode, "down"))
    {
        ProcessManager::getInstance().serverChange(false, id);
    }
    else
    {
        std::cout << "Invalid server state specified" << std::endl;
    }
}

void InteractiveLoop::setparam(const std::vector<std::string>& args)
{
    unsigned long id = getId(args[0]);
    const std::string& paramName = args[1];
    std::string paramValue;

    if (2 < args.size())
    {
        paramValue = args[2];
    }

    ProcessManager::getInstance().setClientRunParam(id, paramName, paramValue);
}

void InteractiveLoop::setparamthis(const std::vector<std::string>& args)
{
    const std::string& paramName = args[0];
    std::string paramValue;

    if (1 < args.size())
    {
        paramValue = args[1];
    }

    RunParams::getInstance().set(paramName, paramValue);
    ProcessManager::getInstance().onRunParamChange(paramName, paramValue);
}
