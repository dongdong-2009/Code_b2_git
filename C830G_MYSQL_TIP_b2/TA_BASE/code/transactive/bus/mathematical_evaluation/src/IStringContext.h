#ifndef M_Eval_IStringContext_H
#define M_Eval_IStringContext_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/IStringContext.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * IStringContext is a interface to a context for evaluating a string expression. 
  * It provides a method of resolving values of string variables from their name.
  */

#include <string>

namespace TA_Base_Bus
{
    class IStringContext
    {
    public:
      /**
        * Destructor
        * 
        */
        virtual ~IStringContext(){}

        /**
         * getValue
         *
         * @exception MathematicalEvaluationException - if variable cannot be found
         */
        virtual std::string getValue(const std::string& name) const = 0;
    };
}

#endif // M_Eval_IStringContext_H
