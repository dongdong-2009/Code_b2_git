/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Robert Young
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * <description>
  */
// VideoSourcesTreeCtrl.h : header file
//

#if !defined(AFX_VIDEOSOURCESTREECTRL_H__95A6F313_6FB9_11D7_B176_0050BAB094A9__INCLUDED_)
#define AFX_VIDEOSOURCESTREECTRL_H__95A6F313_6FB9_11D7_B176_0050BAB094A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "videodisplaymanager.h"
#include "tooltipwnd.h"
#include <vector>
#include <map>


namespace TA_IRS_App
{
    class VideoInput;

    class VideoSourcesTreeCtrl : public CTreeCtrl
    {
    // Construction
    public:
        VideoSourcesTreeCtrl();

        virtual ~VideoSourcesTreeCtrl();

        inline void setToolTipTimes( UINT tipInitialiseDelay,
                                     UINT tipPopDelay,
                                     UINT tipTransferDelay,
                                     UINT tipTimeoutDelay )
            { m_tipInitialiseDelay = tipInitialiseDelay;
              m_tipPopDelay = tipPopDelay;
              m_tipTransferDelay = tipTransferDelay;
              m_tipTimeoutDelay = tipTimeoutDelay; }

        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(VideoSourcesTreeCtrl)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
        virtual void PreSubclassWindow();
        // }}AFX_VIRTUAL

    // Implementation
    public:

        void insertVideoSources( std::vector<VideoInput*> sources );

        void insertVideoSource( VideoInput* source );

        void setToSequenceEditorTree(BOOL set = TRUE) { m_isSequenceEditorTree = set; };

        void setToQuadControllerTree(BOOL set = TRUE) { m_isQuadControllerTree = set; };

        void endDragging();

        CImageList* CreateDragImageEx(HTREEITEM hItem);

        void expandToGroups();

        void resetToolTip();

        void setDetailsPointer(CEdit* pDetailsEdit) {m_pDetailsEdit = pDetailsEdit;}

        HTREEITEM findItemFromSubstr(const CString& substr, HTREEITEM TreeItem = NULL);

        /**
          * videoInputUpdated
          *
          * when a video input is updated either via a comms message
          * or as a result of a local action, then this will be called by the owning page.
          * The list control should update the display if it has the focus and the
          * item is selected.
          * It will update the description of the item if it changes.
          *
          * @param input The pointer to the item that changed
          */
        void videoInputUpdated(VideoInput* input);


        /**
          * getSelectedInput
          *
          * retreives the pointer to the selected video input item.
          * This is used by dialogs wanting to know what was 'dropped'
          * on them.
          *
          * @return the selected item, NULL if none
          */
        VideoInput* getSelectedInput();

        int getSuitItemHeight( int nHeight ); // CL-21275

        // Generated message map functions
    protected:
        // {{AFX_MSG(VideoSourcesTreeCtrl)
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnKillFocus(CWnd* pNewWnd);
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
        afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
        afx_msg BOOL OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnTimer(UINT nIDEvent);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        /**
          * getGroupItem
          *
          * Given a group name and a parent name, return the HTREEITEM for the
          * group. If there is no matching item, return NULL.
          *
          * @param groupName    The group name
          * @param parentName   The parent name (or empty string)
          *
          * @return the htreeitem, or NULL if not found.
          */
        HTREEITEM getGroupItem( const std::string& groupName,
                                 const std::string& parentName) const;

        /**
          * handleSelectionChanged
          *
          * This will update the details page in the main dialog
          * with the details of the selected item.
          */
        void handleSelectionChanged();

        std::vector<std::string> tokeniseGroupName(const std::string& groupName);
        std::vector<std::string> tokenizeString(const std::string& theString,
                                                const std::string& separatorList );

        CImageList m_imageList;
        std::multimap<std::string, HTREEITEM> m_videoSourcesGroups;
        BOOL m_isSequenceEditorTree;
        BOOL m_isQuadControllerTree;
        BOOL m_isDragging;
        CEdit* m_pDetailsEdit;

        // members to manage tooltip
        HTREEITEM        m_nCurItem;        // index of current item
        BOOL    m_bCapture;                // whether mouse is captured
        ToolTipWnd m_wndTip;            // THE tip window
        UINT m_tipInitialiseDelay;
        UINT m_tipPopDelay;
        UINT m_tipTransferDelay;
        UINT m_tipTimeoutDelay;

        // helpers to manage tooltip
        BOOL IsRectCompletelyVisible(const CRect& rc);
        HTREEITEM OnGetItemInfo(CPoint p, CRect& rc, CString& s);
    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_VIDEOSOURCESTREECTRL_H__95A6F313_6FB9_11D7_B176_0050BAB094A9__INCLUDED_)
