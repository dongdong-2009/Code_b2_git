#ifndef M_Eval_RealNumAbsoluteExpression_H
#define M_Eval_RealNumAbsoluteExpression_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_TA_BASE_DDB/transactive/bus/mathematical_evaluation/src/Attic/RealNumberSquareRootExpression.h,v $
  * @author:  Robert Stagg
  * @version: $Revision: $
  *
  * Last modification: $DateTime: 2005/03/01 05:55:22 $
  * Last modified by:  $Author: roberts $
  * 
  * RealNumExpression represents a real number absolute operation
  */
#include <memory>

#include "bus/mathematical_evaluation/src/IRealNumberExpression.h"

namespace TA_Base_Bus
{
        class RealNumberAbsoluteExpression : public IRealNumberExpression
        {
        public:
      /**
        * Constructor
        *
        * Constructs a RealNumberExpression. This object takes ownership of the 
        * leftHandSide and rightHandSide pointers. Client of this class must 
        * *not* be delete them.
        * 
        */
        RealNumberAbsoluteExpression(std::auto_ptr< IRealNumberExpression > rightHandSide);

      /**
        * Destructor
        */
        virtual ~RealNumberAbsoluteExpression();
                 
      /**
        * evaluate
        *
        * Evaluates the expression and returns 
        *  "leftHandSide ^ rightHandSide"
        *
        */
        virtual double evaluate() const;

        private:

        // Disable copy constructor and assignment operator.
        RealNumberAbsoluteExpression( const RealNumberAbsoluteExpression& theRealNumberAbsoluteExpression);
        RealNumberAbsoluteExpression& operator=(const RealNumberAbsoluteExpression&);

        std::auto_ptr< IRealNumberExpression > m_rightHandSide;
        };
}
#endif // M_Eval_RealNumAbsoluteExpression_H
