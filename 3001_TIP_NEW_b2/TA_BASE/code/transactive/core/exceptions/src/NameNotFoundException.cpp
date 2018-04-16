/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/NameNotFoundException.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Used by TA_Naming, thrown when a CORBA object can-not be resolved
  */

#include "NameNotFoundException.h"

namespace TA_Base_Core
{
    NameNotFoundException::NameNotFoundException() throw()
    {

    }

    NameNotFoundException::NameNotFoundException(const std::string& msg) throw() 
        : TransactiveException(msg)
	{

	}

    NameNotFoundException::~NameNotFoundException() throw()
    {

    }
};
