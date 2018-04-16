/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * This class extends AbstractCallEntry to provide the implementation of
  * a radio call entry.
  * When activated, it opens the radio manager with the corresponding
  * communication selected.
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/exceptions/src/ApplicationException.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_IRS_Bus
{
    const std::string RadioCallEntry::CALL_ENTRY_TYPE = "Radio";

    RadioCallEntry::RadioCallEntry(
        unsigned long id,
        const TA_IRS_Bus::RadioMFTAgentCallbackIDL::IncomingCallEvent& details)
        :
        AbstractCallEntry(
            id,
            RadioCallFactory::getCallSourceAsString(details),
            RadioCallFactory::getCallInformation(details) ),

        m_details(details),
		m_status(details.status)
    {
    }


    RadioCallEntry::~RadioCallEntry()
    {
    }


    std::string RadioCallEntry::getType() const
    {
        return CALL_ENTRY_TYPE;
    }

    
    void RadioCallEntry::onActivate()
    {
        FUNCTION_ENTRY("onActivate");

        std::ostringstream runParams;
		std::string mode = "";
		if ( TA_IRS_Bus::Radio::PC_NOT_INVOLVED == m_status ) 
		{
			if (!m_details.isEmergencyMode)
				mode = "4";		// radio make private call when missed call
			else
				mode = "1";		// begin console emergency when missed call
		}
		else if ( m_details.isEmergencyMode ) 
		{
			mode = "1";
		} 
		else 
		{
			mode = "0";
		}
        runParams << "--ResourceSelection=" << "ID-" <<m_details.caller.id <<","
			<< "Type-" << mode.c_str();//m_details.isEmergencyMode ? 1 : 0; 


        std::string message = "Launching Radio Manager with run params '";
        message            += runParams.str();
        message            += "'";
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, message.c_str());

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


    std::string RadioCallEntry::getLocationStr() const
    {
        // source location
        return "";
    }


    std::string RadioCallEntry::getStatusStr() const
    {
		std::string status = "";
		if (TA_IRS_Bus::Radio::PC_NOT_INVOLVED == m_status)
			status = "Missed Call";
		else 
			status = "Incoming";
		return status;
    }

    time_t RadioCallEntry::getCreateTime() const
    {
        return m_details.timeOfCall;
    }

    
    bool RadioCallEntry::isEmergency() const
    {
        return m_details.isEmergencyMode;
    }

    
    int RadioCallEntry::getSeverity() const
    {
        if (isEmergency())
        {
            return 2;
        }
        else
        {
            return 1;
        }
    }

	void RadioCallEntry::updateStatus(Radio::EPrivateCallState status)
	{
		m_status = status;
	}

}// TA_Bus
