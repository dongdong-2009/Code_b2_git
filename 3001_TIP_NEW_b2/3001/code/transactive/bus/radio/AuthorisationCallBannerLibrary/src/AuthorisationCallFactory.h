/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This factory listens for three Authorisation notifications from the
  * Radio Agent:
  *
  * 1. New Authorisation - A new authorisation request has been received.
  *    An AuthorisationCallEntry is created and added to the CallEntryManager.
  *
  * 2. Accepted Authorisation - An authorisation request has been approved.
  *    If the Authorisation call dialog is open, it is told to close.
  *    The call entry is then removed from the call banner.
  *
  * 3. Rejected Authorisation - An authorisation request has been denied.
  *    If the Authorisation call dialog is open, it is told to close.
  *    The call entry is then removed from the call banner.
  *
  * 4. Cancelled Authorisation - An authorisation request has been cancelled.
  *    If the Authorisation call dialog is open, it is told to close.
  *    The call entry is then removed from the call banner.
  *    If the Authorisation call dialog is not open, the information field
  *    of the call entry in the banner is changed to Cancelled, and the
  *    entry is not deleted until it is activated.
  */


#ifndef CALL_BANNER_AUTHORISATION_CALL_FACTORY_H
#define CALL_BANNER_AUTHORISATION_CALL_FACTORY_H

#include "bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h"
#include "bus/banner_pages/call_page_library/src/CallEntryManager.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"

namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 11-Feb-2004 03:38:33 PM
     */
    class AuthorisationCallFactory :
		public TA_IRS_Bus::AbstractCallEntryFactory,
        public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:

        /**
         *  Constructor.
         *
         *  @param  callManager  the call entry manager that is used to manage
         *                       the memory for this factory's call entries.
         *  @param  locationKey  the location of the radio agent that this
         *                       factory's call entries communicate with.
         *
         *  @exception DataException
         *  @exception DatabaseException
         */
        AuthorisationCallFactory(
			TA_IRS_Bus::CallEntryManager& callManager,
            unsigned long locationKey);


        /**
         *  Destructor
         */
        virtual ~AuthorisationCallFactory();


        /**
         *  Called when an Authorisation notification is received from the
         *  Message subsystem.
         */
        virtual void receiveSpecialisedMessage(
            const TA_Base_Core::CommsMessageCorbaDef& message);


        /**
         *  Gets the given notification as a string.
         */
        static std::string getNotificationAsString(
            const TA_IRS_Bus::RadioServerAgentIDL::recvText& message);


        /**
         * @return a string representation suitable for use in the 'Information' field
         *          of the call banner
         */
        static std::string getCallInformation(
            const TA_IRS_Bus::RadioServerAgentIDL::recvText& message);
        
        /**
         * @return string representation of the source of the specified call
         */
        static std::string getCallSourceAsString(
            const TA_IRS_Bus::RadioServerAgentIDL::recvText& message);

    private:
        AuthorisationCallFactory();
        AuthorisationCallFactory(const AuthorisationCallFactory&);


        /**
         *  Initialises the member variables and reference to the Radio
         *  Agent, and gets the existing messages.
         *
         *  @param locationKey  the location of the Radio Agent.
         */
        void initialise(unsigned long locationKey);


        /**
         *  Deletes a call entry from the banner.
         */
        void deleteCallEntry(unsigned long callEntryId);

        /**
         *  Method to process incoming text message
         */
		void processEvent(const TA_IRS_Bus::RadioServerAgentIDL::recvText& message);

        // The call manager that this factory informs of updates to Authorisation
        // messages
		TA_IRS_Bus::CallEntryManager& m_callManager;

        // The location of the banner, eg "OCC"
        std::string m_locationName;

        // Acquired when a message arrives from the notification service,
        // and released when the message has been processed.
        TA_Base_Core::NonReEntrantThreadLockable m_notificationsLock;
    };

}// TA_IRS_Bus

#endif // CALL_BANNER_AUTHORISATION_CALL_FACTORY_H
