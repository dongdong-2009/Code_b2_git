/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * This class extends AbstractCallEntry to provide the implementation of
  * an authorisation call entry. It responds to the messages received by the
  * AuthorisationCallFactory (see the comments for that class for more details)
  * and implements the onActivate method.
  * When activated, the a dialog is displayed with information about the
  * authorisation request.
  * When an accept, reject or cancel authorisation notification arrives, the
  * user is informed and the dialog is closed (if open).
  */


#ifndef CALL_BANNER_AUTHORISATION_CALL_ENTRY_H
#define CALL_BANNER_AUTHORISATION_CALL_ENTRY_H

#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h"
#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 11-Feb-2004 03:38:50 PM
     */
    class AuthorisationCallEntry : public TA_IRS_Bus::AbstractCallEntry 
    {
    
    public:
        
        // The type of call entry this is, ie Authorisation
        static const std::string CALL_ENTRY_TYPE;

        
        /**
         *  Creates a new call entry.
         *
         *  @param id          the id of this Authorisation call entry.
         *  @param request     the details of the notification that caused
         *                     this entry to be created.
         *  @param radioAgent  a reference to the Radio Agent CORBA interface.
         *  @param factory     the call factory that created this object.
         */
        AuthorisationCallEntry(
            unsigned long id,
         	const TA_IRS_Bus::RadioServerAgentIDL::recvText& message,
            AuthorisationCallFactory& factory);


        /**
         *  The destructor.
         */
        virtual ~AuthorisationCallEntry();


        /**
         *  Gets the type of call entry that this object represents, as a
         *  string.
         */
        virtual std::string getType() const;


/*        
        virtual void acceptNotificationReceived();
        virtual void rejectNotificationReceived();
        virtual bool cancelNotificationReceived();
        virtual std::string getDestination();
        virtual std::string getAuthorisationType();
        virtual void dialogAcceptButtonPressed();
        virtual void dialogRejectButtonPressed();
*/

        /**
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
        virtual void onActivate();


        /**
         * Pure virtual implementations (see AbstractCallEntry for more details)
         */
        virtual std::string getLocationStr() const;
        virtual std::string getStatusStr() const;
        virtual time_t getCreateTime() const;
        // Authorisation have a constant severity of 1
        virtual int getSeverity() const { return 1; }    
        virtual std::string getCategory() const { return CALL_ENTRY_TYPE; }

/*    protected:
        
      virtual std::string getAuthorisationType(
             const TA_Base_Bus::IRadioCorbaDef::CallAuthorisationRequest& request);
*/
    private:
        AuthorisationCallEntry();
        AuthorisationCallEntry(const AuthorisationCallEntry& theAuthorisationCallEntry);


		TA_IRS_Bus::RadioServerAgentIDL::recvText m_message;

        // A reference to the call factory that created this call entry.
        AuthorisationCallFactory& m_authorisationCallFactory;

    };

}// TA_IRS_Bus

#endif // CALL_BANNER_AUTHORISATION_CALL_ENTRY_H
