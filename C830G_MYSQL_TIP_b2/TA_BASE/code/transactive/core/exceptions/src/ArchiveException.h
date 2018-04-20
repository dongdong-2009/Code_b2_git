/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/ArchiveException.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Exception used by the ArchiveManager and the archive_manager_dai.
  */

#if !defined(AFX_ARCHIVE_EXCEPTION_H__DCB83ED0_5DA6_11D7_B157_0080C8E32E69__INCLUDED_)
#define AFX_ARCHIVE_EXCEPTION_H__DCB83ED0_5DA6_11D7_B157_0080C8E32E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class ArchiveException : public TransactiveException  
	{
	public:
        /**
          * ArchiveException
          *
          * Constructor with reason message.
          *
          * @param  const std::string& msg
          *         The reason for the exception.
          */

		ArchiveException( const std::string& msg ) throw();
		
        /**
          * ~ArchiveException
          *
          * Standard destructor.
          */
     
        virtual ~ArchiveException() throw() {};

        //
        // The standard copy constructor and assignment operator are fine.
        //
	};
}

#endif // !defined(AFX_ARCHIVE_EXCEPTION_H__DCB83ED0_5DA6_11D7_B157_0080C8E32E69__INCLUDED_)

