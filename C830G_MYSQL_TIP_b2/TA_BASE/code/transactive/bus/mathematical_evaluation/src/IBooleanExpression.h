#ifndef M_Eval_IBooleanExpression_H
#define M_Eval_IBooleanExpression_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/IBooleanExpression.h $
  * @author:  Daniel Hoey 
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * IBooleanExpression is an interface that allows evaluation of boolean
  * expressions
  */

namespace TA_Base_Bus
{
    class IBooleanExpression
    {
    public:
       /**
        * Destructor
        * 
        */
        virtual ~IBooleanExpression(){}


        /**
         * evaluate
         *
         * Evaluates the boolean expression and returns the result
         *
         */
        virtual bool evaluate() const = 0;
    };
}
#endif // M_Eval_IBooleanExpression_H
