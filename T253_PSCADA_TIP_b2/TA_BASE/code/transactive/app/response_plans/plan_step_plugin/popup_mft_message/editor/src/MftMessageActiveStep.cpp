/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/MftMessageActiveStep.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/10/30 19:47:26 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  **/

#pragma warning ( disable : 4250 )

#include <afxwin.h>
#include "core/utilities/src/RunParams.h"
#include "bus/mfc_extensions/src/message_dialog_helper/MessageDialogHelper.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/common/PopupMftResponseParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/RightsManager.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"
#include "MftMessageActiveStep.h"

namespace TA_Base_App
{
	MftMessageActiveStep::MftMessageActiveStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters ) :
	InteractiveStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters),
    m_pMessageDialog(NULL),
	m_eType(eType),
	m_ulLocation(ulLocation),
	m_ulPlanId(ulPlanId),
	m_ulInstanceNum(ulInstanceNum),
	m_ulStepNum(ulStepNum),
	m_parameters(vecParameters)
    {
        FUNCTION_ENTRY( "MftMessageActiveStep" );
		m_params = new PopupMftMessageStepParameter(ulStepNum, eType);
		m_params->deserialize(vecParameters);
        FUNCTION_EXIT;
    }

    MftMessageActiveStep::~MftMessageActiveStep()
    {
        FUNCTION_ENTRY( "~MftMessageActiveStep" );

        if (NULL != m_pMessageDialog)
        {
            delete m_pMessageDialog;
            m_pMessageDialog = NULL;
        }

		if (NULL != m_params)
		{
			delete m_params;
			m_params = NULL;
		}
        FUNCTION_EXIT;
    }

    void MftMessageActiveStep::launch()
    {
        FUNCTION_ENTRY( "launch" );
        
        if ( !isRunning() )
        {
            FUNCTION_EXIT;
            return;
        }

        if ( !RightsManager::getInstance().isProfileMatched( m_params->operatorProfileNumber ))
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not lunch the dialog, request profile = %d", m_params->operatorProfileNumber);
            FUNCTION_EXIT;
            return;
        }

        if ( !isRunning() )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //ActivePlanAgency::instance()->acknowledgeRequest( m_unId );
			TA_Base_Bus::RpcAgency::getInstance().asyncSendAck( RightsManager::getInstance().locationId(), m_unId );
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute MftMessageActiveStep fail");
        }

        if ( !isRunning() )
        {
            FUNCTION_EXIT;
            return;
        }

        if ( NULL != m_pMessageDialog )
        {
            m_pMessageDialog->closeMessageBox();
        }
        else
        {
            m_pMessageDialog = new TA_Base_Bus::MessageDialogHelper();
        }

        if ( !isRunning() )
        {
            FUNCTION_EXIT;
            return;
        }

        static const unsigned int MsgBoxStyle = MB_ICONWARNING | m_params->messageBoxType | MB_SYSTEMMODAL | MB_DEFBUTTON2;
        int nRes = m_pMessageDialog->showMessageBox( NULL, m_params->decisionMessage.c_str(), "", MsgBoxStyle, 0, m_params->timeSpan );

        TA_Base_Core::EPlanStepCompletionStatus completionStatus = TA_Base_Core::PSCS_FAILURE;

        if ( IDYES == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_YES_RESPONSE;
        }
        else if ( IDNO == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_NO_RESPONSE;
        }
        else if ( IDCLOSE == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_UNDEFINED;
        }
        else if ( IDABORT == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_ABORT_RESPONSE;
        }
        else if ( IDIGNORE == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_IGNORE_RESPONSE;
        }
        else if ( IDRETRY == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_IGNORE_RESPONSE;
        }
        else if ( IDOK == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_OK_RESPONSE;
        }
        else if ( IDCANCEL == nRes )
        {
            completionStatus = TA_Base_Core::PSCS_CANCEL_RESPONSE;
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Result %d can not be recognized.", nRes );
            completionStatus = TA_Base_Core::PSCS_FAILURE;
        }
        
        if ( !isRunning() )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[MftMsg] Send response %d after user response", completionStatus );

            reportCompletionStatus(completionStatus);
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute MftMessageActiveStep fail");
        }

        FUNCTION_EXIT;
    }

    void MftMessageActiveStep::close()
    {
        if (NULL != m_pMessageDialog)
        {
            m_pMessageDialog->closeMessageBox();
        }
    }

	void MftMessageActiveStep::reportCompletionStatus(const int completionStatus)
	{
		PopupMftResponseParameter responseParam(completionStatus);
		std::vector<std::string> stringParams = responseParam.serialize();

		TA_Base_Core::InteractiveStepData stepData;
		stepData.id = m_ulStepNum;
		stepData.type = m_eType;

		TA_Base_Core::StepParameters parameters;
		parameters.length(stringParams.size());
		unsigned long index = 0;
		for(std::vector<std::string>::const_iterator itParameter = stringParams.begin(); itParameter != stringParams.end(); ++itParameter)
		{
			parameters[index] = (*itParameter).c_str();
			index++;
		}

		stepData.parameters = parameters;

		TA_Base_Core::ActivePlanId activePlanId;
		activePlanId.instance = m_ulInstanceNum;
		activePlanId.location = m_ulLocation;
		activePlanId.plan = m_ulPlanId;

		MftPlanAgentAccessFactory::instance().processStepResponse(activePlanId, stepData);
	}
}