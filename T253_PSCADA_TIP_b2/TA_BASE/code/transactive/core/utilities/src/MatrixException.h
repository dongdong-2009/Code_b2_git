#if !defined(_MATRIXEXCEPTION_H__A7A12810_888E_48F1_A959_D1E6DCAFFB9B__INCLUDED_)
#define _MATRIXEXCEPTION_H__A7A12810_888E_48F1_A959_D1E6DCAFFB9B__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/utilities/src/MatrixException.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * Thrown by Matrix when an attempt is made to access a nonexisting element.
  *
  */

#include "core/exceptions/src/TransactiveException.h"


namespace TA_Base_Core
{
    class MatrixException : public TransactiveException  
    {
    public:
        MatrixException() throw();
        MatrixException(const std::string& msg) throw();
        virtual ~MatrixException() throw();
    };
}

#endif // !defined(_MATRIXEXCEPTION_H__A7A12810_888E_48F1_A959_D1E6DCAFFB9B__INCLUDED_)
