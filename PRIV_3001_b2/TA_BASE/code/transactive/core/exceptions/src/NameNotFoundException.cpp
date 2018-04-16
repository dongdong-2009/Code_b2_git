/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/NameNotFoundException.cpp $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
