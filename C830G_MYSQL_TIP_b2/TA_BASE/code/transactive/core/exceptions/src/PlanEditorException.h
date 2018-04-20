/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/PlanEditorException.h $
  * @author J. Ebedes
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: 
  * 
  * This file outlines a simple exception class for use with
  * the Plan Editor. It is derived from TransActiveException.
  */
#if !defined(AFX_PLANEDITOREXCEPTION_H__44BE187A_4870_11D7_B150_0080C8E32E69__INCLUDED_)
#define AFX_PLANEDITOREXCEPTION_H__44BE187A_4870_11D7_B150_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class PlanEditorException : public TransactiveException  
	{
	public:
		PlanEditorException();
		PlanEditorException(const std::string& msg);
		virtual ~PlanEditorException();

	};
}

#endif // !defined(AFX_PLANEDITOREXCEPTION_H__44BE187A_4870_11D7_B150_0080C8E32E69__INCLUDED_)

