#pragma once
#include "Utilities.h"

typedef boost::function<void(const std::vector<std::string>&)> Function;

struct Command
{
    Command(Function f, const std::string& command, const std::string& format, const std::string& description);
    void execute(const std::vector<std::string>& args);
    void checkArgs(const std::vector<std::string>& args) const;
    static void parseFormat(const std::string& format, size_t& required, size_t& optional);

    size_t m_required;
    size_t m_optional;
    std::string m_command;
    std::string m_format;
    std::string m_description;
    Function m_function;
};

typedef boost::shared_ptr<Command> CommandPtr;
typedef std::map<std::string, CommandPtr, Utilities::StriCmp> CommandMap;
typedef std::vector<CommandPtr> CommandList;
