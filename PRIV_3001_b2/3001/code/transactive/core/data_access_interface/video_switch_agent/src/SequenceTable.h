#if !defined(SequenceTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define SequenceTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/SequenceTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Contains the details for the Sequence table.
  */

#include <string>

namespace TA_IRS_Core
{
    namespace TA_VideoSwitchAgentDAI
    {
        static const std::string SEQUENCE_TABLE_NAME    = "vi_sequence";
        static const std::string SEQUENCE_KEY_COLUMN    = "coenti_id_01";
        static const std::string POSITION_COLUMN        = "position";
        static const std::string VIDEO_INPUT_KEY_COLUMN = "coenti_id_02";
    }
}

#endif // SequenceTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
