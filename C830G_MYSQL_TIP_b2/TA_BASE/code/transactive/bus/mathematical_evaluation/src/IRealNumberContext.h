#ifndef M_Eval_IRealNumberContext_H
#define M_Eval_IRealNumberContext_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/IRealNumberContext.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Interface for resolving the values of RealNumber variables.
  */

#include <string>

namespace TA_Base_Bus
{
    class IRealNumberContext
    {
    public:
      /**
        * Destructor
        * 
        */
        virtual ~IRealNumberContext(){}

        virtual double getValue(const std::string& variableName) const = 0;
    };
}
#endif // M_Eval_IRealNumberContext_H
