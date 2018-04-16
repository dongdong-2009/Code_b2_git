/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/CamerasPage.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * TODO: <description>
  */

#if !defined(AFX_AMERASPAGE_H__ADE530CA_6E1F_11D7_B175_0050BAB094A9__INCLUDED_)
#define AFX_AMERASPAGE_H__ADE530CA_6E1F_11D7_B175_0050BAB094A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "app/cctv/video_display_manager/src/VideoSourcesTreeCtrl.h"

namespace TA_IRS_App
{
    class CamerasPage : public CPropertyPage
    {
        DECLARE_DYNCREATE(CamerasPage)

    // Construction
    public:
        CamerasPage();   // standard constructor
        ~CamerasPage();

    // void Init(REBProgressManager & mgr);
        // libo
        void Init();


    // Dialog Data
        // {{AFX_DATA(CamerasPage)
        enum { IDD = IDD_CAMERAS };
        VideoSourcesTreeCtrl    m_cameraTreeCtrl;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CamerasPage)
        public:
        virtual BOOL OnKillActive();
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CamerasPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
        afx_msg void OnOK();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnDestroy();
        afx_msg LRESULT onCameraUpdated(WPARAM wParam, LPARAM lParam);
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void setOffset();

        bool m_isOffsetInitialised;
        int m_bottomOffset;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_AMERASPAGE_H__ADE530CA_6E1F_11D7_B175_0050BAB094A9__INCLUDED_)
