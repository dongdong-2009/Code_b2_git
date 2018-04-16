/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TisMessageStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the Message parameter of a Station/Train Traveller Info System step.
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

#include "TisMessageStepParameter.h"
#include "TisConfigAccess.h"
#include "PlanStepParameterTisMessageEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// TisMessageStepParameter

TisMessageStepParameter::TisMessageStepParameter(PlanStep& parent, const std::string& name, EMessageType messageType) :
    PlanStepParameter(parent, name, messageType),
    m_message(),
    m_messageText("")
{
    FUNCTION_ENTRY( "TisMessageStepParameter" );

    resetMessage();

    FUNCTION_EXIT;
}


TisMessageStepParameter::TisMessageStepParameter(PlanStep& parent, const std::string& name, TISMessage message, EMessageType messageType) :
    PlanStepParameter(parent, name, messageType),
    m_message()
{
    FUNCTION_ENTRY( "TisMessageStepParameter" );

    setMessage(message);

    FUNCTION_EXIT;
}

TisMessageStepParameter::~TisMessageStepParameter()
{
    FUNCTION_ENTRY( "~TisMessageStepParameter" );
    FUNCTION_EXIT;
}


void TisMessageStepParameter::setMessage(const TISMessage& message)
{
    FUNCTION_ENTRY( "setMessage" );

    m_message = message;

    switch (m_refSrcValue)
    {
        case MT_STATION:
            m_messageText = TisConfigAccess::getSTisMessageText(m_message);
            break;

        case MT_TRAIN:
            m_messageText = TisConfigAccess::getTTisMessageText(m_message);
            break;

        default:
            m_messageText = "";
    }

    FUNCTION_EXIT;
}


void TisMessageStepParameter::resetMessage()
{
    FUNCTION_ENTRY( "resetMessage" );

    m_message.libraryVersion = 0;
    m_message.librarySection = 0;
    m_message.messageId = TIS_UNDEFINED_MESSAGE_ID;

    m_messageText = "";

    FUNCTION_EXIT;
}

std::vector<std::string> TisMessageStepParameter::getStepParameterValues()
{
	std::vector<std::string> parametersVect;
	parametersVect.push_back(m_messageText);
	return parametersVect;
}


bool TisMessageStepParameter::editParameter(unsigned short libraryVersion, TisConfigAccess::PredefinedTisMessage& selectedMessage)
{
    FUNCTION_ENTRY( "editParameter" );

    CPlanStepParameterTisMessageEditDlg editDlg(libraryVersion, *this, true);

    if (editDlg.DoModal() == IDOK)
    {
        selectedMessage = editDlg.GetSelectedMessage();

        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}


IPlanStepParameterForm* TisMessageStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );

    TA_ASSERT(false, "Function not supported, use editParameter(unsigned short libraryVersion) instead");

    FUNCTION_EXIT;
    return NULL;
}

const std::string TisMessageStepParameter::displayValue() const
{
   return m_messageText;
}
