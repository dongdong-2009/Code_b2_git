/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_agent/src/TSAPINotificationManager.cpp $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the 
  */

#include "TSAPINotificationManager.h"
#include "TelephoneSwitch.h"
#include "TSAPINotificationThread.h"

namespace TA_App
{
	TSAPINotificationManager::TSAPINotificationManager(TelephoneSwitch* telephoneSwitch)
	{
		m_switch = telephoneSwitch;
		//start the thread
		m_thread = AfxBeginThread(RUNTIME_CLASS(TSAPINotificationThread));
		((TSAPINotificationThread*)m_thread)->setSwitch(m_switch);
	}

	TSAPINotificationManager::~TSAPINotificationManager()
	{
		AfxEndThread(0,0);
	}

	HWND TSAPINotificationManager::getWindowHandle()
	{
		return ((TSAPINotificationThread*)m_thread)->getWindowHandle();
	}
};