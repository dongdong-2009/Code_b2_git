/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/CalculatedExpression.cpp $
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
#include "core/utilities/src/TA_MinMax.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "bus/scada/datapoint_library/src/IExpressionNodeNotification.h"
#include "bus/scada/datapoint_library/src/RealNumberContext.h"
#include "bus/scada/datapoint_library/src/BooleanContext.h"
#include "bus/scada/datapoint_library/src/CalculatedExpression.h"


namespace TA_Base_Bus
{

	CalculatedExpression::CalculatedExpression ( IExpressionNodeNotification * user,
												const std::string & calculatedExpression )
	:
	m_calculatedExpression ( calculatedExpression ),	
	m_BusExpressionEvaluator ( 0 ),
	m_booleanContext ( 0 ),
	m_realNumberContext ( 0 )
	{
		// if the user is valid
		if ( 0 != user )
		{
			// create an instance of the expression evaluator (TA_Base_Bus library)
			m_BusExpressionEvaluator = new TA_Base_Bus::ExpressionEvaluator ( m_calculatedExpression );
			m_booleanContext = new BooleanContext ( user );
			m_realNumberContext = new RealNumberContext ( user );

			// add the context to the ExpressionEvaluator
			m_BusExpressionEvaluator->addBooleanContext ( m_booleanContext );
			m_BusExpressionEvaluator->addRealNumberContext ( m_realNumberContext );
		}
	}


	CalculatedExpression::~CalculatedExpression()
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


	void CalculatedExpression::updateCalculatedExpression ( const std::string & newExpression )
	{
		// only proceed if there is a difference
		if ( 0 != newExpression.compare ( m_calculatedExpression ) )
		{
			m_calculatedExpression = newExpression;

			// if the Evaluator is available
			if ( 0 != m_BusExpressionEvaluator )
			{
				m_BusExpressionEvaluator->setNewExpression ( m_calculatedExpression );
			}
		}
	}


    void CalculatedExpression::evaluateBooleanExpression ( bool & returnedValue, TA_Base_Bus::EQualityStatus & returnedStatus )
	{
		if ( 0 != m_BusExpressionEvaluator )
		{
			returnedValue = m_BusExpressionEvaluator->evaluateBooleanExpression();

            // TD18587
            // The ExpressionEvaluator may use either boolean or realnumber context to valuate the expression,
            // how can we know which context is used, or both of them are used, for the expression?
            // So can not just call boolean context to get combined status, should use the worst status of both of them if not forced state
			TA_Base_Bus::EQualityStatus boolReturnedStatus = m_booleanContext->getCombinedStatus();
            TA_Base_Bus::EQualityStatus realNumberReturnedStatus = m_realNumberContext->getCombinedStatus();
            
            if (boolReturnedStatus >= QUALITY_GOOD_NO_SPECIFIC_REASON && realNumberReturnedStatus >= QUALITY_GOOD_NO_SPECIFIC_REASON)
            {
                // both are good, set the better one to the result
                returnedStatus = TA_MAX(boolReturnedStatus, realNumberReturnedStatus);
            }
            else
            {
                // at least one of them is bad, set the worse one to the result
                returnedStatus = TA_MIN(boolReturnedStatus, realNumberReturnedStatus);
            }
        }
		else
		{
			TA_THROW ( TA_Base_Core::MathematicalEvaluationException ( "Handle to TA_Base_Bus::ExpressionEvaluator not being created" ) );
		}
	}


	void CalculatedExpression::evaluateRealNumberExpression ( double & returnedValue, TA_Base_Bus::EQualityStatus & returnedStatus )
	{
		if ( 0 != m_BusExpressionEvaluator )
		{
			returnedValue = m_BusExpressionEvaluator->evaluateRealNumberExpression();
            
            // TD18587
            // The ExpressionEvaluator may use either boolean or realnumber context to valuate the expression,
            // how can we know which context or both of them are used for the expression?
            // So can not just call boolean context to get combined status, should use the worst status of both of them if not forced state
            TA_Base_Bus::EQualityStatus boolReturnedStatus = m_booleanContext->getCombinedStatus();
            TA_Base_Bus::EQualityStatus realNumberReturnedStatus = m_realNumberContext->getCombinedStatus();
            
            if (boolReturnedStatus >= QUALITY_GOOD_NO_SPECIFIC_REASON && realNumberReturnedStatus >= QUALITY_GOOD_NO_SPECIFIC_REASON)
            {
                // both are good, set the better one to the result
                returnedStatus = TA_MAX(boolReturnedStatus, realNumberReturnedStatus);
            }
            else
            {
                // at least one of them is bad, set the worse one to the result
                returnedStatus = TA_MIN(boolReturnedStatus, realNumberReturnedStatus);
            }
        }
		else
		{
			TA_THROW ( TA_Base_Core::MathematicalEvaluationException ( "Handle to TA_Base_Bus::ExpressionEvaluator not being created" ) );
		}
	}


	void CalculatedExpression::enableCallback ( bool enabled )
	{
		if ( 0 != m_booleanContext )
		{
			m_booleanContext->enableCallback ( enabled );
		}
		
		if ( 0 != m_realNumberContext )
		{
			m_realNumberContext->enableCallback ( enabled );
		}
	}
}
