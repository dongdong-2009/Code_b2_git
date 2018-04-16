/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_authorise_gui/prototype/Authorise/src/PECDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// PECDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Authorise.h"
#include "PECDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPECDialog dialog


CPECDialog::CPECDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPECDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPECDialog)
	m_type = _T("");
	m_source = _T("");
	m_destination = _T("");
	m_statusBar = _T("");
	//}}AFX_DATA_INIT
}


void CPECDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPECDialog)
	DDX_Text(pDX, IDC_EDIT1, m_type);
	DDX_Text(pDX, IDC_EDIT2, m_source);
	DDX_Text(pDX, IDC_EDIT3, m_destination);
	DDX_Text(pDX, IDC_STATUS_BAR, m_statusBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPECDialog, CDialog)
	//{{AFX_MSG_MAP(CPECDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPECDialog message handlers

BOOL CPECDialog::OnInitDialog() 
{
	m_type = "PEC";
	m_source = "Train 087/0123 Car#2 PEC#1";
	m_destination = "OCR";

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
