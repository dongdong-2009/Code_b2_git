/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: //depot/CODE_TA_BASE_DDB/transactive/bus/mathematical_evaluation/src/Attic/RealNumberSquareRootExpression.cpp,v $
  * @author:  Robert Stagg
  * @version: $Revision:  $
  *
  * Last modification: $DateTime: 2005/03/01 05:55:22 $
  * Last modified by:  $Author: roberts $
  * 
  */

#include <math.h>

#include "bus/mathematical_evaluation/src/RealNumberAbsoluteExpression.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    RealNumberAbsoluteExpression::RealNumberAbsoluteExpression(std::auto_ptr< IRealNumberExpression > rightHandSide)
    :m_rightHandSide(rightHandSide)
    {
        TA_ASSERT(m_rightHandSide.get() != NULL, "rightHandSide is null!");
    }


    RealNumberAbsoluteExpression::~RealNumberAbsoluteExpression()
    {
        m_rightHandSide = std::auto_ptr< IRealNumberExpression >(static_cast< IRealNumberExpression* >(NULL));
    }


    double RealNumberAbsoluteExpression::evaluate() const
    {
		double value = fabs(m_rightHandSide->evaluate());

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
				"Power Factor Value=%f", value);

        return fabs(m_rightHandSide->evaluate());
    }
}
