/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ReportCommentDialog.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Class which encapsulated a dialog class and simply gets the printing comment from the operator
 *
 */

#if !defined(AFX_REPORTCOMMENTDIALOG_H__E9CD867B_B00C_4E78_82AA_CE31D050DD75__INCLUDED_)
#define AFX_REPORTCOMMENTDIALOG_H__E9CD867B_B00C_4E78_82AA_CE31D050DD75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
	class CReportCommentDialog : public CDialog
	{

	public:

		/**
		 *	GetComment()
		 *
		 *	Retrieves the comment for the print job
		 *
		 *  @return		Comment to be printed
		 *
		 */
		CString& GetComment();

		/**
		 *	CReportCommentDialog()
		 *
		 *	Default constructor
		 *
		 */
		CReportCommentDialog(CWnd* pParent = NULL);

	private:
	
		//{{AFX_DATA(CReportCommentDialog)
		enum { IDD = IDD_PRINT_COMMENT_DIALOG };
		CEdit	m_ReportComment;
		//}}AFX_DATA


		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CReportCommentDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:

		// Generated message map functions
		//{{AFX_MSG(CReportCommentDialog)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:

		/**
		 *	Comment for the print job
		 *
		 */
		CString m_szComment;
	};

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTCOMMENTDIALOG_H__E9CD867B_B00C_4E78_82AA_CE31D050DD75__INCLUDED_)
