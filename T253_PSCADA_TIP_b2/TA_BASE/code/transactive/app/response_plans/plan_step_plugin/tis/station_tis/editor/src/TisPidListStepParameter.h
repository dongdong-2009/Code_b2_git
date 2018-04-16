/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TisPidListStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the PID List parameter of a Station Traveller Info System step.
  *
  **/

#if !defined(AFX_TISPIDLISTSTEPPARAMETER_H__7B48AE28_0589_4F81_A604_E5AA24D52683__INCLUDED_)
#define AFX_TISPIDLISTSTEPPARAMETER_H__7B48AE28_0589_4F81_A604_E5AA24D52683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TisPidListStepParameter

    class TisPidListStepParameter : public PlanStepParameter<std::vector<std::string>>
    {
    // Operations
    public:
        TisPidListStepParameter(PlanStep& parent, const std::string& name, std::vector<std::string>& pids);
        virtual ~TisPidListStepParameter();

		virtual std::vector<std::string> getStepParameterValues();
        virtual bool editParameter(const std::string& stationName);
		virtual const std::string displayValue() const;

        std::vector<std::string> getPids() const { return m_refSrcValue; }

        void resetPids();

    protected:
        virtual IPlanStepParameterForm* getEditDialog(bool canEdit);

    private:
        TisPidListStepParameter& operator=(const TisPidListStepParameter& parameter);

    };
}

#endif // !defined(AFX_TISPIDLISTSTEPPARAMETER_H__7B48AE28_0589_4F81_A604_E5AA24D52683__INCLUDED_)
