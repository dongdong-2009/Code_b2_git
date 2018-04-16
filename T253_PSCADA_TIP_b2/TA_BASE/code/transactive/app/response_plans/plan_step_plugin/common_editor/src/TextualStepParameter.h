/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/TextualStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/06/03 09:20:18 $
  * Last modified by:  $Author: huangjian $
  *
  * <description>
  *
  **/

#if !defined(AFX_TEXTUALSTEPPARAMETER_H__45B04442_3C60_40D1_8FE6_CF999AACEAFF__INCLUDED_)
#define AFX_TEXTUALSTEPPARAMETER_H__45B04442_3C60_40D1_8FE6_CF999AACEAFF__INCLUDED_

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TextualStepParameter

    class PLAN_COMM_DLL TextualStepParameter : public PlanStepParameter<std::string>
    {
    // Operations
    public:
        TextualStepParameter(PlanStep& parent, const std::string& name, std::string& text);
        virtual ~TextualStepParameter();

        virtual std::vector<std::string> getStepParameterValues();
        virtual const std::string displayValue() const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        TextualStepParameter& operator=(const TextualStepParameter& parameter);
    };
}

#endif // !defined(AFX_TEXTUALSTEPPARAMETER_H__45B04442_3C60_40D1_8FE6_CF999AACEAFF__INCLUDED_)
