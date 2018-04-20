// File: IncidentDetectionAgentAlarms_MessageTypes.h
// Created: 1/11/2005 3:40:25 PM
// This file is automatically generated from the message types register spreadsheet
//
// * DO NOT MODIFY! *

#ifndef INCIDENTDETECTIONAGENTALARMS_MESSAGETYPES_H
#define INCIDENTDETECTIONAGENTALARMS_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace IncidentDetectionAgentAlarms
	{
		// Root context
		static MessageContext Context("Alarm", "IncidentDetectionAgentAlarms", AlarmMessage);

		// Message Types for context IncidentDetectionAgentAlarms
		static const MessageType IncidentDetectionCommsAlarm(Context, "50079");
		static const MessageType IncidentDetectedAlarm(Context, "50080");
		static const MessageType IncidentDetectionConfigurationError(Context, "50291");
	};
};

#endif // INCIDENTDETECTIONAGENTALARMS_MESSAGETYPES_H