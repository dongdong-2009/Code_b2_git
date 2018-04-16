/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageFacade.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageFacade.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "core/message/types/RadioAudit_MessageTypes.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


bool RadioTextMessageFacade::addMessage(PredefinedMessage& predefinedMessageData)
{
    FUNCTION_ENTRY("addMessage");

	TA_Base_Core::DescriptionParameters descParams;
	The_RadioOperations::instance()->auditEventPosted(TA_Base_Core::RadioAudit::RadioPredefinedMessageAdd,descParams);

	FUNCTION_EXIT;
	return The_RadioSystemStatus::instance()->addPredefinedTextMessage(predefinedMessageData);
}


bool RadioTextMessageFacade::modifyMessage(PredefinedMessage& predefinedMessageData)
{
    FUNCTION_ENTRY("modifyMessage");

	TA_Base_Core::DescriptionParameters descParams;

	std::ostringstream sKey;
	sKey << predefinedMessageData.key;

	TA_Base_Core::NameValuePair param1("MessageID",sKey.str());
	descParams.push_back(&param1);

	The_RadioOperations::instance()->auditEventPosted(TA_Base_Core::RadioAudit::RadioPredefinedMessageModify,descParams);

    FUNCTION_EXIT;
	
	return The_RadioSystemStatus::instance()->modifyPredefinedTextMessage(predefinedMessageData);
}


bool RadioTextMessageFacade::removeMessage(PredefinedMessage& predefinedMessageData)
{
    FUNCTION_ENTRY("removeMessage");

	TA_Base_Core::DescriptionParameters descParams;

	std::ostringstream sKey;
	sKey << predefinedMessageData.key;

	TA_Base_Core::NameValuePair param1("MessageID",sKey.str());
	descParams.push_back(&param1);

	The_RadioOperations::instance()->auditEventPosted(TA_Base_Core::RadioAudit::RadioPredefinedMessageDeleted,descParams);

    FUNCTION_EXIT;
	
	return The_RadioSystemStatus::instance()->removePredefinedTextMessage(predefinedMessageData);
}


bool RadioTextMessageFacade::sendMessage(RadioTextMessageData& messageData,RadioResource& recepient)
{
    FUNCTION_ENTRY("sendMessage");

	TA_Base_Core::DescriptionParameters descParams;
	TA_Base_Core::NameValuePair param1("ResourceName",recepient.alias);
	descParams.push_back(&param1);

	The_RadioOperations::instance()->auditEventPosted(TA_Base_Core::RadioAudit::RadioTextMessageSend,descParams,recepient);

    FUNCTION_EXIT;
	return The_RadioSystemStatus::instance()->sendTextMessage(messageData,recepient);
}


std::vector<MessageInbox> RadioTextMessageFacade::getHistoricalMessages()
{
    FUNCTION_ENTRY("getHistoricalMessages");
    FUNCTION_EXIT;
	return The_RadioSystemStatus::instance()->getHistoricalTextMessages();
}


std::vector<PredefinedMessage> RadioTextMessageFacade::getPredefinedMessageList()
{
    FUNCTION_ENTRY("getPredefinedMessageList");
    FUNCTION_EXIT;
	return The_RadioSystemStatus::instance()->getPredefinedMessageList();
}


void RadioTextMessageFacade::updateTextMessage(unsigned long key, bool flag)
{
    FUNCTION_ENTRY("updateTextMessage");
    
	std::ostringstream sKey;
	sKey << key;

	TA_Base_Core::DescriptionParameters descParams;
	TA_Base_Core::NameValuePair param1("MessageID",sKey.str());
	descParams.push_back(&param1);

	The_RadioOperations::instance()->auditEventPosted(TA_Base_Core::RadioAudit::RadioTextMessageRead,descParams);

	The_RadioSystemStatus::instance()->updateTextMessageReadState(key,flag);

	FUNCTION_EXIT;
}