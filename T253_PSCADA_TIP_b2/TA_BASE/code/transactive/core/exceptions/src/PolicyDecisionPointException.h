#ifndef POLICYDECISIONPOINTEXCEPTION_H_INCLUDED
#define POLICYDECISIONPOINTEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/PolicyDecisionPointException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class PolicyDecisionPointException : public TransactiveException
		{
		public:
			PolicyDecisionPointException() throw();
			PolicyDecisionPointException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~PolicyDecisionPointException() throw();
		private:
			const std::string m_msg;
		};
}; // TA_Base_Core

#endif // POLICYDECISIONPOINTEXCEPTION_H_INCLUDED
