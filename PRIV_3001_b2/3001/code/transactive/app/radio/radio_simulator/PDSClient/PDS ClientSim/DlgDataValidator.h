#if !defined(AFX_DLGDATAVALIDATOR_H__F8B7EA1D_3237_44E7_84F9_A5730CF801A4__INCLUDED_)
#define AFX_DLGDATAVALIDATOR_H__F8B7EA1D_3237_44E7_84F9_A5730CF801A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDataValidator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDataValidator dialog

class CDlgDataValidator : public CDialog
{
// Construction
public:
	CDlgDataValidator(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDataValidator)
	enum { IDD = IDD_DLG_DATAVALIDATOR };
	CEdit	m_edtDataFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDataValidator)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDataValidator)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	void setDataFrame(CString strData);

private:
	CString		m_stringData;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDATAVALIDATOR_H__F8B7EA1D_3237_44E7_84F9_A5730CF801A4__INCLUDED_)
