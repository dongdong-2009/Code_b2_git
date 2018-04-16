/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h"


namespace TA_IRS_Bus
{
    // Forward declarations
    class AuthorisationDialog;



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
            const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request,
            AuthorisationCallFactory::RadioSessionNamedObject& radioSession,
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


        /**
         *  Called when a notification is received from the Radio Agent that
         *  the authorisation has been accepted.
         *  If the dialog is open the client is informed and the dialog closed.
         */
        virtual void acceptNotificationReceived();

        /**
         *  Called when a notification is received from the Radio Agent that
         *  the authorisation has been rejected.
         *  If the dialog is open the client is informed and the dialog closed.
         */
        virtual void rejectNotificationReceived();

        /**
         *  Called when a notification is received from the Radio Agent that
         *  the authorisation has been cancelled.
         *  1. If the dialog is open the client is informed and the dialog is
         *     closed and deleted.
         *  2. If the dialog is not open, the call entry's state is set to
         *     cancelled and the next time it is activated it is deleted.
         *
         *  @return bool  true if the dialog should be deleted (case 1 above)
         *                false if the dialog should not be deleted (case 2)
         */
        virtual bool cancelNotificationReceived();


        /**
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
        virtual void onActivate();


        /**
         *  The field displayed in the Destination text box of the dialog
         *  associated with this class.
         */
        virtual std::string getDestination();

        /**
         *  The field displayed in the Type text box of the dialog
         *  associated with this class.
         */
        virtual std::string getAuthorisationType();


        /**
         *  Called by the dialog when the accept button is pressed.
         */
        virtual void dialogAcceptButtonPressed();


        /**
         *  Called by the dialog when the reject button is pressed.
         */
        virtual void dialogRejectButtonPressed();

        /**
         * Pure virtual implementations (see AbstractCallEntry for more details)
         */
        virtual std::string getLocationStr() const;
        virtual std::string getStatusStr() const;
        virtual time_t getCreateTime() const;
        // Authorisation have a constant severity of 1
        virtual int getSeverity() const { return 4; }    
        virtual std::string getCategory() const { return CALL_ENTRY_TYPE; }

    protected:
        
        /**
         *  Shows a message box indicating that an error has occurred.
         *  The dialog box must exist before this method is called or it
         *  will assert.
         */
        void showErrorMessage(const std::string& errorMessage);
        
        /*
         *  Gets the authorisation type from the source and destination types.
         *  eg "Operator to Station"
         */
        virtual std::string getAuthorisationType(
             const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request);

    private:
        AuthorisationCallEntry();
        AuthorisationCallEntry(const AuthorisationCallEntry& theAuthorisationCallEntry);


        // The dialog that is opened when the onActivate method is called.
        // There is only one copy for all authorisation call entries.
        static AuthorisationDialog* s_dialog;

        // The details of the authorisation request
        TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest m_request;

        // A reference to the radio agent corba interface
        AuthorisationCallFactory::RadioSessionNamedObject & m_radioSession;

        // A reference to the call factory that created this call entry.
        AuthorisationCallFactory& m_authorisationCallFactory;

        // Indicates whether this authorisation has been cancelled.
        // When a cancelled authorisation is activated, it is deleted.
        bool m_cancelled;
    };

}// TA_IRS_Bus

#endif // CALL_BANNER_AUTHORISATION_CALL_ENTRY_H
