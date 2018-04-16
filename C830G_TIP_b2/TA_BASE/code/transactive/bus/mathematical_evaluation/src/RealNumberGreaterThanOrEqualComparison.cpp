/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/RealNumberGreaterThanOrEqualComparison.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "bus/mathematical_evaluation/src/RealNumberGreaterThanOrEqualComparison.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
    RealNumberGreaterThanOrEqualComparison::RealNumberGreaterThanOrEqualComparison(std::auto_ptr< IRealNumberExpression > leftHandSide,
                                                                                   std::auto_ptr< IRealNumberExpression > rightHandSide)
    :m_rightHandSide(rightHandSide), m_leftHandSide(leftHandSide)
    {
        TA_ASSERT(m_leftHandSide.get()  != NULL, "leftHandSide is null!");
        TA_ASSERT(m_rightHandSide.get() != NULL, "rightHandSide is null!");
    }


    RealNumberGreaterThanOrEqualComparison::~RealNumberGreaterThanOrEqualComparison()
    {
        m_leftHandSide  = std::auto_ptr< IRealNumberExpression >(static_cast< IRealNumberExpression* >(NULL));
        m_rightHandSide = std::auto_ptr< IRealNumberExpression >(static_cast< IRealNumberExpression* >(NULL));
    }


    bool RealNumberGreaterThanOrEqualComparison::evaluate() const
    {
        return m_leftHandSide->evaluate() >= m_rightHandSide->evaluate();
    }
}
