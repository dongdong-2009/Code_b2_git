/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/VideoSourcesListBox.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_VIDEOSOURCESLISTBOX_H__CD7695BE_4026_4352_89E1_50064C457C1B__INCLUDED_)
#define AFX_VIDEOSOURCESLISTBOX_H__CD7695BE_4026_4352_89E1_50064C457C1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoSourcesListBox.h : header file
//

#include "ToolTipWnd.h"


namespace TA_IRS_App
{

    class VideoSourcesListBox : public CListBox
    {
    // Construction
    public:
        VideoSourcesListBox();

    // Attributes
    public:

    // Operations
    public:
        void setToolTipTimes(UINT tipInitialiseDelay, UINT tipPopDelay, UINT tipTransferDelay, UINT tipTimeoutDelay)
            { m_tipInitialiseDelay = tipInitialiseDelay; m_tipPopDelay = tipPopDelay;
                m_tipTransferDelay = tipTransferDelay; m_tipTimeoutDelay = tipTimeoutDelay; }

        void addSourceString( LPCTSTR lpszItem );
        void deleteSourceString();
        void moveSourceTop();
        void moveSourceUp();
        void moveSourceDown();
        void moveSourceBottom();

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(VideoSourcesListBox)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
        virtual void PreSubclassWindow();
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~VideoSourcesListBox();

        // Generated message map functions
    protected:
        // {{AFX_MSG(VideoSourcesListBox)
        afx_msg void OnKillfocus();
        afx_msg void OnSetfocus();
        afx_msg void OnSelchange();
        afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnTimer(UINT nIDEvent);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:
        int m_prevSelection;

        // members to manage tooltip
        int        m_nCurItem;                        // index of current item
        BOOL    m_bCapture;                        // whether mouse is captured
        ToolTipWnd m_wndTip;                    // THE tip window
        UINT m_tipInitialiseDelay;
        UINT m_tipPopDelay;
        UINT m_tipTransferDelay;
        UINT m_tipTimeoutDelay;

        // helper functions:
        void updateButtonEnable();
        BOOL IsRectCompletelyVisible(const CRect& rc);
        int OnGetItemInfo(CPoint p, CRect& rc, CString& s);
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_VIDEOSOURCESLISTBOX_H__CD7695BE_4026_4352_89E1_50064C457C1B__INCLUDED_)
