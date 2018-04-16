/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Huang Qi
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * subclass control of AutoSortListCtrl for DVA message list
  *
  */
#if !defined(AFX_PABROADCASTLISTCTRL_H__8C40284B_5179_4804_814C_0C933813F795__INCLUDED_)
#define AFX_PABROADCASTLISTCTRL_H__8C40284B_5179_4804_814C_0C933813F795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\mfc_extensions\src\auto_sort_list_ctrl\autosortlistctrl.h"

class CMenu;
class PaRuntimeModel;

class PaDVAMsgListCtrl : public TA_Base_Bus::AutoSortListCtrl  
{
public:
	PaDVAMsgListCtrl( PaRuntimeModel& refBroadcastModel, int nRecordType );
    virtual ~PaDVAMsgListCtrl();

    void updateList( const std::string& strFilterWord );
    bool selQuickBroadcastItem( WPARAM wParam );
    bool selectItemByItemData( DWORD dwData );

private:
    PaDVAMsgListCtrl();
    const PaDVAMsgListCtrl& operator=( const PaDVAMsgListCtrl& );
    PaDVAMsgListCtrl( const PaDVAMsgListCtrl& );

protected:
    virtual void PreSubclassWindow();
    virtual afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
    virtual afx_msg void OnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
    virtual afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    virtual afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    virtual afx_msg LRESULT OnUpdateSelectedItem( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()

protected:
    // Hide InsertItem function
    virtual int InsertItem( int nItem, LPCTSTR lpszItem );

protected:
    PaRuntimeModel& m_refBroadcastModel;
    int m_nDVAMsgType;
    CFont m_fntList;
    CMenu* m_pRootConfigMenu;
};

#endif // !defined(AFX_PABROADCASTLISTCTRL_H__8C40284B_5179_4804_814C_0C933813F795__INCLUDED_)
