/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/ReportManagerException.h $
  * @author A. Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by: 
  * 
  * This file outlines a simple exception class for the Report Manager
  */


#if !defined(REPORTMANAGEREXCEPTION)
#define REPORTMANAGEREXCEPTION


#include <string>																
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    class ReportManagerException : public TransactiveException
    {
		public:
			ReportManagerException() throw();
			ReportManagerException(const std::string& msg) throw();
			virtual const char* what() const throw();
			virtual ~ReportManagerException() throw();
		private:
			const std::string m_msg;
    };
}

#endif // !defined(REPORTMANAGEREXCEPTION)

