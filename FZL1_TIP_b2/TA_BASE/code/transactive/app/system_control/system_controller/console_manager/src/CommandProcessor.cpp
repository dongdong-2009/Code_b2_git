#include "StdAfx.h"
#include "CommandProcessor.h"
#include "Utilities.h"

CommandProcessor& CommandProcessor::addCommand(Function f, ECommandGroup group, const std::string& command, const std::string& format, const std::string& description)
{
    CommandPtr cmd(new Command(f, command, format, description));
    m_commands[command] = cmd;
    m_group2Commands[group].push_back(cmd);
    return *this;
}

CommandProcessor& CommandProcessor::operator()(Function f, ECommandGroup group, const std::string& command, const std::string& format, const std::string& description)
{
    addCommand(f, group, command, format, description);
    return *this;
}

CommandPtr CommandProcessor::getCommand(const std::string& command)
{
    if (m_commands.find(command) == m_commands.end())
    {
        throw std::exception(str(boost::format("'%s' is not a command") % command).c_str());
    }

    return m_commands[command];
}

std::string CommandProcessor::getHelpString()
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

    return strm.str();
}

void CommandProcessor::processCommand(const std::string& line)
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
    CommandPtr cmd = getCommand(command);
    cmd->execute(args);
}
