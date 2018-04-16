/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/RadioMFTAgent/src/RadioMFTAgentCommandImpl.h $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 * This is the implementation of the IDL methods, free from any CORBA types or
 * dependanceis.
 * 
 * This is the business logic that drives API commands.

 */

#ifndef RADIO_MFT_AGENT_COMMAND_H_INCLUDED
#define RADIO_MFT_AGENT_COMMAND_H_INCLUDED

#include "bus/radio/radio_bus/src/radiotypes.h"
#include "app/radio/radio_mft_agent_icdv4/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/FutureManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioEventDispatcher.h"
#include "bus/radio/radio_agent/idl/src/RadioServerAgentIDL.h"
#include"RemoteApiConnection.h"
#include "ace/Future.h"


class RadioMFTAgentCommandImpl : public IRadioMFTAgentCommand
{

	public:

		RadioMFTAgentCommandImpl();

		virtual ~RadioMFTAgentCommandImpl();


	public:

		/**
		 * Returns a collection of RadioResource's that are currently associated with the
		 * CIE.
		 * 
		 * @param type is optional, if not specified (or set to RR_NILL), then all
		 * RadioResourceType's are returned.
		 * 
		 * @return indicates successful execution of the request
		 * @exception ...
		 */
		virtual std::vector<RadioResource> getResourceCollection(RadioResourceType type);

		/**
		 * Attempts to associate the requested resource with the unselect speaker of the
		 * CIE. This implies that the resource is being passively monitored (ie. the
		 * operator can only listen, they cannot communicate to the resource).
		 * 
		 * @return indicates successful execution of the request
		 * @exception InvalidParameterException
		 */
		virtual void assignResource(RadioResource& resource);

		/**
		 * Attempts to break the association between the requested resource and the CIE.
		 * This causes the resource to be removed from the unselect speaker, and no longer
		 * being monitored by the CIE, and it ceases to send events related to the
		 * resource.
		 * 
		 * @return indicates successful execution of the request
		 * @exception InvalidParameterException
		 */
		virtual void deassignResource(RadioResource& resource,LogicalChannel channel =0);

		/**
		 * Attempts to associate the requested resource with the select speaker of the CIE.
		 * This implies that the resource is being actively monitored (ie. the operator
		 * can both listen and communicate to the resource).
		 * 
		 * @return indicates successful execution of the request
		 * @exception InvalidParameterException
		 */
		virtual void selectResource(RadioResource& resource);
		virtual void selectAG(RadioResource& resource,CommandState state=START);

		virtual void deselectResource(RadioResource& resource,LogicalChannel channel =0);

		/**
		 * @note speaker is redundant, as the volume change message will only affect the
		 * volume level of the where the resource is currently single-selected.
		 */
		virtual void changeVolume(RadioResource& resource, unsigned int volume);
		
		virtual void connectPatchGroup(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);
		virtual void releasePatchGroup(RadioResource& resource);

		virtual void connectMselGroup(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);
		virtual void releaseMselGroup(RadioResource& resource,bool bSolicited);

		virtual void connectDynRegroup(RadioResource& resource);
		virtual	void releaseDynRegroup(RadioResource& resource);

		virtual void makePrivateCall(RadioResource& resource,CommandState state=START);
		virtual void answerPrivateCall(RadioResource& resource,CommandState state=ACTION);
		virtual void endPrivateCall(RadioResource& resource);

// ++ Huangxin TD14319 
		virtual void cancelPrivateCall(RadioResource& resource,PrivateCallState ptCallState);
// TD14319++

		virtual void makeEmergencyCall(RadioResource& resource);
		virtual void endEmergencyCall(RadioResource& resource);
		virtual void ackEmergency(RadioResource& resource);

		virtual void makeAmbienceListeningCall(RadioResource& resource);
		virtual void endAmbienceListeningCall(RadioResource& resource);

		virtual void connectSite(RadioResource& resource,CommandState state=START);
		virtual void releaseSite(RadioResource& resource);

		virtual void connectMselSite(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);
		virtual void releaseMselSite(RadioResource& resource);

		virtual void generalTransmit(CommandState state);

		virtual void assignSelectSpeaker(RadioResource& resource); 
		
		virtual void callback_initialisation();
		virtual void callback_termination();

		FutureManager* getFutureManager() 
		{ 
			return m_pFutureManager; 
		};

		virtual void setApStateMgr(RadioApplicationStateManager* p) { m_pApStateMgr = p; };

		virtual RadioApplicationStateManager* getApStateMgr() { return m_pApStateMgr; };
		

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
		virtual bool registerRadioMFTAgentCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback);

		/**
		 * Create a temporary Patch Group for patching Radio-DLTS
		 * @return 
		 * @exception...
		 */	
		virtual void createPatchGroup(RadioResource& resource, bool bIsDLTSPatch=true);

		//Making / Ending Phone Calls was not there in the C955 
		/**
		 * Dial a phone Resource
		 * @return 
		 * @exception...
		 */	
		//virtual void makePhoneCall(std::string number,CommandState state=START);

		/**
		 * End call to a phone resource
		 * @return 
		 * @exception...
		 */	
		//virtual void endPhoneCall(std::string number);

	private:

		//Disabled Methods
		RadioMFTAgentCommandImpl(const RadioMFTAgentCommandImpl&);
		RadioMFTAgentCommandImpl& operator=(const RadioMFTAgentCommandImpl&);

		void addPatchMember(RadioResource& resource, unsigned int grpNum);
		
		void addMselMember(RadioResource& resource, unsigned int grpNum);
		void connectTempPG(RadioResource& resource, ResourceCollection& members);


	private:
		
		TA_Base_Core::NamedObject<TA_IRS_Bus::RadioServerAgentIDL,
							  TA_IRS_Bus::RadioServerAgentIDL_ptr,
					  		  TA_IRS_Bus::RadioServerAgentIDL_var> m_serverAgent;

        FutureManager* m_pFutureManager;

		RadioApplicationStateManager* m_pApStateMgr;
		RadioEventDispatcher* m_pEventDispatcher;

		void checkApState();
		static const int CONNECTION_LIMIT; // Fixed TD14320
};
#endif // !defined({8563EF6E-5CB7-4d54-A82C-3D6F4484166F}__INCLUDED_)
