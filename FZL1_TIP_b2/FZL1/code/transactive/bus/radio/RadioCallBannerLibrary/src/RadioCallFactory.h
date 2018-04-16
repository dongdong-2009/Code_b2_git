/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/11/15 17:17:15 $
  * Last modified by:  $Author: bihui.luo $
  * 
  * This factory listens for Radio notifications from the
  * Radio Agent.
  *
  * If a notification of type Ringing is received, a new call entry is
  * created. Any other types of notification cause the call entry to be
  * deleted.
  *
  */


#ifndef CALL_BANNER_RADIO_CALL_FACTORY_H
#define CALL_BANNER_RADIO_CALL_FACTORY_H

#include "bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h"
#include "bus/banner_pages/call_page_library/src/CallEntryManager.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

// krtc
#include "bus/radio/radio_agent/idl/src/RadioMFTAgentCallbackIDL.h"


#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainDriverCallUpdateObserver.h"
#include "bus/security/rights_library/src/RightsLibrary.h"


namespace TA_Base_Bus
{
    class RightsLibrary;
}


namespace TA_IRS_Bus
{
    class RadioCallFactory :
        public TA_IRS_Bus::AbstractCallEntryFactory,
        public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
		public ITrainDriverCallUpdateObserver,
		public ITrainInformationUpdateObserver
    {

    public:


        /**
         *  Constructor.
         *
         *  @param  callManager      the call entry manager that is used to
         *                           manage the memory for this factory's call
         *                           entries.
         *  @param  locationKey      the location of the radio agent that this
         *                           factory's call entries communicate with.
         *  @param  radioSessionKey  the radio session used to distinguish
         *                           calls that are for this console.
         *
         *  @exception DataException
         *  @exception DatabaseException
         */
        RadioCallFactory(
            TA_IRS_Bus::CallEntryManager& callManager,
            unsigned long locationKey,
            long radioSessionKey );


        /**
         *  Destructor
         */
        virtual ~RadioCallFactory();


        /**
         *  Called when a Radio notification is received from the
         *  Message subsystem.
         */
        virtual void receiveSpecialisedMessage(
            const TA_Base_Core::CommsMessageCorbaDef& message);


            
        /**
         *  Gets the given notification as a string.
         */
        static std::string getNotificationAsString(
            const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event);


        /**
         * @return a string representation suitable for use in the 'Information' field
         *          of the call banner
         * @param radioAgent required to query agent for text message information (for
         *          those calls that have category 'TextMessage')
         */
        static std::string getCallInformation(
                    const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event);

        /**
         * getCallSourceAsString
         *
         * @return string representation of the source of the specified call
         */
        static std::string getCallSourceAsString( 
             const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event);

		        /**
         * canPerformAction
         *
         * This checks if the specified action can be performed on the banner
         * by the current session.
         *
         * @param action  The action to test
         *
         * @return  true if the action can be performed, false otherwise
         */
        bool canPerformAction(unsigned long action);


		/*
		 * For KRTC incoming call event
		 */
		void processEvent(const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event);

		/**
         * Creates the train agent proxy objects
         */
        void createProxies();

		/**
         * loads the radio and train calls from the agents
         */
        void getAllTrainDetails();

		/**
         * This will receive a OccCallRequest message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processOccCallRequest( const CallTypes::OccCallRequest& event );

		/**
         * This will receive a OccCallReset message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processOccCallReset( const CallTypes::OccCallReset& event );

		/**
         * This will receive a TrainDetails message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainDetails( const TrainInformationTypes::TrainDetails& event );

		/**
         * This will receive a TrainSelectionUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event );


		/**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event );


		/**
         * Updates an existing call driver entry in the banner
         * 
         * @param callId the call ID in string format
         * @param occCall the call details
         */
        void updateCallDriverCallEntry( const std::string& callId,
                                        const CallTypes::OccCallRequest& occCall );

		/**
         * Updates an existing call driver entry in the banner
         * 
         * @param callId the call ID in string format
         * @param trainDetails the updated train details
         */
        void updateCallDriverCallEntry( const std::string& callId,
                                        const TrainInformationTypes::TrainDetails& trainDetails );

		/**
         * Adds a new call driver entry to the banner
         * 
         * @param callId the call ID in string format
         * @param occCall the call details
         */
        void addCallDriverCallEntry( const std::string& callId,
                                     const CallTypes::OccCallRequest& occCall );

		/** Remove Radio Call Entry
		 *
		 * @param callId the call Id
		 * @return the corresponding entry Id of the call Id
		 */
		unsigned long removeRadioCallEntry(std::string callId);


    protected:

            
        /**
         *  Creates a new RadioCallEntry from the given call details that has
         *  the given call id and adds it to the CallEntryManager.
         */
        void addRadioCallEntry(
            const std::string& callId,
            const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& event);

		/**
		 *  Update existing RadioCallEntry with new call status (eg. Incoming, Missed Call)
		 */
		void updateRadioCallEntry(const std::string &callId, Radio::EPrivateCallState status);

		/**
		 * get status of call entry
		 */
		std::string getCallEntryStatus(std::string callId);


    private:
        RadioCallFactory();
        RadioCallFactory(const RadioCallFactory&);


        /**
         *  Initialises the member variables and reference to the Radio
         *  Agent, and gets the existing messages.
         *
         *  @param locationKey  the location of the Radio Agent.
         */
        void initialise(unsigned long locationKey);


        // The call manager that this factory informs of updates to Radio
        // messages
        TA_IRS_Bus::CallEntryManager& m_callManager;


        // The session key associated with radio calls that are directed
        // to this console.
        long m_radioSessionKey;

        // The location of the banner, eg "OCC"
        std::string m_locationName;


		/**
         * The location key
         */
        unsigned long m_locationKey;


        // Acquired when a message arrives from the notification service,
        // and released when the message has been processed.
        TA_Base_Core::NonReEntrantThreadLockable m_notificationsLock;
		TA_Base_Bus::RightsLibrary* m_rightsLibrary;
		unsigned long m_bannerResourceId;

        unsigned long m_lastCallId;


		/**
         * Used to access the train agent, and also to subscribe for OCC call updates
         */
        TrainDriverCallCorbaProxyPtr m_trainDriverCallCorbaProxy;


        /**
         * Used to access the train agent, and also to subscribe for train information updates
         */
        TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;

		/**
         * Used to make sure that the initial query of existing calls and incoming updates dont conflict
         */
        TA_Base_Core::ReEntrantThreadLockable m_callUpdateLock;

		/**
         * This map holds the train details to all the trains. It updates automatically when the details of the trains change.
         */
        TrainInformationTypes::TrainDetailsMap m_trainDetailsStore;
		/**
		 * Initialize RightsLibraryFactory 
		 */
		TA_Base_Bus::RightsLibraryFactory m_rightsLibraryFactoryObj;

		static const char* LAUNCH_RADIOMANAGER;
		static const char* LAUNCH_NOT_ALLOWED;
    };

}// TA_Bus

#endif // CALL_BANNER_RADIO_CALL_FACTORY_H
