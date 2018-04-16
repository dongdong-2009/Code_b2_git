/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RADIO_MFT_AGENT_STATUS_MANAGER_H_INCLUDED
#define RADIO_MFT_AGENT_STATUS_MANAGER_H_INCLUDED

#include "ace/Singleton.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "ace/Null_Mutex.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

class IRadioMFTAgentCallback;

class RadioMFTAgentStatusManager
{
    struct DynGroup
    {
        RadioResource dynGroup;
        bool isFree;
		TransactiveResourceIdentifier dgID; //TD14133-the Transactive DG entity that is associated with the DG resource
    };

	public:

		RadioMFTAgentStatusManager(); 
		virtual ~RadioMFTAgentStatusManager();


	public:

		/*
		 * MFTResourceCollection accessor methods
		 *
		 */
		void setFeatureAttributeIsAssigned(const RadioResource& resource, bool isAssigned);
		bool getFeatureAttributeIsAssigned(const RadioResource& resource);

		bool getFeatureAttributeIsMonitored(const RadioResource& resource);

		bool setFeatureAttributeButton(const RadioResource& resource, bool bProfile, unsigned int offset);
		bool getFeatureAttributeButton(const RadioResource& resource, bool& bProfile, unsigned int& offset);

		void setFeatureAttributeIsSelected(const RadioResource& resource, AssociatedSpeaker state);
		AssociatedSpeaker getFeatureAttributeIsSelected(const RadioResource& resource);

		void setFeatureAttributeSpeakerVolume(const RadioResource& resource, unsigned int volume);
		unsigned int getFeatureAttributeSpeakerVolume(const RadioResource& resource);

		void setFeatureAttributeUnSpeakerVolume(const RadioResource& resource, unsigned int volume);
		unsigned int getFeatureAttributeUnSpeakerVolume(const RadioResource& resource);

		void setFeatureAttributeIsGrpConnected(const RadioResource& resource, GroupConnectionState state);
		GroupConnectionState getFeatureAttributeIsGrpConnected(const RadioResource& resource);

		void setFeatureAttributeIsActive(const RadioResource& resource, bool isActive);
		bool getFeatureAttributeIsActive(const RadioResource& resource);

		void setFeatureAttributePtCallState(const RadioResource& resource, PrivateCallState isActive);
		PrivateCallState getFeatureAttributePtCallState(const RadioResource& resource);

		void setFeatureAttributeALCallState(const RadioResource& resource, AmbienceListeningCallState isActive);
		AmbienceListeningCallState getFeatureAttributeALCallState(const RadioResource& resource);

		void setFeatureAttributeIsPatchInvolved(const RadioResource& resource, unsigned int patchId);
		unsigned int getFeatureAttributeIsPatchInvolved(const RadioResource& resource);

		void setFeatureAttributeEmergencyMode(const RadioResource& resource, RadioEmergencyModeType type);
		RadioEmergencyModeType getFeatureAttributeEmergencyMode(const RadioResource& resource);

		void setFeatureAttributeEmergencyRadioUnit(const RadioResource& resource, unsigned int radioSSI);
		unsigned int getFeatureAttributeEmergencyRadioUnit(const RadioResource& resource);

		/**
		  *  Adds either a Motorola Console resource or a Transactive
		  *  Managed resource to the appropriate collection
		  *
		  *  @param id represents the logical identifier for the physical resource. This may
		  *  be either a LogicalChannel, PatchGroupId, MselGroupId.
		  * 
		  *  @note for a DynGroup, the id parameter is unused.
		  */
		void addResource(RadioResource& resource,std::vector<RadioResource>& members,unsigned long id);

		/**
		  *  Deletes either a Motorola Console resource or a Transactive
		  *  Managed resource from the appropriate collection
		  */
		void delResource(RadioResource& resource);

		/**
		  *  @returns the LogicalChannel from the ChannelCollecton
		  *   corresponding to the requested ResourceIdentifier
		  */
		LogicalChannel getChannel(ResourceIdentifier resourceID);


		/**
		  *  @returns the ResourceIdentifier from the ChannelCollecton
		  *   corresponding to the requested LogicalChannel
		  */
		RadioResource getResourcebyChannel(LogicalChannel identifier);


		std::vector<RadioResource> getMFTResourceCollection(RadioResourceType type);

		/**
		  *  @returns the GroupNumber for PatchGroup/MSel/MSite
		  *   corresponding to the requested ResourceIdentifier
		  */
		unsigned int getGroupNumber(TransactiveResourceIdentifier rid);

		/**
		  *  @returns members of a PatchGroup/MSel/MSite
		  */
		std::vector<RadioResource> getGroupMembers(TransactiveResourceIdentifier rid);

		/**
		  *  map resourceID to channel
		  *
		  * @Note add resource to collection only on initialization mode/refresh
		  */
		void addChannel(ResourceIdentifier rid, LogicalChannel lcid, bool bInitMode = false);
		void deleteChannel(ResourceIdentifier rid);

		/**
		  *  @returns resource with a given ssi
		  */
		RadioResource getResourceBySSI(unsigned long unitID);
		RadioResource getRadioResourceBySSI(unsigned long unitID);

		/**
		  *  RadioResourceType is used to filter searching
		  *  of MFTResourceCollection
		  *
		  *  @returns resource with a given Resource Id
		  */
		RadioResource getConsoleResourceByID(ResourceIdentifier rid);

		RadioResource getTransactiveResourceByID(ResourceIdentifier rid);

		RadioResource getResourceByID(ResourceIdentifier rid, RadioResourceType type);

		/**
		  *  @returns group collection of Patch group/msel/msite
		  */
		ResourceGroupCollection getMselGroupCollection()
        {
            return m_groupCollectionMSel;
        }

		/**
		  *  Patch Group Management
		  */
		void addPatchGroup(unsigned int patchGrp,ResourceIdentifier identifier);

		void delPatchGroup(ResourceIdentifier identifier);

		void addPatchMember(unsigned int patchGrp, RadioResource& member);		

		void delPatchMember(unsigned int patchGrp,RadioResource& member);

		RadioResource getResourceByGroupNo(unsigned int grpNo, RadioResourceType type);
	
		/**
		  *  MSel Group Management
		  */
		void addMselMember(unsigned int mselGrp, RadioResource& member);		
		
		void addMselGroup(unsigned int mselGrp,ResourceIdentifier identifier);

		void delMselMember(unsigned int mselGrp, RadioResource& member);


		std::vector<RadioResource> getResourceMembers(const RadioResource& resource);


		/*
		 * Method  to initialize collection from data retrieved in RCDB and Transactive DB
		 */
		void initDataFromDB();
		
		RadioResource getSelectedSpeaker();
		
		unsigned long getNextTransResourceId();

		/*
		 * Method to get a free talkgroup resource to regroup a Radio resource
		 */
		RadioResource getFreeDynamicGroup(const RadioResource& resource);

		/*
		 * Release the dynamic group to be used by next resource
		 */
		void releaseDynamicGroup(const RadioResource& resource, unsigned long groupId);

        RadioResource getDynamicGroupForResource(const RadioResource& resource);
		/*
		 * Return the number of active DG connections
		 * Fixed TD14320
		 */
		int getNumberOfActiveDG();

		RadioResource getPrivateCallResource() { return m_privateCallRes; };
		RadioResource getBIMResource() { return m_BIMRes; };
		RadioResource getPhoneResource();

		void setTempPatchGroup(RadioResource& resource) { m_tempPatchGroup = resource; };

		RadioResource getTempPatchGroup() { return m_tempPatchGroup; };

		bool isTempPatchInvolved(RadioResource& resource);


		/*
		 * Check if resource is still being monitored somewhere else
		 */
		bool canDeassignResource(const RadioResource& resource, const RadioResource* parent);


		/*
		 * For resources that need to be connected, limit the connection to be done one by one
         * so we can check when there is an error from Motorola event
		 */
        RadioResource getPendingConnectResource(RadioResourceType pType);
        void setPendingConnectResource(const RadioResource& pResource);
        void clearPendingConnectResource(RadioResourceType pType);
        bool hasPendingConnectResource(RadioResourceType pType);

        void setMFTAgentCallback(IRadioMFTAgentCallback* pRadioMFTAgentCallback )
        {
            m_RadioMFTAgentCallback = pRadioMFTAgentCallback;
        }

		// TD14133
		void setSelectedDG(const RadioResource& dynGrp,TransactiveResourceIdentifier id);
		bool getSelectedDGState(const RadioResource& resource);
		// TD14133

		bool isAudibleAlarmOn();
		void setAudibleAlarm(bool alarmMode);
		//TD18803
		bool bPermitProfileAcquireDG();

		void sendErrorMessage(char* msg);
		void failConnectToRadio();


		
	private:

		//Disabled Methods
		RadioMFTAgentStatusManager(const RadioMFTAgentStatusManager&);
		RadioMFTAgentStatusManager& operator=(const RadioMFTAgentStatusManager&);

		/*
		 * Reads the transactive managed resources from database,
         * and adds them to the collection
		 */
		void loadTransactiveResources();

		/*
		 * Method to check if this DG Talkgroup is can be allocated for this MFT
         * based on Radio Numbering Plan doc
		 */
		bool isDGAvailable(const RadioResource& resource);
		/*
		*
		*/
		bool bCheckConnectionPermission();


		std::string getProfileName();
	private:

		//provides a thread lock
		static TA_Base_Core::ReEntrantThreadLockable m_lock;

		RadioResource  m_selectSpeaker;

		ChannelCollection m_channelCollection;

		// AZ: need to separate the group id's for PG & MSel
		ResourceGroupCollection m_groupCollectionPG;
		ResourceGroupCollection m_groupCollectionMSel;
		ResourceGroupMembersMap m_groupMembers;

		ResourceGroupMembersMap m_dynGroupMembers;
        std::map<TransactiveResourceIdentifier, DynGroup> m_dynGroupMap;
        std::vector<DynGroup> m_dynamicRegroup;
		
		MFTResourceCollection m_MFTResourceCollection;


		RadioResource m_privateCallRes;
		RadioResource m_BIMRes;
		RadioResource m_phoneResource;

		RadioResource m_tempPatchGroup;
		bool m_bStart;

        RadioResource m_pendingPatchGroup;
        RadioResource m_pendingMSel; // used by both msite & msel
        RadioResource m_pendingSite;

        IRadioMFTAgentCallback* m_RadioMFTAgentCallback;
		static const unsigned long MIN_DYNAMICGROUP_ID;
		static const unsigned long MAX_DYNAMICGROUP_ID;

//TD18803
		bool bPermitProfile;
//TD18803
		std::string m_profile;

		bool m_audibleAlarmOn;
};

typedef ACE_Singleton<RadioMFTAgentStatusManager, ACE_Null_Mutex> The_StatusManager;

#endif
