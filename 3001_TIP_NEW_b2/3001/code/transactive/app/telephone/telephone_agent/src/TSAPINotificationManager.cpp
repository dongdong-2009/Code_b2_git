/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_agent/src/TSAPINotificationManager.cpp $
  * @author:  Bavo De Pauw
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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