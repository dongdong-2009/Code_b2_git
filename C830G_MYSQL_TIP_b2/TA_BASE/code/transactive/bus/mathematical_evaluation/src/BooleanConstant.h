#ifndef M_Eval_BooleanConstant_H
#define M_Eval_BooleanConstant_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/BooleanConstant.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * A constant in a boolean expression.
  */

#include "bus/mathematical_evaluation/src/IBooleanExpression.h"

namespace TA_Base_Bus
{
        class BooleanConstant : public IBooleanExpression
        {
        public:
          /**
            * Constructor
            * 
            */
        BooleanConstant(bool value);
     
          /**
            * Destructor
            * 
            */
        virtual ~BooleanConstant();

        /**
         * evaluate
         *
         * Evaluates the boolean expression and returns the result
         *
         */
        virtual bool evaluate() const;

        // Allow BooleanConstant to be copyable.
        BooleanConstant( const BooleanConstant& theBooleanConstant);

        private:

        // Disable assignment operator.
        BooleanConstant& operator=(const BooleanConstant&);

        bool m_value;
        };
}
#endif // M_Eval_BooleanConstant_H
