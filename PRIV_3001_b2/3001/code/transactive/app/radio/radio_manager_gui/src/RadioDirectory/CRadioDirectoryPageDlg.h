/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioDirectory/CRadioDirectoryPageDlg.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_CRADIODIRECTORYPAGEDLG_H__B3AF3E17_9A22_4ABF_A0A0_112CD430679C__INCLUDED_)
#define AFX_CRADIODIRECTORYPAGEDLG_H__B3AF3E17_9A22_4ABF_A0A0_112CD430679C__INCLUDED_

#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioDirectory/IRadioDirectoryPageDlgStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioUserDefinedEvents.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "app\radio\radio_manager_gui\src\RadioDirectory\RadioManagerFilterList.h"
//#include "bus/mfc_extensions/src/filter_list_ctrl/MultiFilterListCtrl.h"
#include "app/radio/radio_manager_gui/src/TabCtrlEx.h"

#include <vector>
#include <list>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

/////////////////////////////////////////////////////////////////////////////
// CRadioDirectoryPageDlg dialog

class CRadioDirectoryPageDlg : public CDialog
{
	friend class AnnouncementGroupDirectoryPanelStrategy;
	friend class RadioDirectoryPageStrategy;
	friend class DynamicGroupDirectoryPageStrategy;
	friend class MultiSelectDirectoryPageStrategy;
	friend class MultiSiteDirectoryPageStrategy;
	friend class PatchGroupDirectoryPageStrategy;
	friend class SiteDirectoryPageStrategy;
	friend class TalkGroupDirectoryPageStrategy;
		
	// Construction
	public:

		CRadioDirectoryPageDlg(CWnd* pParent = NULL);   // standard constructor

		virtual ~CRadioDirectoryPageDlg();

	// Dialog Data
		//{{AFX_DATA(CRadioDirectoryPageDlg)
	enum { IDD = IDD_DIRECTORY_PAGE };
		CEdit	m_filterELName;
		CEdit	m_filterELID;
		CTabCtrlEx	m_directoryTabCtrl;
	//}}AFX_DATA


	// Overrides
		//{{AFX_VIRTUAL(CRadioDirectoryPageDlg)
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	// Implementation
	protected:

		//{{AFX_MSG(CRadioDirectoryPageDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnBegindragList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnTabSelChanging(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg LRESULT onNotifiedEditMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedOperationMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedEntityUpdated(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedDatabaseReady(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedEntityBeingCreated(WPARAM wParam = 0, LPARAM unused = 0);
		afx_msg LRESULT onNotifiedEntitySelectionChanged(WPARAM wParam = 0, LPARAM unused = 0);
//TD18869
		afx_msg LRESULT onNotifiedEntityDeleted(WPARAM wParam = 0, LPARAM unused = 0);
//TD18869
		afx_msg LRESULT onNotifiedEntityEmptySelection(WPARAM pRadioResourceType, LPARAM unused);
		afx_msg LRESULT onNotifiedDbUpdate(WPARAM pOldResource = 0, LPARAM pNewResource = 0);
		afx_msg LRESULT onNotifiedDbDelete(WPARAM pOldResource = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedDbNew(WPARAM pNewResource = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedDbFinish(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg void OnChangeFilterName();
		afx_msg void OnChangeFilterID();
		afx_msg void OnSetfocusFilterName();
		afx_msg void OnSetfocusFilterID();
		
		afx_msg void OnColumnClicked(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:

		void setFilterListRowData();

		void enablePage (int nPage, BOOL bEnable);

	private:
//TD18869
		void focusDirectoryActiveResource(RadioResource &);
//TD18869

		void sortResources(int column, TA_Base_Bus::AutoSortListCtrl::ECurrentSort sort);

		/**
		 * The main list control listing the details of the resources in the selected tab.
		 * This control will be used on all tabs.
		 */
		RadioManagerFilterList m_directoryListCtrl;

		volatile IRadioDirectoryPageDlgStrategy* m_pRadioDirectoryPageDlgStrategy;
		
		RadioResourceType m_currentRadioResourceType;
		
		std::vector<RadioResource> m_ResourceTypeCollectionarray;
		
		int m_currentPage;

        struct DirectoryTabState
        {
            RadioResourceType type;
            bool enabled;
        };

        enum 
        {
            RADIO_TABS_NUM = 5
        };

		/*
		 * array associating Tab index and 
		 * the corresponding Resource type		
		 */
		DirectoryTabState m_tabStates[RADIO_TABS_NUM];

		/*
		 * Drag 'n' Drop variables
		 */
		CImageList* m_pDragImage;
		bool m_bDragging;

		bool m_selectedFromDirectory;
		RadioResource m_selectedResource;

		bool refreshList;
		bool descending;

		static short DGNA_INDEX;
		static short RADIO_INDEX;
		static short TALKGROUP_INDEX;

		// to store what resource type Edit Mode has, used for max members allowed.
		RadioResourceType resTypeEditMode;

		TA_Base_Bus::AutoSortListCtrl::ECurrentSort m_sortDescOrder;
		TA_Base_Bus::AutoSortListCtrl::ECurrentSort m_sortIdOrder;
		short m_columnSort;
};


#endif 
