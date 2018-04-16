#if !defined(AFX_RANDFRAMEDLG_H__13685930_B1EE_4EB9_B9E1_AA1F2E52F567__INCLUDED_)
#define AFX_RANDFRAMEDLG_H__13685930_B1EE_4EB9_B9E1_AA1F2E52F567__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RandFrameDlg.h : header file
//

#include <vector>
#include "IFrameRandGenerator.h"

/////////////////////////////////////////////////////////////////////////////
// CRandFrameDlg dialog

class CRandFrameDlg : public CDialog
{
// Construction
public:
	void attachObserver(IFrameRandGenerator * ptrNotifier);
	CRandFrameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRandFrameDlg)
	enum { IDD = IDD_DLG_FRAME_RAND };
	CEdit	m_editFrameRand;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRandFrameDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRandFrameDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	long m_lPreviousData;
	std::vector <std::string> m_vctrRandFrameID;
	IFrameRandGenerator * m_frameRandGen;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RANDFRAMEDLG_H__13685930_B1EE_4EB9_B9E1_AA1F2E52F567__INCLUDED_)
