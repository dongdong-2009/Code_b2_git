// File: LessAgentAudit_MessageTypes.h
// Created: 12/04/2006 4:13:32 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef LESSAGENTAUDIT_MESSAGETYPES_H
#define LESSAGENTAUDIT_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace LessAgentAudit
	{
		// Root context
		static MessageContext Context("Audit", "LessAgentAudit", AuditMessage);

		// Message Types for context LessAgentAudit
		static const MessageType OpcServerTagUpdate(Context, "30175");
	};
};

#endif // LESSAGENTAUDIT_MESSAGETYPES_H