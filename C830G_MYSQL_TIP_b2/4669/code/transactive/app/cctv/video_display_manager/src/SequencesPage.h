/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/SequencesPage.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * TODO: <description>
  */

#if !defined(AFX_SEQUENCESPAGE_H__ADE530C9_6E1F_11D7_B175_0050BAB094A9__INCLUDED_)
#define AFX_SEQUENCESPAGE_H__ADE530C9_6E1F_11D7_B175_0050BAB094A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoSourcesTreeCtrl.h"

#include "CamerasPage.h"

namespace TA_IRS_App
{
    class SequencesPage : public CPropertyPage
    {
        DECLARE_DYNCREATE(SequencesPage)
    // Construction
    public:
        SequencesPage();                        // standard constructor
        ~SequencesPage();

    // void Init(REBProgressManager & mgr);
    // libo
        void Init();

    // Dialog Data
        // {{AFX_DATA(SequencesPage)
        enum { IDD = IDD_SEQUENCES };
        VideoSourcesTreeCtrl    m_sequenceTreeCtrl;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(SequencesPage)
        public:
        virtual BOOL OnKillActive();
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(SequencesPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnOK();
        afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnDestroy();
        afx_msg LRESULT onSequenceUpdated(WPARAM wParam, LPARAM lParam);
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

#endif // !defined(AFX_SEQUENCESPAGE_H__ADE530C9_6E1F_11D7_B175_0050BAB094A9__INCLUDED_)
