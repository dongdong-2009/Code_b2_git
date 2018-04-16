/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StepNumberStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_STEPNUMBERSTEPPARAMETER_H__5A56B69A_6A40_48BF_93B7_4FF23CCA70E8__INCLUDED_)
#define AFX_STEPNUMBERSTEPPARAMETER_H__5A56B69A_6A40_48BF_93B7_4FF23CCA70E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // StepNumberStepParameter

    class PLAN_COMM_DLL StepNumberStepParameter : public PlanStepParameter<unsigned int>
    {
    // Operations
    public:
        StepNumberStepParameter(PlanStep& parent, const std::string& name, unsigned int& stepId);
        virtual ~StepNumberStepParameter();

        virtual std::vector<std::string> getStepParameterValues();
        virtual const std::string displayValue() const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        StepNumberStepParameter& operator=(const StepNumberStepParameter& parameter);
    };
}

#endif // !defined(AFX_STEPNUMBERSTEPPARAMETER_H__5A56B69A_6A40_48BF_93B7_4FF23CCA70E8__INCLUDED_)
