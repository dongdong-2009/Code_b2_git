// File: MmsAlarm_MessageTypes.h
// Created: 12/04/2006 4:13:32 PM
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.06, Status: Draft, Dated: 06 Apr 06
//
// * DO NOT MODIFY! *

#ifndef MMSALARM_MESSAGETYPES_H
#define MMSALARM_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace MmsAlarm
	{
		// Root context
		static MessageContext Context("Alarm", "MmsAlarm", AlarmMessage);

		// Message Types for context MmsAlarm
		static const MessageType MmsSubsystemReportFailure(Context, "50025");
		static const MessageType MmsAlarmQueueError(Context, "50200");
		static const MessageType MmsAnalogueQueueError(Context, "50201");
		static const MessageType MmsDbLinkFailure(Context, "50202");
		static const MessageType MmsGenericMonitorAlarm(Context, "50203");
	};
};

#endif // MMSALARM_MESSAGETYPES_H
