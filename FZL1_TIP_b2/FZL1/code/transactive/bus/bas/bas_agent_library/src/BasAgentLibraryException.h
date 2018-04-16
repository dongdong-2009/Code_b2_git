/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/FZL1/transactive/bus/bas/bas_agent_library/src/BasAgentLibraryException.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:59:10 $
  * Last modified by:  $Author: CM $
  *
  * This is used to get access to the pa agent.
  */

#if !defined(BAS_AGENT_LIVRARY_EXCEPTION_H)
#define BAS_AGENT_LIVRARY_EXCEPTION_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{
class BasNotDeployStationException : public TA_Base_Core::TransactiveException
{
public:
    BasNotDeployStationException(const std::string& msg) throw()
        :TA_Base_Core::TransactiveException(msg)
    {
    }
protected:
private:
};

class BasAgentInvalidException : public TA_Base_Core::TransactiveException
{
public:
    BasAgentInvalidException(const std::string& msg) throw()
        :TA_Base_Core::TransactiveException(msg)
    {
    }
protected:
private:
};
}

#endif