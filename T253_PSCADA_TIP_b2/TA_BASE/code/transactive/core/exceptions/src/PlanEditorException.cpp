/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/PlanEditorException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// PlanEditorException.cpp: implementation of the PlanEditorException class.
//
//////////////////////////////////////////////////////////////////////

#include "PlanEditorException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{

	PlanEditorException::PlanEditorException()
	{

	}

	PlanEditorException::PlanEditorException(const std::string& msg) 
		: TransactiveException(msg)
	{

	}

	PlanEditorException::~PlanEditorException()
	{

	}
}

