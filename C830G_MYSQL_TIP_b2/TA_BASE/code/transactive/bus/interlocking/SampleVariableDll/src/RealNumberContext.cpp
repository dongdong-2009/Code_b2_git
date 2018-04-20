/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/SampleVariableDll/src/RealNumberContext.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */
#include "stdafx.h"

#include "RealNumberContext.h"

namespace TA_Base_Bus
{
   /**
	* Constructor
	* 
	*/
    RealNumberContext::RealNumberContext()
    {
    }

   /**
	* Destructor
	* 
	*/
    RealNumberContext::~RealNumberContext()
    {
    }

    /**
     * getValue
     *
     * Gets the value of a variable. 
     *
     * @exception MathematicalEvaluationException - if variable cannot be found
     */
    double RealNumberContext::getValue(const std::string& variableName) const
    {
        // TODO add the required code to correctly evaluate this variable
        return( variableName.length() + .5 );
    }

}
