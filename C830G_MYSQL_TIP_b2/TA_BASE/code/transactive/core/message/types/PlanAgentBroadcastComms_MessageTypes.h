// File: PlanAgentBroadcastComms_MessageTypes.h
// Created: 12/04/2006 4:13:33 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef PLANAGENTBROADCASTCOMMS_MESSAGETYPES_H
#define PLANAGENTBROADCASTCOMMS_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace PlanAgentBroadcastComms
	{
		// Root context
		static MessageContext Context("BroadcastComms", "PlanAgentBroadcastComms", CommsMessage);

		// Message Types for context PlanAgentBroadcastComms
		static const MessageType ActivePlanUpdate(Context, "40093");
		static const MessageType PlanConfigUpdate(Context, "40094");
		static const MessageType PlanCustomisationUpdate(Context, "40095");
	};
};

#endif // PLANAGENTBROADCASTCOMMS_MESSAGETYPES_H
