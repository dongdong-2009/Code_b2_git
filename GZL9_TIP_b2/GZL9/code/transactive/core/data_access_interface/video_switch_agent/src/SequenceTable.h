#if !defined(SequenceTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define SequenceTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/SequenceTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
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
		
		static const std::string RELATION_TABLE_NAME    = "VI_ENTITY_RELATIONSHIP";
		static const std::string PKEY_COLUMN			= "PKEY";
		static const std::string PARENT_PKEY_COLUMN		= "PARENT_PKEY";
		static const std::string DWELLTIME_COLUMN		= "DWELLTIME";
		static const std::string ENTITY_TYPE_COLUMN		= "ENTITY_TYPE";
		static const std::string DESCRIPTION_COLUMN		= "DESCRIPTION";
		static const std::string ORDERINSEQ_COLUMN		= "ORDERINSEQ";

		static const std::string ENTITY_TABLE_NAME		= "ENTITY_V";

	}
}

#endif // SequenceTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
