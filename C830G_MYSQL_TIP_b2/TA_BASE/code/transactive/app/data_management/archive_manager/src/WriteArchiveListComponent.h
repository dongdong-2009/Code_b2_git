/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/data_management/archive_manager/src/WriteArchiveListComponent.h $
  * @author:  Wenguang
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * List control for the Write Archive Dialog.
  *
  */

#if !defined(ICE__WriteArchiveListComponet__H__)
#define ICE__WriteArchiveListComponet__H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// WriteArchiveListComponent.h : header file
//


class WriteArchiveListComponent: public CListCtrl
{
public:
	WriteArchiveListComponent();
	
	int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
	BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	void AutoResizeColumn();
	
	// Overrides
	
	//{{AFX_VIRTUAL(WriteArchiveListComponent)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

		// Generated message map functions
protected:
	//{{AFX_MSG(WriteArchiveListComponent)
	
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
   
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
		
private:

	static int	CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParam);
	
	int		CellRectFromPoint( CPoint & point, RECT * cellrect, int * col ) const;

	int		m_nCurrentSortColumn;
    bool	m_fSortAscending;
	bool	m_bToolTipCtrlCustomizeDone;
	
	std::auto_ptr<TCHAR> m_pchTip;
	std::auto_ptr<WCHAR> m_pwchTip;

};

#endif