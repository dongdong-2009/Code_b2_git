#ifndef DUTYAGENTEXCEPTION_H_INCLUDED
#define DUTYAGENTEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/DutyAgentException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  <description>
  */

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class DutyAgentException : public TransactiveException
		{
		public:
			DutyAgentException() throw();
			DutyAgentException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~DutyAgentException() throw();
		private:
			const std::string m_msg;
		};
}; // TA_Base_Core

#endif // DUTYAGENTEXCEPTION_H_INCLUDED
