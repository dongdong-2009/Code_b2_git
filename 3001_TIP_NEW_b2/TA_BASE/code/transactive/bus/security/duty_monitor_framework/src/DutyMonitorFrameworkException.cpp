/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_monitor_framework/src/DutyMonitorFrameworkException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Thrown when an internal Duty Monitor Framework error occurs.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


DutyMonitorFrameworkException::DutyMonitorFrameworkException() throw()
{
    FUNCTION_ENTRY("DutyMonitorFrameworkException()");
    FUNCTION_EXIT;
}


DutyMonitorFrameworkException::DutyMonitorFrameworkException(const std::string& msg) throw() :
    TA_Base_Core::TransactiveException(msg)
{
    FUNCTION_ENTRY("DutyMonitorFrameworkException(const std::string& msg)");
    FUNCTION_EXIT;
}


DutyMonitorFrameworkException::DutyMonitorFrameworkException(const std::string& msg, const std::string& details) throw() :
    TA_Base_Core::TransactiveException(msg),
    m_details(details)
{
    FUNCTION_ENTRY("DutyMonitorFrameworkException(const std::string& msg, const std::string& details)");
    FUNCTION_EXIT;
}


DutyMonitorFrameworkException::~DutyMonitorFrameworkException() throw()
{
    FUNCTION_ENTRY("~DutyMonitorFrameworkException()");
    FUNCTION_EXIT;
}


const char* DutyMonitorFrameworkException::getDetails() const throw()
{
    FUNCTION_ENTRY("getDetails()");
    FUNCTION_EXIT;

    return m_details.c_str();
}
