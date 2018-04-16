#include "VideoWallStepComponent.h"
#include "ActiveVideoWallStep.h"

namespace TA_Base_App
{

    ActiveStep* VideoWallStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveVideoWallStep(detail, flowControl);
    }

    void VideoWallStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* VideoWallStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    VideoWallStepComponentImpl::VideoWallStepComponentImpl()
    {

    }

    VideoWallStepComponentImpl::~VideoWallStepComponentImpl()
    {

    }

}