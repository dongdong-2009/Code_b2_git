/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/RadioMFTAgent/src/IRadioMFTAgentCommand.h $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifndef IRADIO_MFT_AGENT_COMMAND_H_INCLUDED
#define IRADIO_MFT_AGENT_COMMAND_H_INCLUDED

#include <vector>
#include "bus/radio/radio_bus/src/radiotypes.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.h"
#include "bus/radio/radio_agent/idl/src/RadioMFTAgentCommandIDL.h"
#include "bus/radio/radio_bus/src/RadioResourceMFTServerAttributes.h"
class IRadioMFTAgentCommand
{

	public:

		virtual ~IRadioMFTAgentCommand() {};

		
	public:

		enum CommandState
		{
			START,
			ACTION,
			END,
			OPEN
		};
/**
		 * To register the Radio Manager (or other client application) with the Radio MFT
		 * Agent, to enable the client application to receive update notifications.
		 * 
		 * @note Since the MFT Radio Agent is tied to a single MFT and CIE, it is more
		 * efficient to bypass transActive Notification Service, and keep the
		 * communications local to the MFT.
		 * 
		 * @return indicates successful execution of the request
		 * @exception...
		 */
		virtual bool registerRadioMFTAgentCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback)=0;
		virtual void cancelPrivateCall(RadioResource &resource, PrivateCallState ptCallState)=0;
		virtual void endAmbienceListeningCall(RadioResource& resource)=0;
		virtual void assignSelectSpeaker(RadioResource& resource)=0;

		virtual std::vector<RadioResource> getResourceCollection(RadioResourceType type) =0;
		virtual void assignResource(RadioResource& resource) =0;
		virtual void deassignResource(RadioResource& resource,LogicalChannel channel =0) =0;

		virtual void selectResource(RadioResource& resource) =0;
		virtual void deselectResource(RadioResource& resource,LogicalChannel channel =0) =0;
		virtual void selectAG(RadioResource& resource,CommandState state=START) = 0;

		virtual void changeVolume(RadioResource& resource, unsigned int volume) =0;

		virtual void connectPatchGroup(RadioResource& resource,CommandState state=ACTION,unsigned int grpNo=0) =0;
		virtual void releasePatchGroup(RadioResource& resource) =0;

		virtual void connectMselGroup(RadioResource& resource,CommandState state=OPEN,unsigned int grpNo=0) =0;
		virtual void releaseMselGroup(RadioResource& resource,bool bSolicited) =0;

		virtual void connectSite(RadioResource& resource,CommandState state=START) =0;
		virtual void releaseSite(RadioResource& resource) =0;

		virtual void connectMselSite(RadioResource& resource,CommandState state=START,unsigned int grpNo=0) = 0;
		virtual	void releaseMselSite(RadioResource& resource) =0;

		virtual void connectDynRegroup(RadioResource& resource) = 0;
		virtual	void releaseDynRegroup(RadioResource& resource) =0;

		virtual void makePrivateCall(RadioResource& resource,CommandState state=ACTION) =0;
		virtual void answerPrivateCall(RadioResource& resource,CommandState state=ACTION) =0;
		virtual void endPrivateCall(RadioResource& resource) =0;
		
		virtual void makeEmergencyCall(RadioResource& resource) =0;
		virtual void endEmergencyCall(RadioResource& resource) =0;
		virtual void ackEmergency(RadioResource& resource) = 0;

		virtual void makeAmbienceListeningCall(RadioResource& resource) =0;

		virtual void generalTransmit(CommandState state=START) = 0;

		virtual void beginTone(unsigned int tone) = 0;
		virtual void endTone() = 0;

		virtual void callback_initialisation() = 0;
		virtual void callback_termination() = 0;

		virtual void setApStateMgr(RadioApplicationStateManager* p) = 0;

		virtual RadioApplicationStateManager* getApStateMgr() = 0;

		virtual void createPatchGroup(RadioResource& resource, bool bIsDLTSPatch=true) = 0;
/*
		virtual void makePhoneCall(std::string number,CommandState state=START) = 0;

		virtual void endPhoneCall(std::string number) = 0;*/
};
#endif // !defined({D488AC75-0598-45eb-BAEA-42BF9A0D6EE4}__INCLUDED_)