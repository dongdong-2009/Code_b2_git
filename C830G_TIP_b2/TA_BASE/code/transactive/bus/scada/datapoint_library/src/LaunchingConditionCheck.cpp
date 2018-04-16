/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/LaunchingConditionCheck.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "bus/scada/datapoint_library/src/RealNumberContext.h"
#include "bus/scada/datapoint_library/src/BooleanContext.h"
#include "bus/scada/datapoint_library/src/LaunchingConditionCheck.h"


namespace TA_Base_Bus
{

	LaunchingConditionCheck::LaunchingConditionCheck ( const std::string & launchingExpression )
	:
	m_launchingExpression ( launchingExpression ),
	m_BusExpressionEvaluator ( 0 ),
	m_booleanContext ( 0 ),
	m_realNumberContext ( 0 )
	{
		// create an instance of the expression evaluator (TA_Base_Bus library)
		m_BusExpressionEvaluator = new TA_Base_Bus::ExpressionEvaluator ( m_launchingExpression );
		m_booleanContext = new BooleanContext();
		m_realNumberContext = new RealNumberContext();

		// add the context to the ExpressionEvaluator
		m_BusExpressionEvaluator->addBooleanContext ( m_booleanContext );
		m_BusExpressionEvaluator->addRealNumberContext ( m_realNumberContext );
	}


	LaunchingConditionCheck::~LaunchingConditionCheck()
	{
		// delete expression evaluator stuffs
		if ( 0 != m_BusExpressionEvaluator )
		{
			delete m_BusExpressionEvaluator;
			m_BusExpressionEvaluator = 0;
		}

		if ( 0 != m_booleanContext )
		{
			delete m_booleanContext;
			m_booleanContext = 0;
		}

		if ( 0 != m_realNumberContext )
		{
			delete m_realNumberContext;
			m_realNumberContext = 0;
		}
	}


	void LaunchingConditionCheck::updateLaunchingExpression ( const std::string & newExpression )
	{
		// only proceed if there is a difference
		if ( 0 != newExpression.compare ( m_launchingExpression ) )
		{
			m_launchingExpression = newExpression;

			if ( 0 != m_BusExpressionEvaluator )
			{
				m_BusExpressionEvaluator->setNewExpression ( m_launchingExpression );
			}
		}
	}


	bool LaunchingConditionCheck::evaluateLaunchingExpression()
	{
		bool result = false;
		try
		{
			// get the result from the expression evaluator
			result = m_BusExpressionEvaluator->evaluateBooleanExpression();

			// if the status of the result is bad
			if ( false == m_booleanContext->isGoodCombinedStatus() )
			{
				// force the result to false, no matter what result have
				// come out of the expression evaluator
				result = false;

				std::stringstream ss1;
				ss1 << "Forced the result of the Launching Expression " << m_launchingExpression << " to FALSE because the dependencies have bad status";
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss1.str().c_str() );
			}
			// else do nothing, accept the result from the expression evaluator
		}
		catch ( TA_Base_Core::MathematicalEvaluationException & )
		{
		}
		catch ( ... )
		{
		}

		return result;
	}


	std::string LaunchingConditionCheck::getLaunchingConditionResultString()
	{
		//TD16218 Add AI datapoint reference to the LCC result
		std::string RealNumberContextResult = m_realNumberContext->getExpressionResultString();
		if(!RealNumberContextResult.empty())
		{
			std::ostringstream result;
			result << m_booleanContext->getExpressionResultString() << ", " << RealNumberContextResult;
			return result.str();
		}
		else
		{
			return m_booleanContext->getExpressionResultString();
		}
		//TD16218
    }
}

