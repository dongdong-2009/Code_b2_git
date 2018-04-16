#ifndef ACTIVE_FAILURE_SUMMARY_STEP_INCLUDED_H
#define ACTIVE_FAILURE_SUMMARY_STEP_INCLUDED_H

#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_Core
{
    struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveFailureSummaryStep: public ActiveStep
    {
    public:
        ActiveFailureSummaryStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveFailureSummaryStep();

    protected:
		virtual void initImpl();
        virtual bool executeImpl();
        virtual int waitImpl();

    private:
        ActiveFailureSummaryStep(const ActiveFailureSummaryStep&);
        ActiveFailureSummaryStep& operator=(const ActiveFailureSummaryStep&);
    private:
        TA_Base_App::FailureSummaryStepParameter m_params;
    };

}


#endif //ACTIVE_FAILURE_SUMMARY_STEP_INCLUDED_H
