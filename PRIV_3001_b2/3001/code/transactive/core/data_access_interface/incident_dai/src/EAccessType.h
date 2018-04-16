/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/incident_dai/src/EAccessType.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * The access level. Must be between 0 and 4
  * 0 - no access
  * 1 - view-only access
  * 2 - can control (view/edit/close/reopen/delete/view) its own station only
  * 3 - can control (view/edit/close/reopen/delete/view) at OCC only
  * 4 - can control (view/edit/close/reopen/delete/view) any station
  */


#if !defined(EAccessType_7398823C_AEFA_4bb3_8C59_C2C49F0858D4__INCLUDED_)
#define EAccessType_7398823C_AEFA_4bb3_8C59_C2C49F0858D4__INCLUDED_

/**
 * @version 1.0
 * @created 16-May-2005 12:32:13 PM
 */
namespace TA_IRS_Core
{	
enum EAccessType
{
	NO_CONTROL,
	VIEW,
	CONTROL_OWN,
	CONTROL_OCC,
	CONTROL_ALL
};
}// end TA_IRS_Core
#endif // !defined(EAccessType_7398823C_AEFA_4bb3_8C59_C2C49F0858D4__INCLUDED_)