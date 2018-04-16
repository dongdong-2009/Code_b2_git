/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
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


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "core/utilities/src/RunParams.h"

#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"
//#include "bus/radio/AuthorisationCallBannerLibrary/src/stdafx.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationDialog.h"


using TA_Base_Core::RunParams;

namespace TA_IRS_Bus
{
    AuthorisationDialog* AuthorisationCallEntry::s_dialog = NULL;

    // TES #982
    const std::string AuthorisationCallEntry::CALL_ENTRY_TYPE = "Auth";


    AuthorisationCallEntry::AuthorisationCallEntry(
        unsigned long id,
        const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request,
        AuthorisationCallFactory::RadioSessionNamedObject& radioSession,
        AuthorisationCallFactory& factory)
        :
        AbstractCallEntry(
            id,
            AuthorisationCallFactory::getCallSourceAsString(request),
            AuthorisationCallFactory::getCallInformation(request)
            ),

        m_request(request),
        m_radioSession(radioSession),
        m_authorisationCallFactory(factory),
        m_cancelled(false)
    {
    }


    AuthorisationCallEntry::~AuthorisationCallEntry()
    {
    }


    std::string AuthorisationCallEntry::getType() const
    {
        return CALL_ENTRY_TYPE;
    }


    std::string AuthorisationCallEntry::getDestination()
    {
        // TES #457: Destination should show name and TSI
        std::ostringstream ostr;
        ostr << m_request.calledName.in() << " (" 
             << m_request.calledTSI.in() << ")";

        return ostr.str();
    }


    std::string AuthorisationCallEntry::getAuthorisationType()
    {
        return getAuthorisationType(m_request);
    }


    std::string AuthorisationCallEntry::getAuthorisationType(
        const TA_Base_Bus::IRadioSessionCorbaDef::CallAuthorisationRequest& request)
    {
        std::ostringstream type;

        std::string sourceType =
            AuthorisationCallFactory::getRadioUserTypeAsString(
                request.sourceType);

        std::string destType =
            AuthorisationCallFactory::getRadioUserTypeAsString(
                request.sourceType);
        
        type << sourceType << " to " << destType;

        return type.str();        
    }


    void AuthorisationCallEntry::acceptNotificationReceived()
    {
        FUNCTION_ENTRY("acceptNotificationReceived");

        //
        // If the dialog is displaying the current call entry,
        // inform the user that the authorisation request has
        // been accepted and close the dialog.
        // Otherwise just ignore the notification (because the
        // user has already closed the dialog)
        //
        if( s_dialog!=NULL && s_dialog->IsWindowVisible() &&
            s_dialog->getCallEntry().getId() == getId() )
        {
            // UW-XXXX
            s_dialog->showInformationMessage(
                "The authorisation request has been accepted");

            s_dialog->ShowWindow(SW_HIDE);
        }

        FUNCTION_EXIT;
    }


    void AuthorisationCallEntry::rejectNotificationReceived()
    {
        FUNCTION_ENTRY("rejectNotificationReceived");
        
        //
        // If the dialog is displaying the current call entry,
        // inform the user that the authorisation request has
        // been rejected and close the dialog.
        // Otherwise just ignore the notification (because the
        // user has already closed the dialog)
        //
        if( s_dialog!=NULL && s_dialog->IsWindowVisible() &&
            s_dialog->getCallEntry().getId() == getId() )
        {
            // UW-XXXX
            s_dialog->showInformationMessage(
                "The authorisation request has been rejected");

            s_dialog->ShowWindow(SW_HIDE);
        }

        FUNCTION_EXIT;
    }


    bool AuthorisationCallEntry::cancelNotificationReceived()
    {
        FUNCTION_ENTRY("cancelNotificationReceived");
        
        //
        // Flag the authorisation request as having been cancelled
        //
        m_cancelled = true;

        //
        // If the dialog is displaying the current call entry,
        // inform the user that the authorisation request has
        // been cancelled and close the dialog.
        // Otherwise just ignore the notification (because the
        // user has already closed the dialog)
        //
        if( s_dialog!=NULL && s_dialog->IsWindowVisible() &&
            s_dialog->getCallEntry().getId() == getId() )
        {
            // UW-XXXX
            s_dialog->showInformationMessage(
                "The authorisation request has been cancelled");

            s_dialog->ShowWindow(SW_HIDE);

            //
            // If the dialog window is open, the call entry should be deleted
            // as soon as this method returns.
            //
            FUNCTION_EXIT;
            return true;
        }

        //
        // If the dialog window is not open, the call entry should not be
        // deleted until the onActivate method is called. It's information
        // field is updated however.
        //
        std::string action =
            AuthorisationCallFactory::getActionAsString(
                TA_Base_Bus::IRadioSessionCorbaDef::CANCELLED_AUTHORISATION);
        setInformation(action);

        FUNCTION_EXIT;
        return false;
    }

    
    void AuthorisationCallEntry::onActivate()
    {
        FUNCTION_ENTRY("onActivate");


        //
        // If this authorisation request was cancelled previously, inform the
        // factory that it has now been activated (cancelled calls are deleted
        // when activated). Note that this means that the method must return
        // straight after this call, otherwise undefined behaviour will result.
        //
        if(m_cancelled)
        {
            m_authorisationCallFactory.cancelledEntryActivated( getId() );
            // The above call results in the object being deleted, so the
            // method must return straight away to avoid undefined behaviour.
            return;
        }


        //
        // If the AuthorisationDialog does not yet exist, create it with
        // the details of this call entry.
        //
        if(s_dialog == NULL)
        {
            s_dialog = new AuthorisationDialog(this);
        }
        //
        // If the dialog is already open, don't overwrite it
        //
        else if( s_dialog->IsWindowVisible() )
        {
            return;
        }
        //
        // If the dialog is not open, set the call entry being
        // displayed to this object.
        //
        else
        {
            s_dialog->setCallEntry(this);
        }


        //
        // Show the dialog
        //
        s_dialog->ShowWindow(SW_SHOW);


        
        FUNCTION_EXIT;
    }


    void AuthorisationCallEntry::dialogAcceptButtonPressed()
    {
        FUNCTION_ENTRY("dialogAcceptButtonPressed");

        try
        {
            CWaitCursor wait;

            //
            // Tell the radio agent to accept the authorisation request
            //
            std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
            CORBA_CALL( m_radioSession, authoriseCall, ( m_request.callID, true, sessionId.c_str() ) );//limin
            m_authorisationCallFactory.callEntryAccepted( getId() );
        }
        catch(...)
        {
            // UE ????
            showErrorMessage("Failed to accept authorisation request");
        }

        //
        // Close the dialog
        //
        s_dialog->ShowWindow(SW_HIDE);


        FUNCTION_EXIT;
    }


    void AuthorisationCallEntry::dialogRejectButtonPressed()
    {
        FUNCTION_ENTRY("dialogRejectButtonPressed");

        try
        {
            //
            // Tell the radio agent to reject the authorisation request
            //
            std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
            CORBA_CALL( m_radioSession, authoriseCall, ( m_request.callID, false, sessionId.c_str() ) );//limin
            m_authorisationCallFactory.callEntryRejected( getId() );
        }
        catch(...)
        {
            // UE ????
            showErrorMessage("Failed to reject authorisation request");
        }

        //
        // Close the dialog
        //
        s_dialog->ShowWindow(SW_HIDE);


        FUNCTION_EXIT;
    }



    void AuthorisationCallEntry::showErrorMessage(
        const std::string& errorMessage)
    {
        TA_ASSERT(
            s_dialog != NULL,
            "Cannot display error message when dialog is NULL");


        s_dialog->showErrorMessage(errorMessage);
    }


    std::string AuthorisationCallEntry::getLocationStr() const
    {
        return m_request.callerLocation.in();
    }


    std::string AuthorisationCallEntry::getStatusStr() const
    {
        // Once call authorisations are accepted, rejected or cancelled, they
        // are automatically removed from the list (so the only status is "New")
        return "New";
    }


    time_t AuthorisationCallEntry::getCreateTime() const
    {
        return m_request.time;
    }

}// TA_IRS_Bus
