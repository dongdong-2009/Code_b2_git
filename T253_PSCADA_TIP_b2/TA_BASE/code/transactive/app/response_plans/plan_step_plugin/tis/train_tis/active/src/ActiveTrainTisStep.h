#ifndef ACTIVE_TRAIN_TIS_STEP_INCLUDED_H
#define ACTIVE_TRAIN_TIS_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/tis/train_tis/common/TrainTisStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveTrainTisStep: public ActiveStep
    {
    public:
        ActiveTrainTisStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveTrainTisStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveTrainTisStep(const ActiveTrainTisStep&);
        ActiveTrainTisStep& operator=(const ActiveTrainTisStep&);
    private:
        TA_Base_App::TrainTisStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_TRAIN_TIS_STEP_INCLUDED_H
