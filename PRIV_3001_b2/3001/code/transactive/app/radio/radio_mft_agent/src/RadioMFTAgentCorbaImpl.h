

virtual void assignResource(RadioResource& resource);
virtual void deassignResource(RadioResource& resource,LogicalChannel channel  =0);

virtual void selectResource(RadioResource& resource);
virtual void deselectResource(RadioResource& resource,LogicalChannel channel =0);

virtual void changeVolume(RadioResource& resource, unsigned int volume);


virtual void connectPatchGroup(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);

virtual void releasePatchGroup(RadioResource& resource);

virtual void connectMselGroup(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);

virtual void releaseMselGroup(RadioResource& resource,bool bSolicited);

virtual void makePrivateCall(RadioResource& resource,CommandState state=START);
virtual void answerPrivateCall(RadioResource& resource,CommandState state=START);
virtual void endPrivateCall(RadioResource& resource);


virtual void makeEmergencyCall(RadioResource& resource);
virtual void endEmergencyCall(RadioResource& resource);
virtual void ackEmergency(RadioResource& resource);

virtual void makeAmbienceListeningCall(RadioResource& resource);
virtual void endAmbienceListeningCall(RadioResource& resource);

virtual void connectSite(RadioResource& resource,CommandState state=START);


virtual void selectAG(RadioResource& resource,CommandState state=START);

virtual void generalTransmit(CommandState state);

		

		virtual void callback_initialisation();




// ++ Huangxin TD14319 
		virtual void cancelPrivateCall(RadioResource& resource,PrivateCallState ptCallState);
// TD14319++


		virtual void releaseSite(RadioResource& resource);


		virtual void assignSelectSpeaker(RadioResource& resource); 

		FutureManager* getFutureManager() 
		{ 
			return m_pFutureManager; 
		};

		virtual void setApStateMgr(RadioApplicationStateManager* p) { m_pApStateMgr = p; };

		virtual RadioApplicationStateManager* getApStateMgr() { return m_pApStateMgr; };
		

		/**
		 * Create a temporary Patch Group for patching Radio-DLTS
		 * @return 
		 * @exception...
		 */	
		virtual void createPatchGroup(RadioResource& resource, bool bIsDLTSPatch=true);

		/**
		 * Dial a phone Resource
		 * @return 
		 * @exception...
		 */	
		virtual void makePhoneCall(std::string number,CommandState state=START);

		/**
		 * End call to a phone resource
		 * @return 
		 * @exception...
		 */	
		virtual void endPhoneCall(std::string number);

	private:

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
