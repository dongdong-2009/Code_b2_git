#ifndef StringContext_H
#define StringContext_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/SampleVariableDll/src/StringContext.h $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include "bus\mathematical_evaluation\src\IStringContext.h"

namespace TA_Base_Bus
{
	class StringContext : public IStringContext
	{
	public:
	   /**
	    * Constructor
	    * 
	    */
	    StringContext();

	   /**
	    * Destructor
	    * 
	    */
	    virtual ~StringContext();

        /**
         * getValue
         *
         * Gets the value of a variable. The variable will be resolved if it has been
         * already.
         *
         * @exception MathematicalEvaluationException - if variable cannot be found
         */
        virtual std::string getValue(const std::string& variableName) const;


	private:
	};
}
#endif // DatapointContext_H
