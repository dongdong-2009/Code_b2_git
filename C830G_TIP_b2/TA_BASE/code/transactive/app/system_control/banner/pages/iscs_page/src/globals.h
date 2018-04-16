/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/banner/pages/iscs_page/src/globals.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef GLOBALS_H
#define GLOBALS_H

static const ULONG WM_ON_CHANGE_MSG		= WM_USER + 1;
static const ULONG WM_UPDATE_ALARMS_MSG = WM_USER + 2;
static const ULONG WM_ON_SYSTEM_DEGRADED =	WM_USER + 3;
static const ULONG WM_ON_SYSTEM_RECOVERED =	WM_USER + 4;

static const ULONG PROFILE_LOCAL   = 0;
static const ULONG PROFILE_CENTRAL = 1;

#endif
