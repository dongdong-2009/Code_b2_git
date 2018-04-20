#ifndef TRAIN_TIME_SCHEDULE_H_INCLUDED
#define TRAIN_TIME_SCHEDULE_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleTable.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Contains the details for the TrainTimeSchedule table.
  */

#include <string>

namespace TA_IRS_Core
{
    namespace TA_TISAgentDAI
    {
        static const std::string TRAIN_TIME_SCHEDULE_TABLE_NAME      = "ti_train_time_schedule";
		static const std::string TRAIN_TIME_SCHEDULE_PKEY_COLUMN	= "TITTSC_ID";
        static const std::string SCHEDULE_NAME_COLUMN              = "name";
        static const std::string ADHOC_MESSAGE1_TITLE_COLUMN      = "adhoc1_title";
        static const std::string ADHOC_MESSAGE1_COLUMN            = "adhoc1";
        static const std::string ADHOC_MESSAGE2_TITLE_COLUMN      = "adhoc2_title";
		static const std::string ADHOC_MESSAGE2_COLUMN            = "adhoc2";
        static const std::string ADHOC_MESSAGE3_TITLE_COLUMN      = "adhoc3_title";
		static const std::string ADHOC_MESSAGE3_COLUMN            = "adhoc3";
        static const std::string ADHOC_MESSAGE4_TITLE_COLUMN      = "adhoc4_title";
		static const std::string ADHOC_MESSAGE4_COLUMN            = "adhoc4";
        static const std::string ADHOC_MESSAGE5_TITLE_COLUMN      = "adhoc5_title";
		static const std::string ADHOC_MESSAGE5_COLUMN            = "adhoc5";
        static const std::string ADHOC_MESSAGE6_TITLE_COLUMN      = "adhoc6_title";
		static const std::string ADHOC_MESSAGE6_COLUMN            = "adhoc6";
        static const std::string TRAIN_TIME_SCHEDULE_PKEY_SEQUENCE  = "TITTSC_SEQ.NEXTVAL";
        
		static const std::string TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME = "ti_train_time_schedule_entry";
		static const std::string TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN = "TITTSE_ID";
		static const std::string TRAIN_TIME_SCHEDULE_PARENT_SCHEDULE_COLUMN = "TITTSC_ID";
		static const std::string TRAIN_TIME_SCHEDULE_START_TIME_COLUMN = "start_time";
		static const std::string TRAIN_TIME_SCHEDULE_END_TIME_COLUMN = "end_time";
		static const std::string TRAIN_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN = "repeat_interval";
		static const std::string TRAIN_TIME_SCHEDULE_MESSAGE_TABLE_COLUMN = "message_table";
		static const std::string TRAIN_TIME_SCHEDULE_MESSAGE_ID_COLUMN = "message_id";
		static const std::string TRAIN_TIME_SCHEDULE_PRIORITY_COLUMN = "priority";
        static const std::string TRAIN_TIME_SCHEDULE_ENTRY_PKEY_SEQUENCE = "TITTSE_SEQ.NEXTVAL";
    }
}

#endif 
