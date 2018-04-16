/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/database/src/DBException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * This is a simple exception class that stores the error status code for
  * the result of an OCI function call and the reason that the call failed.
  * This class can then be queried for a summary of the error.
  * 
  */

#ifndef DB_EXCEPTION_H
#define DB_EXCEPTION_H

#include <string>
#include <exception>

class DBException : public std::exception
{
public:
	DBException( const std::string status, const std::string reason, 
                     long nativeCode ) throw();
	virtual ~DBException() throw();

	std::string getSummary() const;
	std::string getStatus() const;
	std::string getReason() const;
	long getNativeCode() const;
private:
	std::string    m_status;
	std::string    m_reason;
	long	       m_nativeCode;
};

#endif // DB_EXCEPTION_H

