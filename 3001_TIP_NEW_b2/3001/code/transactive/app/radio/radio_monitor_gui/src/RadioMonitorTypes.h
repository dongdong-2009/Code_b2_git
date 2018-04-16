/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorTypes.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Defines some specialised types for use in Radio Monitor application
  *
  */

#pragma once

#include <list>
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioDirectoryCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"

namespace TA_IRS_App
{
    namespace RadioMonitor
    {
        /**
         * Provide our own structure (a reflection of the CORBA structure), since
         *  CORBA structure copying can cause memory problems (the non-const copy constructor
         *  frees memory of the source)
         */            
        /// Provide a shared base class purely to allow dynamic_cast operator on derived 
        struct CallDataBase
        {
            virtual ~CallDataBase() {}
        };          

        struct RmCallParticipant : public CallDataBase
        {
            std::string name;           // Corresponds to 'callerName'
            std::string TSI;            // Corresponds to 'callerTSI'
            std::string location;       // Corresponds to 'callerLocation'

            /// @return true if there is a correspondence with primary identifier properties
            /// (these properties should uniquely identify a call)
            bool keyMatch(const struct RmCallParticipant& cp) const
            {
                return name == cp.name &&
                        TSI == cp.TSI &&
                        location == cp.location &&
                        callID == cp.callID;
            }

            // Store to allow for unique identification
            // callID is same as parent nodes 'callID' parameter            
            long callID;
        }; 

        struct RmMonitorUpdate : public CallDataBase
        {    
            long callID;
        	long		    sessionRef;
        	long		    monitorID;
        	std::string		callTime;
        	std::string		callerName;
        	std::string		callerTSI;
        	std::string		callerLocation;
        	std::string		calledName;
        	std::string		calledTSI;
        	std::string		calledLocation;
        	TA_Base_Bus::IRadioCorbaDef::EMonitorStatusType	callStatus;
			TA_Base_Bus::IRadioCorbaDef::ECallCategory		callCategory;

            /// @return true if there is a correspondence with primary identifier properties
            /// (these properties should uniquely identify a call)
            bool keyMatch(const struct RmMonitorUpdate& rm) const
            {
                return callID == rm.callID && 
                        callerTSI == rm.callerTSI &&
                        calledTSI == rm.calledTSI;
            }
        	
        	std::list<RmCallParticipant> participants;        	
        };       
        
        struct SelectedEntity
        {
            std::string id;
            std::string location;
            std::string tsi;
        };

        typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
	   					     TA_Base_Bus::IRadioCorbaDef_ptr, 
	           			     TA_Base_Bus::IRadioCorbaDef_var> RadioAgentNamedObject;
        typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
		                        TA_Base_Bus::IRadioCorbaDef_ptr,
		                        TA_Base_Bus::IRadioCorbaDef_var> IRadio;
        typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioDirectoryCorbaDef,
		                        TA_Base_Bus::IRadioDirectoryCorbaDef_ptr,
		                        TA_Base_Bus::IRadioDirectoryCorbaDef_var> IRadioDirectory;
        typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioSessionCorbaDef,
		                        TA_Base_Bus::IRadioSessionCorbaDef_ptr,
		                        TA_Base_Bus::IRadioSessionCorbaDef_var> IRadioSession;      

    }   // end namespace RadioMonitor

}   // end namespace TA_IRS_App
    
