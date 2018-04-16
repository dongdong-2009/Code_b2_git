/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/PlanEditorException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

