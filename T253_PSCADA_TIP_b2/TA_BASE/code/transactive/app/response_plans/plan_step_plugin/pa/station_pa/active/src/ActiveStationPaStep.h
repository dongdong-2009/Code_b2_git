#ifndef ACTIVE_STATION_PA_STEP_INCLUDED_H
#define ACTIVE_STATION_PA_STEP_INCLUDED_H

#include "app/response_plans/plan_step_plugin/pa/station_pa/common/StationPaStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_App
{
    class ActiveStationPaStep: public ActiveStep
    {
    public:
        ActiveStationPaStep(const BasicParameter& detail, IActivePlan& iPlan);
        virtual ~ActiveStationPaStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveStationPaStep(const ActiveStationPaStep&);
        ActiveStationPaStep& operator=(const ActiveStationPaStep&);
    private:
        TA_Base_App::StationPaStepParameter m_params;
    };
}


#endif //ACTIVE_STATION_PA_STEP_INCLUDED_H
