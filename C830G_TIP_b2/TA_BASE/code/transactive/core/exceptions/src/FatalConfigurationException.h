#ifndef FATALCONFIGURATIONEXCEPTION_H_INCLUDED
#define FATALCONFIGURATIONEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/FatalConfigurationException.h $
  *  @author:  Darren Sampson
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class FatalConfigurationException : public TransactiveException
		{
		public:
			FatalConfigurationException() throw();
			FatalConfigurationException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~FatalConfigurationException() throw();
		private:
			const std::string m_msg;
		};
}; // TA_Base_Core

#endif // FATALCONFIGURATIONEXCEPTION_H_INCLUDED
