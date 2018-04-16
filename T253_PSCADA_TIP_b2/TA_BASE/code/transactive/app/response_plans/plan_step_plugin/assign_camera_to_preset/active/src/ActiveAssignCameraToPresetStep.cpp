#include <memory>
#include "app/response_plans/plan_step_plugin/assign_camera_to_preset/active/src/ActiveAssignCameraToPresetStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"

namespace TA_Base_App
{
    ActiveAssignCameraToPresetStep::ActiveAssignCameraToPresetStep(const BasicParameter& detail, IActivePlan& flowControl):
        ActiveStep(m_params, flowControl),
        m_params(detail)
    {
    }

    ActiveAssignCameraToPresetStep::~ActiveAssignCameraToPresetStep()
    {
    }

    bool ActiveAssignCameraToPresetStep::executeImpl()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", id());

        bool bExecSucceed = false;
        std::string session = m_iPlan.session();
        bool cameraLocked = false;
        std::auto_ptr<TA_Base_Bus::VideoSwitchAgentFactory::VideoInputNamedObject> videoInputObject;
        try
        {
            videoInputObject.reset(TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoInput(m_params.camera));
            CORBA_CALL((*videoInputObject), lock, (session.c_str()));
            cameraLocked = true;
        }
        catch(const TA_Base_Core::ObjectResolutionException& e)
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what());
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what.in());
        }
        catch(...)
        {
            m_remark = RemarkCanNotLockCamera;
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", dumpContext().c_str());
        }

        if(!cameraLocked)
        {
            return false;
        }

        cameraLocked = false;
        try
        {
            std::auto_ptr<TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject> cameraObj(TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCamera(m_params.camera));
			session = m_iPlan.session();
            TA_Base_Bus::MovementControlCorbaDef_var movementCtrl;
            CORBA_CALL_RETURN(movementCtrl, (*cameraObj), getMovementControl, ());
            movementCtrl->activatePreset(m_params.preset, session.c_str());
            cameraLocked = true;
        }
        catch(const TA_Base_Core::ObjectResolutionException& e)
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what());
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what.in());
        }
        catch(...)
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", dumpContext().c_str());
        }

        if(!cameraLocked)
        {
            return false;
        }

        try
        {
            if(videoInputObject.get() != NULL)
            {
                (*videoInputObject)->unlock(session.c_str());
                cameraLocked = false;
                bExecSucceed = true;
            }
        }
        catch(const TA_Base_Core::ObjectResolutionException& e)
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException, could not unlock camera", e.what());
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            m_remark = RemarkCanNotFoundVideoSwitchAgent;
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what.in());
        }
        catch(...)
        {
            m_remark = RemarkCanNotLockCamera;
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", dumpContext().c_str());
        }
        if(cameraLocked)
        {
            return false;
        }
        return bExecSucceed;
    }
}