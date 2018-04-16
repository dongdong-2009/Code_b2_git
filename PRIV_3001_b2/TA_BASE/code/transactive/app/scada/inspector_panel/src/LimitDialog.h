/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/inspector_panel/src/LimitDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class manages the dialog which displays the equiment information tab shown
  * by the inspector panel.  It allows the operator to view the values of the data points.
  */

#ifndef LIMITDIALOG_H
#define LIMITDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scada/inspector_panel/src/LimitListCtrl.h"
#include "app/scada/inspector_panel/src/LimitDetailsListCtrl.h"
#include "app/scada/inspector_panel/src/resource.h"
#include "app/scada/inspector_panel/src/ITabDialog.h"

namespace TA_Base_App
{
	class CEquipment;

	/////////////////////////////////////////////////////////////////////////////
	// CLimitDialog dialog

	class CLimitDialog : public CDialog, public ITabDialog
	{
	
	public:

	// Construction
		CLimitDialog(CWnd* pParent = NULL);   // standard constructor

		//
		// ITabDialog Implementation
		//
		virtual void setEquipment(CEquipment* equipment);
		virtual void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
		virtual void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType);
		virtual bool isUnsavedChangeOnTab();
		virtual bool isOkToChangeTab();
		virtual void getWindowRect(CRect* rect);
		virtual unsigned int getMinimumHeight();
		virtual void resize(unsigned int height);
		virtual void updatePermissions(/*TA_Base_Bus::RightsLibrary* rightsMgr*/);
		virtual bool isAnyDataPointsDisplayedInTab() const;
		virtual void resetList();

	// Dialog Data
		//{{AFX_DATA(CLimitDialog)
		enum { IDD = IDD_LIMITS };
		CEdit	m_limitInfo;
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CLimitDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CLimitDialog)
		virtual BOOL OnInitDialog();
		afx_msg void OnItemchangedInfoDatapointlist(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:

		CLimitListCtrl			m_limitList;
		CLimitDetailsListCtrl	m_limitDetailsList;
		unsigned int			m_heightDifference;
		unsigned int			m_originalHeight;
		bool					m_accessAllowed;
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // #ifndef LIMITDIALOG_H
