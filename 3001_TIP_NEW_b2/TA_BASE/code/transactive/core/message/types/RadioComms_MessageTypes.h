// File: RadioComms_MessageTypes.h
// Created: 10/05/2006 16:03:41
// This file is automatically generated from 9999D02170001-TransActive_CORBA_Messages_Register.xls
// Reference: 9999-D02-17-0001, Version: 34.08, Status: Draft, Dated: 10 May 06
//
// * DO NOT MODIFY! *

#ifndef RADIOCOMMS_MESSAGETYPES_H
#define RADIOCOMMS_MESSAGETYPES_H

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace RadioComms
	{
		// Root context
		static MessageContext Context("GeneralComms", "RadioComms", CommsMessage);

		// Message Types for context RadioComms
		static const MessageType RadioPTTNotification(Context, "40096");
		static const MessageType TrainMessage(Context, "40097");
		static const MessageType PredefinedTextMessageAdded(Context, "40099");
		static const MessageType PredefinedTextMessageDeleted(Context, "40100");
		static const MessageType PredefinedTextMessageModified(Context, "40101");
		static const MessageType RadioCircuitModeNotification(Context, "40102");
		static const MessageType ReceiveSDS(Context, "40103");
		static const MessageType ReceiveText(Context, "40104");
		static const MessageType RadioAuthorisationNotification(Context, "40105");
		static const MessageType RadioCallStatus(Context, "40106");
		       
		static const MessageType RadioMonitorNotification(Context, "40108");
		static const MessageType RadioSpeedDialUpdateNotification(Context, "40109");
		static const MessageType EmergencyRegroupGroupStatus(Context, "40190");
		static const MessageType EmergencyRegroupStatus(Context, "40191");
		static const MessageType RadioSessionUpdateNofification(Context, "40192");
	};

    //Broadcast message must be added into below ContextBroadcast
    namespace RadioComms
    {
        //RadioDirectoryNotification is a broadcast message, so must add below broadcast context
        static MessageContext ContextBroadcast("BroadcastComms", "RadioBroadcastComms", CommsMessage);
        // Message Types for context RadioBroadComms        
        static const MessageType RadioDirectoryNotification(ContextBroadcast, "40107");		
    }

   
};

#endif // RADIOCOMMS_MESSAGETYPES_H
