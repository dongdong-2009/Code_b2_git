/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/utilities/src/MatrixException.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Thrown by Matrix when an attempt is made to access a nonexisting element.
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/MatrixException.h"


namespace TA_Base_Core
{
    MatrixException::MatrixException() throw()
    {
    }


    MatrixException::MatrixException(const std::string& msg) throw() :
        TransactiveException(msg)
    {
    }


    MatrixException::~MatrixException() throw()
    {
    }
}
