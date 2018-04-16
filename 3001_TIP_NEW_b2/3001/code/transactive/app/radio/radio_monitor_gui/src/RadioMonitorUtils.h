/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorUtils.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Provides various helper functions for specific use in Radio Monitor application
  * 
  */

#pragma once

namespace TA_IRS_App
{
    namespace RadioMonitorUtils
    {                
        typedef TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType CorbaMonitorUpdate;

        /**
         * convertCorbaStruct
         *
         * Converts a CORBA structure into a client defined structure
         *
         * @return the converted (new format) data structure
         *
         * @param data the input CORBA structure
         *
         */
        RadioMonitor::RmMonitorUpdate convertCorbaStruct(const CorbaMonitorUpdate& data);

        /**
         * getCallCategoryString
         *
         * @param category input category
         *
         * @return string representation of the input category, suitable for display to user
         * 
         */
        std::string getCallCategoryString(TA_Base_Bus::IRadioCorbaDef::ECallCategory category);

    }   // end namespace RadioMonitorUtils

} // end namespace TA_IRS_App
