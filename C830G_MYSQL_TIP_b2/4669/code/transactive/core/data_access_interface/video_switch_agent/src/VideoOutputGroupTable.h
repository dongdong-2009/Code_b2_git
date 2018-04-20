#if !defined(VideoOutputGroupTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define VideoOutputGroupTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/VideoOutputGroupTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Contains the details for the VideoOutputGroup table.
  */

#include <string>

namespace TA_IRS_Core
{
    namespace TA_VideoSwitchAgentDAI
    {
        static const std::string VIDEO_OUTPUT_GROUP_TABLE_NAME  = "vi_video_output_group";
        static const std::string VIDEO_OUTPUT_GROUP_KEY_COLUMN  = "coenti_id_01";
        static const std::string POSITION_COLUMN                = "position";
        static const std::string VIDEO_OUTPUT_KEY_COLUMN        = "coenti_id_02";
    }
}

#endif // VideoOutputGroupTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
