/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/NumericStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/06/03 09:20:18 $
  * Last modified by:  $Author: huangjian $
  *
  * <description>
  *
  **/

#if !defined(AFX_NUMERICSTEPPARAMETER_H__606936CA_94C8_4D55_8939_288854A4F86D__INCLUDED_)
#define AFX_NUMERICSTEPPARAMETER_H__606936CA_94C8_4D55_8939_288854A4F86D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // NumericStepParameter

    class PLAN_COMM_DLL NumericStepParameter : public PlanStepParameter<unsigned long>
    {
    // Operations
    public:
        NumericStepParameter(PlanStep& parent, const std::string& name, unsigned long& value, bool isHidden = false);
        virtual ~NumericStepParameter();

        virtual std::vector<std::string> getStepParameterValues();
        virtual const std::string displayValue() const;

        void hideValue( bool isHidden );
        bool isHiddenValue() const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        NumericStepParameter& operator=(const NumericStepParameter& parameter);

    // Attributes
    private:
        bool m_isHiddenValue;
    };
}

#endif // !defined(AFX_NUMERICSTEPPARAMETER_H__606936CA_94C8_4D55_8939_288854A4F86D__INCLUDED_)
