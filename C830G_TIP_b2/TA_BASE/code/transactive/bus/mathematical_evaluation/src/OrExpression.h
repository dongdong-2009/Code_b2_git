#ifndef M_Eval_OrExpression_H
#define M_Eval_OrExpression_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/OrExpression.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * OrExpression is an object that represents a logical OR operation.
  */
#include <memory>

#include "bus/mathematical_evaluation/src/IBooleanExpression.h"

namespace TA_Base_Bus
{
        class OrExpression : public IBooleanExpression
        {
        public:
        /**
          * Constructor
          *
          * Constructs an AndExpression. This object takes ownership of the 
          * leftHandSide and rightHandSide pointers. Client of this class must 
          * *not* be delete them.
          * 
          */
        OrExpression(std::auto_ptr< IBooleanExpression > leftHandSide,
                     std::auto_ptr< IBooleanExpression > rightHandSide);

        /**
          * Destructor
          */
        virtual ~OrExpression();
  
        /**
          * evaluate
          *
          * Evaluates the boolean expression and returns the result
          *
          */
        virtual bool evaluate() const;

        private:

        // Disable copy constructor and assignment operator.
        OrExpression( const OrExpression& theOrExpression);
        OrExpression& operator=(const OrExpression&);

        std::auto_ptr< IBooleanExpression > m_leftHandSide;
        std::auto_ptr< IBooleanExpression > m_rightHandSide;
        };
}
#endif // M_Eval_OrExpression_H
