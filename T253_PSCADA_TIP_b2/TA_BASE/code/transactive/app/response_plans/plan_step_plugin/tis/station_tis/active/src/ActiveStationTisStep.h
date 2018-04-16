#ifndef ACTIVE_STATION_TIS_STEP_INCLUDED_H
#define ACTIVE_STATION_TIS_STEP_INCLUDED_H


#include "app/response_plans/plan_step_plugin/tis/station_tis/common/StationTisStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveStationTisStep: public ActiveStep
    {
    public:
        ActiveStationTisStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveStationTisStep();

    protected:
        virtual bool executeImpl();
        virtual int waitImpl();
    private:
        ActiveStationTisStep(const ActiveStationTisStep&);
        ActiveStationTisStep& operator=(const ActiveStationTisStep&);
    private:
        TA_Base_App::StationTisStepParameter m_params;
    };
}


#endif //ACTIVE_STATION_TIS_STEP_INCLUDED_H
