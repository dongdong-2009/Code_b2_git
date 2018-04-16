/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/MftMessageActiveStep.cpp $
  * @author:  Huang Jian
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  **/

#pragma warning ( disable : 4250 )

#include <afxwin.h>
#include "core/utilities/src/RunParams.h"
#include "bus/mfc_extensions/src/message_dialog_helper/MessageDialogHelper.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "app/response_plans/plan_mft_processor/src/ActivePlanAgency.h"
#include "app/response_plans/plan_mft_processor/src/RightsManager.h"
#include "app/response_plans/plan_mft_processor/src/MftMessageActiveStep.h"

namespace TA_Base_App
{
    MftMessageActiveStep::MftMessageActiveStep( const TA_Base_Core::CurrentPlanStepDetail& stepDetail ) : ActivePlanStep(stepDetail),
    m_pMessageDialog(NULL)
    {
        FUNCTION_ENTRY( "MftMessageActiveStep" );

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

        if ( !RightsManager::getInstance().isProfileMatched( m_stepDetail.data.popupMFTMessage().operatorProfileNumber ))
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not lunch the dialog, request profile = %d", m_stepDetail.data.popupMFTMessage().operatorProfileNumber);
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
            ActivePlanAgency::instance()->getPlanAgent()->processPopupMftMessageStepResponse( m_stepDetail, TA_Base_Core::PSCS_SUCCESS );
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

        static const unsigned int MsgBoxStyle = MB_ICONWARNING | m_stepDetail.data.popupMFTMessage().messageBoxType | MB_SYSTEMMODAL | MB_DEFBUTTON2;
        int nRes = m_pMessageDialog->showMessageBox( NULL, m_stepDetail.data.popupMFTMessage().decisionMessage.in(), "", MsgBoxStyle, 0, m_stepDetail.data.popupMFTMessage().timeSpan );

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

            ActivePlanAgency::instance()->getPlanAgent()->processPopupMftMessageStepResponse( m_stepDetail, completionStatus );
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute MftMessageActiveStep fail");
        }

        FUNCTION_EXIT;
    }

    void MftMessageActiveStep::execute()
    {
        launch();
    }

    void MftMessageActiveStep::stop()
    {
        if (NULL != m_pMessageDialog)
        {
            m_pMessageDialog->closeMessageBox();
        }
    }
}