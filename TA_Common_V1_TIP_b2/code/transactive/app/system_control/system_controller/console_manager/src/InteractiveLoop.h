#pragma once
#include "Command.h"
#include "Singleton.h"

struct InteractiveLoop : Singleton<InteractiveLoop>
{
public:

    struct ExitLoopException {};
    enum ECommandGroup { GeneralCommand, AgentCommand, GuiCommand };
    typedef std::map<ECommandGroup, CommandList> Group2CommandListMap;

public:

    InteractiveLoop();
    void InteractiveLoop::run();

protected:

    void initCommands();
    void initHelpString();
    void addCommand(Function f, ECommandGroup group, const std::string& command, const std::string& format, const std::string& description);
    void processCommand(const std::string& line);
    CommandPtr getCommand(const std::string& command);
    static unsigned long getId(const std::string& idStr);

protected:

    static void exit(const std::vector<std::string>& args);
    static void help(const std::vector<std::string>& args);
    static void cls(const std::vector<std::string>& args);
    static void list(const std::vector<std::string>& args);
    static void terminate(const std::vector<std::string>& args);
    static void chprofile(const std::vector<std::string>& args);
    static void set(const std::vector<std::string>& args);
    static void systemcontroller(const std::vector<std::string>& args);
    static void controlstation(const std::vector<std::string>& args);
    static void maximise(const std::vector<std::string>& args);
    static void minimise(const std::vector<std::string>& args);
    static void restore(const std::vector<std::string>& args);
    static void server(const std::vector<std::string>& args);
    static void setparam(const std::vector<std::string>& args);
    static void setparamthis(const std::vector<std::string>& args);

protected:

    std::string m_helpString;
    CommandMap m_commands;
    Group2CommandListMap m_group2Commands;
};
