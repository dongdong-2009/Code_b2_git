/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/XorExpression.cpp $
  * @author:  Daniel Hoey
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include "bus/mathematical_evaluation/src/XorExpression.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
    XorExpression::XorExpression(std::auto_ptr< IBooleanExpression > leftHandSide,
                                 std::auto_ptr< IBooleanExpression > rightHandSide)
    :m_rightHandSide(rightHandSide), m_leftHandSide(leftHandSide)
    {
        TA_ASSERT(m_leftHandSide.get()  != NULL, "leftHandSide is null!");
        TA_ASSERT(m_rightHandSide.get() != NULL, "rightHandSide is null!");
    }


    XorExpression::~XorExpression()
    {
        m_leftHandSide  = std::auto_ptr< IBooleanExpression >(static_cast< IBooleanExpression* >(NULL));
        m_rightHandSide = std::auto_ptr< IBooleanExpression >(static_cast< IBooleanExpression* >(NULL));
    }


    bool XorExpression::evaluate() const
    {
        return m_leftHandSide->evaluate() && !m_rightHandSide->evaluate() ||
               m_rightHandSide->evaluate() && !m_leftHandSide->evaluate();
    }
}

