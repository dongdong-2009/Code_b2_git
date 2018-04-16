/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PAMessageStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Represents the PaMessage parameter of a Station PA step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include "PAMessageStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessage.h"
#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/IPaTrainDvaMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
const CString NO_MESSAGE = "No message selected";

/////////////////////////////////////////////////////////////////////////////
// PaMessageStepParameter

PaMessageStepParameter::PaMessageStepParameter(PlanStep& parent, const std::string& name, bool bPATrainMsg) :
    PlanStepParameter(parent, name)
{
    FUNCTION_ENTRY( "PaMessageStepParameter" );

    m_paMessage.messageId = TA_Base_Core::PA_UNDEFINED_MESSAGE_ID;
    m_paMessage.libVersion = 0;
    m_paMessage.libSection = 0;
    m_bTrain = bPATrainMsg;

    FUNCTION_EXIT;
}


PaMessageStepParameter::PaMessageStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::PAMessage& message, bool bPATrainMsg) :
    PlanStepParameter(parent, name),
    m_paMessage (message)
{
    FUNCTION_ENTRY( "PaMessageStepParameter" );

    m_bTrain = bPATrainMsg;

    FUNCTION_EXIT;
}


PaMessageStepParameter::PaMessageStepParameter(PlanStep& parent, const PaMessageStepParameter& parameter, bool bPATrainMsg) :
    PlanStepParameter(parent, parameter.getName()),
    m_paMessage(parameter.getPAMessage())
{
    FUNCTION_ENTRY( "PaMessageStepParameter" );

    m_bTrain = bPATrainMsg;

    FUNCTION_EXIT;
}


PaMessageStepParameter::PaMessageStepParameter(const PaMessageStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_paMessage(parameter.getPAMessage())
{
    FUNCTION_ENTRY( "PaMessageStepParameter" );
    FUNCTION_EXIT;
}


PaMessageStepParameter::~PaMessageStepParameter()
{
    FUNCTION_ENTRY( "~PaMessageStepParameter" );
    FUNCTION_EXIT;
}


void PaMessageStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    CString messageLabel;
    unsigned long messageID = m_paMessage.messageId;
    if (messageID > 0)
    {
        if (m_bTrain)
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
                messageLabel = paMessage->getName ().c_str ();
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
                messageLabel = message->getLabel().c_str ();
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

    listCtrl.AddParameter(getName().c_str(), messageLabel.GetBuffer (1), this);

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


TA_Base_App::CPlanStepParameterEditDlg* PaMessageStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this, canEdit);
}


void PaMessageStepParameter::setPAMessageType (bool bTrain)
{
    FUNCTION_ENTRY( "setPAMessageType" );

    m_bTrain = bTrain;

    FUNCTION_EXIT;
}


#endif // defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
