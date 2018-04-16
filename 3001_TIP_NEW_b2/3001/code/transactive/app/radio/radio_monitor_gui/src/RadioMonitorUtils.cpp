/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorUtils.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Provides various helper functions for specific use in Radio Monitor application
  * 
  */

#include "app/radio/radio_monitor_gui/src/stdafx.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorUtils.h"

namespace TA_IRS_App
{
    using namespace TA_IRS_App::RadioMonitor;

    namespace RadioMonitorUtils
    {        
        RmMonitorUpdate convertCorbaStruct(const CorbaMonitorUpdate& data)
        {
            RmMonitorUpdate result;

            result.callID           = data.callID;
            result.sessionRef       = data.sessionRef;
            result.monitorID        = data.monitorID;
                                
            result.callTime         = data.callTime.in();
        	result.callerName       = data.callerName.in();
        	result.callerTSI        = data.callerTSI.in();
        	result.callerLocation   = data.callerLocation.in();
        	result.calledName       = data.calledName.in();
        	result.calledTSI        = data.calledTSI.in();
        	result.calledLocation   = data.calledLocation.in();
        	result.callStatus       = data.callStatus;
			result.callCategory     = data.callCategory;

            for (DWORD i = 0; i < data.participants.length(); i ++)
            {
                RmCallParticipant p;
                p.name          = data.participants[i].name.in();
                p.TSI           = data.participants[i].TSI.in();
                p.location      = data.participants[i].location.in();

                // Store some extra parameters of our own
                p.callID        = data.callID;
                result.participants.push_back(p);
            }        

            return result;
        }


        std::string getCallCategoryString(TA_Base_Bus::IRadioCorbaDef::ECallCategory category)
        {
            switch (category)
            {
            case TA_Base_Bus::IRadioCorbaDef::HalfDuplex:
                return "CONFERENCE";
            case TA_Base_Bus::IRadioCorbaDef::FullDuplex:
                return "SINGLE";
            case TA_Base_Bus::IRadioCorbaDef::TextMessage:
                return "TEXT MESSAGE";
            case TA_Base_Bus::IRadioCorbaDef::CallDriver:
                return "CALL DRIVER";
            default:
                static bool reportedOnceOff = false;
                if (!reportedOnceOff)
                {
                    reportedOnceOff = true;
                    RadioMonitorErrorHandler::getInstance().displayError(
                            RadioMonitorErrorHandler::INTERNAL_FAILURE);                                
                }
                return "Unknown";
            }

        }
		enum ECallCategory
		{
			HalfDuplex,
			FullDuplex,
			TextMessage,
			CallDriver
		};

    }   // end namespace RadioMonitorUtils

} // end namespace TA_IRS_App
