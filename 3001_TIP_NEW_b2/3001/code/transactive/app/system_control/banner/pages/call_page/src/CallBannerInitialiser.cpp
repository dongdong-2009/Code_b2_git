/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/system_control/banner/pages/call_page/src/CallBannerInitialiser.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/06/29 12:01:20 $
  * Last modified by:  $Author: raghu.babu $
  *
  * This thread is used to separate the initialisation of the call banner
  * from MFC windowing functions so that the banner is viewable without
  * having to be initialised completely.
  *
  */

#include "app\system_control\banner\pages\call_page\src\stdafx.h"

#if defined(_MSC_VER)
	#pragma warning(disable:4250)	// inherits via dominance
#endif // defined (_MSC_VER)


#include "app/system_control/banner/pages/call_page/src/CallPage.h"
#include "app/system_control/banner/pages/call_page/src/CallBannerInitialiser.h"

#include "bus\mfc_extensions\src\mfc_extensions.h"

#include "core/utilities/src/DebugUtil.h"
namespace TA_IRS_App
{

    CallBannerInitialiser::CallBannerInitialiser(CallPage& callPage)
        :
        m_callPage(callPage),
		m_running(true)
    {
    }

    CallBannerInitialiser::~CallBannerInitialiser()
    {
    }

    void CallBannerInitialiser::run()
    {
        //
        // Sleep before calling initialise the first time, so that
        // the OnInitDialog method has time to return
        //
        Thread::sleep(1000);

        //
        // While this thread is running, and the call page has not yet been
        // initialised, attempt to initialise the call page every ten seconds.
        //
		int maxRetryCount = 0;
		while( m_running && !m_callPage.isInitialised() && maxRetryCount< 6)
        {
            m_callPage.initialiseBanner();
            Thread::sleep(10000);
			maxRetryCount++;
        }
		if (maxRetryCount == 6)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "maximun retry number reached, going to remove call banner page ...");
			PostMessage(m_callPage.m_hWnd,CALL_PAGE_TOBE_REMOVED,0,0);
        }
    }

    void CallBannerInitialiser::terminate()
    {
        m_running = false;
    }



} // TA_IRS_App

