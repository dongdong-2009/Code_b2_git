/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/NoAlarmAgentException.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * Used by TA_Naming, thrown when a CORBA object can-not be resolved
  */

#include "NoAlarmAgentException.h"

namespace TA_Base_Core
{
    NoAlarmAgentException::NoAlarmAgentException() throw()
    {

    }

    NoAlarmAgentException::NoAlarmAgentException(const std::string& msg) throw() 
        : TransactiveException(msg)
	{

	}

    NoAlarmAgentException::~NoAlarmAgentException() throw()
    {

    }
};
