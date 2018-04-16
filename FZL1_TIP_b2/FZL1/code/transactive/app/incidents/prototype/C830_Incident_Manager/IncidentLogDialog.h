/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/prototype/C830_Incident_Manager/IncidentLogDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_IncidentLogDialog_H__DECA8C66_1C9E_4B5F_B7E6_F1CA6A5F883A__INCLUDED_)
#define AFX_IncidentLogDialog_H__DECA8C66_1C9E_4B5F_B7E6_F1CA6A5F883A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IncidentLogDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// IncidentLogDialog dialog

class IncidentLogDialog : public CDialog
{
// Construction
public:
	IncidentLogDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(IncidentLogDialog)
	enum { IDD = IDD_INCIDENT_LOG_SHEET };
	CEdit	m_kiloLoss;
	CButton	m_innerTrack;
	CEdit	m_comments;
	CEdit	m_accDelayS;
	CEdit	m_accDelayM;
	CEdit	m_accDelayH;
	CEdit	m_initDelayH;
	CEdit	m_initDelayM;
	CEdit	m_initDelayS;
	CComboBox	m_location1;
	CEdit	m_trainId;
	CDateTimeCtrl	m_endTime;
	CDateTimeCtrl	m_endDate;
	CDateTimeCtrl	m_startTime;
	CDateTimeCtrl	m_startDate;
	CEdit	m_assetname;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(IncidentLogDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(IncidentLogDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IncidentLogDialog_H__DECA8C66_1C9E_4B5F_B7E6_F1CA6A5F883A__INCLUDED_)
