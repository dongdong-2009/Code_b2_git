/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2008/11/28 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(RadioCallFactory_H)
#define RadioCallFactory_H


#include "bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainDriverCallUpdateObserver.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/naming/src/NamedObject.h"


// forward declarations
namespace TA_Base_Bus
{
    class RightsLibrary;
}


namespace TA_IRS_Bus
{
    class CallEntryManager;
}


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 11-Feb-2004 03:38:33 PM
     * @author oliverk
     * @version 1.0
     */

    class RadioCallFactory :
                public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
                public TA_IRS_Bus::AbstractCallEntryFactory,
                public ITrainDriverCallUpdateObserver,
                public ITrainInformationUpdateObserver
    {

    public:

        /**
         * Used to access the radio agent
         * @author oliverk
         * @version 1.0
         * @created 30-Jan-2008 2:38:25 PM
         */
        typedef TA_Base_Core::NamedObject < TA_Base_Bus::IRadioCorbaDef,
                                            TA_Base_Bus::IRadioCorbaDef_ptr,
                                            TA_Base_Bus::IRadioCorbaDef_var > RadioAgentNamedObject;


        /**
         * Creates the radio call factory
         *
         * @param callManager the call manager for the call banner page
         * @param locationKey the local location key
         */
        RadioCallFactory( TA_IRS_Bus::CallEntryManager& callManager, unsigned long locationKey );


        /**
         * Destructor
         */
        virtual ~RadioCallFactory();


        /**
         * Gets a reference to the radio agent named object
         *
         * @return the radio agent remote object reference
         */
        RadioAgentNamedObject& getRadioAgent();
        

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
         * Called when a Radio notification is received from the Message subsystem.
         *
         * @param message
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );


    private:


        /**
         * Private Constructor
         */
        RadioCallFactory();
        RadioCallFactory( const RadioCallFactory& prm1 );


        /**
         * Will load all configuration, create agent interfaces, and subscribe for updates.
         */
        void initialise();


        /**
         * Creates the train agent proxy objects
         */
        void createProxies();


        /**
         * loads the radio and train calls from the agents
         */
        void createExistingCalls();


        /**
         * This checks if the specified action can be performed on the banner by the current session.
         *
         * @param action The action to test
         *
         * @return true if the action can be performed, false otherwise
         */
        bool canPerformAction( unsigned long action );


        /**
         * Called when a Ringing type notification is received from the Radio Agent.
         * Creates a new RadioCallEntry and adds it to the CallEntryManager.
         *
         * @param callId the new call ID
         */
        void receiveRingingNotification( CORBA::Long callId );


        /**
         * Called when any notification type except Ringing is received from the Radio Agent.
         * Deletes the corresponding RadioCallEntry.
         *
         * @param callId The call ID to remove
         * @param callCategory The call category
         */
        void receiveNonRingingNotification( CORBA::Long callId,
                                            TA_Base_Bus::IRadioCorbaDef::ECallCategory callCategory );

        
         /**
          * Creates a new RadioCallEntry from the given call details that has
          * the given call id and adds it to the CallEntryManager.
          *
          * @param callId The new call ID
          * @param details The call details
          */
        void addRadioCallEntry( const std::string& callId,
                                const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details );


        /**
         * Updates an existing RadioCallEntry from the given call details that has
         * the given call id.
         *
         * @param callId The call ID
         * @param details The updated details
         */
        void updateRadioCallEntry( const std::string& callId, const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& details );


        /**
         * Adds a new call driver entry to the banner
         * 
         * @param callId the call ID in string format
         * @param occCall the call details
         */
        void addCallDriverCallEntry( const std::string& callId,
                                     const CallTypes::OccCallRequest& occCall );


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


    private:

        /**
         * The banner resource ID, used to check rights
         */
        unsigned long m_bannerResourceId;


        /**
         * The call manager that this factory informs of updates to Radio messages
         */
        TA_IRS_Bus::CallEntryManager& m_callManager;


        /**
         * The Id of this console - doesn't change throughout the life of the banner
         */
        unsigned long m_consoleId;


        /**
         * The location key
         */
        unsigned long m_locationKey;


        /**
         * A reference to the radio agent CORBA interface
         */
        RadioAgentNamedObject m_radioAgent;


        /**
         * Used to check rights
         */
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;


        /**
         * Used to make sure that the initial query of existing calls and incoming updates dont conflict
         */
        TA_Base_Core::ReEntrantThreadLockable m_callUpdateLock;


        /**
         * This map holds the train details to all the trains. It updates automatically when the details of the trains change.
         */
        TrainInformationTypes::TrainDetailsMap m_trainDetailsStore;


        /**
         * Used to protect m_trainDetailsStore
         */
        TA_Base_Core::NonReEntrantThreadLockable m_trainDetalsLock;


        /**
         * Used to access the train agent, and also to subscribe for OCC call updates
         */
        TrainDriverCallCorbaProxyPtr m_trainDriverCallCorbaProxy;


        /**
         * Used to access the train agent, and also to subscribe for train information updates
         */
        TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;

    };

}

#endif // !defined(RadioCallFactory_H)

