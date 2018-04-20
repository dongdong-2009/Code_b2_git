#ifndef RATIS_MESSAGE_TABLE_H_INCLUDED
#define RATIS_MESSAGE_TABLE_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/RATISMessageTable.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Contains the details for the TI_RATIS_MESSAGE table.
  * This table facilitates the storage of incoming RATIS messages
  */

#include <string>

namespace TA_IRS_Core
{
    namespace TA_TISAgentDAI
    {

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  RATIS Message Table
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////
         const std::string RATIS_MESSAGE_TABLE_NAME			  = "ti_ratis_message";
		 const std::string RATIS_MESSAGE_PKEY_SEQUENCE		  = "tirmes_seq.nextval";

		 const std::string RATIS_MESSAGE_PKEY_COLUMN		  = "tirmes_id";
		 const std::string SESSION_REF_COLUMN				  = "session_ref";
		 const std::string START_TIME_COLUMN				 = "start_time";
		 const std::string END_TIME_COLUMN				 = "end_time";
		 const std::string REQUIRES_VETTING_COLUMN		  = "requires_vetting";
		 const std::string OVERRIDABLE_COLUMN			  = "overridable";
		 const std::string TYPE_COLUMN					= "type";
		 const std::string STATUS_COLUMN				  = "status";
		 const std::string CREATION_DATE_COLUMN			= "creation_date";
		 const std::string MESSAGE_TEXT_COLUMN			= "message_text";
		 const std::string PRIORITY_COLUMN				= "priority";
		 const std::string TAG_COLUMN				  = "tag";
		 const std::string DESTINATION_COLUMN				  = "destination";
    }
}

#endif 
