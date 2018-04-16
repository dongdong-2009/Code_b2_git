// File: DutyAgentAudit_MessageTypes.h
// Created: 12/04/2006 4:13:31 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef DUTYAGENTAUDIT_MESSAGETYPES_H
#define DUTYAGENTAUDIT_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace DutyAgentAudit
	{
		// Root context
		static MessageContext Context("Audit", "DutyAgentAudit", AuditMessage);

		// Message Types for context DutyAgentAudit
		static const MessageType SubsystemDutyRequest(Context, "30564");
		static const MessageType SubsystemDutyRequestReceived(Context, "30565");
		static const MessageType SubsystemDutyRequestAccept(Context, "30566");
		static const MessageType SubsystemDutyRequestAcceptReceived(Context, "30567");
		static const MessageType SubsystemDutyRequestRejected(Context, "30568");
		static const MessageType SubsystemDutyRequestRejectedReceived(Context, "30569");
		static const MessageType SubsystemDutyRequestTimeout(Context, "30570");
		static const MessageType SubsystemDutyRequestTimeoutReceived(Context, "30571");
		static const MessageType SubsystemDutyGain(Context, "30572");
		static const MessageType SubsystemDutyLost(Context, "30573");
		static const MessageType SubsystemDutyAllRequest(Context, "30574");
		static const MessageType SubsystemDutyAllRequestReceived(Context, "30575");
		static const MessageType SubsystemDutyAllRequestAccept(Context, "30576");
		static const MessageType SubsystemDutyAllRequestAcceptReceived(Context, "30577");
		static const MessageType SubsystemDutyAllRequestRejected(Context, "30578");
		static const MessageType SubsystemDutyAllRequestRejectedReceived(Context, "30579");
		static const MessageType SubsystemDutyAllRequestTimeout(Context, "30580");
		static const MessageType SubsystemDutyAllRequestTimeoutReceived(Context, "30581");
		static const MessageType SubsystemDutyAllGain(Context, "30582");
		static const MessageType SubsystemDutyAllLost(Context, "30583");
		static const MessageType RegionDutyTransfer(Context, "30584");
		static const MessageType RegionDutyTransferReceived(Context, "30585");
		static const MessageType SystemWideDelegation(Context, "30586");
		static const MessageType AutomaticDelegation(Context, "30587");
		static const MessageType SubsystemDegraded(Context, "30588");
	};
};

#endif // DUTYAGENTAUDIT_MESSAGETYPES_H
