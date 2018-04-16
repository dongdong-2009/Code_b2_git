/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/MathematicalEvaluationException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// ApplicationException.cpp: implementation of the ApplicationException class.
//
//////////////////////////////////////////////////////////////////////

#include "MathematicalEvaluationException.h"
//#include "core\utilities\src\DebugUtil.h"


namespace TA_Base_Core
{
    MathematicalEvaluationException::MathematicalEvaluationException(const std::string& message) throw()
        : TransactiveException(message)
    {
    }

    MathematicalEvaluationException::~MathematicalEvaluationException() throw()
    {
    }
} // Closes TA_Base_Core
