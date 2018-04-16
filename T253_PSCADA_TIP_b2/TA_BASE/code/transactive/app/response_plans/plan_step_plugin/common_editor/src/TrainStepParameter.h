/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the Train parameter of a Station TIS/PA step.
  *
  **/

#if !defined(AFX_TRAINSTEPPARAMETER_H__0E045215_D0F1_4801_A0DA_1904F7457551__INCLUDED_)
#define AFX_TRAINSTEPPARAMETER_H__0E045215_D0F1_4801_A0DA_1904F7457551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // TrainStepParameter

    enum ETrainGroup
    {
        TG_INBOUND_TRAINS, TG_OUTBOUND_TRAINS, TG_ALL_TRAINS, TG_UNDEFINED
    };

    class PLAN_COMM_DLL TrainStepParameter : public PlanStepParameter<unsigned short>
    {
    // Operations
    public:
        TrainStepParameter(PlanStep& parent, const std::string& name, unsigned short& trainId, const ETrainGroup trainGroup);
        virtual ~TrainStepParameter();

		virtual std::vector<std::string> getStepParameterValues();
		virtual const std::string displayValue() const;

		ETrainGroup getTrainGroup() const;
        void setTrainGroup(ETrainGroup trainGroup);

	private:
		const std::string convertToString(const ETrainGroup trainGroup) const;

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        TrainStepParameter& operator=(const TrainStepParameter& parameter);

    // Attributes
    private:
        unsigned short m_trainId;
        ETrainGroup m_trainGroup;
    };
}

#endif // !defined(AFX_TRAINSTEPPARAMETER_H__0E045215_D0F1_4801_A0DA_1904F7457551__INCLUDED_)
