/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCommandServant.h $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 * Purpose is to decouple CORBA types from internal types, and delegate to an
 * internal implementation of the method.
 */

#ifndef RADIO_MFT_AGENT_COMMAND_SERVANT_H_INCLUDED
#define RADIO_MFT_AGENT_COMMAND_SERVANT_H_INCLUDED

#include <vector>
//TD18785
#include <map>
//TD18785

#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "bus/radio/radio_agent/idl/src/RadioMFTAgentCommandIDL.h"
#include "bus/radio/radio_agent/idl/src/RadioMFTAgentCallbackIDL.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCommandImpl.h"

#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentSummary.h"



class RadioMFTAgentCommandServant : public virtual TA_Base_Core::ServantBase,
									public virtual POA_TA_IRS_Bus::RadioMFTAgentCommandIDL
{

	public:
		RadioMFTAgentCommandServant(RadioMFTAgentSummary& summary, TA_Base_Core::IEntityData& EntityData);
		virtual ~RadioMFTAgentCommandServant();

		/**
		 * start the servant
         */
		virtual void start();

		/**
		 * stop the servant
         */
        virtual void stop();

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
		virtual void registerRadioMFTAgentCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback);

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
		virtual TA_IRS_Bus::Radio::RadioResourceCollection* getResourceCollection( TA_IRS_Bus::Radio::ERadioResourceType type);

		/**
		 * Attempts to associate the requested resource with the unselect speaker of the
		 * CIE. This implies that the resource is being passively monitored (ie. the
		 * operator can only listen, they cannot communicate to the resource).
		 * 
		 * As a side effect, the CIE informs the MFT Radio Agent of any subsequent events
		 * related to the resource.
		 * 
		 * @note Some resources are unidirectional broadcasts from the operator, so
		 * listening has no effect.
		 * 
		 * @return indicates successful execution of the request
		 * @exception CommandFailureException
		 * @exception ...
		 */	
		virtual void assignResource(
			const char* session,const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Short buttonRef);

		/**
		 * Attempts to break the association between the requested resource and the CIE.
		 * This causes the resource to be removed from the unselect speaker, and no longer
		 * being monitored by the CIE, and it ceases to send events related to the
		 * resource.
		 * 
		 * @return indicates successful execution of the request
		 * @exception CommandFailureException
		 * @exception ...
		 */
		virtual void deassignResource(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Attempts to associate the requested resource with the select speaker of the CIE.
		 * This implies that the resource is being actively monitored (ie. the operator
		 * can both listen and communicate to the resource).
		 * 
		 * @note Some resources are unidirectional broadcasts from the operator, so
		 * listening has no effect.
		 * 
		 * @return indicates successful execution of the request
		 * @exception CommandFailureException
		 * @exception ...
		 */
		virtual void selectResource(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		virtual void deselectResource(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * @exception CommandFailureException
		 * @exception ...
		 * 
		 * @note speaker is redundant, as the volume change message will only affect the
		 * volume level of the where the resource is currently single-selected.
		 * 
		 */
		virtual void changeVolume( const char* session, const TA_IRS_Bus::Radio::RadioResource& resource, 
						   TA_IRS_Bus::Radio::ESpeakerState speaker,CORBA::UShort volume );

		/**
		 * Connect a resource to a Transactive Managed Resource
		 * i.e. PatchGroup, DynamicRegroup, MultiSelect, MultiSite
		 * 
		 */
		virtual void connectGroup(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Release a resource from a Transactive Managed Resource
		 * i.e. PatchGroup, DynamicRegroup, MultiSelect, MultiSite
		 * 
		 */
		virtual void releaseGroup(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Begin an Ambience Listening Call
		 *
		 * @return indicates successful execution of the request
		 * @exception CommandFailureException
		 * @exception ...
		 */
		virtual void makeAmbienceListeningCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Begin a Private Call
		 *
		 * @return indicates successful execution of the request
		 * @exception CommandFailureException
		 * @exception ...
		 */
		virtual void makePrivateCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Boolean bEmergencyMode);

		/**
		 * Answer an incoming call
		 *
		 * @return indicates successful execution of the request
		 * @exception CommandFailureException
		 * @exception ...
		 */
		virtual void answerPrivateCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * End a private/emergency/ambience listening call
		 *
		 * @return indicates successful execution of the request
		 * @exception CommandFailureException
		 * @exception ...
		 */
		virtual void endCall(const char* session,const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Boolean bEmergencyMode);

// AZ++: TD 14314
        virtual void beginEmergency(const TA_IRS_Bus::Radio::RadioResource& resource);
		virtual void acknowledgeEmergency(const TA_IRS_Bus::Radio::RadioResource& resource);
		virtual void knockdownEmergency(const TA_IRS_Bus::Radio::RadioResource& resource);
// ++AZ: TD 14314

		virtual void assignSelectSpeaker(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Get private call status of resource
		 *
		 * @return true if resource is involved in a private call, false otherwise
		 */
		virtual TA_IRS_Bus::Radio::EPrivateCallState isPrivateCallResourceInUse(const TA_IRS_Bus::Radio::RadioResource& resource);        

		/**
		 * Check if group resource is connected
		 *
		 * @return true if resource is Connected, false if Released state
		 */
		virtual TA_IRS_Bus::Radio::EGroupConnectionState isResourceGroupConnected(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Check if resource is active
		 *
		 * @return indicates resource transmit state
		 */
		virtual CORBA::Boolean isResourceActive(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Get AmbienceListening call state of resource
		 *
		 * @return true if resource is in ambience listening call state, false otherwise
		 */
		virtual CORBA::Boolean isResourceInAmbienceListeningCall(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Check if this resource is assigned to a resource button
		 *
		 * @return true if resource is monitored, false otherwise
		 */
		virtual CORBA::Boolean isResourceBeingMonitored(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Check if this resource is selected
		 *
		 * @return true if resource is selected, false otherwise
		 */
		virtual CORBA::Boolean isResourceSelected(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Check if this resource is assigned to a channel
		 *
		 * @return true if resource is assigned, false otherwise
		 */
		virtual CORBA::Boolean isResourceAssigned(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Check if this resource is in EmergencyMode
		 *
		 * @return true if in Emergency Mode, false otherwise
		 */
		virtual TA_IRS_Bus::Radio::EmergencyModeState isResourceInEmergencyMode(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Get the volume of the resource in the select/unselect speaker
		 *
		 * @return volume
		 */
		virtual CORBA::UShort getResourceVolume(const TA_IRS_Bus::Radio::RadioResource& resource);

		/**
		 * Get the members of the resource
		 *
		 * @return sequence<RadioResource>
		 */
		virtual void getResourceMembers(const TA_IRS_Bus::Radio::RadioResource& resource, TA_IRS_Bus::Radio::RadioResourceCollection_out members);

		/**
		 * Get resource with the given SSI
		 *
		 * @return RadioResource
		 */
		virtual void getResourceBySSI(CORBA::ULong ssi, TA_IRS_Bus::Radio::RadioResource_out resource);

		/**
		 * Get resource with the given Resource ID & type
		 *
		 * @return RadioResource
		 */
        virtual void getResourceByID(CORBA::ULong rid, TA_IRS_Bus::Radio::ERadioResourceType type, TA_IRS_Bus::Radio::RadioResource_out resource);
		
		/**
		 * Create a temporary Patch Group with a BIM resource 
		 * to be used by DLTS & EPAX
		 *
		 * @return RadioResource
		 */
		virtual void createPatchGroup(const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Boolean bIsDLTSPatch);

		/**
		 * Release the temporarily created Patch Group with a BIM/SPI Phone resource 
		 * to be used by DLTS & EPAX
		 *
		 * @param true if patch with DLTS, false if patch with EPAX
		 */
		virtual void releasePatchGroup(CORBA::Boolean bIsDLTSPatch);

		void assignAndSelectResource( 	const char* session,const TA_IRS_Bus::Radio::RadioResource& resource,CORBA::Short buttonRef);
		
		void deassignAndDeselectResource( const char* session,const TA_IRS_Bus::Radio::RadioResource& resource);

		/*virtual void makePhoneCall(const char* number, CORBA::Short length);

		virtual void endPhoneCall(const char* epaxNumber);*/
//TD18785
		virtual void addSelectSpeakerResource(const TA_IRS_Bus::Radio::RadioResource& resource);
		virtual void deleteSelectSpeakerResource(unsigned long rid);
		virtual void addActiveResource(const TA_IRS_Bus::Radio::RadioResource& resource);
		virtual void deleteActiveResource(unsigned long rid);

//TD18785
		/**
		 * Method to get the allocated DG Talkgroup on DynamicRegroup connection
		 * Fixed TD#14320
		 * 
		 * @return RadioResource
		 */		
		virtual TA_IRS_Bus::Radio::RadioResource* getDGTalkgroup(const char* session, const TA_IRS_Bus::Radio::RadioResource& resource);

        IRadioMFTAgentCommand* getImplementation() { return m_pMFTAgentCommandImpl;};

		virtual void addRadioResource(const ::TA_IRS_Bus::Radio::RadioResource& resource, 
												   const ::TA_IRS_Bus::Radio::RadioResourceCollection& members, 
												   ::CORBA::ULong memberSize, ::CORBA::ULong id);

		void restartConnection() {/* do nothing */};

		void getRadioResourceCount(const ::TA_IRS_Bus::Radio::RadioResource& resource, ::CORBA::Short& count){/* do nothing */};

		void getResourceByAlias(const char* rid, TA_IRS_Bus::Radio::ERadioResourceType type, TA_IRS_Bus::Radio::RadioResource_out resource) {};
		void updateResource(const ::TA_IRS_Bus::Radio::RadioResource& oldResource, const ::TA_IRS_Bus::Radio::RadioResource& newResource) {};
		void addNewResource(const ::TA_IRS_Bus::Radio::RadioResource& newResource) {};
		void deleteResource(const ::TA_IRS_Bus::Radio::RadioResource& oldResource) {};

	private:
		//Disabled Methods
		RadioMFTAgentCommandServant(const RadioMFTAgentCommandServant&);
		RadioMFTAgentCommandServant& operator=(const RadioMFTAgentCommandServant&);

//TD18785
		void terminateCurrentSessions();		
//TD18785

	
	private:
		
		IRadioMFTAgentCommand* m_pMFTAgentCommandImpl;


        RadioMFTAgentSummary& m_summary;

        std::string m_objectName;

		TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr m_pCallback;

//TD18785
		std::map<unsigned long, RadioResource> m_SelectedSpeakerResources;
		std::map<unsigned long, RadioResource> m_ActiveResources;
//TD18785
};
#endif // !defined({54748338-E88C-4a3c-9D95-8B1A04881884}__INCLUDED_)
