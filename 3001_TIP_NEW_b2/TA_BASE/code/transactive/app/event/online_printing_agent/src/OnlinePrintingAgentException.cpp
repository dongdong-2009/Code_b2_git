/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgentException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Thrown when an internal Online Printing Agent error occurs.
  *
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

using namespace TA_Base_App;


OnlinePrintingAgentException::OnlinePrintingAgentException() throw()
{
}


OnlinePrintingAgentException::OnlinePrintingAgentException(const std::string& msg) throw() :
    TA_Base_Core::TransactiveException(msg)
{
}


OnlinePrintingAgentException::OnlinePrintingAgentException(const std::string& msg, const std::string& details) throw() :
    TA_Base_Core::TransactiveException(msg),
    m_details(details)
{
}


OnlinePrintingAgentException::~OnlinePrintingAgentException() throw()
{
}


const char* OnlinePrintingAgentException::getDetails() const throw()
{
    return m_details.c_str();
}
