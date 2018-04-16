/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/PrintTitleDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// PrintTitleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "historyviewer.h"
#include "PrintTitleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MAX_CHARS = 200;

/////////////////////////////////////////////////////////////////////////////
// CPrintTitleDlg dialog


CPrintTitleDlg::CPrintTitleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintTitleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintTitleDlg)
	m_printTitle = _T("<No Title>");
	m_changePen = FALSE;
    m_printDateTime = TRUE;
	//}}AFX_DATA_INIT
}


void CPrintTitleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintTitleDlg)
	DDX_Control(pDX, IDC_TITLE, m_printTitleCtrl);
	DDX_Text(pDX, IDC_TITLE, m_printTitle);
	DDV_MaxChars(pDX, m_printTitle, MAX_CHARS);
	DDX_Check(pDX, IDC_CHANGE_PEN, m_changePen);
    DDX_Check(pDX, IDC_DATE_TIME_CHECKBOX, m_printDateTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintTitleDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintTitleDlg)
	ON_EN_CHANGE(IDC_TITLE, OnChangeTitle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintTitleDlg message handlers

void CPrintTitleDlg::OnOK() 
{
	UpdateData(TRUE);

    m_titles.clear();

    for(int i = 0; i < m_printTitleCtrl.GetLineCount(); ++i)
    {
        CString buffer;

        m_printTitleCtrl.GetLine(i, buffer.GetBuffer(m_printTitleCtrl.LineLength(m_printTitleCtrl.LineIndex(i))));
        buffer.ReleaseBuffer();

        CString str(buffer.GetBuffer(0));
        
        if(str == "<No Title>")
        {
            m_titles.push_back("");
            CDialog::OnOK();
            return;
        }

        m_titles.push_back(str);
    }

	CDialog::OnOK();
}

void CPrintTitleDlg::OnCancel() 
{
    m_printTitle = "";
	CDialog::OnCancel();
}

void CPrintTitleDlg::OnChangeTitle() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if(m_printTitleCtrl.GetLineCount() == 3)
    {
        m_printTitleCtrl.Undo();
    }
}
