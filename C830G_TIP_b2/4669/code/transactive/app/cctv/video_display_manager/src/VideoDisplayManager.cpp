/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/VideoDisplayManager.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
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
      m_defaultDwellTime(5),
      m_videoWallMonitorKeys(7, 0) // make sure its got 7 entries
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


    void VideoDisplayManagerApp::setVideoWallMonitorKeys(std::vector<unsigned long> keys)
    {
        TA_ASSERT(keys.size() == 7, "There must be 7 monitor keys");
        m_videoWallMonitorKeys = keys;
    }


    std::vector<unsigned long> VideoDisplayManagerApp::getVideoWallMonitorKeys()
    {
        TA_ASSERT(m_videoWallMonitorKeys.size() == 7, "There must be 7 monitor keys");
        return m_videoWallMonitorKeys;
    }


} // TA_IRS_App