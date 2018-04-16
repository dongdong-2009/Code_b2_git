/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/NewTextMessageDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// NewTextMessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "radiomanager.h"
#include "NewTextMessageDlg.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewTextMessageDlg dialog


CNewTextMessageDlg::CNewTextMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewTextMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewTextMessageDlg)
	m_SDSShortName = _T("");
	//}}AFX_DATA_INIT
}


void CNewTextMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewTextMessageDlg)
	DDX_Text(pDX, IDC_EDIT1, m_SDSShortName);
	DDV_MaxChars(pDX, m_SDSShortName, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewTextMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CNewTextMessageDlg)
	ON_EN_CHANGE(IDC_EDIT1, onChangeSDSName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CNewTextMessageDlg::OnInitDialog()
{
    // Set current text in control to internal value of m_SDSShortName
    UpdateData(false);

    // Refresh enabled state of OK button on startup
    onChangeSDSName();

    return CDialog::OnInitDialog();    
}


/////////////////////////////////////////////////////////////////////////////
// CNewTextMessageDlg message handlers

void CNewTextMessageDlg::onChangeSDSName() 
{
	UpdateData(true);
	bool isEnabled = !m_SDSShortName.IsEmpty();

	CWnd*  pWnd = GetDlgItem(IDOK);
	TA_ASSERT(pWnd!=NULL,"Unable to get OK button");
	pWnd->EnableWindow(isEnabled);
}
