/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/SampleVariableDll/src/BooleanContext.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * DatapointContext is a class that provides resolution of datapoint variables.
  */

#include "stdafx.h"
#include "BooleanContext.h"

namespace TA_Base_Bus
{
   /**
	* Constructor
	* 
	*/
    BooleanContext::BooleanContext()
    {
    }

   /**
	* Destructor
	* 
	*/
    BooleanContext::~BooleanContext()
    {
    }

    /**
     * getValue
     *
     * Gets the value of a variable. 
     *
     * @exception MathematicalEvaluationException - if variable cannot be found
     */
    bool BooleanContext::getValue(const std::string& variableName) const
    {
        // TODO add the required code to correctly evaluate this variable
        if ( variableName.length() > 4)
        {
            return true;
        }
        return false;
    }

}
