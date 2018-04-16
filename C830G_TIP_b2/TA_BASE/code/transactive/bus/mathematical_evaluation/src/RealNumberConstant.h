#ifndef M_Eval_RealNumConstant_H
#define M_Eval_RealNumConstant_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/RealNumberConstant.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * A constant in a real number expression.
  */

#include "bus/mathematical_evaluation/src/IRealNumberExpression.h"

namespace TA_Base_Bus
{
        class RealNumberConstant : public IRealNumberExpression
        {
        public:
        /**
          * Constructor
          * 
          */
        RealNumberConstant(double value);
     
        /**
          * Destructor
          * 
          */
        virtual ~RealNumberConstant();

        /**
         * evaluate
         *
         * Evaluates the expression and returns the result
         *
         */
        virtual double evaluate() const;

        // Allow RealNumberConstant to be copyable.
        RealNumberConstant( const RealNumberConstant& theRealNumberConstant);
        
        private:

        // Disable assignment operator.
        RealNumberConstant& operator=(const RealNumberConstant&);

        double m_value;
        };
}
#endif // M_Eval_RealNumConstant_H
