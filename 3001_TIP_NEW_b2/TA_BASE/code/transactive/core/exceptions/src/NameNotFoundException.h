#if !defined(NAME_NOT_FOUND_EXCEPTION)
#define NAME_NOT_FOUND_EXCEPTION
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/NameNotFoundException.h $
  * @author:  <Cameron Rochester>
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Used by TA_Naming, thrown when a CORBA object can-not be resolved
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
    // OBSOLETE.  
    // Seems to have been replaced by ObjectResolutionException

    class NameNotFoundException : public TransactiveException  
    {
    public:
        NameNotFoundException() throw();
        NameNotFoundException(const std::string& msg) throw();
        virtual ~NameNotFoundException() throw();

    protected:

        // This is where any exception messages are stored
        std::string m_message;
    };
};

#endif // !defined(NAME_NOT_FOUND_EXCEPTION)
