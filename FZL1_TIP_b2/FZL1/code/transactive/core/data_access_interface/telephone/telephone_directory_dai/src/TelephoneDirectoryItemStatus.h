/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryItemStatus.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  * Simply a placeholder structure for a Telehpone Directory
  */


#if !defined(TelephoneDirectoryItemStatus_59F7BB71_EAA2_4985_A865_165EF84CBA09__INCLUDED_)
#define TelephoneDirectoryItemStatus_59F7BB71_EAA2_4985_A865_165EF84CBA09__INCLUDED_

namespace TA_IRS_Core
{
    enum TelephoneDirectoryItemStatus
    {
        TELDIR_STATUS_NEW = 0,
        TELDIR_STATUS_ACTIVE = 1,
        TELDIR_STATUS_INACTIVE = 3
    };

} // closes namespace
#endif // !defined(TelephoneDirectoryItemStatus_59F7BB71_EAA2_4985_A865_165EF84CBA09__INCLUDED_)