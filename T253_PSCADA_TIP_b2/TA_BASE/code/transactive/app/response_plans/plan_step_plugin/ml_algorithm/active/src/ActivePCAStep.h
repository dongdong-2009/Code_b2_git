/**
* The source code in this file is the property of
* ST Engineering Electronics (LSG) and is not for redistribution
* in any form.
*
* Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/response_plans/plan_step_plugin/ml_algorithm/active/src/ActivePCAStep.h $
* @author:  Hoa Le
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2018/01/25 13:03:08 $
* Last modified by:  $Author: hoa.le $
*
* Active step class derived from based class MLActiveStep for PCA ML algorithm.
*/


#ifndef ACTIVE_MLALGORITHM_STEP_H
#define ACTIVE_MLALGORITHM_STEP_H

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/active_ml/src/MLStepParameter.h"
#include "app/response_plans/plan_step_plugin/active_ml/src/MLActiveStep.h"
#include "app/response_plans/plan_step_plugin/ml_algorithm/common/PCAStepParameter.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActivePCAStep: public MLActiveStep
    {
    public:
        ActivePCAStep( const BasicParameter& detail, IActivePlan& plan );
        virtual ~ActivePCAStep();

    protected:
		virtual void initImpl();
        virtual bool executeImpl();

    private:
        ActivePCAStep(const ActivePCAStep&);
        ActivePCAStep& operator=(const ActivePCAStep&);

    private:
		PCAStepParameter m_params;
    };
}


#endif //ACTIVE_MLALGORITHM_STEP_H
