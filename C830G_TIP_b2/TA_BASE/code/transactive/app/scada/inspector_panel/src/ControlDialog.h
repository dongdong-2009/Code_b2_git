/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/inspector_panel/src/ControlDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which displays the control information tab shown
  * by the inspector panel.  It allows the operator to set the values of the data points.
  */

#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlDialog.h : header file
//

#include "app/scada/inspector_panel/src/resource.h"
#include "app/scada/inspector_panel/src/ControlListCtrl.h"
#include "app/scada/inspector_panel/src/ITabDialog.h"

namespace TA_Base_App
{
	class CEquipment;

	/////////////////////////////////////////////////////////////////////////////
	// CControlDialog dialog

	class CControlDialog : public CDialog, public ITabDialog
	{
		// Construction
		public:
			CControlDialog(CWnd* pParent = NULL);   // standard constructor

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
			virtual void updatePermissions();
			virtual bool isAnyDataPointsDisplayedInTab() const;
			virtual void resetList();

		// Dialog Data
			//{{AFX_DATA(CControlDialog)
			enum { IDD = IDD_CONTROL };
				// NOTE: the ClassWizard will add data members here
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CControlDialog)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CControlDialog)
			virtual BOOL OnInitDialog();
			afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);	//wenching++ (TD13353)
	//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:

			CControlListCtrl	m_list;
			unsigned int		m_heightDifference;
			unsigned int		m_originalHeight;
			//bool				m_accessAllowed;
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_CONTROLDIALOG_H__201810A3_6FDF_11D7_B16F_0050BAB1D931__INCLUDED_)
