#ifndef M_Eval_EqualityComparison_H
#define M_Eval_EqualityComparison_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/EqualityComparison.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * EqualityComparison represent the equality operation.
  */
#include <memory>

#include "bus/mathematical_evaluation/src/IBooleanExpression.h"
#include "bus/mathematical_evaluation/src/IRealNumberExpression.h"
#include "bus/mathematical_evaluation/src/IStringExpression.h"

namespace TA_Base_Bus
{
        class EqualityComparison : public IBooleanExpression
        {
        public:
        /**
          * Constructor
          *
          * Constructs a EqualityComparison expression. This object takes ownership
          * of the leftHandSide and rightHandSide pointers. Client of this class
          * must *not* be delete them.
          *
          */
        EqualityComparison(std::auto_ptr< IRealNumberExpression > leftHandSide,
                           std::auto_ptr< IRealNumberExpression > rightHandSide);

        /**
          * Constructor
          *
          * Constructs a EqualityComparison expression. This object takes ownership
          * of the leftHandSide and rightHandSide pointers. Client of this class
          * must *not* be delete them.
          *
          */
        EqualityComparison(std::auto_ptr< IBooleanExpression > leftHandSide,
                           std::auto_ptr< IBooleanExpression > rightHandSide);

        /**
          * Constructor
          *
          * Constructs a EqualityComparison expression. This object takes ownership
          * of the leftHandSide and rightHandSide pointers. Client of this class
          * must *not* be delete them.
          *
          */
        EqualityComparison(std::auto_ptr< IStringExpression > leftHandSide,
                           std::auto_ptr< IStringExpression > rightHandSide);

        /**
          * Destructor
          */
        ~EqualityComparison();

        /**
          * evaluate
          *
          * Evaluates the boolean expression and returns the result
          *
          */
        virtual bool evaluate() const;

        private:

        // Disable copy constructor and assignment operator.
        EqualityComparison( const EqualityComparison& theEqualityComparison);
        EqualityComparison& operator=(const EqualityComparison&);

        std::auto_ptr< IRealNumberExpression > m_realNumberLefthandSide;
        std::auto_ptr< IRealNumberExpression > m_realNumberRighthandSide;
        std::auto_ptr< IBooleanExpression > m_booleanLefthandSide;
        std::auto_ptr< IBooleanExpression > m_booleanRighthandSide;
        std::auto_ptr< IStringExpression > m_stringLefthandSide;
        std::auto_ptr< IStringExpression > m_stringRighthandSide;
        };
}
#endif // M_Eval_EqualityComparison_H
