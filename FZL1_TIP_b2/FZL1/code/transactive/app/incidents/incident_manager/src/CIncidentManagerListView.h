/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/incident_manager/src/CIncidentManagerListView.h $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Provide GUI for user to Create/View/Edit/Print/Open/Close incident reports
  */

#if !defined(AFX_INCIDENTMANAGERVIEW_H__1437F56D_544E_40BD_A38F_52B99D3D313D__INCLUDED_)
#define AFX_INCIDENTMANAGERVIEW_H__1437F56D_544E_40BD_A38F_52B99D3D313D__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786)
#endif

#define _WIN32_IE 0x0400	// use extended style of ListView

#include <Afxcview.h>   	// CListView
#include <commctrl.h>
#include <vector>
//Mintao ++
//Bug 652 (TD12313)
#include <string>
//Mintao ++
//Bug 652 (TD12313)
#include <map>

#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"

#include "app/incidents/incident_manager/src/ColumnInfo.h"
#include "app/incidents/incident_manager/src/IncidentReportDialog.h"
#include "app/incidents/incident_manager/src/IncidentManagerDoc.h"
#include "app/incidents/incident_manager/src/IncidentManagerFramework.h"

/**
 * Displays the incident list.
 * 
 * NOTE: The list view does not actually replicate the incident list in the view.
 * The objects associated with each row in the list are just pointing to the
 * incident list in IncidentHandler.
 * @version 1.0
 * @created 16-May-2005 12:18:50 PM
 */
namespace TA_IRS_App
{
	
	class CIncidentManagerListView : public CListView, public TA_Base_Bus::ISortingCallback
	{
	protected: // create from serialization only
		DECLARE_DYNCREATE(CIncidentManagerListView)
			
			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CIncidentManagerListView)
	public:
		virtual void OnDraw(CDC* pDC);  // overridden to draw this view
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
		virtual void OnInitialUpdate();
	protected:
		virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
		virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
		virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
		virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL
		
		// Implementation
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif
		
		// Generated message map functions
	protected:
		//{{AFX_MSG(CIncidentManagerListView)
		afx_msg void onIncidentNew();
		afx_msg void onIncidentClose();
		afx_msg void onIncidentDelete();
		afx_msg void onIncidentEdit();
		afx_msg void onIncidentView();
		afx_msg void onPrint();
		afx_msg void OnClose();
		afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg long OnReloadIncidents( WPARAM wParam, LPARAM lParam );
		afx_msg long OnCreateIncident( WPARAM wParam, LPARAM lParam );
		afx_msg long OnRemoveIncident( WPARAM wParam, LPARAM lParam );
		afx_msg long OnUpdateIncident( WPARAM wParam, LPARAM lParam );
		afx_msg long OnUpdateRight( WPARAM wParam, LPARAM lParam );
		//}}AFX_MSG
		afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()
			
			
	public:
		static const UINT WM_APP_CREATEALARM;
		static const UINT WM_APP_RELOADINCIDENTS;
		static const UINT WM_APP_UPDATEINCIDENT;
		static const UINT WM_APP_REMOVEINCIDENT;
		static const UINT WM_APP_UPDATERIGHTS;
		
		CIncidentManagerListView();
		virtual ~CIncidentManagerListView();
		
		/**
		* GetIncidentReportDialogFromId Get a dialog from Id
		* @return  the report dialog
		* @param id    the id of the incident
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		IncidentReportDialog* getIncidentReportDialogFromId(const std::string& id);
		//Mintao ++
		//Bug 652 (TD12313)
		
		/**
		* RemoveIncidentReportDialog  Remove a dialog
		* @return None
		* @param pDlg  the dialog to be removed
		*/
		void removeIncidentReportDialog( IncidentReportDialog* pDlg);
		
		/**
		* Updates the incident list by clearing it first then repopulating it.  If a row
		* is selected before the update, this method needs to remember which row it is
		* (store the incident Id locally) and then restore the selection after it has
		* populated the list.
		*/
		void updateIncidentList();
		/**
		* Called when the Re-open button/menu is clicked.
		*/
		void onIncidentReopen();
		/**
		* Called when an alarm is to be associated with a new incident.
		* @param alarmId
		* 
		*/
		void onIncidentNewAlarmAssociation(const std::string& alarmId, const std::string & alarmValue);
		/**
		* It will iterate through all its child dialogs and prompt the operator if there
		* are any unsaved changes.  It will close any dialogs that don't have any changes
		* in them automatically.
		*/
		void onPrepareForClose();
		/**
		* This is called when the actual incident has been removed from the incident list.
		*  This method will remove the incident so that it no longer displays in the list
		* view.  It will also notify the incident report dialogs of the event.
		* @param incidentId
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		//TD13540 - modified parameter to follow programming guidelines
		void onIncidentRemoved(const std::string& incidentId);
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* This is called when the actual incident has been updated in the incident list.
		* This method will update the incident list so that it displays the correct
		* details of the incident.  It will also notify the incident report dialogs of
		* the event.
		* @param incident
		* 
		*/
		void onIncidentUpdated(ItaIncidentReportData* incident);
		
		
		IncidentManagerDoc* GetDocument();  // generated by MFC Class Wizard

		int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;		
		BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

	private:
		/**
		* sortCallback
		*
		* User defined sorting function
		*
		* @return positive value if item lParam2 before lParam1, negative if lParam1 should
		*      come first
		*
		* @param lParam1, lParam2 user defined values passed in from sort operation (app specific)
		* @param columnIndex the index of the column being sorted
		* @param currentSort defines the type of sorting specified (ie ascending or descending)
		*/
		virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort);
		/**
		* This is called by the Incident Report dialog objects to add them to the
		* list.
		*
		* @return none
		* @param pDlg
		*/
		void addIncidentReportDialog( IncidentReportDialog* pDlg);
		
		/**
		* Get number of Incident Report dialogs currently being displayed
		* @return number of dialogs
		* @param none
		*/
		int getTotalDisplayIncidentReportDialogs() const;
		
		/**
		* get the selected incidents
		* @return number of selected incidents
		* @param 
		*/
		int GetSelectedIncidents( CListCtrl& clc, std::vector<ItaIncidentReportData *> &arraySelectedIncidents );

		 /**
		 * CellRectFromPoint
		 *
		 * Determines the row, column, and bounding rect of a cell.
		 *
		 * @param point The point to be tested.
		 * @param collrect To hold the bounding rect.
		 * @param col To hold the column index.
		 *
		 * @return int The row index, or -1 on failure.
		 */
		int CellRectFromPoint( CPoint & point, RECT * cellrect, int * col ) const;

        
	private:
		
		static const unsigned long MAX_INCIDENT_TO_DISPLAY;
		static const unsigned long MAX_INCIDENT_REPORT_DIALOG;
		
		// Maintains the current sort column and order
		TA_Base_Bus::AutoSortListCtrl::SortInfo m_sortInfo;
		
		// Maintains the information for each column
		struct ColumnInfo
		{
			ColumnType type;        // Column Type
			unsigned int width;     // The width of the column
			unsigned int name;      // The string Id in the resource
			TA_Base_Bus::AutoSortListCtrl::ESortType sortBy; // The way that the column is sorted by
		};
		
		enum
		{
			//TD15204 - reverting back the changes of TD15041
			COLUMNS_COUNT = 12		
		};
		
		// Contains the information regarding all columns
		static const ColumnInfo COLUMN_INFO[COLUMNS_COUNT];
		
		std::vector<IncidentReportDialog*> m_arrayDisplayedIncidentReports;
		
		ColumnInfo m_ColumnInfo[COLUMNS_COUNT];
		IncidentManagerDoc* m_IncidentManagerDoc;
		
		int m_SortedColumnIndex;
		TA_Base_Bus::AutoSortListCtrl::ECurrentSort m_CurrentSort;
		//Bugzilla Bug 164: added by Mintao
		//Mintao ++
		//Bug 652 (TD12313)
		std::map<std::string, int> m_selectedItemMaps;
		//Mintao ++
		//Bug 652 (TD12313)
		//the string used to launch the report manager application
		static const std::string STRING_TO_LAUNCH_REPORT_MANAGER;
		static const std::string FILE_NAME_PARAM;
		static const std::string REPORT_ID_PARAM;
		//Mintao ++
		//Bug 643
		static const std::string DB_CONNECTION_STRING;
		//Mintao ++
		//Bug 643
		static const std::string RPARAM_REPORTTEMPLATE;

		//libo
		static const std::string DB_CONNECTION_FILE;
		//libo

		// Variables for multiline tooltips.
		std::auto_ptr<TCHAR> m_pchTip;
		std::auto_ptr<WCHAR> m_pwchTip;
		BOOL m_bToolTipCtrlCustomizeDone;

	};

	#ifndef _DEBUG  // debug version in IncidentManagerView.cpp
	inline IncidentManagerDoc* CIncidentManagerListView::GetDocument()
	{ return (IncidentManagerDoc*)m_pDocument; }
	#endif

}// end TA_IRS_App
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INCIDENTMANAGERVIEW_H__1437F56D_544E_40BD_A38F_52B99D3D313D__INCLUDED_)
