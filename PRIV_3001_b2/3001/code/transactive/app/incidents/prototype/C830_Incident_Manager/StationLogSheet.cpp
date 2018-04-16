/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/incidents/prototype/C830_Incident_Manager/StationLogSheet.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// StationLogSheet.cpp : implementation file
//

#include "stdafx.h"
#include "c830_incident_manager.h"
#include "StationLogSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StationLogSheet dialog


StationLogSheet::StationLogSheet(CWnd* pParent /*=NULL*/)
	: CDialog(StationLogSheet::IDD, pParent)
{
	//{{AFX_DATA_INIT(StationLogSheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void StationLogSheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StationLogSheet)
	DDX_Control(pDX, IDC_EDIT2, m_id);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_date);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StationLogSheet, CDialog)
	//{{AFX_MSG_MAP(StationLogSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StationLogSheet message handlers

BOOL StationLogSheet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    m_date.SetFormat("     dd-MMM-yy");
    m_id.SetWindowText("1473");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
