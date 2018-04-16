/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/VideoSourcesPropertySheet.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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


//---------------------------

namespace TA_Base_Bus
{
    class CTabCtrlEx2 : public CTabCtrl
    {
    // Construction
    public:
	    CTabCtrlEx2();

    // Attributes
    public:

    // Operations
    public:
	    BOOL	Install( CTabCtrl *pTabCtrl );
	    BOOL	Install( CPropertySheet *pPropSheet );
	    BOOL	EnableItem( int nItem, BOOL bEnable = TRUE );
	    BOOL	IsItemEnabled( int nItem, int *disabledIdx = NULL );

	    int		SetItemImage( int nItem, int nResBmp, int nResMsk = -1 );
	    int		SetItemImage( int nItem, HICON hIcon );

	    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    protected:
	    int		FindNextEnabled( int nStart, BOOL bNext );
	    void	SetCurrentTab( int nItem );
	    BOOL	SendKillActive( int nItem = -1 );

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CTabCtrlEx2)
	    protected:
	    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	    //}}AFX_VIRTUAL

    // Implementation
    public:
	    virtual ~CTabCtrlEx2();

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CTabCtrlEx2)
	    afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnDestroy();
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:
	    CDWordArray	m_disabled;
	    NMHDR		m_nmhdr;
    };
};


//---------------------------


class VideoSourcesPropertySheet : public CPropertySheet
{
protected:
//    void BuildPropPageArray();
    RECT m_rctPage,m_rctCtrl;
    CFont m_fntPage;

    DECLARE_DYNAMIC(VideoSourcesPropertySheet)

// Construction
public:
    VideoSourcesPropertySheet(UINT nIDCaption, RECT rect,CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    VideoSourcesPropertySheet(LPCTSTR pszCaption,RECT rect,CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

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
    TA_Base_Bus::CTabCtrlEx2 m_customTabControl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOSOURCESPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_)
