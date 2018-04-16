/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoSourcesListBox.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_VIDEOSOURCESLISTBOX_H__CD7695BE_4026_4352_89E1_50064C457C1B__INCLUDED_)
#define AFX_VIDEOSOURCESLISTBOX_H__CD7695BE_4026_4352_89E1_50064C457C1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideoSourcesListBox.h : header file
//

#include "app/cctv/video_display_manager/src/ToolTipWnd.h"

namespace TA_IRS_App
{

    class VideoSourcesListBox : public CListBox
    {
            // Construction
        public:
            VideoSourcesListBox(bool withPrefix = false);
            virtual ~VideoSourcesListBox();
            void setToolTipTimes(UINT tipInitialiseDelay,
                                 UINT tipPopDelay,
                                 UINT tipTransferDelay,
                                 UINT tipTimeoutDelay);
            void addSourceString(LPCTSTR lpszItem);
            void deleteSourceString();
            void moveSourceTop();
            void moveSourceUp();
            void moveSourceDown();
            void moveSourceBottom();
            virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
            virtual void PreSubclassWindow();
            afx_msg void OnKillfocus();
            afx_msg void OnSetfocus();
            afx_msg void OnSelchange();
            afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
            afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
            afx_msg void OnMouseMove(UINT nFlags, CPoint point);
            afx_msg void OnTimer(UINT nIDEvent);

            DECLARE_MESSAGE_MAP()

        private:
            int m_prevSelection;

            // members to manage tooltip
            int m_nCurItem;                        // index of current item
            BOOL m_bCapture;                        // whether mouse is captured
            ToolTipWnd m_wndTip;                    // THE tip window
            UINT m_tipInitialiseDelay;
            UINT m_tipPopDelay;
            UINT m_tipTransferDelay;
            UINT m_tipTimeoutDelay;
            bool m_withPrefix;

            // helper functions:
            void updateButtonEnable();
            BOOL IsRectCompletelyVisible(const CRect& rc);
            int OnGetItemInfo(CPoint p, CRect& rc, CString& s);
    };
} // TA_IRS_App

#endif // !defined(AFX_VIDEOSOURCESLISTBOX_H__CD7695BE_4026_4352_89E1_50064C457C1B__INCLUDED_)
