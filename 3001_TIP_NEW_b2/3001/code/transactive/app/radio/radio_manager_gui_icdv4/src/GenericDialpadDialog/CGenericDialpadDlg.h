/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/GenericDialpadDialog/CGenericDialpadDlg.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $ Haipeng 
 * 1: added the flag to be suited  different requirement.
 */
#if !defined(AFX_GENERICDIALPADDLG_H__DF3FA741_6E18_4745_BD5E_2711A61F5A5E__INCLUDED_)
#define AFX_GENERICDIALPADDLG_H__DF3FA741_6E18_4745_BD5E_2711A61F5A5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenericDialpadDlg.h : header file
//

#include <list>

#pragma warning (disable : 4786)

/////////////////////////////////////////////////////////////////////////////
// CGenericDialpadDlg dialog
//namespace TA_IRS_App

	class CGenericDialpadDlg : public CDialog
	{
		public:
			bool isValidInput();
			CGenericDialpadDlg(CWnd* pParent, 
                int maxDialLength = 0, int maxHistoryLength = 0,
                bool usedByRadio = true);  


			//{{AFX_DATA(CGenericDialpadDlg)
	enum { IDD = IDD_GENERIC_DIALPAD };
			CComboBox	m_recentlyDialedCB;
			CString	m_userDialedNumber;
	//}}AFX_DATA
			

			/**
			 * Returns the user-entered number. This may represent any type of resource, the
			 * caller/subscriber shall be aware of the particular type.
			 */	
			CString userDialedNumber();

			void setRecentlyDialedList(std::list<std::string>* pNumberList);

			
			//{{AFX_VIRTUAL(CGenericDialpadDlg)
			public:
			virtual int DoModal();
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			//{{AFX_MSG(CGenericDialpadDlg)
			virtual BOOL OnInitDialog();
			virtual void OnOK();
			afx_msg void OnEditChangeNumber();
			afx_msg void OnKeyButtonClicked(UINT ctrlID);
			afx_msg void OnGenericDialpadOk();
			afx_msg void OnKeyPadBackSpace();
	//}}AFX_MSG
			DECLARE_MESSAGE_MAP()


		private:

			std::list<std::string>* m_precentlyDialed;	

            int m_maxDialLength;
			int m_maxHistoryLength;
			bool m_usedInRadio;  // used in Radio or Telphone, true for Radio
	};


#endif
