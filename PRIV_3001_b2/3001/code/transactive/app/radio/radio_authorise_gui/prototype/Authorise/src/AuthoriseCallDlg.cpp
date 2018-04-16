/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_authorise_gui/prototype/Authorise/src/AuthoriseCallDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// AuthoriseCallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Authorise.h"
#include "AuthoriseCallDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAuthoriseCallDlg dialog


CAuthoriseCallDlg::CAuthoriseCallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAuthoriseCallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAuthoriseCallDlg)
	m_type = _T("");
	m_source = _T("");
	m_destination = _T("");
	//}}AFX_DATA_INIT
}


void CAuthoriseCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAuthoriseCallDlg)
	DDX_Text(pDX, IDC_EDIT1, m_type);
	DDX_Text(pDX, IDC_EDIT2, m_source);
	DDX_Text(pDX, IDC_EDIT3, m_destination);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAuthoriseCallDlg, CDialog)
	//{{AFX_MSG_MAP(CAuthoriseCallDlg)
	ON_BN_CLICKED(IDCANCEL3, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuthoriseCallDlg message handlers

BOOL CAuthoriseCallDlg::OnInitDialog() 
{
	m_type = "STATION TO STATION";
	m_source = "DBG SO";
	m_destination = "CVC SO";

	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAuthoriseCallDlg::OnClose() 
{
	OnCancel();
	
}
