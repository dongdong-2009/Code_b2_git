/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveInclusiveGateway.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2013/03/20 14:16:56 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveInclusiveGateway.h"
#include "app/response_plans/plan_agent/src/RealNumberContextImpl.h"
#include "app/response_plans/plan_agent/src/StringContextImpl.h"
#include "app/response_plans/plan_agent/src/BoolContextImpl.h"

namespace TA_Base_App
{

    ActiveInclusiveGateway::ActiveInclusiveGateway(const BasicParameter& detail, IActivePlan& plan) : ActiveStep( m_params, plan ),
    m_bSuccess( false ),
    m_bNextReturned( false ),
    m_outgoingStepIds(),
    m_strFailureReason( "" ),
    m_szConvergingSize( 0u ),
    m_params(detail)
    {
    }

    void ActiveInclusiveGateway::initImpl()
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

    bool ActiveInclusiveGateway::executeImpl()
    {
        if ( !m_bSuccess )
        {
            remark( m_strFailureReason );
        }

        return m_bSuccess;
    }

    int ActiveInclusiveGateway::waitImpl()
    {
        // Enter only if m_refParameter.basic.timeout > 0u
        if ( m_bCompleted )
        {
            return eWaitSucceed;
        }

        size_t szCompletedCount = 0u;

        for (std::list<ActiveStep*>::iterator itLoop = m_lstIncomingStep.begin(); m_lstIncomingStep.end() != itLoop; ++itLoop)
        {
            if ( m_iPlan.stepCompleted( (*itLoop)->id() ))
            {
                ++szCompletedCount;
            }
        }

        int eWaitState = eAsyncWaitContinue;

        if ( szCompletedCount == m_szConvergingSize )
        {
            eWaitState = eWaitSucceed;
        }
        else if ( m_iPlan.allStepPaused( true ))
        {
            remark( RemarkSyncFailedByPausedSteps );
            eWaitState = eWaitFailed;
        }

        return eWaitState;
    }

    std::list<ActiveStep*> ActiveInclusiveGateway::getNextStep()
    {
        if ( m_bNextReturned )
        {
            static std::list<ActiveStep*> EmptyList;
            return EmptyList;
        }

        m_bNextReturned = true;

        std::list<ActiveStep*> outgoingStepList;
        
        for ( std::list<ActiveStep*>::iterator itActiveStep = m_lstOutgoingStep.begin(); itActiveStep != m_lstOutgoingStep.end(); ++itActiveStep )
        {
            if ( m_outgoingStepIds.end() != m_outgoingStepIds.find( (*itActiveStep)->id() ))
            {
                outgoingStepList.push_back(*itActiveStep);
            }
        }
        
        return outgoingStepList;
    }

    void ActiveInclusiveGateway::initAsDiverging()
    {
        m_bSuccess = false;

        try
        {
            for (std::vector<ExpressionPair>::iterator itLoop = m_params.stepExpressionPairs.begin();
                m_params.stepExpressionPairs.end() != itLoop; ++itLoop)
            {
                std::auto_ptr<TA_Base_Bus::ExpressionEvaluator> expEval(new TA_Base_Bus::ExpressionEvaluator);
                std::auto_ptr<RealNumberContextImpl> numberContext(new RealNumberContextImpl(m_iPlan.planSharedData()));
                std::auto_ptr<StringContextImpl> stringContext(new StringContextImpl(m_iPlan.planSharedData()));
                std::auto_ptr<BoolContextImpl> boolContext(new BoolContextImpl(m_iPlan.planSharedData()));

                expEval->setNewExpression(itLoop->expression);
                expEval->addRealNumberContext(numberContext.get());
                expEval->addStringContext(stringContext.get());
                expEval->addBooleanContext(boolContext.get());

                if (expEval->evaluateBooleanExpression())
                {
                    m_outgoingStepIds.insert( itLoop->nextStep );
                }
            }
        }
        catch (std::exception& expWhat)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Failed to get expressions");
        }

        m_bSuccess = !m_outgoingStepIds.empty();

        if ( !m_bSuccess )
        {
            m_strFailureReason = "No expression satisfied";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get next step");
        }
        else
        {
            m_iPlan.planSharedData( outputName( id() ), outputValue( m_outgoingStepIds.size() ));
        }

        m_bParallel = false;
    }

    void ActiveInclusiveGateway::initAsConverging()
    {
        m_bSuccess = true;

        m_refParameter.basic.timeout = std::numeric_limits<time_t>::max();

        m_szConvergingSize = divergingSize( m_params.pairStep );

        for ( std::list<ActiveStep*>::iterator itLoop = m_lstOutgoingStep.begin(); m_lstOutgoingStep.end() != itLoop; ++itLoop )
        {
            m_outgoingStepIds.insert( (*itLoop)->id() );
        }

        m_bParallel = true;
        m_lWaitingInterval = 500000;
    }

    std::string ActiveInclusiveGateway::outputName( const unsigned int unId )
    {
        std::string strName = "InclusiveGateway:";
        strName += TA_Base_Core::DataConversion::toString( static_cast<unsigned long>(unId) );

        return strName;
    }

    std::string ActiveInclusiveGateway::outputValue( const size_t szOutgoingNum )
    {
        return TA_Base_Core::DataConversion::toString( static_cast<unsigned long>(szOutgoingNum) );
    }

    size_t ActiveInclusiveGateway::divergingSize( const unsigned int unId )
    {
        std::string strSize = m_iPlan.planSharedData( outputName( unId ));

        size_t szSize = 0u;
        if ( !strSize.empty() )
        {
            szSize = TA_Base_Core::DataConversion::toUnsignedInteger( strSize );
        }

        return szSize;
    }

} // TA_Base_App
