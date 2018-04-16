#if !defined(CameraPresetsTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define CameraPresetsTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/CameraPresetsTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Contains the details for the CameraPresets table.
  */

#include <string>

namespace TA_IRS_Core
{
    namespace TA_VideoSwitchAgentDAI
    {
        static const std::string CAMERA_PRESETS_TABLE_NAME      = "vi_camera_presets";
        static const std::string CAMERA_KEY_COLUMN              = "coenti_id_01";
        static const std::string PRESET_INDEX_COLUMN            = "preset_index";
        static const std::string PRESET_NAME_COLUMN             = "preset_name";
    }
}

#endif // CameraPresetsTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
