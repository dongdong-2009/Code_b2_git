/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/VideoSourcesPropertySheet.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_VIDEOSOURCESPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_)
#define AFX_VIDEOSOURCESPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// videosourcespropertysheet.h : header file
//

#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// VideoSourcesPropertySheet

class VideoSourcesPropertySheet : public CPropertySheet
{
protected:
//    void BuildPropPageArray();
    RECT m_rctPage;
    CFont m_fntPage;

    DECLARE_DYNAMIC(VideoSourcesPropertySheet)

// Construction
public:
    VideoSourcesPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    VideoSourcesPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

   void increaseHeight( int units );

   BOOL EnableItem( int nItem, BOOL bEnable = TRUE );
// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(VideoSourcesPropertySheet)
    public:
    virtual BOOL OnInitDialog();
    protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~VideoSourcesPropertySheet();

    // Generated message map functions
protected:
    virtual void BuildPropPageArray ();
    // {{AFX_MSG(VideoSourcesPropertySheet)
    // }}AFX_MSG
    afx_msg LONG OnResizePage (UINT, LONG);
    DECLARE_MESSAGE_MAP()
    TA_Base_Bus::CTabCtrlEx m_customTabControl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOSOURCESPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_)
