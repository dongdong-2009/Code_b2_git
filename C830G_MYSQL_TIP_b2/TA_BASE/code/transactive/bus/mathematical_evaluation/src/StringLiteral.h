#ifndef M_Eval_StringLiteral_H
#define M_Eval_StringLiteral_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/StringLiteral.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include <string>

#include "bus/mathematical_evaluation/src/IStringExpression.h"

namespace TA_Base_Bus
{
        class StringLiteral : public IStringExpression
        {
        public:
      /**
        * Constructor
        * 
        */
        StringLiteral(const std::string& value);
     
      /**
        * Destructor
        * 
        */
        virtual ~StringLiteral();

      /**
        * evaluate
        *
        * Evaluates the expression and returns the result
        *
        */
        virtual std::string evaluate() const;
        
        // Allow StringLiteral to be copyable.
        StringLiteral( const StringLiteral& theStringLiteral);

        private:

        // Disable assignment operator.
        StringLiteral& operator=(const StringLiteral&);

        std::string m_value;
        };
}
#endif // M_Eval_StringLiteral_H
