#if !defined(ArchiveTablesTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ArchiveTablesTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveTableTable.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Contains the details for the AR_TABLES table.
  */

// KT 26 Feb 04, PW #3133:  ONLINE_PERIOD has been moved from AR_TABLES and put in AR_ONLINE.

#include <string>

namespace TA_Base_Core
{
    namespace TA_ArchiveManagerDAI
    {
        static const std::string ARCHIVE_TABLE_TABLE_NAME       = "ar_tables";
        static const std::string ARCHIVE_TABLE_KEY_COLUMN       = "artabl_id";
        static const std::string TABLE_NAME_COLUMN              = "table_name";
        static const std::string QUERY_COLUMN                   = "query_col";
        static const std::string FILE_POSTFIX_COLUMN            = "file_postfix";
    }
}

#endif // ArchiveTablesTable_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_
