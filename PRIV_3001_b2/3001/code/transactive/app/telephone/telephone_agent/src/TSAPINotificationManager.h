#if !defined(TSAPINOTIFICATIONMANAGER_H)
#define TSAPINOTIFICATIONMANAGER_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_agent/src/TSAPINotificationManager.h $
  * @author:  Bavo De Pauw
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
