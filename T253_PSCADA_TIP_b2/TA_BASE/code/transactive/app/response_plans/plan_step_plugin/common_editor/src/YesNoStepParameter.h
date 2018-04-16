/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/YesNoStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/06/03 09:20:18 $
  * Last modified by:  $Author: huangjian $
  *
  * <description>
  *
  **/

#if !defined(AFX_YESNOSTEPPARAMETER_H__A2C4AD29_FE3D_4AFC_8763_A0E6F0AA2CD6__INCLUDED_)
#define AFX_YESNOSTEPPARAMETER_H__A2C4AD29_FE3D_4AFC_8763_A0E6F0AA2CD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // YesNoStepParameter

    class PLAN_COMM_DLL YesNoStepParameter : public PlanStepParameter<bool>
    {
    // Operations
    public:
        YesNoStepParameter(PlanStep& parent, const std::string& name, bool& affirmative);
        virtual ~YesNoStepParameter();

        virtual std::vector<std::string> getStepParameterValues();
        virtual const std::string displayValue() const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        YesNoStepParameter& operator=(const YesNoStepParameter& parameter);

        std::string getYesNoLabel();
    };
}

#endif // !defined(AFX_YESNOSTEPPARAMETER_H__A2C4AD29_FE3D_4AFC_8763_A0E6F0AA2CD6__INCLUDED_)
