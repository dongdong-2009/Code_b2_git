/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/VideoDisplayManager.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
#include "core/data_access_interface/src/ILocation.h"

#include <map>
#include <vector>

namespace TA_IRS_App
{

    class VideoDisplayManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
        VideoDisplayManagerApp();

        virtual BOOL InitInstance();
        /* Parameters that are used application wide */

        void setLocationType(TA_Base_Core::ILocation::ELocationType locationType) { m_locationType = locationType; }
        TA_Base_Core::ILocation::ELocationType getLocationType() { return m_locationType; }

        void setDefaultDwellTime(unsigned short dwellTime);
        unsigned short getDefaultDwellTime();

        /* monitor 1,2,3,ODS,4,5,6 */
        //void setVideoWallMonitorKeys(std::vector<unsigned long> keys);
        //std::vector<unsigned long> getVideoWallMonitorKeys();


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(VideoDisplayManagerApp)
        public:
        virtual int ExitInstance();
        // }}AFX_VIRTUAL

    // Implementation

        // {{AFX_MSG(VideoDisplayManagerApp)
            // NOTE - the ClassWizard will add and remove member functions here.
            // DO NOT EDIT what you see in these blocks of generated code !
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        TA_Base_Core::ILocation::ELocationType m_locationType;
        unsigned short m_defaultDwellTime;
        //std::vector<unsigned long> m_videoWallMonitorKeys;

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_VIDEODISPLAYMANAGER_H__911C5095_68C5_11D7_B175_0050BAB094A9__INCLUDED_)
