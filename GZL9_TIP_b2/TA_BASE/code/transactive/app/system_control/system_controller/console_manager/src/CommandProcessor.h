#pragma once
#include "Command.h"

enum ECommandGroup { GeneralCommand, AgentCommand, GuiCommand };
typedef boost::function<void(const std::vector<std::string>&)> Function;
typedef std::vector<CommandPtr> CommandList;
typedef std::map<ECommandGroup, CommandList> Group2CommandListMap;

struct CommandProcessor
{
    CommandProcessor& addCommand(Function f, ECommandGroup group, const std::string& command, const std::string& format, const std::string& description);
    CommandProcessor& operator()(Function f, ECommandGroup group, const std::string& command, const std::string& format, const std::string& description);
    CommandPtr getCommand(const std::string& command);
    std::string getHelpString();
    void processCommand(const std::string& line);
    CommandMap m_commands;
    Group2CommandListMap m_group2Commands;
};
