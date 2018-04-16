#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "ActivePopupMftMessageStep.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/common/PopupMftResponseParameter.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/threads/src/Thread.h"
/*
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PlanAgentComms_MessageTypes.h"
*/

namespace TA_Base_App
{
	static const unsigned short SEND_POPUP_MESSAGE_TRY_MAX_TIMES = 3;

    ActivePopupMftMessageStep::ActivePopupMftMessageStep(const BasicParameter& detail, IActivePlan& flowControl)
		: ActiveStep(m_params, flowControl),
		  m_params(detail),
		  m_nextStepId(0)
    {
    }

    ActivePopupMftMessageStep::~ActivePopupMftMessageStep()
    {
    }

    bool ActivePopupMftMessageStep::executeImpl()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Popup MFT Message step %d", m_params.basic.id);

        // Reset 
        m_sendPopupMessageTryTimes = 0;

        // clear response
		m_iPlan.stepResponsed(id(), true);

        sendActiveStepUpdateMessage();

        return true;
    }
    
    int ActivePopupMftMessageStep::waitImpl()
    {
		// Wait for operator response
		std::vector<std::string> vecResponse = m_iPlan.stepData( id() );

		if ( vecResponse.empty() )
		{
			return eAsyncWaitContinue;
		}

		PopupMftResponseParameter responseParam( vecResponse );

		int nWaitStatus = eWaitFailed;

		switch (responseParam.response)
		{
		case TA_Base_Core::PSCS_SUCCESS:
			m_nextStepId = m_params.pendingStep;
			nWaitStatus = eWaitSucceed;
			break;
		case TA_Base_Core::PSCS_UNDEFINED:
			// send message
			if ( m_sendPopupMessageTryTimes < SEND_POPUP_MESSAGE_TRY_MAX_TIMES )
			{
				++m_sendPopupMessageTryTimes;
				sendActiveStepUpdateMessage();
				m_nextStepId = m_params.pendingStep;
				nWaitStatus = eWaitSucceed;
			}
			else
			{
				m_remark = RemarkNoMftResponse;
				nWaitStatus = eWaitFailed;
				m_sendPopupMessageTryTimes = 0;
			}
			break;
		case TA_Base_Core::PSCS_YES_RESPONSE:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
			m_nextStepId = m_params.yesStep;
			nWaitStatus = eWaitSucceed;
			break;
		case TA_Base_Core::PSCS_NO_RESPONSE:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
            m_nextStepId = m_params.noStep;
			nWaitStatus = eWaitSucceed;
			break;
		case TA_Base_Core::PSCS_ABORT_RESPONSE:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
			m_nextStepId = m_params.abortStep;
			nWaitStatus = eWaitSucceed;
			break;
		case TA_Base_Core::PSCS_IGNORE_RESPONSE:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
			m_nextStepId = m_params.ignoreStep;
			nWaitStatus = eWaitSucceed;
			break;
		case TA_Base_Core::PSCS_RETRY_RESPONSE:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
            m_nextStepId = m_params.retryStep;
			nWaitStatus = eWaitSucceed;
			break;
		case TA_Base_Core::PSCS_OK_RESPONSE:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
			m_nextStepId = m_params.okStep;
			nWaitStatus = eWaitSucceed;
			break;
		case TA_Base_Core::PSCS_CANCEL_RESPONSE:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
			m_nextStepId = m_params.cancelStep;
			nWaitStatus = eWaitSucceed;
			break;
		default:
			m_sendPopupMessageTryTimes = 0;
			//ActivePlanBulletinBoard::instance()->removeStepCompletionStatus( srtStep );
			m_remark = RemarkInvalidStepParameter;
			nWaitStatus = eWaitFailed;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Result can not be recognized.");
		}

		return nWaitStatus;
    }

	std::list<ActiveStep*> ActivePopupMftMessageStep::getNextStep()
	{
		std::list<ActiveStep*> outgoingStepList;
		std::list<ActiveStep*>::iterator itActiveStep = m_lstOutgoingStep.begin();
		for(; itActiveStep != m_lstOutgoingStep.end(); ++itActiveStep)
		{
			if(m_nextStepId == (*itActiveStep)->id())
			{
				outgoingStepList.push_back(*itActiveStep);
				break;
			}
		}
		return outgoingStepList;
	}

    void ActivePopupMftMessageStep::sendActiveStepUpdateMessage()
    {
        std::vector<std::string> vecParameters = m_params.serialize();

        TA_Base_Bus::RpcAgency::getInstance().syncSendReq( m_iPlan.userLocation(), m_refParameter.basic.type,
            m_iPlan.id().location, m_iPlan.id().plan, m_iPlan.id().instance, id(), m_iPlan.userWorkstation(), vecParameters );
		/*
        static const unsigned int IdleTime = 500;
        TA_Base_Core::Thread::sleep(IdleTime);
		*/
    }

}