/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/ToolTipWnd.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_ToolTipWnd_H__ABA4AE4D_034E_46B4_9EDE_A1223D7A84DC__INCLUDED_)
#define AFX_ToolTipWnd_H__ABA4AE4D_034E_46B4_9EDE_A1223D7A84DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTipWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ToolTipWnd window

class ToolTipWnd : public CWnd
{
// Construction
public:
    ToolTipWnd();
    CSize m_szMargins;      // extra space around text: change if you like
    enum {JUSTIFYLEFT=0, JUSTIFYRIGHT};
    BOOL Create(CPoint pt, CWnd* pListBox, UINT nStyle=0, UINT nID=0);
    void ShowDelayed(UINT msec);
    void Cancel();
// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(ToolTipWnd)
    protected:
    virtual void PostNcDestroy();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~ToolTipWnd();

    // Generated message map functions
protected:
    UINT  m_nStyle;         // style (see enum below)
    CWnd* m_fontWnd;

//helpers
    void DrawText(CDC& dc, LPCTSTR lpText, CRect& rc, UINT flags);

    // {{AFX_MSG(ToolTipWnd)
    afx_msg void OnPaint();
    afx_msg LRESULT OnSetText(WPARAM wp, LPARAM lp);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    // }}AFX_MSG
    DECLARE_DYNAMIC(ToolTipWnd);
    DECLARE_MESSAGE_MAP()
};


#define PTS_JUSTIFYLEFT  0x0000
#define PTS_JUSTIFYRIGHT 0x0001
#define PTS_TRANSPARENT  0x0002

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ToolTipWnd_H__ABA4AE4D_034E_46B4_9EDE_A1223D7A84DC__INCLUDED_)
