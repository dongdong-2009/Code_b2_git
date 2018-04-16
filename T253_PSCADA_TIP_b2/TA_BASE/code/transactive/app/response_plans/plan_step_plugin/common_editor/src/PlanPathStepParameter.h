/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanPathStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_PLANPATHSTEPPARAMETER_H__AE893342_BF8C_43F6_8BEE_94C05F0A9F48__INCLUDED_)
#define AFX_PLANPATHSTEPPARAMETER_H__AE893342_BF8C_43F6_8BEE_94C05F0A9F48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanPathStepParameter

    class PLAN_COMM_DLL PlanPathStepParameter : public PlanStepParameter<unsigned long>
    {
    // Operations
    public:
        PlanPathStepParameter(PlanStep& parent, const std::string& name, unsigned long& planNodeId);
        virtual ~PlanPathStepParameter();

        virtual std::vector<std::string> getStepParameterValues();

        virtual const std::string displayValue() const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        PlanPathStepParameter& operator=(const PlanPathStepParameter& parameter);
    };
}

#endif // !defined(AFX_PLANPATHSTEPPARAMETER_H__AE893342_BF8C_43F6_8BEE_94C05F0A9F48__INCLUDED_)
