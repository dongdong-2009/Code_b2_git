#if !defined(AFX_SELECTSESSUBDUTYDLG_H__85895F42_72D8_4963_9DBD_9FCA100F0EE7__INCLUDED_)
#define AFX_SELECTSESSUBDUTYDLG_H__85895F42_72D8_4963_9DBD_9FCA100F0EE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectSesSubDutyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectSesSubDutyDlg dialog
namespace TA_Base_App
{
	class SelectSesSubDutyDlg : public CDialog
	{
		// Construction
	public:
		SelectSesSubDutyDlg(CWnd* pParent = NULL);   // standard constructor
		
		int GetColumnCount();
		void AdjustColumnWidth();

		TA_Base_Bus::SessionInfoEx getSelectedSessionInfo();
		
		// Dialog Data
		//{{AFX_DATA(SelectSesSubDutyDlg)
		enum { IDD = IDD_SELECT_SES_SUB_DUTY_DLG };
		CButton	m_selectSesBtn;
		CListCtrl	m_selectSessionList;
		//}}AFX_DATA
		
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(SelectSesSubDutyDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		
		// Generated message map functions
		//{{AFX_MSG(SelectSesSubDutyDlg)
		virtual void OnCancel();
		virtual BOOL OnInitDialog();
		afx_msg void OnItemchangedSelectSessionList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnSelectBtn();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
	private:
		TA_Base_Bus::RegionDutyMatrix& m_regionDutyMatrix;
		TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
		TA_Base_Bus::SessionInfoEx m_selectedSessionInfo;
	};
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSESSUBDUTYDLG_H__85895F42_72D8_4963_9DBD_9FCA100F0EE7__INCLUDED_)
