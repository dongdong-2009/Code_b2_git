#if !defined(TSAPINOTIFICATIONMANAGER_H)
#define TSAPINOTIFICATIONMANAGER_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_agent/src/TSAPINotificationManager.h $
  * @author:  Bavo De Pauw
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the 
  */
#include "stdafx.h"
//#include "TSAPINotificationThread.h"

namespace TA_App
{
	class TelephoneSwitch;
	class TSAPINotificationManager
	{
	public:
		TSAPINotificationManager(TelephoneSwitch* telephoneSwitch);
		~TSAPINotificationManager();

		HWND getWindowHandle();
	private:
//		TSAPINotificationThread* m_thread;
		TelephoneSwitch* m_switch;
		CWinThread* m_thread;
	};
}


#endif // !defined(TSAPINOTIFICATIONMANAGER_H)
