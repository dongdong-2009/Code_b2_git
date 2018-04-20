/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/system_control/banner/pages/call_page/src/CallBannerInitialiser.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/12 10:20:16 $
  * Last modified by:  $Author: builder $
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


namespace TA_IRS_App
{

    CallBannerInitialiser::CallBannerInitialiser(CallPage& callPage)
        :
        m_callPage(callPage)
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

        m_running = true;
        //
        // While this thread is running, and the call page has not yet been
        // initialised, attempt to initialise the call page every ten seconds.
        //

        while( m_running && !m_callPage.isInitialised() )
        {
            m_callPage.initialiseBanner();
            Thread::sleep(10000);
        }
    }

    void CallBannerInitialiser::terminate()
    {
        m_running = false;
    }



} // TA_IRS_App

