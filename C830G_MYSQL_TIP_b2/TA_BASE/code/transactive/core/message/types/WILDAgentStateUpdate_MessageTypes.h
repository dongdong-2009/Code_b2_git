// File: WILDAgentStateUpdate_MessageTypes.h
// Created: 12/04/2006 4:13:35 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef WILDAGENTSTATEUPDATE_MESSAGETYPES_H
#define WILDAGENTSTATEUPDATE_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace WILDAgentStateUpdate
	{
		// Root context
		static MessageContext Context("StateUpdate", "WILDAgentStateUpdate", StateUpdateMessage);

		// Message Types for context WILDAgentStateUpdate
		static const MessageType WILDAgentStateUpdate(Context, "10097");
	};
};

#endif // WILDAGENTSTATEUPDATE_MESSAGETYPES_H