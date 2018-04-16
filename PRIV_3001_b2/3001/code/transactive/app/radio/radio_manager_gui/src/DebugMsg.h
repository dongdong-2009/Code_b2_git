/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/DebugMsg.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#ifndef __DEBUGMSG203429304__
#define __DEBUGMSG203429304__

#include <windows.h>
#include <string>

extern void AddDebugMessage(const std::string& msg);
extern void InitializeDebugWindow(HINSTANCE hInstance);

extern void TempLog(const std::string& msg);

#endif
