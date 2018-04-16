/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManager.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * TODO: <description>
  */

// VideoDisplayManager.h : main header file for the VIDEODISPLAYMANAGER application
//

#if !defined(AFX_VIDEODISPLAYMANAGER_H__911C5095_68C5_11D7_B175_0050BAB094A9__INCLUDED_)
#define AFX_VIDEODISPLAYMANAGER_H__911C5095_68C5_11D7_B175_0050BAB094A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"        // main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

namespace TA_IRS_App
{

class VideoDisplayManagerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
    VideoDisplayManagerApp();
    virtual int ExitInstance();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_VIDEODISPLAYMANAGER_H__911C5095_68C5_11D7_B175_0050BAB094A9__INCLUDED_)
