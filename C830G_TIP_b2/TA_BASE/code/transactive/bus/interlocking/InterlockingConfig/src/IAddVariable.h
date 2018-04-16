/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/IAddVariable.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
///////////////////////////////////////////////////////////
//  IAddVariable.h
//  Implementation of the Interface IAddVariable
//  Created on:      08-Feb-2005 02:19:40 PM
///////////////////////////////////////////////////////////

#if !defined(IAddVariable_CE099298_F2A6_4144_B90B_FA5709D7A92C__INCLUDED_)
#define IAddVariable_CE099298_F2A6_4144_B90B_FA5709D7A92C__INCLUDED_

#include <string>

namespace TA_Base_Bus
{
    /**
     * This Interface is used by the variable selectors to add a variable to a rule.
     */
    class IAddVariable
    {

    public:
	    /**
	     * @param variableName
	     * @param variableAlias
	     * 
	     */
        virtual void addVariable(const std::string& variableName, 
                                 const std::string& variableAlias, 
                                 const std::string& variableType) =0;

        virtual ~IAddVariable() {};
    };

}//namespace
#endif // !defined(IAddVariable_CE099298_F2A6_4144_B90B_FA5709D7A92C__INCLUDED_)