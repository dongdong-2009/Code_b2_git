/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PopupMFTMessageStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_POPUP_MFT_MESSAGE)

#include "core/utilities/src/TAAssert.h"
#include "resource.h"
#include "PopupMFTMessageStep.h"
#include "StepNumberStepParameter.h"
#include "NumericStepParameter.h"
#include "TextualStepParameter.h"
#include "PlanStepEditPopupMFTMessagePage.h"
#include "PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/ProfileConfigAccess.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

static const std::string MESSAGESTYLE_DESCRIPTION[MAX_MESSAGEBOX_STYLE] = {
    "OK", //MB_OK 0x00000000L
    "OK and Cancel",//MB_OKCANCEL 0x00000001L
    "Abort, Retry, and Ignore",//MB_ABORTRETRYIGNORE 0x00000002L
    "Yes, No, and Cancel",//MB_YESNOCANCEL 0x00000003L
    "Yes and No",//MB_YESNO 0x00000004L
    "Retry and Cancel"//MB_RETRYCANCEL 0x00000005L
};
/////////////////////////////////////////////////////////////////////////////
// PlanStep

PopupMFTMessageStep::PopupMFTMessageStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_messageParameter (NULL),
    m_abortParameter(NULL),
    m_ignoreParameter(NULL),
    m_retryParameter(NULL),
    m_okParameter(NULL),
    m_cancelParameter(NULL),
    m_yesParameter(NULL),
    m_noParameter(NULL),
    m_pendingParameter(NULL),
    m_messageboxStyle(NULL),
    m_messageboxStyleDescription(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDescription(NULL),
    m_timeSpanParameter(NULL)
{
    FUNCTION_ENTRY( "PopupMFTMessageStep" );
    FUNCTION_EXIT;
}


PopupMFTMessageStep::PopupMFTMessageStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::POPUP_MFT_MESSAGE_STEP),
    m_messageParameter (NULL),
    m_abortParameter(NULL),
    m_ignoreParameter(NULL),
    m_retryParameter(NULL),
    m_okParameter(NULL),
    m_cancelParameter(NULL),
    m_yesParameter(NULL),
    m_noParameter(NULL),
    m_pendingParameter(NULL),
    m_messageboxStyle(NULL),
    m_messageboxStyleDescription(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDescription(NULL),
    m_timeSpanParameter(NULL)
{
    FUNCTION_ENTRY( "PopupMFTMessageStep" );
    FUNCTION_EXIT;
}


PopupMFTMessageStep::PopupMFTMessageStep(const PopupMFTMessageStep& planStep) :
    PlanStep(planStep),
    m_messageParameter (NULL),
    m_abortParameter(NULL),
    m_ignoreParameter(NULL),
    m_retryParameter(NULL),
    m_okParameter(NULL),
    m_cancelParameter(NULL),
    m_yesParameter(NULL),
    m_noParameter(NULL),
    m_pendingParameter(NULL),
    m_messageboxStyle(NULL),
    m_messageboxStyleDescription(NULL),
    m_operatorProfileNumber(NULL),
    m_operatorProfileDescription(NULL),
    m_timeSpanParameter(NULL)
{
    FUNCTION_ENTRY( "PopupMFTMessageStep" );

    m_messageParameter = new TextualStepParameter(*this, *planStep.m_messageParameter);
    m_yesParameter = new StepNumberStepParameter(*this, *planStep.m_yesParameter);
    m_noParameter = new StepNumberStepParameter(*this, *planStep.m_noParameter);
    m_pendingParameter = new StepNumberStepParameter(*this, *planStep.m_pendingParameter);
    m_operatorProfileNumber = new NumericStepParameter(*this, *planStep.m_operatorProfileNumber);

    m_abortParameter = new StepNumberStepParameter(*this, *planStep.m_abortParameter);
    m_ignoreParameter = new StepNumberStepParameter(*this, *planStep.m_ignoreParameter);
    m_retryParameter = new StepNumberStepParameter(*this, *planStep.m_retryParameter);
    m_okParameter = new StepNumberStepParameter(*this, *planStep.m_okParameter);
    m_cancelParameter = new StepNumberStepParameter(*this, *planStep.m_cancelParameter);
    m_messageboxStyle = new NumericStepParameter(*this, *planStep.m_messageboxStyle);
    m_timeSpanParameter = new TimeSpanStepParameter(*this, *planStep.m_timeSpanParameter);


    addPlanStepParameter(*m_messageParameter);
    addPlanStepParameter(*m_yesParameter);
    addPlanStepParameter(*m_noParameter);
    addPlanStepParameter(*m_pendingParameter);
    addPlanStepParameter(*m_operatorProfileNumber);

    addPlanStepParameter(*m_abortParameter);
    addPlanStepParameter(*m_ignoreParameter);
    addPlanStepParameter(*m_retryParameter);
    addPlanStepParameter(*m_okParameter);
    addPlanStepParameter(*m_cancelParameter);
    addPlanStepParameter(*m_messageboxStyle);
    addPlanStepParameter(*m_timeSpanParameter);

    FUNCTION_EXIT;
}


PopupMFTMessageStep::~PopupMFTMessageStep()
{
    FUNCTION_ENTRY( "~PopupMFTMessageStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* PopupMFTMessageStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new PopupMFTMessageStep(*this);
}


void PopupMFTMessageStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_messageParameter;
    delete m_yesParameter;
    delete m_noParameter;
    delete m_pendingParameter;
    delete m_operatorProfileNumber;

    delete m_abortParameter;
    delete m_ignoreParameter;
    delete m_retryParameter;
    delete m_okParameter;
    delete m_cancelParameter;
    delete m_messageboxStyle;
    delete m_messageboxStyleDescription;
    delete m_operatorProfileDescription;
    delete m_timeSpanParameter;

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DECISION_MESSAGE), "");
    m_yesParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_YES_STEP), 0);
    m_noParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_STEP), 0);
    m_pendingParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PENDING_STEP), 0);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), TA_Base_Core::DatabaseKey::getInvalidKey());
    m_operatorProfileDescription = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), "");

    m_abortParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ABORT_STEP), 0);
    m_ignoreParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_IGNORE_STEP), 0);
    m_retryParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RETRY_STEP), 0);
    m_okParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_OK_STEP), 0);
    m_cancelParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_CANCEL_STEP), 0);
    m_messageboxStyle = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MESSAGEBOX_STYLE), MB_OK);
    m_messageboxStyleDescription = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MESSAGEBOX_STYLE), MESSAGESTYLE_DESCRIPTION[MB_OK]); 

    m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), 0, TimeSpanStepParameter::TR_SECONDS);

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_operatorProfileDescription);
    stepParameters.push_back(m_messageboxStyleDescription);
    stepParameters.push_back(m_pendingParameter);
    stepParameters.push_back(m_okParameter);
    stepParameters.push_back(m_timeSpanParameter);

    FUNCTION_EXIT;
}


void PopupMFTMessageStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                                  PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::POPUP_MFT_MESSAGE_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for popup MFT message step.";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::POPUP_MFT_MESSAGE_STEP));
    }
    delete m_messageParameter;
    delete m_yesParameter;
    delete m_noParameter;
    delete m_pendingParameter;
    delete m_operatorProfileNumber;
    delete m_operatorProfileDescription;

    delete m_abortParameter;
    delete m_ignoreParameter;
    delete m_retryParameter;
    delete m_okParameter;
    delete m_cancelParameter;
    delete m_messageboxStyle;
    delete m_messageboxStyleDescription;
    delete m_timeSpanParameter;

    bool bValidProfile = true;
    TA_Base_Core::IProfile *profile = ProfileConfigAccess::instance()->getProfileByKey(parameterSet.popupMFTMessage().operatorProfileNumber);
    if (NULL == profile)
    {
        bValidProfile = false;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Invalid profile key = %lu", parameterSet.popupMFTMessage().operatorProfileNumber);
    }

    m_messageParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_DECISION_MESSAGE), parameterSet.popupMFTMessage().decisionMessage.in());
    m_yesParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_YES_STEP), parameterSet.popupMFTMessage().yesStep);
    m_noParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NO_STEP), parameterSet.popupMFTMessage().noStep);
    m_pendingParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PENDING_STEP), parameterSet.popupMFTMessage().pendingStep);
    m_operatorProfileNumber = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), parameterSet.popupMFTMessage().operatorProfileNumber);
    m_operatorProfileDescription = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_PROFILE_NAME), bValidProfile? profile->getName():"");

    m_abortParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_ABORT_STEP), parameterSet.popupMFTMessage().abortStep);
    m_ignoreParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_IGNORE_STEP), parameterSet.popupMFTMessage().ignoreStep);
    m_retryParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_RETRY_STEP), parameterSet.popupMFTMessage().retryStep);
    m_okParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_OK_STEP), parameterSet.popupMFTMessage().okStep);
    m_cancelParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_CANCEL_STEP), parameterSet.popupMFTMessage().cancelStep);
    m_messageboxStyle = new NumericStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MESSAGEBOX_STYLE), parameterSet.popupMFTMessage().messageBoxType);
    m_messageboxStyleDescription = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_MESSAGEBOX_STYLE), MESSAGESTYLE_DESCRIPTION[parameterSet.popupMFTMessage().messageBoxType]); 
    m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), parameterSet.popupMFTMessage().timeSpan, TimeSpanStepParameter::TR_SECONDS);

    stepParameters.clear();
    stepParameters.push_back(m_messageParameter);
    stepParameters.push_back(m_operatorProfileDescription);
    stepParameters.push_back(m_messageboxStyleDescription);
    stepParameters.push_back(m_pendingParameter);

    switch(parameterSet.popupMFTMessage().messageBoxType)
    {
    case MB_OK:
        stepParameters.push_back(m_okParameter);
        break;
    case MB_OKCANCEL:
        stepParameters.push_back(m_okParameter);
        stepParameters.push_back(m_cancelParameter);
        break;
    case MB_ABORTRETRYIGNORE:
        stepParameters.push_back(m_abortParameter);
        stepParameters.push_back(m_retryParameter);
        stepParameters.push_back(m_ignoreParameter);
        break;
    case MB_YESNOCANCEL:
        stepParameters.push_back(m_yesParameter);
        stepParameters.push_back(m_noParameter);
        stepParameters.push_back(m_cancelParameter);
        break;
    case MB_YESNO:
        stepParameters.push_back(m_yesParameter);
        stepParameters.push_back(m_noParameter);
        break;
    case MB_RETRYCANCEL:
        stepParameters.push_back(m_retryParameter);
        stepParameters.push_back(m_cancelParameter);
        break;
    default:
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Messagebox Style:%lu", parameterSet.popupMFTMessage().messageBoxType);
    }
    stepParameters.push_back(m_timeSpanParameter);
    FUNCTION_EXIT;
}


void PopupMFTMessageStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::PopupMFTMessageParameters  parameter;

    parameter.decisionMessage = CORBA::string_dup(m_messageParameter->getMessageText().c_str());
    parameter.yesStep = m_yesParameter->getStepId();
    parameter.noStep = m_noParameter->getStepId();
    parameter.pendingStep = m_pendingParameter->getStepId();
    parameter.abortStep = m_abortParameter->getStepId();
    parameter.retryStep = m_retryParameter->getStepId();
    parameter.ignoreStep = m_ignoreParameter->getStepId();
    parameter.okStep = m_okParameter->getStepId();
    parameter.cancelStep = m_cancelParameter->getStepId();
    parameter.messageBoxType = m_messageboxStyle->getNumericValue();
    parameter.operatorProfileNumber = m_operatorProfileNumber->getNumericValue();
    parameter.timeSpan = m_timeSpanParameter->getNumericValue();

    step.parameters.popupMFTMessage(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* PopupMFTMessageStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditPopupMFTMessagePage(this);
}

void TA_Base_App::PopupMFTMessageStep::updateParameterList()
{
    TA_Base_Core::ThreadGuard paramGuard(m_planStepParameterListLock);

    m_messageboxStyleDescription->setText(MESSAGESTYLE_DESCRIPTION[m_messageboxStyle->getNumericValue()]);

    bool bValidProfile = true;
    TA_Base_Core::IProfile *profile = ProfileConfigAccess::instance()->getProfileByKey(m_operatorProfileNumber->getNumericValue());
    if (NULL == profile)
    {
        bValidProfile = false;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Invalid profile key = %lu", m_operatorProfileNumber->getNumericValue());
    }
    m_operatorProfileDescription->setText(bValidProfile? profile->getName():"");

    m_planStepParameterList->clear();
    m_planStepParameterList->push_back(m_messageParameter);
    m_planStepParameterList->push_back(m_operatorProfileDescription);
    m_planStepParameterList->push_back(m_messageboxStyleDescription);
    m_planStepParameterList->push_back(m_pendingParameter);

    switch(m_messageboxStyle->getNumericValue())
    {
    case MB_OK:
        m_planStepParameterList->push_back(m_okParameter);
        break;
    case MB_OKCANCEL:
        m_planStepParameterList->push_back(m_okParameter);
        m_planStepParameterList->push_back(m_cancelParameter);
        break;
    case MB_ABORTRETRYIGNORE:
        m_planStepParameterList->push_back(m_abortParameter);
        m_planStepParameterList->push_back(m_retryParameter);
        m_planStepParameterList->push_back(m_ignoreParameter);
        break;
    case MB_YESNOCANCEL:
        m_planStepParameterList->push_back(m_yesParameter);
        m_planStepParameterList->push_back(m_noParameter);
        m_planStepParameterList->push_back(m_cancelParameter);
        break;
    case MB_YESNO:
        m_planStepParameterList->push_back(m_yesParameter);
        m_planStepParameterList->push_back(m_noParameter);
        break;
    case MB_RETRYCANCEL:
        m_planStepParameterList->push_back(m_retryParameter);
        m_planStepParameterList->push_back(m_cancelParameter);
        break;
    default:
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Messagebox Style:%lu", m_messageboxStyle->getNumericValue());
    }
    m_planStepParameterList->push_back(m_timeSpanParameter);
}
#endif // defined (STEPTYPE_POPUP_MFT_MESSAGE)


