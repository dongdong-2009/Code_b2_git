#ifndef INHIBITLISTCTRL_H
#define INHIBITLISTCTRL_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/inspector_panel/src/InhibitListCtrl.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the basic information in
  * the CInformationDialog class.
  *
  */


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scada/inspector_panel/src/DataPointListCtrl.h"
#include "bus/scada/common_library/src/AuditMessageUtility.h"

namespace TA_Base_Bus
{
	class DataPointProxySmartPtr;
}

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CInhibitListCtrl window

	class CInhibitListCtrl : public CAbstractDataPointListCtrl, ICellManager
	{
		// Construction
		public:
			CInhibitListCtrl();
			void refreshDataPoints();

		// Attributes
		public:

		// Operations
		public:


		// Implementation
		public:
			virtual ~CInhibitListCtrl();

			virtual bool isAnyDataPointsDisplayedOnScreen() const;

			// ICellManager methods
			virtual CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);
            virtual char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);
			virtual bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);
			//TD15438, override the function to get a chance to adjust columns width.
			virtual BOOL SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags );
			

		protected:

			bool inhibitOverrideConfirm(const int inhibitOverrideItem, TA_Base_Bus::DataPointProxySmartPtr& dataPoint, bool enable);
			void displayItem(int nItem);
			bool isCellEnabled (int nItem, int nSubItem);
			void submitAuditMessage(std::string inhibitType, std::string addRemove, std::string dataPoint, unsigned long entityKey);

			//
			// CAbstractDataPointListCtrl methods
			//
			void setupList();
			void displayDataPoints();
			void updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType);			

		protected:

			volatile bool m_dataPointsDisplayedOnScreen;
			TA_Base_Bus::AuditMessageUtility		m_auditMessageUtility;
	};

	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_)
