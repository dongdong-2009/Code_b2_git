/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/GuiAppTypeStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_GUIAPPTYPESTEPPARAMETER_H__6CE0643B_7AC2_414E_8F93_3DF24BEB3FEB__INCLUDED_)
#define AFX_GUIAPPTYPESTEPPARAMETER_H__6CE0643B_7AC2_414E_8F93_3DF24BEB3FEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // GuiAppTypeStepParameter

    class GuiAppTypeStepParameter : public NumericStepParameter
    {
    // Operations
    public:
        GuiAppTypeStepParameter(PlanStep& parent, std::string name, unsigned long appType, bool isHidden = false);
		//GuiAppTypeStepParameter(PlanStep& parent, const GuiAppTypeStepParameter& parameter);
  //      GuiAppTypeStepParameter(const GuiAppTypeStepParameter& parameter);
        virtual ~GuiAppTypeStepParameter();

        virtual std::vector<std::string> getStepParameterValues();
        std::string getApplicationName();

    private:
        GuiAppTypeStepParameter& operator=(const GuiAppTypeStepParameter& parameter);
    };
}

#endif // !defined(AFX_GUIAPPTYPESTEPPARAMETER_H__6CE0643B_7AC2_414E_8F93_3DF24BEB3FEB__INCLUDED_)

