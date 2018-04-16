/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManager.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * TODO: <description>
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "stdafx.h"
#include "VideoDisplayManager.h"
#include "VideoDisplayManagerGUI.h"
#include "VideoDisplayManagerDlg.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Bus::TransActiveWinApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    /////////////////////////////////////////////////////////////////////////////
    // VideoDisplayManagerApp

    BEGIN_MESSAGE_MAP(VideoDisplayManagerApp, TransActiveWinApp)
        // {{AFX_MSG_MAP(VideoDisplayManagerApp)
            // NOTE - the ClassWizard will add and remove mapping macros here.
            // DO NOT EDIT what you see in these blocks of generated code!
        // }}AFX_MSG
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // VideoDisplayManagerApp construction

    VideoDisplayManagerApp::VideoDisplayManagerApp()
    : TA_Base_Bus::TransActiveWinApp( new VideoDisplayManagerGUI(), "Display Manager" ),
      m_locationType(TA_Base_Core::ILocation::OCC),
      m_defaultDwellTime(5)
    {
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only VideoDisplayManagerApp object

    VideoDisplayManagerApp theApp;


    int VideoDisplayManagerApp::ExitInstance()
    {
        return TransActiveWinApp::ExitInstance();
    }


    void VideoDisplayManagerApp::setDefaultDwellTime(unsigned short dwellTime)
    {
        m_defaultDwellTime = dwellTime;
    }


    unsigned short VideoDisplayManagerApp::getDefaultDwellTime()
    {
        return m_defaultDwellTime;
    }


} // TA_IRS_App