/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/InterlockingException.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * Exception for the Interlocking library
  */

#ifndef InterlockingException_H
#define InterlockingException_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"

namespace TA_Base_Core
{
    class InterlockingException : public TransactiveException
    {
    public:

     /**
       * Constructor
       *
       * This constructs an InterlockingException.
       *
       * @param message - This contains as much information as possible about the error that occurred. This can be any
       *                additional information about the error.
       *
       */
       InterlockingException(const std::string& message) throw();


       virtual ~InterlockingException() throw();
    };
} // Closes TA_Base_Core

#endif // InterlockingException_H
