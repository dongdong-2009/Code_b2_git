/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/DbConnectionFailed.h $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Raised by the db code when a connection to the database fails.
  */


#if !defined(DBCONNECTIONFAILED_H_INCLUDED)
#define DBCONNECTIONFAILED_H_INCLUDED

#include "DatabaseException.h"
#include <string>

namespace TA_Base_Core
{
	class DbConnectionFailed : public DatabaseException  
	{
	public:

		DbConnectionFailed(const std::string& msg) throw();
	};
} // Closes TA_Base_Core

#endif // !defined(DBCONNECTIONFAILED_H_INCLUDED)

