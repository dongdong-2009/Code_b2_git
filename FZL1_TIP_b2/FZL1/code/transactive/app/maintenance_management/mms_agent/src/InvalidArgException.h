/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/maintenance_management/mms_agent/src/InvalidArgException.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides an exception that should be thrown if you value
  * is passed to a method that is invalid
  *
  */
#if !defined(AFX_INVALIDARGVALUEEXCEPTION_H__27C5C1BD_FDD1_4626_B72F_A5FEEB9E1D62__INCLUDED_)
#define AFX_INVALIDARGVALUEEXCEPTION_H__27C5C1BD_FDD1_4626_B72F_A5FEEB9E1D62__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{
	class InvalidArgException : public TA_Base_Core::TransactiveException
	{
	public:
		InvalidArgException() {}

		InvalidArgException(const std::string& msg)		
			: TA_Base_Core::TransactiveException(msg) {}

		virtual ~InvalidArgException() throw() {};

	};
} //namespace TA_IRS_Bus

#endif // !defined(AFX_INVALIDARGVALUEEXCEPTION_H__27C5C1BD_FDD1_4626_B72F_A5FEEB9E1D62__INCLUDED_)
