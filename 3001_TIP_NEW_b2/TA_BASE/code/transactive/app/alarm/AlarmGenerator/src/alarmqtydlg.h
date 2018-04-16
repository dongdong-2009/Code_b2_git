#if !defined(AFX_ALARMQTYDLG_H__787FD236_9959_4EC7_8AAE_C60F8B253F29__INCLUDED_)
#define AFX_ALARMQTYDLG_H__787FD236_9959_4EC7_8AAE_C60F8B253F29__INCLUDED_

#include "alarmgeneratorDlg.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// alarmqtydlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AlarmQtyDlg dialog

class AlarmQtyDlg : public CDialog
{
	
// Construction
public:
	AlarmQtyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AlarmQtyDlg)
	enum { IDD = IDD_ALARM_QTY_DLG };
	CEdit	m_AlarmQtyEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AlarmQtyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AlarmQtyDlg)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		unsigned long m_AlarmQty;
		CAlarmGeneratorDlg* m_AlarmGenDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMQTYDLG_H__787FD236_9959_4EC7_8AAE_C60F8B253F29__INCLUDED_)
