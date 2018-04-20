/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/prototype/C830_Incident_Manager/IncidentLogDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// IncidentLogDialog.cpp : implementation file
//

#include "stdafx.h"
#include "c830_incident_manager.h"
#include "IncidentLogDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// IncidentLogDialog dialog


IncidentLogDialog::IncidentLogDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IncidentLogDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(IncidentLogDialog)
	//}}AFX_DATA_INIT
}


void IncidentLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(IncidentLogDialog)
	DDX_Control(pDX, IDC_EDIT16, m_kiloLoss);
	DDX_Control(pDX, IDC_CHECK1, m_innerTrack);
	DDX_Control(pDX, IDC_EDIT11, m_comments);
	DDX_Control(pDX, IDC_EDIT20, m_accDelayS);
	DDX_Control(pDX, IDC_EDIT19, m_accDelayM);
	DDX_Control(pDX, IDC_EDIT15, m_accDelayH);
	DDX_Control(pDX, IDC_EDIT8, m_initDelayH);
	DDX_Control(pDX, IDC_EDIT14, m_initDelayM);
	DDX_Control(pDX, IDC_EDIT18, m_initDelayS);
	DDX_Control(pDX, IDC_COMBO3, m_location1);
	DDX_Control(pDX, IDC_EDIT17, m_trainId);
	DDX_Control(pDX, IDC_DATETIMEPICKER4, m_endTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER3, m_endDate);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_startTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_startDate);
	DDX_Control(pDX, IDC_EDIT21, m_assetname);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(IncidentLogDialog, CDialog)
	//{{AFX_MSG_MAP(IncidentLogDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// IncidentLogDialog message handlers

BOOL IncidentLogDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    m_startDate.SetFormat("     dd-MMM-yy");
    m_endDate.SetFormat("     dd-MMM-yy");
    m_startTime.SetFormat("     HH:mm:ss");
    m_endTime.SetFormat("     HH:mm:ss");
    
    m_assetname.SetWindowText("CDBG/ECS/TVS/TVF03");

    m_innerTrack.SetCheck(TRUE);
	m_comments.SetWindowText("Comments");
	m_accDelayS.SetWindowText("0");
	m_accDelayM.SetWindowText("5");
	m_accDelayH.SetWindowText("0");
	m_initDelayH.SetWindowText("0");
	m_initDelayM.SetWindowText("0");
	m_initDelayS.SetWindowText("35");
	m_location1.SetWindowText("DBG");
	m_trainId.SetWindowText("17");
    m_kiloLoss.SetWindowText("0");


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
