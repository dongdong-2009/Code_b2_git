#include "StdAfx.h"
#include "Command.h"

Command::Command(Function f, const std::string& command, const std::string& format, const std::string& description)
    : m_function(f),
      m_command(command),
      m_format(format),
      m_description(description),
      m_required(0),
      m_optional(0)
{
    parseFormat(m_format, m_required, m_optional);
}

void Command::execute(const std::vector<std::string>& args)
{
    checkArgs(args);
    m_function(args);
}

void Command::checkArgs(const std::vector<std::string>& args) const
{
    if (args.size() < m_required || (m_required + m_optional) < args.size())
    {
        throw std::exception(str(boost::format("bad args number, expected: [%d, %d], actual: %d. %s")
                                 % m_required % (m_required + m_optional) % args.size() % m_format).c_str());
    }
}

void Command::parseFormat(const std::string& format, size_t& required, size_t& optional)
{
    static const boost::regex e("(?x) (<[^<>]+>) | (\\[[^\\[\\]]+\\])");
    boost::sregex_iterator it(format.begin(), format.end(), e);
    boost::sregex_iterator end;

    for (; it != end; ++it)
    {
        if ((*it)[1].matched)
        {
            required++;
        }

        if ((*it)[2].matched)
        {
            optional++;
        }
    }
}
