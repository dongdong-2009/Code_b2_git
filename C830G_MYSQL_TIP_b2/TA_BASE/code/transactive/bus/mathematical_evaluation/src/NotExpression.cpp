/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/NotExpression.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include "bus/mathematical_evaluation/src/NotExpression.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
    NotExpression::NotExpression(std::auto_ptr< IBooleanExpression > rightHandSide)
    :m_rightHandSide(rightHandSide)
    {
        TA_ASSERT(m_rightHandSide.get() != NULL, "rightHandSide is null!");
    }


    NotExpression::~NotExpression()
    {
        m_rightHandSide = std::auto_ptr< IBooleanExpression >(static_cast< IBooleanExpression* >(NULL));
    }


    bool NotExpression::evaluate() const
    {
        return !m_rightHandSide->evaluate();
    }
}

