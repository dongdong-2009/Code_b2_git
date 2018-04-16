/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/UtilitiesException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if !defined(AFX_UTILITIESEXCEPTION_H__DCB83ED0_5DA6_11D7_B157_0080C8E32E69__INCLUDED_)
#define AFX_UTILITIESEXCEPTION_H__DCB83ED0_5DA6_11D7_B157_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class UtilitiesException : public TransactiveException  
	{
	public:
		UtilitiesException() throw();
		UtilitiesException(const std::string& msg) throw();
		virtual ~UtilitiesException() throw();

	};
}

#endif // !defined(AFX_UTILITIESEXCEPTION_H__DCB83ED0_5DA6_11D7_B157_0080C8E32E69__INCLUDED_)

