/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/MmsController/src/InhibitResult.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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