#ifndef ACTIVE_TRAIN_PA_STEP_INCLUDED_H
#define ACTIVE_TRAIN_PA_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/pa/train_pa/common/TrainPaStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
    class ActiveTrainPaStep: public ActiveStep
    {
    public:
        ActiveTrainPaStep(const BasicParameter& detail, IActivePlan& iPlan);
        virtual ~ActiveTrainPaStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveTrainPaStep(const ActiveTrainPaStep&);
        ActiveTrainPaStep& operator=(const ActiveTrainPaStep&);
    private:
        TA_Base_App::TrainPaStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_TRAIN_PA_STEP_INCLUDED_H
