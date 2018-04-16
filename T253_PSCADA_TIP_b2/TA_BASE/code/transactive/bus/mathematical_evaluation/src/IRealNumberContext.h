#ifndef M_Eval_IRealNumberContext_H
#define M_Eval_IRealNumberContext_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/mathematical_evaluation/src/IRealNumberContext.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
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
        virtual ~IRealNumberContext()
		{
		}

        virtual double getValue(const std::string& variableName) const = 0;
    };
}
#endif // M_Eval_IRealNumberContext_H
