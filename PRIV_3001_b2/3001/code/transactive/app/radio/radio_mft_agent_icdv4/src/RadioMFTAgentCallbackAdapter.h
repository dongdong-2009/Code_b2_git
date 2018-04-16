/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCallbackAdapter.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Adapter which decouples CORBA-based RadioMFTAgentCallbackIDL from the rest of
 * the Radio MFT Agent.
 * 
 * The lifetime of this object is tied to the lifetime of the
 * RadioMFTAgentCommandServant. ie. if the servant terminates/dies/destructs, then
 * any callbacks should be removed.
 */

#ifndef RADIO_MFT_AGENT_CALLBACK_ADAPTER_H_INCLUDED
#define RADIO_MFT_AGENT_CALLBACK_ADAPTER_H_INCLUDED

#include <string>
#include <vector>

#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h"
#include "app/radio/radio_mft_agent_icdv4/src/IRadioMFTAgentCallback.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "bus/radio/radio_bus/src/RadioResourceMFTServerAttributes.h"
#include "bus/radio/radio_agent/idl/src/RadioMFTAgentCallbackIDL.h"
#include "bus/radio/radio_agent/idl/src/CommonRadioAgentIDL.h"
#include "core/naming/src/NamedObject.h"


class RadioMFTAgentCallbackAdapter : public IRadioMFTAgentCallback
{

	public:

		RadioMFTAgentCallbackAdapter();

		virtual ~RadioMFTAgentCallbackAdapter();

	public:

		virtual void callback_applicationStateMessage(
				ApplicationState state, 
				unsigned int percent);

		virtual void callback_applicationErrorMessage(
            const std::string& message);

        virtual void callback_groupStateMessage(
                const RadioResource& resource, 
                GroupConnectionState state);

		virtual void callback_resourceAssignmentMessage(
				const RadioResource& resource, 
				unsigned int buttonSet, 
				unsigned int buttonRef, 
				bool bAssigned);

		virtual void callback_resourceSelectMessage(
				const RadioResource& resource, 
				AssociatedSpeaker status);

		virtual void callback_resourceVolumeMessage(
				const RadioResource& resource, 
				AssociatedSpeaker speaker, 
				unsigned int volume);

		virtual void callback_inboundCallMessage(
				const RadioResource& caller, 
				PrivateCallState status);

		virtual void callback_privateCallMessage(
				const RadioResource& caller, 
				PrivateCallState status);

		virtual void callback_resourceActivityMessage(
				const RadioResource& resource, 
				ResourceActivityState status);

		virtual void callback_resourcePTTMessage(const RadioResource& listener, const RadioResource& src, bool bPushed);

		virtual void callback_resourceEmergencyModeMessage(
				const RadioResource& resource, 
				RadioEmergencyModeType type,
				RadioResource changedBy);

		virtual void callback_ambienceListeningMessage(
				const RadioResource& resource, 
				AmbienceListeningCallState state);
		

		bool addCallbackCollection(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback);

		
	private:

		/**
		 * To store the callback reference to enable the Radio MFT Agent to 'notify'
		 * interested clients.
		 * 
		 */
		std::vector<TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr> m_callbackCollection;
    
		//Disabled Methods
		RadioMFTAgentCallbackAdapter(const RadioMFTAgentCallbackAdapter&);

		RadioMFTAgentCallbackAdapter& operator=(const RadioMFTAgentCallbackAdapter&);

};

#endif 
