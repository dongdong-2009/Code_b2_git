/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/IRadioMFTAgentCallback.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef IRADIO_MFT_AGENT_CALLBACK_H_INCLUDED
#define IRADIO_MFT_AGENT_CALLBACK_H_INCLUDED

#include <string>
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "bus/radio/radio_bus/src/RadioResourceMFTServerAttributes.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"

class IRadioMFTAgentCallback
{

	public:

		typedef enum 
		{ 
			No_Resource_Activity, 
			Resource_Tx_Begun, 
			Resource_Tx_Busy, 
			Resource_Tx_Ended
		} ResourceActivityState;


		/**
		 * Provides notification to the client that the application state of the
		 * RadioMFTAgent has changed.
		 * 
		 * @param percent Reserved for future use. At present always zero.
		 */
		virtual void callback_applicationStateMessage(ApplicationState state, unsigned int percent) =0;

        virtual void callback_applicationErrorMessage(const std::string& message) =0;

        /**
		 * Provides notification to the client that the group connection state 
		 * has changed.
		 */
		virtual void callback_groupStateMessage(const RadioResource& resource, GroupConnectionState state) =0;
        
		/**
		 * Informs observer of a RadioResource that it is currently assigned or deassigned.
		 * 
		 * 
		 * @note This has different significance to the Motorola API depending upon
		 * whether the resource is a Motorola managed resource, or a Transactive managed
		 * (ie. configurable) resource.
		 * 
		 */	
		virtual void callback_resourceAssignmentMessage(
			const RadioResource& resource, 
			unsigned int buttonSet, 
			unsigned int buttonRef, 
			bool bAssigned) =0;

		/**
		 * Informs observer of a RadioResource that it is currently associated with the
		 * select or unselect speaker. 
		 */	
		virtual void callback_resourceSelectMessage(
			const RadioResource& resource, 
			AssociatedSpeaker status) = 0;

		/**
		 * Informs observer of a RadioResource that its volume level for a particular
		 * speaker has changed. 
		 */
		virtual void callback_resourceVolumeMessage(const RadioResource& resource, AssociatedSpeaker speaker, unsigned int volume) =0;

		/**
		 * Informs observer of the status of an inbound private call.
		 * 
		 * @param status shall indicate whether the call is [new, ended]
		 */
		virtual void callback_inboundCallMessage(
				const RadioResource& caller, 
				PrivateCallState status) = 0;


		virtual void callback_privateCallMessage(
				const RadioResource& caller, 
				PrivateCallState status) =0;

		/**
		 * Informs observer of a RadioResource that some form of activity has occurred.
		 * 
		 * @note Activity is not simple constrained to operation of the PTT, it may also
		 * include other activities that could be construed as activity, such as:
		 * affiliation events, usage of a full-duplex resource, resource transmit status
		 * events, ... 
		 */
		virtual void callback_resourceActivityMessage(
				const RadioResource& resource, 
				ResourceActivityState status ) = 0;

		/**
		 * Informs observer of a RadioResource that the PTT has been pushed, for a half-
		 * duplex resource.
		 * 
		 * @note The Motorola API has different indicators for Radio Unit PTT usage versus
		 * Dispatch Console PTT usage. These differences are encapsulated behind this
		 * method.  
		 */
		virtual void callback_resourcePTTMessage(
                const RadioResource& listener, const RadioResource& src, bool bPushed) =0;

		/**
		 * Informs observer of a RadioResource that it has been set into emergency mode.
		 * 
		 * @param status Used to show whether the resource has: UnacknowledgedAlarm,
		 * AcknowledgedAlarm, NoAlarm
		 * @param changedBy Used to indicate who (if known) caused the change in emergency
		 * mode. 
		 */
		virtual void callback_resourceEmergencyModeMessage(
				const RadioResource& resource, 
				RadioEmergencyModeType type,
				RadioResource changedBy) = 0;

		/**
		 * Informs observer of a RadioResource of AmbienceListeningCall state
		 * 
		 * @param status Used to show whether the resource has: UnacknowledgedAlarm,
		 * AcknowledgedAlarm, NoAlarm
		 * @param changedBy Used to indicate who (if known) caused the change in emergency
		 * mode. 
		 */
		virtual void callback_ambienceListeningMessage(
				const RadioResource& resource, 
				AmbienceListeningCallState state) = 0;


		/*
		 * Informs observer that API operation fail and should be canceled
		 * 
		 * @param state The state of current operation
		 * @param resource the related radio resouce
		 */
		virtual void callback_operationCancelMessage(
			ApiState state,
			const RadioResource& resource) = 0;


		/*
		 * Informs observer there is a change in the resource collection
		 *
		 * @param oldResource details of resource before change
		 * @param newResource details of resource after change
		 */
		virtual void callback_resourceChangeMessage(
			const RadioResource& oldResource,
			const RadioResource& newResource) = 0;

		virtual void callback_callTerminated (RadioResource& resource) = 0;
		virtual void callback_callSetupFailure(/*RadioResource& resource*/) = 0;
		virtual long callback_sendErrorMessage(long messageID) = 0;
};

#endif

