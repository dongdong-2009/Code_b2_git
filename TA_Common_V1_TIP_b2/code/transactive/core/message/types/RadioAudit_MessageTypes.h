// File: RadioAudit_MessageTypes.h
// Created: 12/04/2006 4:13:33 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef RADIOAUDIT_MESSAGETYPES_H
#define RADIOAUDIT_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace RadioAudit
	{
		// Root context
		static MessageContext Context("Audit", "RadioAudit", AuditMessage);

		// Message Types for context RadioAudit
		static const MessageType RadioAdjustCallVolume(Context, "30284");
		static const MessageType RadioAnswerCall(Context, "30285");
		static const MessageType RadioAuthoriseCall(Context, "30286");
		static const MessageType RadioCeaseTX(Context, "30287");
		static const MessageType RadioCreatePatch(Context, "30288");
		static const MessageType RadioDeletePatch(Context, "30289");
		static const MessageType RadioDemandTX(Context, "30290");
		static const MessageType RadioDeselectCall(Context, "30291");
		static const MessageType RadioDynamicGroupCreated(Context, "30292");
		static const MessageType RadioDynamicGroupReleased(Context, "30293");
		static const MessageType RadioHoldCall(Context, "30294");
		static const MessageType RadioIncludeCall(Context, "30295");
		static const MessageType RadioMonitorEndCall(Context, "30296");
		static const MessageType RadioMonitorJoinCall(Context, "30297");
		static const MessageType RadioMonitorOneShot(Context, "30298");
		static const MessageType RadioMonitorStartListening(Context, "30299");
		static const MessageType RadioMonitorStartMonitoring(Context, "30300");
		static const MessageType RadioMonitorStopListening(Context, "30301");
		static const MessageType RadioMonitorStopMonitoring(Context, "30302");
		static const MessageType RadioReadTextMessage(Context, "30303");
		static const MessageType RadioReleaseCall(Context, "30304");
		static const MessageType RadioResumeCall(Context, "30305");
		static const MessageType RadioSelectCall(Context, "30306");
		static const MessageType RadioSendTextMessage(Context, "30307");
		static const MessageType RadioSetCallForwarding(Context, "30639"); // Radio_Call_Forwarding
		static const MessageType RadioTerminateCall(Context, "30309");
		static const MessageType RadioVoiceCall(Context, "30310");
		static const MessageType RadioAmbienceListeningActivated(Context, "30311");
		static const MessageType RadioAmbienceListeningTerminated(Context, "30312");
		static const MessageType RadioDynamicRegroupConnect(Context, "30635"); // Radio_Initiate_DGNA
		static const MessageType RadioDynamicRegroupRelease(Context, "30314");
		static const MessageType RadioEndPrivateCall(Context, "30315");
		static const MessageType RadioGroupMonitored(Context, "30631"); // Radio_Initiate_GroupCall
		static const MessageType RadioGroupSelect(Context, "30632"); // Radio_Select_GroupCall
		static const MessageType RadioGroupUnmonitored(Context, "30318");
		static const MessageType RadioGroupUnselect(Context, "30319");
		static const MessageType RadioIncomingPrivateCall(Context, "30320");
		static const MessageType RadioMakePrivateCall(Context, "30630"); // Radio_Initiate_IndividualCall
		static const MessageType RadioMultiselectConnect(Context, "30637"); // Radio_Initiate_Broadcast
		static const MessageType RadioMultiSiteConnect(Context, "30323");
		static const MessageType RadioPatchEstablished(Context, "30324");
		static const MessageType RadioPatchGroupConnect(Context, "30636"); // Radio_Initiate_CAD
		static const MessageType RadioPatchGroupRelease(Context, "30326");
		static const MessageType RadioPatchTerminated(Context, "30327");
		static const MessageType RadioPredefinedMessageAdd(Context, "30328");
		static const MessageType RadioPredefinedMessageCreated(Context, "30329");
		static const MessageType RadioPredefinedMessageDeleted(Context, "30330");
		static const MessageType RadioPredefinedMessageModify(Context, "30331");
		static const MessageType RadioPredefinedMessageRemove(Context, "30332");
		static const MessageType RadioPredefinedMessageUpdated(Context, "30333");
		static const MessageType RadioPrivateCallAnswered(Context, "30638"); // Radio_Answer_Individual_Call
		static const MessageType RadioSessionLogin(Context, "30335");
		static const MessageType RadioSessionLogout(Context, "30336");
		static const MessageType RadioSiteConnect(Context, "30337");
		static const MessageType RadioTextMessageRead(Context, "30338");
		static const MessageType RadioTextMessageReceive(Context, "30339");
		static const MessageType RadioTextMessageSend(Context, "30340");
		static const MessageType RadioEmergencyReceived(Context, "30620");
		static const MessageType RadioEmergencyAcknowledged(Context, "30633"); // Radio_Ack_EmergencyNotification
		static const MessageType RadioEmergencyEnded(Context, "30622");		
		static const MessageType RadioInitiateEmergency(Context, "30634"); // Radio_Initiate_EmergencyGroup
        static const MessageType RadioCallForwardingCancel(Context, "30640"); 
	};
};

#endif // RADIOAUDIT_MESSAGETYPES_H
