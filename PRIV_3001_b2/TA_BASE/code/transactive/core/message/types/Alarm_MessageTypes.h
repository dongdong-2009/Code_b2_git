// File: Alarm_MessageTypes.h
// Created: 23/04/2003 2:08:57 PM
// This file is automatically generated from the message types register spreadsheet
//
// * DO NOT MODIFY! *

#ifndef ALARM_MESSAGETYPES_H
#define ALARM_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace Alarm
	{
		// Root context
		static MessageContext Context("Alarm", "*", AlarmMessage);

		// Message Types for context Alarm

        static const MessageType AllAlarms(Context, "*");
	};
};

#endif // ALARM_MESSAGETYPES_H
