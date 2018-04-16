/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/MathematicalEvaluationException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
