#if !defined (CONFIRMCOMMANDDIALOG_H)
#define CONFIRMCOMMANDDIALOG_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmCommandDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which confirms that the data point is to be updated.
  * It provides details of the data point and the new value.
  * The return from DoModal() is used to indicate if the update should proceed (IDOK) or
  * be cancelled (IDCANCEL).
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
	class CConfirmCommandDialog : public CDialog
	{
		// Construction
		public:
			CConfirmCommandDialog(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CConfirmCommandDialog)
			enum { IDD = IDD_CONFIRM_CONTROL_DIALOG };
			CString	m_equipment;
			CString	m_newValue;
			CString	m_dataPoint;
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CConfirmCommandDialog)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CConfirmCommandDialog)
				// NOTE: the ClassWizard will add member functions here
			afx_msg void OnHelpButton(); //TD15124 azenitha
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(CONFIRMCOMMANDDIALOG_H)
