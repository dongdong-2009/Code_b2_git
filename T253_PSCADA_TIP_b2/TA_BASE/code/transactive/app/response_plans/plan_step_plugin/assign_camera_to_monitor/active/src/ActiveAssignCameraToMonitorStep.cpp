#include <memory>
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/active/src/ActiveAssignCameraToMonitorStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/common/src/StringConstants.h"
//#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
/*
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
*/

namespace TA_Base_App
{
	ActiveAssignCameraToMonitorStep::ActiveAssignCameraToMonitorStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
		m_params(detail)
	{
	}

	ActiveAssignCameraToMonitorStep::~ActiveAssignCameraToMonitorStep()
	{
	}

	bool ActiveAssignCameraToMonitorStep::executeImpl()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", id());

		bool bExecSucceed = false;

		try
		{
			std::string session = m_iPlan.session();
			TA_Base_Bus::VideoSwitchAgentFactory::getInstance().setLocationNameFromSession(session);
			std::auto_ptr<TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject> monitorObj(TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoOutput(m_params.monitor));

			CORBA_CALL((*monitorObj), requestSwitchOfInput, (m_params.camera, session.c_str(), true));
			//On successful completion of the step
			bExecSucceed = true;
		}
		catch (const TA_Base_Core::ObjectResolutionException& e)
		{
			m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what());
		}
		catch (const TA_Base_Bus::VideoSwitchAgentException& e)
		{
			m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_App::VideoSwitchAgentException", e.what.in());
		}
		catch (...)
		{
			m_remark = RemarkCanNotFoundVideoSwitchAgent;
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", dumpContext().c_str());
		}

		return bExecSucceed;
	}
}