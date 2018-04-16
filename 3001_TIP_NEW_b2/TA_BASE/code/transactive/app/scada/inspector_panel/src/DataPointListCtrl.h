#if !defined(AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_)
#define AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/inspector_panel/src/DataPointListCtrl.h $
  * @author:  Andy Parker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/10/01 16:38:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class manages the list display used to show the basic information in
  * the CInformationDialog class.
  *
  */


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/mfc_extensions/src/editable_list_control/editableListCtrl.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_Bus
{
    class DataPointProxySmartPtr;
}

namespace TA_Base_App
{
	class CEquipment;


	/////////////////////////////////////////////////////////////////////////////
	// CAbstractDataPointListCtrl window

	class CAbstractDataPointListCtrl : public CEditableListCtrl
	{
		// Construction
		public:
			CAbstractDataPointListCtrl();

		// Attributes
		public:

			// This extended style enables "tool tips" for partially hidden text
			static const int LVS_EX_LABELTIP_XXX;

		// Operations
		public:
			void setEquipment(CEquipment* equipment);
			void updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType);
			void updatePermissions(bool allowed);
			virtual bool isAnyDataPointsDisplayedOnScreen() const;
			virtual void updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType);

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CAbstractDataPointListCtrl)
			protected:
			virtual void PreSubclassWindow();
			//}}AFX_VIRTUAL

		// Implementation
		public:
			virtual ~CAbstractDataPointListCtrl();

			// Generated message map functions
		protected:
			//{{AFX_MSG(CAbstractDataPointListCtrl)
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			//}}AFX_MSG
            afx_msg void onDividerDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);

			DECLARE_MESSAGE_MAP()

			virtual void setupList() = 0;
			virtual void displayDataPoints() = 0;
			virtual void updateDataPointDisplay(int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType) = 0;
			
			int getDataPointStatusImageIndex(UINT iPoint);
			int insertItemTextImage(int iPoint, int insertNItem, std::string& value, bool addImage);
			void setItemTextImage(int nItem, int nSubItem, std::string value, bool addImage);
			void setItemTextYesNo(int nItem, int nSubItem, bool yes);

            /**
             * Helper function to retrieve the string representation of the 'Current Value' of the 
             *  input datapoint 
             * - for use displaying the 'Override Value' and 'Current Value' column text
             */
            static std::string getCurrentValueAsString(TA_Base_Bus::DataPointProxySmartPtr& dataPoint);

		protected:

			CEquipment*					m_equipment;
			bool						m_accessAllowed;

		private:

			HICON						m_greenIcon;
			int							m_greenIconIndex;
			HICON						m_redIcon;
			int							m_redIconIndex;
			HICON						m_orangeIcon;
			int							m_orangeIconIndex;
			HICON						m_blueIcon;
			int							m_blueIconIndex;
	};

	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_DATAPOINTLISTCTRL_H__8C394273_7B6C_11D7_B175_0050BAB1D931__INCLUDED_)
