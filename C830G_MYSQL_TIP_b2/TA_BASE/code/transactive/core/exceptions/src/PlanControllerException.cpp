/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/PlanControllerException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
// PlanControllerException.cpp: implementation of the PlanControllerException class.
//
//////////////////////////////////////////////////////////////////////

#include "PlanControllerException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{

	PlanControllerException::PlanControllerException()
	{

	}

	PlanControllerException::PlanControllerException(const std::string& msg) 
		: TransactiveException(msg)
	{

	}

	PlanControllerException::~PlanControllerException()
	{

	}
}
