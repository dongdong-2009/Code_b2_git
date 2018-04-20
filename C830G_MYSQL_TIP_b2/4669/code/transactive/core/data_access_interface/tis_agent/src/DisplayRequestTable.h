#ifndef DISPLAY_REQUEST_TABLE_H_INCLUDED
#define DISPLAY_REQUEST_TABLE_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/DisplayRequestTable.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Contains the details for the DisplayRequest table.
  * This facilitates the storage of display requests sent by the TTIS and STIS Managers
  */

#include <string>

namespace TA_IRS_Core
{
    namespace TA_TISAgentDAI
    {

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//  Predefined Message Library Table
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////
         const std::string DISPLAY_REQUEST_TABLE_NAME			  = "ti_display_request";
		 const std::string DISPLAY_REQUEST_PKEY_SEQUENCE		  = "tidreq_seq.nextval";

		 const std::string DISPLAY_REQUEST_PKEY_COLUMN		  = "tidreq_id";
		 const std::string TIMESTAMP_COLUMN				  = "timestamp";
		 const std::string DESTINATION_SYSTEM_COLUMN		  = "destination_system";
		 const std::string SUBMITTED_BY_COLUMN			  = "submitted_by";
		 const std::string MESSAGE_TYPE_COLUMN			  = "message_type";
		 const std::string MESSAGE_TAG_COLUMN				  = "message_tag";
		 const std::string LIBRARY_SECTION_COLUMN			= "library_section";
		 const std::string LIBRARY_VERSION_COLUMN			= "library_version";
		 const std::string AD_HOC_MESSAGE_CONTENT_COLUMN	  = "ad_hoc_message_content";
		 const std::string START_TIME_COLUMN				  = "start_time";
		 const std::string END_TIME_COLUMN				  = "end_time";
		 const std::string CONTINUOUS_COLUMN				  = "continuous";
		 const std::string PRIORITY_COLUMN				  = "priority";
		 const std::string RATIS_TAG_COLUMN			      = "ratis_tag";
		 const std::string RATIS_DESTINATION_COLUMN			      = "ratis_destination";
		 const std::string RATIS_OVERRIDABLE_COLUMN			      = "ratis_overridable";

		 		 // Destination columns
		 const std::string STIS_DESTINATION_STATION_COLUMN		  = "stis_destination_station";
		 const std::string STIS_DESTINATION_LEVELS_COLUMN		  = "stis_destination_levels";
		 const std::string STIS_DESTINATION_PIDS_COLUMN			  = "stis_destination_pids";
		 const std::string TTIS_DESTINATION_TRAIN_ID_COLUMN			  = "ttis_destination_train_id";
		 const std::string TTIS_DESTINATION_PIDS_COLUMN			  = "ttis_destination_pids";

		 // Display attributes
		 const std::string DISPLAY_MODE_COLUMN			  = "display_mode";
		 const std::string SCROLL_SPEED_COLUMN			  = "scroll_speed";
		 const std::string REPEAT_INTERVAL_COLUMN		  = "repeat_interval";
		 const std::string DISPLAY_TIME_COLUMN			  = "display_time";
		 const std::string JUSTIFICATION_COLUMN			  = "justification";

		 const std::string LED_FONT_SIZE_COLUMN = "led_font_size";
		 const std::string LED_INTENSITY_COLUMN = "led_intensity";
		 const std::string LED_FONT_COLOUR_COLUMN = "led_font_colour";
		 const std::string LED_BACKGROUND_COLOUR_COLUMN = "led_background_colour";

		 const std::string PLASMA_FONT_SIZE_COLUMN = "plasma_font_size";
		 const std::string PLASMA_FONT_TYPE_COLUMN = "plasma_font_type";
		 const std::string PLASMA_FOREGROUND_COLOUR_COLUMN = "plasma_foreground_colour";
		 const std::string PLASMA_BACKGROUND_COLOUR_COLUMN = "plasma_background_colour";

		 const std::string DISPLAY_REQUEST_DATA_CREATETIME = "CREATE_TIME";

		 const std::string DISPLAY_REQUEST_IS_SEND_SUCCESS_COLUMN = "is_send_success";
		 const std::string IF_SEND_SUCCESS_FAIL = "Fail";
		 const std::string IF_SEND_SUCCESS_PASS = "Pass";

    }
}

#endif 
