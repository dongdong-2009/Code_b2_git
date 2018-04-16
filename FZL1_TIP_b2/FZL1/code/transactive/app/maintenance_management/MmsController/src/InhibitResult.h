/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/maintenance_management/MmsController/src/InhibitResult.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */

#ifndef _INHIBITRESULT_H_
#define _INHIBITRESULT_H_

namespace TA_IRS_App
{		
	enum EInhibitResult
	{
		INHIBIT_APPLIED,
        INHIBIT_REMOVED,
        INHIBIT_FAILED,
		SYSTEM_ALARM_INHIBIT_APPLIED,
		SYSTEM_ALARM_INHIBIT_FAILED,
		SYSTEM_ALARM_INHIBIT_REMOVED
	};

}

#endif //_INHIBITRESULT_H_