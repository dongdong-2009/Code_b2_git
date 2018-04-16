/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/incidents/prototype/C830_Incident_Manager/StationLogSheet.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined(AFX_STATIONLOGSHEET_H__EB83F08E_99B9_4D20_AEAE_8FD6440F1CC4__INCLUDED_)
#define AFX_STATIONLOGSHEET_H__EB83F08E_99B9_4D20_AEAE_8FD6440F1CC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationLogSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StationLogSheet dialog

class StationLogSheet : public CDialog
{
// Construction
public:
	StationLogSheet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(StationLogSheet)
	enum { IDD = IDD_STATION_REPORT };
	CEdit	m_id;
	CDateTimeCtrl	m_date;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StationLogSheet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(StationLogSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONLOGSHEET_H__EB83F08E_99B9_4D20_AEAE_8FD6440F1CC4__INCLUDED_)
