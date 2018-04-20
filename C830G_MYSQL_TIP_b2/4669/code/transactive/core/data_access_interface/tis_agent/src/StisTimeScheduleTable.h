#ifndef STISTIMESCHEDULETABLE_H 
#define STISTIMESCHEDULETABLE_H 
#include <string>

namespace TA_IRS_Core
{
    namespace TA_TISAgentDAI
    {
        static const std::string STIS_TIME_SCHEDULE_TABLE_NAME             = "ti_stis_time_schedule";
		static const std::string STIS_TIME_SCHEDULE_ID_COLUMN              = "stis_ts_id";
		static const std::string STIS_TIME_SCHEDULE_START_TIME_COLUMN      = "start_time";
		static const std::string STIS_TIME_SCHEDULE_END_TIME_COLUMN        = "end_time";
		static const std::string STIS_TIME_SCHEDULE_PRIORITY_COLUMN        = "priority";
		static const std::string STIS_TIME_SCHEDULE_DISPLAY_MODE_COLUMN    = "display_mode";
		static const std::string STIS_TIME_SCHEDULE_SCROLL_SPEED_COLUMN    = "scroll_speed";
		static const std::string STIS_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN = "repeat_interval";
		static const std::string STIS_TIME_SCHEDULE_LCD_FONT_SIZE_COLUMN   = "lcd_font_size";
		static const std::string STIS_TIME_SCHEDULE_LCD_FONT_COLOUR_COLUMN = "lcd_font_colour";
        static const std::string STIS_TIME_SCHEDULE_LED_FONT_SIZE_COLUMN   = "led_font_size";
        static const std::string STIS_TIME_SCHEDULE_LED_FONT_COLOUR_COLUMN = "led_font_colour";
		static const std::string STIS_TIME_SCHEDULE_DISPLAY_TIME_COLUMN    = "display_time";
		static const std::string STIS_TIME_SCHEDULE_JUSTIFICATION_COLUMN   = "justification";
		static const std::string STIS_TIME_SCHEDULE_TEXT_CONTENT_COLUMN    = "text_content";
    }
}

#endif 