#ifndef ACTIVE_VIDEO_WALL_STEP_INCLUDED_H
#define ACTIVE_VIDEO_WALL_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/video_wall/common/VideoWallStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveVideoWallStep: public ActiveStep
    {
    public:
        ActiveVideoWallStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveVideoWallStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveVideoWallStep(const ActiveVideoWallStep&);
        ActiveVideoWallStep& operator=(const ActiveVideoWallStep&);
    private:
        TA_Base_App::VideoWallStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_LOG_STEP_INCLUDED_H
