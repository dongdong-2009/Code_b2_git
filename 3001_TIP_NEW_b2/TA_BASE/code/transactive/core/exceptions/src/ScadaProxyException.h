#ifndef SCADAPROXYEXCEPTION_H_INCLUDED
#define SCADAPROXYEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/ScadaProxyException.h $
  *  @author:  Darren Sampson
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
	class ScadaProxyException : public TransactiveException
		{
		public:
			ScadaProxyException() throw();
			ScadaProxyException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~ScadaProxyException() throw();
		private:
			const std::string m_msg;
		};
}; // TA_Base_Core

#endif // SCADAPROXYEXCEPTION_H_INCLUDED
