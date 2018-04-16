#pragma once
#include <WinDef.h>
#include "core/process_management/IDL/src/IManagedGuiCorbaDef.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace Utilities
{
    inline std::string toString(const RECT& r, const std::string& format = "(%d, %d, %d, %d)")
    {
        return str(boost::format(format) % r.left % r.top % r.right % r.bottom);
    }

    inline std::string toString(const TA_Base_Core::IManagedGUICorbaDef::RECT& r, const std::string& format = "(%d, %d, %d, %d)")
    {
        return str(boost::format(format) % r.left % r.top % r.right % r.bottom);
    }
}
