/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/signs/RATISCallBannerLibrary/src/RATISCallFactory.h $
  * @author:  Rob Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This factory listens for incoming RATIS message notifications from the
  * TIS Agent.
  *
  *
  */


#ifndef CALL_BANNER_RATIS_CALL_FACTORY_H
#define CALL_BANNER_RATIS_CALL_FACTORY_H

#include "bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h"
#include "bus/banner_pages/call_page_library/src/CallEntryManager.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/naming/src/NamedObject.h"


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 11-Feb-2004 03:38:33 PM
     */
    class RATISCallFactory :
		public TA_IRS_Bus::AbstractCallEntryFactory,
        public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
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
         *  @param  tisSessionKey  the tis session used to distinguish
         *                           incoming messages that are for this console.
         *
         *  @exception DataException
         *  @exception DatabaseException
         */
        RATISCallFactory(
			TA_IRS_Bus::CallEntryManager& callManager,
            unsigned long locationKey,
            long tisSessionKey );


        /**
         *  Destructor
         */
        virtual ~RATISCallFactory();


        /**
         *  Called when an incoming RATIS message notification is received from the
         *  Message subsystem.
         */
        virtual void receiveSpecialisedMessage(
            const TA_Base_Core::CommsMessageCorbaDef& message);


            
        /**
         *  Gets the given notification as a string.
         */
        static std::string getNotificationAsString(
            const TA_Base_Bus::ISTISManagerCorbaDef::IncomingRATISEvent& notification);


        /**
         *  Gets the given RATIS message status as a string.
         */
        static std::string getStatusAsString(
            TA_Base_Core::ERATISMessageStatus status);

        /**
         *  Gets the given RATIS message type as a string.
         */
        static std::string getTypeAsString(
            TA_Base_Core::ERATISMessageType status);


        /**
         * @return a string representation suitable for use in the 'Information' field
         *          of the call banner
         * @param radioAgent required to query agent for text message information (for
         *          those calls that have category 'TextMessage')
         */
        static std::string getMessageInformation(
            const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& call);


        /**
         *  Deletes a call entry from the banner.
         */
        void deleteCallEntry(unsigned long callEntryId);


        /** 
          * getPidName
          *
          * Returns the PID name for the given address on the specified station
          *
          * @param station
          * @param address
          *
          * @return the PID name
          *
          */
//        std::string getPidName(std::string& station, std::string& address);


    protected:


        /**
         *  Creates a new RATISCallEntry and adds it to the
         *  CallEntryManager, if it does not already exist. Updates it if it it.
         */
        void processMessageNotification(TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details);
           
        /**
         *  Creates a new RATISCallEntry from the given call details that has
         *  the given call id and adds it to the CallEntryManager.
         */
        void addRATISCallEntry(
            const std::string& callId,
            const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& details);


    private:
        RATISCallFactory();
        RATISCallFactory(const RATISCallFactory&);


        /**
         *  Initialises the member variables and reference to the Radio
         *  Agent, and gets the existing messages.
         *
         *  @param locationKey  the location of the TIS Agent.
         */
        void initialise(unsigned long locationKey);

        /**
         *  Gets all RATIS messages that already exist at the TIS Agent.
         */
        void getExistingMessages();

        /** 
          * buildPIDMap
          *
          * Build the map of station to pid map.
          *
          */
//        void buildPIDMap();



        // The call manager that this factory informs of updates to Radio
        // messages
		TA_IRS_Bus::CallEntryManager& m_callManager;

        // The session key associated with radio calls that are directed
        // to this console.
        long m_tisSessionKey;

        // The location of the banner, eg "OCC"
        std::string m_locationName;

        // Acquired when a message arrives from the notification service,
        // and released when the message has been processed.
        TA_Base_Core::NonReEntrantThreadLockable m_notificationsLock;

        // This maps the address of a pid to its name
        typedef std::map<std::string, std::string> PIDAddressNameMap;

        // This maps a station to a pid address-name map.
        typedef std::map<std::string, PIDAddressNameMap> StationPIDMap;

        StationPIDMap m_stationPidMap;

    };

}// TA_IRS_Bus

#endif // CALL_BANNER_RATIS_CALL_FACTORY_H
