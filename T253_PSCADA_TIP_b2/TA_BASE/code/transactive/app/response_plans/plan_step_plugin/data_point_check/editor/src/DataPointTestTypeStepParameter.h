/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointTestTypeStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_DATAPOINTTESTTYPESTEPPARAMETER_H__EF0E70D4_C0CB_4C2B_B3FC_0C8A82C4D51E__INCLUDED_)
#define AFX_DATAPOINTTESTTYPESTEPPARAMETER_H__EF0E70D4_C0CB_4C2B_B3FC_0C8A82C4D51E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"
//#include "app/response_plans/plan_step_plugin/data_point_check/editor/src/DataPointCheckStepParameter.h"
//#include "DataPointCheckStepParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointTestTypeStepParameter

    class DataPointTestTypeStepParameter : public PlanStepParameter<int>
    {
    // Operations
    public:
		DataPointTestTypeStepParameter(PlanStep& parent, const std::string& name, int testType);
        virtual ~DataPointTestTypeStepParameter();

		virtual std::vector<std::string> getStepParameterValues();
		virtual const std::string displayValue() const;

        static std::string convertToString( const int eType );

    protected:
        virtual IPlanStepParameterForm* getEditDialog(bool canEdit);

    private:
        DataPointTestTypeStepParameter& operator=(const DataPointTestTypeStepParameter& parameter);

    };
}

#endif // !defined(AFX_DATAPOINTTESTTYPESTEPPARAMETER_H__EF0E70D4_C0CB_4C2B_B3FC_0C8A82C4D51E__INCLUDED_)
