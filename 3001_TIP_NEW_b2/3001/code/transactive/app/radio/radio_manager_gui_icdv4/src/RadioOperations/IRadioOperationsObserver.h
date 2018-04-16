/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioOperations/IRadioOperationsObserver.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This interface provides coupling to external notifications of changes based on
 * an Observer pattern.
 * @created 15-Jun-2005 04:14:56 PM
 */

#if !defined(AFX_IRADIOOPERATIONSOBSERVER_H__72FE0F1A_22B2_442E_93DF_7A5DEF40BFC3__INCLUDED_)
#define AFX_IRADIOOPERATIONSOBSERVER_H__72FE0F1A_22B2_442E_93DF_7A5DEF40BFC3__INCLUDED_

#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h" 
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageData.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"


class IRadioOperationsObserver
{

	public:

		virtual ~IRadioOperationsObserver() { };

	public:
		virtual void onResourceAssignmentEvent(RadioResource& resource, unsigned int buttonRef) =0;
		virtual void onResourceDeAssignmentEvent(RadioResource& resource) =0;
		virtual void onVolumeChangedEvent(RadioResource& resource, unsigned int volume) =0;
		virtual void onSelectSpeakerSelectedEvent(RadioResource& resource) =0;
		virtual void onSelectSpeakerUnselectedEvent(RadioResource& resource) =0;
		virtual void onInboundCallEvent(RadioResource& resource) =0;
		virtual void onInboundCallCanceledEvent(RadioResource& resource) =0;
		virtual void onPrivateCallEstablished(RadioResource& resource) =0;
		virtual void onPrivateCallEnded(RadioResource& resource) =0;
		virtual void onPrivateCallProcessed(RadioResource& resource) =0; // Fixed TD#14131
		virtual void onInboundPTTEvent(RadioResource& listener,RadioResource& resource) =0;
		virtual void onInboundPTTReleasedEvent(RadioResource& resource) =0;
		virtual void onResourceActivityEvent(RadioResource& resource) =0;
		virtual void onResourceActivityEndedEvent(RadioResource& resource) =0;
		virtual void onResourceEmergencyEvent(RadioResource& resource,RadioResource& radioUnit) =0; //TD17224
		virtual void onResourceEmergencyAcknowledgedEvent(RadioResource& resource,RadioResource& radioUnit) =0; //TD17224
		virtual void onResourceEmergencyEndedEvent(RadioResource& resource,RadioResource& radioUnit) =0; // TD17224
		virtual void onPredefinedMessageEvent(RadioUpdateMessageType eventType, PredefinedMessage* messageData) =0;
		virtual void onIncomingTextMessageEvent(MessageInbox* messageData,RadioResource& resource) =0;
		virtual void onAmbienceListeningEvent(RadioResource& resource, AmbienceListeningCallState state) =0;

};
#endif // !defined(AFX_IRADIOOPERATIONSOBSERVER_H__72FE0F1A_22B2_442E_93DF_7A5DEF40BFC3__INCLUDED_)