#ifndef M_Eval_IRealNumExpression_H
#define M_Eval_IRealNumExpression_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/IRealNumberExpression.h $
  * @author:  Ross Tucker 
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * IRealNumberExpression is an interface that allows evaluation of floating point
  * expressions
  */

namespace TA_Base_Bus
{
    class IRealNumberExpression
    {
    public:
       /**
        * Destructor
        * 
        */
        virtual ~IRealNumberExpression(){}


        /**
         * evaluate
         *
         * Evaluates the expression and returns the result
         *
         */
        virtual double evaluate() const = 0;
    };
}

#endif // M_Eval_IRealNumExpression_H
