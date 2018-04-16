/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.cpp $
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


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER



#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationDialog.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/utilities/src/RunParams.h"


using TA_Base_Core::RunParams;

namespace TA_IRS_Bus
{

    // TES #982
    const std::string AuthorisationCallEntry::CALL_ENTRY_TYPE = "SDS";


    AuthorisationCallEntry::AuthorisationCallEntry(
        unsigned long id,
		const TA_IRS_Bus::RadioServerAgentIDL::recvText& message,
        AuthorisationCallFactory& factory)
        :
        AbstractCallEntry(
            id,
            AuthorisationCallFactory::getCallSourceAsString(message),
            AuthorisationCallFactory::getCallInformation(message)
            ),

        m_message(message),
        m_authorisationCallFactory(factory)
    {
    }


    AuthorisationCallEntry::~AuthorisationCallEntry()
    {
    }


    std::string AuthorisationCallEntry::getType() const
    {
        return CALL_ENTRY_TYPE;
    }

    
    void AuthorisationCallEntry::onActivate()
    {
        FUNCTION_ENTRY("onActivate");


        std::ostringstream runParams;
        runParams << "--SDS=" << 0;


        std::string message = "Launching Radio Manager with run params '";
        message            += runParams.str();
        message            += "'";
        
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, message.c_str());

        //
        // Launch the Radio Manager with the call id as a parameter.
        //
        using namespace TA_Base_Bus::TA_GenericGui;
        CPoint pt(GetMessagePos());
        RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect(SCREEN_CURRENT,
                                                                   AREA_SCHEMATIC,
                                                                   pt.x);
        try
        {
            CWaitCursor wait;

            TA_Base_Bus::AppLauncher::getInstance().launchApplication(RADIO_CONTROLLER_GUI_APPTYPE,
                                                             runParams.str(),
                                                             POS_BOUNDED,
                                                             ALIGN_FIT,
                                                             NULL,
                                                             &position);
        }
        catch(const TA_Base_Core::ApplicationException& ae)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "ApplicationException (Radio Manager)", ae.what());
            AfxMessageBox("Error launching Radio Manager");
        }
        catch (...)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown (Radio Manager)", "Launching Radio Manager");
            AfxMessageBox("Unknown error launching Radio Manager");
        }
		
        FUNCTION_EXIT;
    }



    std::string AuthorisationCallEntry::getLocationStr() const
    {
        return "";
    }


    std::string AuthorisationCallEntry::getStatusStr() const
    {
        return "New";
    }


    time_t AuthorisationCallEntry::getCreateTime() const
    {
        return m_message.messageData.time;
    }

}// TA_IRS_Bus
