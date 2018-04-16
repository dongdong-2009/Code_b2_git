/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/SelectedDvaListCtrl.h $
  * @author Li Xiaoxia
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/11/29 22:02:45 $
  * Last modified by: $Author: qi.huang $
  * 
  * Maintains a list control for selected DVA messages
  *
  */

#if !defined(AFX_SELECTEDDVALISTCTRL_H__17F6D56D_591C_4319_B0EB_9D6D9B45BC79__INCLUDED_)
#define AFX_SELECTEDDVALISTCTRL_H__17F6D56D_591C_4319_B0EB_9D6D9B45BC79__INCLUDED_

#include "app/pa/PAManager/src/DvaListCtrl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectedDvaListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectedDvaListCtrl window

class SelectedDvaListCtrl : public DvaListCtrl
{
public:
	enum EScrollDirection
	{
		scrollNull,
		scrollUp,
		scrollDown
	};
	EScrollDirection m_ScrollDirection;
	UINT m_uScrollTimer;

public:
	SelectedDvaListCtrl();
	virtual ~SelectedDvaListCtrl();

	void setSingleDva( bool bSingle );

protected:
    void dropItem();
    void restorePrevDropItemState();
    void updateSelection(int nDropIndex);
	void setScrollTimer(EScrollDirection ScrollDirection);
	void killScrollTimer();
	CImageList* createDragImageEx( CPoint& refPoint );
	virtual bool isSelectable(int rowNumber);

// Generated message map functions
protected:
	//{{AFX_MSG(CDragDropListCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
    bool m_bSingleDva;
	int m_iDragIndex;
	int m_iDropIndex;
	CImageList* m_pDragImage;
	int	 m_iPrevDropIndex;
	UINT m_uiPrevDropState;
	DWORD m_dwStyle;
};

#endif // !defined(AFX_SELECTEDDVALISTCTRL_H__17F6D56D_591C_4319_B0EB_9D6D9B45BC79__INCLUDED_)
