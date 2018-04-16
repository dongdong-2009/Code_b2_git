#if !defined(AFX_FRAMEREPEATDLG_H__156525CD_D523_4B5A_B678_F22E191254D6__INCLUDED_)
#define AFX_FRAMEREPEATDLG_H__156525CD_D523_4B5A_B678_F22E191254D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameRepeatDlg.h : header file
//

#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "IFrameRandGenerator.h"
#include "RandFrameDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFrameRepeatDlg dialog

#include "INotifyFrameRepetition.h"

class CFrameRepeatDlg : public CDialog , public IFrameRandGenerator
{
// Construction
public:
	void setListener (INotifyFrameRepetition * listener);
	
	void SetMaxFrame(long lFrameCount);
	CFrameRepeatDlg(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CFrameRepeatDlg)
	enum { IDD = IDD_DLG_REPEAT };
	CListBox	m_listFrames;
	CListBox	m_listFrameRepeat;
	CEdit	m_editFrameCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameRepeatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrameRepeatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnAddall();
	afx_msg void OnBtnRemove();
	afx_msg void OnBtnRmvall();
	virtual void OnOK();
	afx_msg void OnAddXID();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool isDuplicated ( char * pszFrameName);
	long m_lFrameCount;
	std::vector <std::string> m_vctrFrameNames;
	INotifyFrameRepetition	* m_listener;

	std::vector <long>	m_vctrFrameRepeat;

	CString				m_stringData;
	CRandFrameDlg		m_frameRandGenrator;

public: 
	virtual void notifyGeneratedFrame (std::vector <std::string> vctrRandFrame);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEREPEATDLG_H__156525CD_D523_4B5A_B678_F22E191254D6__INCLUDED_)
