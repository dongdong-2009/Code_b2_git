/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PAMessageStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the PaMessage parameter of a Station PA step.
  *
  **/

#include "StdAfx.h"

#include "PAMessageStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaDvaMessage.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaTrainDvaMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
const std::string NO_MESSAGE = "No message selected";
const unsigned long PA_UNDEFINED_MESSAGE_ID = 0;
/////////////////////////////////////////////////////////////////////////////
// PaMessageStepParameter

PaMessageStepParameter::PaMessageStepParameter(PlanStep& parent, const std::string& name, bool bPATrainMsg) :
    PlanStepParameter(parent, name, bPATrainMsg)
{
    FUNCTION_ENTRY( "PaMessageStepParameter" );

    m_paMessage.messageId = PA_UNDEFINED_MESSAGE_ID;
    m_paMessage.libVersion = 0;
    m_paMessage.libSection = 0;

    FUNCTION_EXIT;
}

PaMessageStepParameter::PaMessageStepParameter(PlanStep& parent, const std::string& name, PAMessage message, bool bPATrainMsg) :
    PlanStepParameter(parent, name, bPATrainMsg),
    m_paMessage (message)
{
    FUNCTION_ENTRY( "PaMessageStepParameter" );
    FUNCTION_EXIT;
}

PaMessageStepParameter::~PaMessageStepParameter()
{
    FUNCTION_ENTRY( "~PaMessageStepParameter" );
    FUNCTION_EXIT;
}

void PaMessageStepParameter::setPaMessageId(const unsigned long& messageId)
{
    FUNCTION_ENTRY( "setPaMessageId" );

    m_paMessage.messageId = messageId;

    FUNCTION_EXIT;
}

void PaMessageStepParameter::setPAMessageLibVersion (const unsigned short& libVersion)
{
    FUNCTION_ENTRY( "setPAMessageLibVersion" );

    m_paMessage.libVersion = libVersion;

    FUNCTION_EXIT;
}

void PaMessageStepParameter::setPAMessageLibSection(const unsigned short& libSection)
{
    FUNCTION_ENTRY( "setPAMessageLibSection" );

    m_paMessage.libSection = libSection;

    FUNCTION_EXIT;
}

IPlanStepParameterForm* PaMessageStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterEditDlg(*this, canEdit);
}

std::vector<std::string> PaMessageStepParameter::getStepParameterValues()
{
	std::string messageLabel;
	unsigned long messageID = m_paMessage.messageId;
	if (messageID > 0)
	{
		if (m_refSrcValue)
		{
			TA_Base_Core::IPaTrainDvaMessage* paMessage = NULL;
			try
			{
				paMessage = TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getPaTrainDvaMessage(messageID, false);
			}
			catch ( std::exception& expWhat )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
			}
			catch ( ... )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unkonwn exception while getPaTrainDvaMessage" );
			}

			if ( NULL != paMessage )
			{
				messageLabel = paMessage->getName ();
				delete paMessage;
				paMessage = NULL;
			}
			else
			{
				messageLabel = NO_MESSAGE;
			}
		}
		else
		{
			TA_Base_Core::IPaDvaMessage* message = NULL;
			try
			{
				message = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getPaDvaMessage (messageID, false);
			}
			catch ( std::exception& expWhat )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
			}
			catch ( ... )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unkonwn exception while getPaTrainDvaMessage" );
			}

			if ( NULL != message )
			{
				messageLabel = message->getLabel();
				delete message;
				message = NULL;
			}
			else
			{
				messageLabel = NO_MESSAGE;
			}
		}
	}
	else
	{
		messageLabel = NO_MESSAGE;
	}

	std::vector<std::string> parametersVect;
	parametersVect.push_back(messageLabel);
	return parametersVect;
}

const std::string PaMessageStepParameter::displayValue() const
{
	return "";
}