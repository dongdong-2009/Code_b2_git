#ifndef ATSAGENTEXCEPTION_H_INCLUDED
#define ATSAGENTEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/AtsAgentException.h $
  *  @author:  Darren Sampson
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class AtsAgentException : public TransactiveException
		{
		public:
			AtsAgentException() throw();
			AtsAgentException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~AtsAgentException() throw();
		private:
			const std::string m_msg;
		};
}; // TA_Base_Core

#endif // ATSAGENTEXCEPTION_H_INCLUDED
