/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveExclusiveGateway.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2013/03/20 14:16:56 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveExclusiveGateway.h"
#include "app/response_plans/plan_agent/src/RealNumberContextImpl.h"
#include "app/response_plans/plan_agent/src/StringContextImpl.h"
#include "app/response_plans/plan_agent/src/BoolContextImpl.h"

namespace TA_Base_App
{

    ActiveExclusiveGateway::ActiveExclusiveGateway(const BasicParameter& detail, IActivePlan& plan) : ActiveStep( m_params, plan ),
    m_bSuccess( false ),
    m_outgoingStepId( 0u ),
    m_strFailureReason( "" ),
    m_params(detail)
    {
    }

    void ActiveExclusiveGateway::initImpl()
    {
        if ( m_params.isConverge() )
        {
            initAsConverging();
        }
        else
        {
            initAsDiverging();
        }
    }

    void ActiveExclusiveGateway::initAsDiverging()
    {
        int nCount = 0;
        unsigned long outgoingStepId = 0u;
        m_bSuccess = false;

        try
        {
            for (std::vector<ExpressionPair>::iterator itLoop = m_params.stepExpressionPairs.begin();
                m_params.stepExpressionPairs.end() != itLoop; ++itLoop )
            {
                std::auto_ptr<TA_Base_Bus::ExpressionEvaluator> expEval(new TA_Base_Bus::ExpressionEvaluator);
                std::auto_ptr<RealNumberContextImpl> numberContext(new RealNumberContextImpl(m_iPlan.planSharedData()));
                std::auto_ptr<StringContextImpl> stringContext(new StringContextImpl(m_iPlan.planSharedData()));
                std::auto_ptr<BoolContextImpl> boolContext(new BoolContextImpl(m_iPlan.planSharedData()));

                expEval->setNewExpression(itLoop->expression);
                expEval->addRealNumberContext(numberContext.get());
                expEval->addStringContext(stringContext.get());
                expEval->addBooleanContext(boolContext.get());

                if(expEval->evaluateBooleanExpression())
                {
                    outgoingStepId = itLoop->nextStep;
                    nCount++;
                }
            }
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Failed to get expressions" );
        }

        if ( 1 == nCount )
        {
            m_outgoingStepId = outgoingStepId;
            m_bSuccess = true;
        }
        else if ( 0 == nCount )
        {
            m_strFailureReason = "No expression satisfied";
        }
        else
        {
            m_strFailureReason = "Multiple expressions satisfied";
        }

        if ( !m_bSuccess )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Don't follow the rules for exclusive gateway step, only need a next step." );
        }
    }

    void ActiveExclusiveGateway::initAsConverging()
    {
        m_bSuccess = true;

        if ( !m_params.stepExpressionPairs.empty() )
        {
            m_outgoingStepId = m_params.stepExpressionPairs.front().nextStep;
        }
    }

    bool ActiveExclusiveGateway::executeImpl()
    {
        if ( !m_bSuccess )
        {
            remark( m_strFailureReason );
        }

        return m_bSuccess;
    }

    std::list<ActiveStep*> ActiveExclusiveGateway::getNextStep()
    {
        std::list<ActiveStep*> outgoingStepList;

        for (std::list<ActiveStep*>::iterator itActiveStep = m_lstOutgoingStep.begin(); itActiveStep != m_lstOutgoingStep.end(); ++itActiveStep)
        {
            if ( (*itActiveStep)->id() == m_outgoingStepId )
            {
                outgoingStepList.push_back(*itActiveStep);
            }
        }

        return outgoingStepList;
    }

} // TA_Base_App
