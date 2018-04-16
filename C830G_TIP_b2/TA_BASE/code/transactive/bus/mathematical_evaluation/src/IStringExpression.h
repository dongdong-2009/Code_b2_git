#ifndef M_Eval_IStringExpression_H
#define M_Eval_IStringExpression_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/IStringExpression.h $
  * @author:  Daniel Hoey 
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * IStringExpression is an interface that allows evaluation of string
  * expressions
  */
#include <string>

namespace TA_Base_Bus
{
    class IStringExpression
    {
    public:
       /**
        * Destructor
        * 
        */
        virtual ~IStringExpression(){}


        /**
         * evaluate
         *
         * Evaluates the string expression and returns the result
         *
         */
        virtual std::string evaluate() const = 0;
    };
}
#endif // M_Eval_IStringExpression_H
