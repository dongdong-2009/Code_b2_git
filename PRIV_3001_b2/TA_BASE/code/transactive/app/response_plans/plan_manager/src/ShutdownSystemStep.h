#if !defined(AFX_DECISIONSTEP_H__01DDAB17_3CEB_4c7b_B5C1_B14A7AE8BDC5__INCLUDED_)
#define AFX_DECISIONSTEP_H__01DDAB17_3CEB_4c7b_B5C1_B14A7AE8BDC5__INCLUDED_
//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/ShutdownSystemStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_SHUTDOWN_SYSTEM)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class YesNoStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // ShutdownSystemStep

    class ShutdownSystemStep : public PlanStep
    {
        // Operations
    public:
        ShutdownSystemStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        ShutdownSystemStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~ShutdownSystemStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return true; }

        YesNoStepParameter* getNotifyPeerParameter()  { return m_notifyPeerParameter; }

    protected:
        ShutdownSystemStep(const ShutdownSystemStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        ShutdownSystemStep &operator=(const ShutdownSystemStep &other);


        // Attributes
    private:
        YesNoStepParameter* m_notifyPeerParameter;
    };
}

#endif // defined (STEPTYPE_POPUP_MFT_MESSAGE)

#endif // defined (AFX_DECISIONSTEP_H__01DDAB17_3CEB_4c7b_B5C1_B14A7AE8BDC5__INCLUDED_)
